#!/bin/bash
# ----------------------------------------------------------------------------------
#
#    Shukllachiska Kichwa del Ecuador, hunspell format dictionary generator
#    Copyright (C) 2009 Arno Teigseth, Henry David Lara
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.
#
#    You should have received a copy of the GNU Affero General Public License
#    along with this program. See LICENSE.txt. If not, see <http://www.gnu.org/licenses/>.
#
#    The author(s) can be contacted at arno at teigseth dot no
#
# ----------------------------------------------------------------------------------
# 
# This is *NOT* the .dic file, but a script to generate such a file, taking the
# words from qu_EC.dic.MASTER. See the file qu_EC.dic.MASTER for dictionary creation rules.
#
# Generate qu_EC.dic file

# Variables
TMPFILE=/tmp/quiwork
VFILE=/tmp/quiworkVerbs
AFILE=/tmp/quiworkADJs
OFILE=/tmp/quiworkOther
OUTFILE=qu_EC.dic
MASTERFILE=qu_EC.dic.MASTER

#FUNCTIONS
 
 fixtilde () # Accents are only allowed when NOT in compound
 {
   # Remove trailing //'s
   INSTR=`echo $1 |sed 's/\/*$//'`

   # Check if entry contains accents
   if [ `echo $INSTR |grep [áóéúí]` ]; then 
      # Does contain accents, print both versions
      # 1) Print non-compound-version
      echo -n $INSTR |sed 's/\/.*//'
      # Don't set COMPOUNDBEGIN flag (/)
      echo /
      # 2) Print compound-version
      echo -n $INSTR | sed y/áóéúí/aoeui/ |sed 's/\/.*//'
      # Set ONLYINCOMPOUND flag (x)
      echo //x
   else
      # No accents, just print the entry
      echo -n $INSTR
      # Set COMPOUNDBEGIN flag (/)
      echo //
      ## Compound or not, it's ok either way
   fi  
 }
## END OF FUNCTIONS


#  Prep: remove comments and rtrim whitespace
cat $MASTERFILE |sed s/#.*$// |sed s/" "*$// |grep -v ^$ > $TMPFILE 


# Find verbs
#echo Forking out verbs
#cat $TMPFILE | grep v$ > $VFILE

# Find adjectives
echo Forking out adjectives
# "ungido" is an adjective, but the corresponding verb is
# "ungina", NOT "ungidoyana"
#
# NOTE using MASTERFILE not TEMPFILE here
cat $MASTERFILE |grep '\/a'|grep -v '#NOCONJ' |sed s/#.*$// |sed s/" "*$// |grep -v ^$ | grep a$ > $AFILE


echo Adding adjectives to DIC
cat $MASTERFILE |grep '\/a' |sed s/#.*$// |sed s/" "*$// |grep -v ^$ | grep a$ > $OFILE

echo Adding adjectives -yana form to VERBS file
# There are two morphs of adjectives: 
# 1. adjective-nayana (e.g. killa+nayana = killanaya|n|y|yuk etc)
# 2. adjective-yana (e.g. achik+yana = achikya|n|y|yuk etc)

# At first I thought they /morph/ depending on the last char of the 
#  cat $AFILE | grep '[aoeuiy]/.*' |sed 's/\/.*$/nayana\/\/r\>+-,whv/' >> $VFILE
#  cat $AFILE | grep '[^aoeuiy]//.*' |sed 's/\/.*$/yana\/\/r\>+-,whv/'>> $VFILE
# However, it seems the words are used:
# 1. killanayana = Making us /want/ to be lazy 
# 2. killayana = Causing lazyness, becoming lazy
#
# Could be I'm wrong. 
# Could be just irregular forms: killa takes -nayana while ruku takes -yana.
# For now I'll leave both forms here.

# up to v.0.9
#cat $AFILE |sed 's/\/.*$/yana\/\/r\>+-,whv/'>> $VFILE
#cat $AFILE |sed 's/\/.*$/nayana\/\/r\>+-,whv/'>> $VFILE
cat $AFILE |sed 's/\/.*$/yana\/\/r\>+-,whv/'>> $OFILE
cat $AFILE |sed 's/\/.*$/nayana\/\/r\>+-,whv/'>> $OFILE

# Find other words
echo 'Writing non-[adjective]s to DIC'

# up to v0.9: for n in `cat $TMPFILE|grep -v v$ |grep -v a$`; do
#From version 1.0 verbs go straight into the 
for n in `cat $TMPFILE|grep -v a$`; do
 # The words from MASTER ending in /x should really go out here

 #echo Adding word $n
 echo -n "."
 fixtilde $n >> $OFILE
done

# Prep outfile 
rm $TMPFILE

# Put infixes onto verbs, into outfile.
#for i in `cat $VFILE`; do
# echo Adding adjective $i to DIC
# echo $i >> $TMPFILE; #
# up to v.0.9: variant $i >> $TMPFILE; # Create variants of verb, according to the rules
 # variant, in turn, calls naInfix() wich conjugates the verbs into their final forms (only for -na- variants of the verbs)
#done

# Append names
for n in `cat names.txt`; do
 echo Adding name $n to DIC
 fixtilde $n >> $TMPFILE
done

# Create outfile
cat $OFILE >> $TMPFILE

# Count it
echo Counting entries in DIC
cat $TMPFILE |sort -u |grep -c $ > $OUTFILE

# Sort it, removing duplicates
echo Sorting DIC
cat $TMPFILE |sort -u >> $OUTFILE


#Clean up
rm $TMPFILE
rm $VFILE
rm $AFILE
rm $OFILE

