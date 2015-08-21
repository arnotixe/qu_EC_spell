#!/usr/local/bin/perl

# FILE:   latin2pc.pl (voorheen lat2ansi.pl)
# DOEL:   conversie van ISO Latin1 naar IBM-PC code page 437 en vice versa.
#         Het teken waarmee afbreekplaatsen worden aangegeven is een verhoogde
#         punt (\267 in Latin1 en \372 in code page 437) zoals in het Groene
#         Boekje.
#         Het `tr' commando (translate) is opgesplitst in meerder regels
#         vanwege de leesbaarheid. Let op dat hierdoor karakters niet
#         tweemaal geconverteerd worden zoals in mijn vorige versie.
#
# AUTEUR: Piet Tutelaers
# VERSIE: 1.5 (12 december 1996)

do 'getopts.pl';

&Getopts('rh') || die &usage;

if ($opt_h) { &help; }

while (<>) {
   if ($opt_r) { # Code page 437 naar Latin1, remove MSDOS \r
      s/\r\n$/\n/;
      tr/\372\233\242\243/\267\242\363\372/;            # ����
      tr/\200\201\202\203\245/\307\374\351\342\321/;    # �����
      tr/\204\205\206\207\210/\344\340\345\347\352/;	# �����
      tr/\211\212\213\214\215/\353\350\357\356\354/;	# �����
      tr/\216\217\220\221\222/\304\305\311\346\306/;	# �����
      tr/\223\224\225\226\227/\364\366\362\373\371/;	# �����
      tr/\231\232\240\241\244/\326\334\341\355\361/;	# �����
   }
   else {       # Latin1 naar code page 437, add MSDOS \r
      s/\n$/\r\n/ unless /\r\n$/;
      tr/\267\242\363\372/\372\233\242\243/;            # ����
      tr/\307\374\351\342\321/\200\201\202\203\245/;    # �����
      tr/\344\340\345\347\352/\204\205\206\207\210/;	# �����
      tr/\353\350\357\356\354/\211\212\213\214\215/;	# �����
      tr/\304\305\311\346\306/\216\217\220\221\222/;	# �����
      tr/\364\366\362\373\371/\223\224\225\226\227/;	# �����
      tr/\326\334\341\355\361/\231\232\240\241\244/;	# �����
   }
   print;
}

sub help
{
   print &usage;
   print <<HELP;

BEKNOPTE UITLEG:
	Latin2pc converteert een ISO Latin1 bestand naar code page 437,
	een populaire karakter set voor de IBM-PC in Nederland. Of omgekeerd
	van code page 437 naar Latin1 met de optie -r.
	Een voorbeeld (na afloop moet latin1.txt identiek zijn met
        latin2.txt):

	   latin2pc latin1.txt | latin2pc -r > latin2.txt
   
HELP
   exit(0);
}

sub usage
{
   return "Gebruik: latin2pc [-h] [-r] [ file ... ]\n";
}

