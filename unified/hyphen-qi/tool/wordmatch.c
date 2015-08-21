/*
 * wordmatch.c
 *
 * Purpose: compares a dictionary containing correct hyphenated words
 *          against a file containing possibly incorrectly hyphenated
 *          words. The output shows the words in which a hyphen is missed
 *          (indicated by `-') or in which a hyphen is placed on a bads
 *          position (indicated by `.'). Correct hyphens are reported
 *          with `*'. The overall percentages of missed and bad hyphens
 *          is also computed.
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

#include <stdio.h>
#include <strings.h>

main(int argc, char *argv[])
{
   char line1[80], line2[80], *w1, *w2; 
   FILE *hyphdict, *hyphfile;
   int good = 0, bad = 0, missed = 0;
   
   if (argc != 3) {
      fprintf(stderr, "Usage: wordmatch hyphdict hyphfile\n");
      exit(1);
   }
   
   hyphdict = fopen(argv[1], "r");
   if (hyphdict == NULL) {
      fprintf(stderr, "%s: can't open\n", argv[1]);
      exit(1);
   }
   
   hyphfile = fopen(argv[2], "r");
   if (hyphfile == NULL) {
      fprintf(stderr, "%s: can't open\n", argv[2]);
      exit(1);
   }
   
   while (1) {
      w1 = line1;
      w2 = line2;
      if (fgets(w1, 80, hyphdict) == NULL) break;
      if (fgets(w2, 80, hyphfile) == NULL) {
         fprintf(stderr, "%s: unexpected eof\n", argv[1]);
         exit(1);
      }
      if (strcmp(w1, w2) == 0) {
	 for ( ; *w1 != '\n'; w1++) {
      	    if (*w1 == 0267) good++;
	 }
      	 continue;
      }
      while (1) {
      	 if (*w1 == '\n' || *w1 == '\0') {
	    putchar('\n');
	    break;
	 }
      	 if (*w1 == *w2) {
      	    if (*w1 == 0267) {
      	       putchar('*'); good++;
      	    }
      	    else putchar(*w1); 
      	    w1++; w2++; continue;
      	 }
	 if (*w2 == 0267) {
	    putchar('.'); w2++; bad++;
	    continue;
	 }
	 if (*w1 == 0267) {
	    putchar(0255); w1++; missed++;
	    continue;
	 }
	 fprintf(stderr, "words don't match (%s,%s)\n", line1, line2);
	 exit(1);
      }
   }
   if (good+missed > 0) {
      fprintf(stderr," good (%7d) bad (%7d) missed (%7d)\n", good, bad, missed);
      fprintf(stderr," good (%6.2f%%) bad (%6.2f%%) missed (%6.2f%%)\n",
         100.0*good/(good+missed), 100.0*bad/(good+missed), 
         100.0*missed/(good+missed));
   }

   fclose(hyphdict);
   fclose(hyphfile);
}
