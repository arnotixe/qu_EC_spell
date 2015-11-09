/*
 * qsort.c
 * -------
 * Purpose: sorting module for dictionary order based upon the Quicksort
 *          from the Modula2 version in ``Algorithms and data structures
 *          (pp. 93)''; Niklaus Wirth; Prentice Hall 1986
 *
 * Author:  Piet Tutelaers (rcpt@urc.tue.nl)
 * Version: 1.0 (May 1993)
 */

extern int dictcompare(unsigned char *, unsigned char *);

static void Sort(unsigned char *a[], int left, int right)
{   int i, j; unsigned char *x, *w;

   i=left; j=right;
   x=a[(left+right)/2];
   do {
      while (dictcompare(a[i], x) < 0) i++;
      while (dictcompare(x, a[j]) < 0) j--;
      if (i<=j) {
         w=a[i]; a[i]=a[j]; a[j]=w;
         i++; j--;
      }
   } while (i<=j);
   if (left<j) Sort(a, left, j);
   if (i<right) Sort(a, i, right);
}

void QuickSort(unsigned char *a[], int n)
{   if (n>1) Sort(a, 0, n-1);
}
