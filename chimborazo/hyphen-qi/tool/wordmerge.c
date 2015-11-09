/*
 * FILE:    wordmerge.c
 *
 * PURPOSE: merge two or more sorted word lists (dictionary order)
 *          into one new sorted list. The program warns in case lists
 *	    are not properly orded or when files do contain double words.
 *
 * AUTHOR:  Piet Tutelaers (rcpt@urc.tue.nl)
 * VERSION: 1.1 (November 1996)
 */

#include <stdio.h>
#include <string.h>

#define MAXDICTS 256
#define MAXSTRLEN 256

struct dictionary {
   char *name;
   FILE *F;
   int eof;
   unsigned char line[MAXSTRLEN];
   struct dictionary *next;
} *dicts[MAXDICTS];

extern int dictcompare(unsigned char *, unsigned char *);
void fetchword(int);
void usage();

int files = 0;

main(int argc, char *argv[])
{
   int comp, i, s;
   unsigned char *filename, smallest[MAXSTRLEN];
   FILE *F;

   if (argc < 2) usage();

   while (--argc) {
      filename = *++argv;
      F = fopen(filename, "r");
      if (F == NULL) {
         fprintf(stderr, "%s: no such file\n", filename);
         exit(1);
      }
      if (feof(F)) continue;

      dicts[files] = (struct dictionary *) malloc(sizeof(struct dictionary));
      if (dicts[files] == NULL) {
         fprintf(stderr, "Out of memory\n");
	 exit(1);
      }
      dicts[files]->name = filename;
      dicts[files]->F = F;
      dicts[files]->eof = 0;
      fetchword(files);
      files++;
      if (files == MAXDICTS) {
         fprintf(stderr, "Can't merge more than %d files\n", MAXDICTS);
         exit(1);
      }
   }

   while (1) {
      /* skip processed files */
      for (i = 0; i<files; i++) {
         if (! dicts[i]->eof) break;
      }
      if (i == files) break;

      /* find smallest word and its file index */
      for (s = i, i = i+1; i<files; i++) {
         if (dicts[i]->eof) continue;
         if (dictcompare(dicts[i]->line, dicts[s]->line) < 0) s = i;
      }
      strcpy(smallest, dicts[s]->line);
      printf("%s\n", smallest);
      fflush(stdout);
      fetchword(s);

      /* remove equal words */
      for (i = 0; i<files; i++) {
         if (dicts[i]->eof) continue;
         while (dictcompare(dicts[i]->line, smallest) == 0) {
	    if (i != s) {
               fprintf(stderr, "%s: double (%s, %s)\n", smallest, 
	          dicts[s]->name, dicts[i]->name);
	       fflush(stderr);
	    }
	    fetchword(i);
	    if (dicts[i]->eof) break;
         }
      }
    }
}

void usage()
{
   fprintf(stderr, "Usage: wordmerge dict ...\n");
   exit(1);
}

void fetchword(int f)
{  int l, comp;
   char prev[MAXSTRLEN];

   strcpy(prev, dicts[f]->line);
   if (fgets(dicts[f]->line, MAXSTRLEN, dicts[f]->F)) {
      l = strlen(dicts[f]->line); dicts[f]->line[l-1] = '\0';
      comp = dictcompare(dicts[f]->line, prev);
      if (comp <= 0)
         fprintf(stderr, "%s: wrong order (%s,%s)\n",
	    dicts[f]->name, prev, dicts[f]->line);
      fflush(stderr);
   }
   else {
      dicts[f]->eof = 1;
   }
}
