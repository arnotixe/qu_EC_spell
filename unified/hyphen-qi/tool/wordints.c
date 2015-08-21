/*
 * FILE:    wordints.c
 *
 * PURPOSE: determine intersection of two sorted word lists (dictionary
 *	    order). The program warns when words are not properly orded.
 *
 * AUTHOR:  Piet Tutelaers (rcpt@urc.tue.nl)
 * VERSION: 1.0 (November 1996)
 */

#include <stdio.h>
#include <string.h>

#define MAXSTRLEN 256

struct dictionary {
   char *name;
   FILE *F;
   int eof;
   unsigned char line[MAXSTRLEN];
   struct dictionary *next;
} dicts[2];

extern int dictcompare(unsigned char *, unsigned char *);
void fetchword(int);
void usage();

main(int argc, char *argv[])
{
   int i;
   FILE *F;

   if (argc != 3) usage();

   argc--; argv++;
   for (i = 0; i < 2; i++) {
      F = fopen(argv[i], "r");
      if (F == NULL) {
         fprintf(stderr, "%s: no such file\n", argv[i]);
         exit(1);
      }

      dicts[i].name = argv[i];
      dicts[i].F = F;
      dicts[i].eof = 0;
      fetchword(i);
   }

   while (!dicts[0].eof) {
      while (dictcompare(dicts[1].line, dicts[0].line) < 0) {
         if (dicts[1].eof) exit(0);
	 fetchword(1);
      }
      if (dictcompare(dicts[0].line, dicts[1].line) == 0)
         printf("%s\n", dicts[0].line);
      fetchword(0);
   }
}

void usage()
{
   fprintf(stderr, "Usage: wordints dict1 dict2\n");
   exit(1);
}

void fetchword(int f)
{  int l, comp;
   char prev[MAXSTRLEN];

   strcpy(prev, dicts[f].line);
   if (fgets(dicts[f].line, MAXSTRLEN, dicts[f].F)) {
      l = strlen(dicts[f].line); dicts[f].line[l-1] = '\0';
      comp = dictcompare(dicts[f].line, prev);
      if (comp <= 0)
         fprintf(stderr, "%s: wrong order (%s,%s)\n",
	    dicts[f].name, prev, dicts[f].line);
      fflush(stderr);
   }
   else {
      dicts[f].eof = 1;
   }
}
