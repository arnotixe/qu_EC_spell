#!/usr/local/bin/perl

# FILE:   hyphens
# SYNTAX: hyphens patronen
# DOEL:   genereer afbreekstreepjes ook voor woorden waarin een
#	  apostroph (') en/of streepje (-) voorkomt.
#
# AUTEUR: Piet Tutelaers
# VERSIE: 1.0 (october 1996)

$HYPHEN = "\267";

die "Gebruik: hyphens patronen\n" unless @ARGV >= 1;
$pat = $ARGV[0]; shift;

# Bepaal root directory
$myroot = &myrootdirectory($0);
die "Can't figure out the root directory\n" unless $myroot;

# De patronen volgens het Electronische groene boekje '95
if ($myroot ne "" && $myroot ne "." && ! -r $pat) {
   $patterns = $myroot . "/patterns/$pat";
}
else {
   $patterns = $pat;
}

die "Can't open $patterns\n" unless -r $patterns;

open(H0, ">patronen0");
open(H1, "|hyphenate -R2 $patterns > patronen1") || die "hyphenation error\n";

select(STDOUT); $| = 1;
while (<>) {
   next if /^%/ || /^$/;
   print(H0);
   chop;
   s/$HYPHEN//g; tr/A-Z/a-z/;
   @woorden = split(/[' -]/);
   $invoer = join("\n", @woorden);

   print(H1 <<"EOF");
$invoer
EOF
}

close(H0); close(H1);
open(H0, "<patronen0") || die("Can not open patronen0\n");
open(H1, "<patronen1") || die("Can not open patronen1\n");

while (<H0>) {
   chop;
   $origineel = $_;
   s/$HYPHEN//g; tr/A-Z/a-z/;
   @woorden = split(/[' -]/);

   # lees woord afgebroken volgens patronen1
   $n = 0; $resultaat = "";
   for ($i = 0; $i < @woorden; $i++) {
      chop($patronen[$i] = <H1>);
      $patronen[$i] =~ s/-/$HYPHEN/g;
      $resultaat .= $patronen[$i];
      $n+=length($woorden[$i]);
      $teken = substr($_, $n, 1);
      if ($teken =~ /[' -]/) {
         $resultaat .= $teken; $n++;
      }
   }

   if ($resultaat ne &tolow($origineel)) {
      @org = split('', $origineel);
      @res = split('', $resultaat);
      $i = 0; $j = 0;
      while ($i < @org) {
	 if (&tolow($org[$i]) eq $res[$j]) {
	    if ($org[$i] eq $HYPHEN) {
	       print $HYPHEN;
	    }
	    else {
	       print $org[$i];
	    }
	    $i++; $j++;
	    next;
	 }
	 if ($org[$i] eq $HYPHEN) {
	    print $HYPHEN; $i++;
	    next;
	 }
	 print $HYPHEN; $j++;
      }
      print "\n";
   }
   else {
      print "$origineel\n";
   }
}
close(H0); close(H1);
unlink("patronen0", "patronen1");

sub tolow {
    local($_) = @_;
    tr/A-Z/a-z/;
    $_;
}

#
# Determine the directory where this program is installed.
#
sub myrootdirectory {
   local($prog) = `basename @_`;
   local($PATHSEP) = ':';

   chop($prog);
   foreach (split(/$PATHSEP/, $ENV{'PATH'})) {
      if (-x "$_/$prog" || -l "$_/$prog") {
         s#/bin$##;
         return $_;
      }
   }
   return "";
}
