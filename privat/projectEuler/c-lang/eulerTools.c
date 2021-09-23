#include <math.h>
#include <string.h>
#include <stdio.h>
#include "eulerCaches.h"

int fibonacci_recurs(int n)
/* Calculates the fibonacci value at n recursively */
{
	return n<2?n:fibonacci_recurs(n-2)+fibonacci_recurs(n-1);
}


int fibonacci_iter(int n)
/* Calculates the fibonacci value at n iteratively */
{
	int a = 0;
	int i = 0;
	int sum = 0;
	int b = 1;

	if (n <= 0)
		return a;
	if (n < 2)
		return n;

	for (i = 1; i < n; i++) {
		sum = (a+b);
		a = b;
		b = sum;
	}

	return sum;
}


long fact_iter(int n)
{
	long acc = 1;
	for (int i = 1; i <= n; i++)
		acc *= i;
	return acc;
}




int is_prime(int n)
/* Simple trial division to find out if n is prime. Caches res as true/false 
 in char array indexed by n. Expence payed at first run-through but will
 minimize calculation times throughout the problems */
{
	int i;
	if (n < 2)
		return 0;

        if (n < NPRIME_MAX && primeCache[n] == 1)
                return 1;
        /* 200ms saved (ch3) by calculating sqrt here & not in loop-sign.
	 * Cause of continous recalc of sqrt. */
	int max = sqrt(n);

	for (i = 3; i <= max; i+=2)
		if (n%i == 0)
			return 0;

        if (n < NPRIME_MAX)
                primeCache[n] = 1;

	return 1;
}



int palindrome(char *seq)
/* Test if sequence is a palindrome with pointer arithmetics */
{
	char *front_ptr, *rear_ptr;

	int len = strlen(seq);
	front_ptr = seq;
	rear_ptr = seq+(len-1);

	for (int i = 0; i <= len / 2; i++) {
		if (*(front_ptr+i) != *(rear_ptr-i))
			return 0;
	}
	return 1;
}


int collatz_seq(long n)
/* Recursively accumulate the number of elements in a sequence
   as defined by the collatz problem */
{
	if (n==1)
		return 0;
	if (n < 1000000 && collatzCache[n] != -1)
		return collatzCache[n];
	n=((n&1)==0)?(n>>1):(3*n+1);
	return 1+collatz_seq(n);
}



void path_walk(int left, int right, int *cnt)
{
	if (right == 0 && left == 0)
		return;

	if (left > 0) {
		*cnt+=1;
		path_walk(left-1, right, cnt);
	}
	if (right > 0) {
		*cnt+=1;
		path_walk(left, right-1, cnt);
	}
	return;
}
