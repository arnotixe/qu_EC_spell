/*
 * wordsort.c
 *
 * Purpose: sorts a dictionary consisting of hyphenanted words possibly
 *          containing accented words.
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEXT_CHUNK	10000
size_t text_left = 0, text_bytes = 0;
unsigned char *text_space_left;
unsigned char *text_space;

#define max(a, b)	(a>b ? a: b)

#define LINE_LENGTH	80
unsigned char line[LINE_LENGTH];	/* one hat: for handling 8 bit-chars */
char *linep;				/* the other hat */

unsigned char *w[600000]; /* pointers to words */
int W = 0;                /* number of words */

int  equal(unsigned char *, unsigned char *, int);
void read_words(char *);
void print_words();
void QuickSort(unsigned char *[], int);
void usage();

main(int argc, char *argv[])
{
   if (argc < 2) usage();
  
   while (--argc) read_words(*++argv);
   QuickSort(w, W);
   print_words();

}

void usage()
{
   fprintf(stderr, "Usage: wordsort dict ...\n");
   exit(1);
}

void read_words(char *dict)
{  FILE *wordfile; int wordlen;
   
   wordfile = fopen(dict, "r");
   if (wordfile==NULL) {
      fprintf(stderr, "%s: no such file\n", dict);
      exit(1);
   }
 
   linep = (char *) line;
   while (fgets(linep, LINE_LENGTH, wordfile)) {
      wordlen = strlen((char *) linep);
      *(linep+wordlen-1) = '\0';
      wordlen--;

      if (wordlen + 1 > text_left) {
         text_space = (unsigned char *) calloc(TEXT_CHUNK, sizeof(unsigned char));
         if (text_space == NULL) {
            fprintf(stderr, "Out of space\n");
            exit(1);
         }
         text_left = TEXT_CHUNK; text_bytes+= TEXT_CHUNK; 
         text_space_left = text_space;
      }
      
      strcpy((char *) text_space_left, (char *) linep);
      w[W++] = text_space_left;
      text_space_left+=(wordlen+1); text_left-=(wordlen+1); 
   } 
   fprintf(stderr, "Read: %d words (%s)\n", W, dict);
   fclose(wordfile);
}

void print_words()
{  int i;

   for (i=0; i<W; i++) printf("%s\n", w[i]);
}
