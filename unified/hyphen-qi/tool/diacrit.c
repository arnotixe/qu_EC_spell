/*
 * diacrit.c
 *
 * Purpose: shows the ISOLatin1 character set
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

#include <stdio.h>

void iso();

main()
{
	iso();
}

void iso()
{	unsigned char i, j;

	printf("     ");
        for (j=0x2; j<=0x7; j++)
	   printf("%4x ", j);
	printf("     ");
        for (j=0xa; j<=0xf; j++)
	   printf("%4x ", j);
	putchar('\n');
	putchar('\n');
	for (i=0x0; i<=0xf; i++) {
	   printf("%4x ", i);
	   for (j=0x2; j<=0x7; j++)
	      printf("   %c ", ((unsigned char) j<<4 | i));
	   if (i == 0xf) printf("      ");
	   else printf("     ");
	   for (j=0xa; j<=0xf; j++)
	      printf("   %c ", ((unsigned char) j<<4 | i));
	   putchar('\n');
	}
}
