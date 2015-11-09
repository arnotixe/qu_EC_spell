/*
 * string.c
 *
 * Purpose: string module for dictionary words.
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.1 (October 1996)
 */

#include <ctype.h>
#include <string.h>
#include "char.h"

static int undictable(unsigned char c)
{
   if (islower(ctab[c]) || isdigit(ctab[c])) return 0;
   return 1;
}

static int strcompare(unsigned char *s, unsigned char *t)
{ 
   while (1) {
      if (*s == *t) {
         if (*s == '\0') return 0;
	 s++; t++;
	 continue;
      }
      return *s - *t;
   }
}

int dictcompare(unsigned char *s, unsigned char *t)
{  unsigned char *s0, *t0;

   s0 = s; t0 = t;
   while (1) {
      /* skip when no digit, alphanumeric or space */
      while (*s && undictable(*s)) s++;
      while (*t && undictable(*t)) t++;
      if (ctab[*s] == ctab[*t]) {
         if (*s == '\0') return strcompare(s0, t0);
	 s++; t++;
	 continue;
      }
      return ctab[*s] - ctab[*t];
   }
}
