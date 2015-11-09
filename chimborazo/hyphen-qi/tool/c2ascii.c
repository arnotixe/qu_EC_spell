/*
 * c2ascii.c
 *
 * Purpose: converts 8-bit characters to '^^xx' hexadecimal notation.
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

#include <stdio.h>
#include <string.h>

void usage();

main(int argc, char *argv[])
{
   FILE *eight_bit_file;
   unsigned char line[80];
   int i;

   if (argc != 2) usage();

   eight_bit_file = fopen(argv[1], "r");
   if (eight_bit_file == NULL) {
      fprintf(stderr, "%s: can\'t open\n", argv[1]);
      exit (1);
   }

   while (fgets(line, 80, eight_bit_file) != NULL) {
      for (i = 0; line[i] != '\0'; i++) {
	 if (line[i] > 128) printf("^^%02x", line[i]);
	 else putchar(line[i]);
      }
   }
}

void usage()
{
   fprintf(stderr, "Usage: c2ascii file\n");
   exit(1);
}
