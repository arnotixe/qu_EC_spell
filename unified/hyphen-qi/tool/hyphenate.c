/*
 * File:    hyphenate.c
 * Purpose: hyphenation tool for TeX based hyphenation patterns
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "char.h"

#define LEFTMIN		2	/* default minimum characters before hyphen */
#define RIGHTMIN	3	/* default minimum characters after hyphen */

int   leftmin = LEFTMIN,
      rightmin = RIGHTMIN;

#define MAXHYFS	       10	/* maximum number of hyfs in a syllable */
#define MAXPAT	       32	/* maximum pattern length */

struct pattern {
   struct pattern *next;
   unsigned char *name;		/* pattern without digits and first 2 letters */
   unsigned int hyfens;		/* contains MAXHYF hyf'es */
};

int pattern_cnt = 0;				/* counter for patterns */

#define PATTERN_CHUNK	1000
size_t patterns_left = 0, pattern_bytes = 0;
struct pattern *pattern_space_left;
struct pattern *pattern_space;

#define MAX_KEY (MAXINTERNAL * MAXINTERNAL)
struct pattern *entry[MAX_KEY];		/* first */
struct pattern *tail[MAX_KEY];		/* last */

#define TEXT_CHUNK	40000
size_t text_left = 0, text_bytes = 0;
unsigned char *text_space_left;
unsigned char *text_space;

#define max(a, b)	(a>b ? a: b)

#define LINE_LENGTH	80
unsigned char line[LINE_LENGTH];	/* one hat: for handling 8 bit-chars */
unsigned char *linep;				/* the other hat */

#define EXCEPTIONS 1801                /* maximum exceptions allowed */
unsigned char *exc[EXCEPTIONS];

/* our forwards */
int  equal(unsigned char *, unsigned char *, int);
int  exccmp(unsigned char *, unsigned char *);
int  exclusive_length(unsigned char *);
void hyphenate(unsigned char word[], int hyf[]);
void insert_exception(unsigned char *);
void packhyfs(int, int [], unsigned int *);
void read_patterns(char *);
int  search_exception(unsigned char *);
void showhyfs(unsigned char [], int []);
void unpackhyfs(int, int [], unsigned int);
void usage(char *);

enum chat { BeQUIET = 0, PATTERNS = 1, STATS = 2, INPUT = 4};
enum chat logging = BeQUIET;	/* amount of chat */

main(int argc, char *argv[])
{
   int c, done, i, j, h, l, len;

   unsigned char *q;
   int hyf[LINE_LENGTH];
   int hif[MAXPAT];
   unsigned char syllable[LINE_LENGTH];
   char *patterns_file = NULL, *prog_name;
   struct pattern *p;
   void syllabe(int, unsigned char *, unsigned char *);
 
   prog_name = argv[0];
   while (--argc > 0 && (*++argv)[0] == '-') {
      done=0;
      while ((!done) && (c = *++argv[0]))  /* allow -bcK like options */
      	 switch (c) {
         case 'd':
      	    if (*++argv[0] == '\0') {
      	       argc--; argv++;
      	    }
	    logging = atoi(argv[0]); done = 1;
	    break;
      	 case 'L':
      	    if (*++argv[0] == '\0') {
      	       argc--; argv++;
      	    }
   	    leftmin = atoi(argv[0]); done = 1;
      	    break;
      	 case 'R':
      	    if (*++argv[0] == '\0') {
      	       argc--; argv++;
      	    }
   	    rightmin = atoi(argv[0]); done = 1;
      	    break;
      	 default:
      	    fprintf(stderr, "%s: %c illegal option\n", prog_name, c);
   	    exit(1);
      	 }
      }
   
   if (argc != 1) usage(prog_name);

   patterns_file = argv[0];

   linep = line;
   read_patterns(patterns_file);

   *linep = '.';
   while (gets(linep+1)) {      
      /* look first for an exception */
      i = search_exception(linep+1);
      if (i >= 0) {
         j=0; l = exclusive_length(exc[i]);
         for (q=exc[i]; *q != '\0'; q++) {
            if (*q != '-') { putchar(*q); j++; }
            else
	       if (j>=leftmin && j <= l-rightmin)
	          putchar('-');
	 }
         putchar('\n');
         continue;
      }

      len = strlen(linep+1); line[len+1] = '.'; line[len+2] = '\0';
      for (i=0; i<=len+2; i++) hyf[i] = 0;
      for (i=0; i<=len; i++) {
      	 syllable[0] = tolow[line[i]];
	 
         /* look for single alfanumeric letter first */
         syllable[1] = '\0';
         if (alpha[syllable[0]]) {
            j = key(syllable); 
            if (j >= 0 && ((p = entry[j]) != NULL)) {
	       unpackhyfs(1, hif, p->hyfens);
	       hyf[i] = max(hyf[i], hif[0]);
	       hyf[i+1] = max(hyf[i+1], hif[1]);
               if (logging & PATTERNS) {
	          showhyfs(syllable, hif);
	          putchar('\n');
               }
            }
         }
               	 
	 /* loop through list of patterns and match them with current
            word */
         syllable[1] = tolow[line[i+1]]; syllable[2] = '\0';
         j = key(syllable); 
         if (j < 0 || (p = entry[j]) == NULL) continue;
         do {
            l = strlen((char *) p->name);
            if (equal(&line[i+2], p->name, l) == 0) {
               strcpy((char *) &syllable[2], (char *) p->name);
	       unpackhyfs(l+2, hif, p->hyfens);
	       for (h=0; h <= strlen((char *) syllable); h++)
		  hyf[i+h] = max(hyf[i+h], hif[h]);
               if (logging & PATTERNS) {
	          showhyfs(syllable, hif);
	          putchar('\n');
               }
	    }
         }
         while (p=p->next);
      }
      if (logging & PATTERNS) {
         showhyfs(line, hyf);
         putchar('\n');
      }
      hyphenate(line, hyf);
   }
}

void usage(char *myname)
{
   fprintf(stderr, "Usage: %s [-d#] [-Lleft] [-Rright] patterns\n", myname);
   exit(1);
}

   
void read_patterns(char *hyph_patterns)
{  FILE *patfile; 
   int i, lineno = 0, h, patlen, exclen, maxpatlen = 0, pairs;
   char *syl; 
   unsigned char *exception;
   unsigned char syllable[MAXPAT];
   int hyf[MAXPAT];
   struct pattern *p;
   
   patfile = fopen(hyph_patterns, "r");
   if (patfile==NULL) {
      fprintf(stderr, "Can not open %s for read\n", hyph_patterns);
      exit(1);
   }
  
   /* the following structure is assumed for the hyphenation patterns:
      % comment
      ...
      % comment
      \patterns{
      pattern
      pattern
      ...
      pattern
      }
   */
   
   /* we assume that all pointers *entry[] and *tail[] are NULL */
   
   while (fgets(linep, LINE_LENGTH, patfile)) { lineno++;
      if (*linep == '%' || *linep == '\\') continue;
      if (*linep == '}') break;

      pairs = 0; h = 0; hyf[0] = 0;
      for (i = 0; line[i] != '\n'; i++) {
	 if (line[i]=='^') { /* Richard Verhoeven */
	    /* ^^?? -> 0x?? */
#define XTON(X)  ((X)>'9'? ((X)-'a'+10):(X)-'0')
            if (line[i+1]=='^' && isxdigit(line[i+2]) &&
	       isxdigit(line[i+3])) {
	       i+=3;
	       line[i]=XTON(line[i-1])*16+XTON(line[i]);
            }
         }
         if (internal[line[i]] >= 0) {
	    syllable[h++] = line[i]; 
      	    hyf[h] = 0;
	 }
         else if (isdigit(line[i])) {
            pairs++;
	    if (pairs > MAXHYFS) {
	    	fprintf(stderr, "more than %d hyphens in %s", MAXHYFS, linep);
	    	continue;
	    }
            hyf[h] = (line[i] - '0'); 
	 }
      }
      syllable[h] = '\0'; 
      if (logging & INPUT) {
         showhyfs(syllable, hyf); putchar('\n');
      }
      patlen = strlen((char *) syllable);

      if (patterns_left == 0) {
         pattern_space = (struct pattern *) 
            calloc(PATTERN_CHUNK, sizeof(struct pattern));
         if (pattern_space == NULL) {
            fprintf(stderr, "Out of pattern space\n");
            exit(1);
         }
         patterns_left = PATTERN_CHUNK; pattern_bytes+= PATTERN_CHUNK; 
         pattern_space_left = pattern_space;
      }
       /* pattern plus \0 minus two first letters */
      if (max(patlen+1-2, 1) > text_left) {
         text_space = (unsigned char *) calloc(TEXT_CHUNK, sizeof(unsigned char));
         if (text_space == NULL) {
            fprintf(stderr, "Out of space\n");
            exit(1);
         }
         text_left = TEXT_CHUNK; text_bytes+= TEXT_CHUNK; 
         text_space_left = text_space;
      }

      i = key(syllable); 
      if (i < 0) {
         fprintf(stderr, "Invalid pattern: %s in line %d\n", linep, lineno);
         exit(1);
      }
      
      p = pattern_space_left++; patterns_left--;
      pattern_cnt++;

      if (entry[i] == NULL) entry[i] = p; 
      else tail[i]->next = p;
      tail[i] = p;
      p->next = NULL;
      p->name = text_space_left;
      if (patlen > 2) {
         strcpy((char *) text_space_left, (char *) syllable+2);
         text_space_left+=(patlen+1-2); text_left-=(patlen+1-2); 
         if (patlen+1 > MAXHYFS) {
            fprintf(stderr, "Can not store more than %d hyf's\n", MAXHYFS);
	    exit(1);
         }
      }
      else {
         *text_space_left++ = '\0'; text_left-=1; 
      }
      packhyfs(patlen, hyf, &p->hyfens);
      maxpatlen = max(maxpatlen, patlen);
   } 

   /* now read exceptions */
   for (i=0; i<EXCEPTIONS; i++) exc[i] = (unsigned char *) NULL;
   linep = line;
   
   while (fgets(linep, LINE_LENGTH, patfile)) { lineno++;
      if (*linep == '%' || *linep == '\\') continue;
      if (*linep == '}') break;
      exclen = strlen(linep); /* including newline */
      
          /* get enough text space */
      if (exclen > text_left) {
         text_space = (unsigned char *)
	    calloc((size_t) TEXT_CHUNK, sizeof(unsigned char));
         if (text_space == NULL) {
            fprintf(stderr, "Out of space\n");
            exit(1);
         }
         text_left = TEXT_CHUNK; text_bytes+= TEXT_CHUNK; 
         text_space_left = text_space;
      }

      exception = text_space_left;
      strncpy((unsigned char *) text_space_left,
         (unsigned char *) linep, exclen - 1);
      text_space_left+=exclen-1; *text_space_left++='\0';
      text_left-=exclen; 

      insert_exception(exception);
   }
   
   fclose(patfile);

   if (logging & STATS) {
      int maximum, N, count, average, M; 
      unsigned char a, b; float sum;
      void syllabe(int, unsigned char *, unsigned char *);
     
      printf("Loaded %d patterns. Maximum pattern length %d\n",
	pattern_cnt, maxpatlen);
      patterns_left*=sizeof(struct pattern);
      pattern_bytes*=sizeof(struct pattern);
      printf("Memory consumption:\n");
      printf("patterns : %d bytes (%d unused)\n", pattern_bytes, patterns_left);
      printf("syllables: %d bytes (%d unused)\n", text_bytes, text_left);
      sum = maximum = N = 0;
      for (i = 0; i < MAX_KEY; i++) {
         if (entry[i] == NULL) continue;
         for (count = 0, p = entry[i]; p != NULL; p=p->next) count++;
         if (count > maximum) {
      	    maximum = count; M = i;
         }
         sum+=count; N++;
      }
      printf("syllables %d   average %5.1f\n", N, ((float) sum/N));
      syllabe(M, &a, &b);
      printf("maximum of patterns %d (%c%c)\n", maximum, a, b);
#     ifdef DEBUGG
      p = entry[M]; count = 0;
      while (p != NULL) {
	 syllable[0] = a; syllable[1] = b; 
	 strcat(syllable, p->name);
	 unpackhyfs(strlen(syllable), hyf, p->hyfens);
	 printf("<");
	 showhyfs(syllable, hyf);
	 printf(">, ");
	 if (++count % 5 == 0) putchar('\n');
         p = p->next;
      }   
      putchar('\n');
#     endif
   }
}

int key(unsigned char pair[])
{
   if (internal[pair[0]] == -1 || internal[pair[1]] == -1) return -1;
   return (internal[pair[0]] << INTERNALSHIFT) | internal[pair[1]];
}

void syllabe(int key, unsigned char *a, unsigned char *b)
{
   *b = external[key % MAXINTERNAL];
   *a = external[key >> INTERNALSHIFT];
   return;
}

void showhyfs(unsigned char word[], int hyf[])
{  int i;

   for (i=0; i < strlen((char *) word); i++) {
      if (hyf[i]) putchar(hyf[i] + '0');
      putchar(word[i]);
   }
   if (hyf[i]) putchar(hyf[i] + '0');
}

void packhyfs(int patlen, int hyf[], unsigned int *hyfens)
{  int i, hyphens;

   hyphens = 0;
   for (i=0; i <= patlen; i++)
      hyphens = (hyphens<<3) | hyf[i];
   *hyfens = hyphens;
}

void unpackhyfs(int patlen, int hyf[], unsigned int hyfens)
{  int i;

   for (i=patlen; i >= 0; i--, hyfens>>=3)
      hyf[i] = hyfens & 07;
}

void hyphenate(unsigned char word[], int hyf[])
{  int i, len;

   len = strlen((char *) word);
   for (i=1; i<len-1; i++) {
      if ((i <= leftmin) || (i >= len-rightmin)) putchar(word[i]);
      else {
         if (hyf[i] % 2) putchar('-');
         putchar(word[i]);
      }
   }
   putchar('\n');
   return;
}

int equal(unsigned char *cs, unsigned char *ct, int n)
{
   while (n > 0 && tolow[*cs] == tolow[*ct]) { 
      n--; cs++; ct++;
   }
   if (n == 0) return 0;
   return (tolow[*cs] < tolow[*ct]? -1 : 1);
}

/* functions to handle exceptions */

static int free_exc = EXCEPTIONS; 

void insert_exception(unsigned char *exception)
{  int j; unsigned char *p;

   if (free_exc == 0) {
      fprintf(stderr, "Too many exceptions (limit=%d)!\n", EXCEPTIONS);
      exit(1);
   }
   /* compute hash function */
   j = *exception;
   for (p = exception+1; *p != '\0'; p++) 
      if (*p != '-') j = (j + j + *p) % EXCEPTIONS;
   
   while (exc[j] != NULL) {
      if (exccmp(exc[j], exception) < 0) {
         p = exc[j]; exc[j] = exception; exception = p;
      }
      j--;
      if (j < 0) j+=EXCEPTIONS;
   }
   exc[j] = exception; free_exc--;
}

int search_exception(unsigned char *exception)
{  int j, res; unsigned char *p;

   /* compute hash function */
   j = *exception;
   for (p = exception+1; *p != '\0'; p++) 
      if (*p != '-') j = (j + j + *p) % EXCEPTIONS;

   if (exc[j] == NULL) return -1;
   while ((res = exccmp(exc[j], exception)) > 0) {
      j--;
      if (j < 0) j+=EXCEPTIONS;
      if (exc[j] == NULL) return -1; /* vreemd! */
   }
   if (res == 0) return j; /* found */
   return -1; /* not found */
}

int exccmp(unsigned char *cs, unsigned char *ct)
{
   while (1) {
      if (*cs == '-') cs++;
      if (*ct == '-') ct++;
      if (*cs == '\0' && *ct == '\0') return 0;
      if (tolow[*cs] != tolow[*ct]) break;
      cs++; ct++;
   }
   return (tolow[*cs] < tolow[*ct]? -1 : 1);
}

int exclusive_length(unsigned char *c)
{  int len = 0;

   for (; *c != '\0'; c++) 
      if (*c != '-') len++;
   return len;
}
