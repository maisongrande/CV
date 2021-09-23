#include "eulerTools.h"
#include "eulerCaches.h"


long challenge_1(void)
/* Find the sum of all numbers divisible by 3 and 5 upto 1000 */
{
	int sum = 0, ix = 0;

	for (ix = 3; ix < 1000; ix += 3)
		sum += ix;
	for (ix = 5; ix < 1000; ix += 5) {
		if (ix%3)
		  sum += ix;
	}
	return sum;
}



long challenge_2(void)
/* Find the sum of all even fibonacci terms < 4000000 */
{
	int final_term = 4000000;
	int n = 0;
	int sum = 0;
	int term = 0;

	while ((term = fibonacci_iter(n)) < final_term) {
		if (term&1)
			sum += term;
		n += 1;
	}
	return sum;
}



long challenge_3(void)
/* What is the largest prime factor of the number 600851475143 ? */
{
	long num = 600851475143;
	int pos_fact = sqrt(600851475143);

	/* make number odd to decrement by 2 */
	if (pos_fact % 2 == 0)
		pos_fact++;

	while(pos_fact > 0) {
		if (is_prime(pos_fact) && num % pos_fact == 0)
				return pos_fact;
		pos_fact -= 2;
	}
	return -1;
}



long challenge_4(void)
/* Find the largest palindrome made from the product of two 3-digit numbers. */
{
	char prod[10];
	int new = 0;
	int old = 0;

	for (int ix = 999; ix >= 100; ix--) {
		for (int jx = 999; jx >= 100; jx--) {
			sprintf(prod, "%d",ix*jx);
			if (palindrome(prod)) {
				new =  ix*jx;
				if (new > old) {
					old = new;
				}
			}
		}
	}
	return old;
}



long challenge_5(void)
/* Find the first number that is evenly divisible by all of the numbers 1-20 */
{
	/* 600ms saved by ugliness */
	for(int i = 2520;;i+=20) {
		if (i%11==0 && i%12==0 && i%13==0 && i%14==0 && i%15==0
		    && i%16==0 && i%17==0 && i%18==0
		    && i%19==0 && i%20==0)
			return i;
	}
}



long challenge_6(void)
/*Find diff between the sum of the squares of the first one hundred natural
 * numbers and the square of the sum */
{
	int square_sum = 0;
	int sum_square = 0;

	for (int i = 1; i <= 100; i++) {
		square_sum += i*i;
		sum_square += i;
	}
	return ((sum_square*sum_square) - square_sum);
}



long challenge_7(void)
/* What is the 10001st prime number? */
{
	int cnt = 1;
	int n = 1;
	int res = 0;

	while (cnt != 10001) {
		res = n;
		if (is_prime(n))
		    cnt++;
		n += 2;
	}
	return res;
}



long challenge_8(void)
/* Find the thirteen adjacent digits in the 1000-digit number that have the
 * greatest product. What is the value of this product */
{
	int adjec_max = 13;
	long sum = 1;
	int len = strlen(oppg8Num);
	long old_sum = 0;
	int c;

	for (int i = 0; i < (len - adjec_max); i++) {
		for (int j = i; j < i+adjec_max; j++) {
			c = oppg8Num[j];
			if (c == 48)
				break;
			sum *= (c-48);
		}
		if (sum > old_sum)
			old_sum = sum;
		sum = 1;
	}
	return old_sum;
}



long challenge_9(void)
/* There exists exactly one Pythagorean triplet for which
   a + b + c = 1000. Find the product abc. */
{
	int a, b;
	double c = 0;
	for (a = 10; a < 500; a++) {
		for (b = a+1; b < 500; b++) {
			c = sqrt(a*a + b*b);
			if (a+b+c == 1000)
				return a*b*c;
		}
	}
	return -1;
}



long challenge_10(void)
/* Find the sum of all the primes below two million. */
{
	int max_num = 2000000;
	long res = 2+3+5+7;
	int i = 9;

	while(1) {
		if (i >= max_num)
			return res;
		res += is_prime(i) ? i:0;
		i += 2;
	}
	return -1;
}



long challenge_11(void)
/* What is the greatest product of four adjacent numbers in the same
 * direction (up, down, left, right, or diagonally) in the 20×20 grid? */
{
	int size = 20*20;
	int up_diag_sum = 0, up_diag_max = 0;
	int down_diag_sum = 0, down_diag_max = 0;
	int ew_sum = 1, ew_max = 0; /* East-West */
	int ns_sum = 0, ns_max = 0; /* Nort-South */
	int sum = 0;

	for (int i = 1; i <= size-4; i++) {
		if (i % 4 == 0) {
			ew_max = ew_sum > ew_max ? ew_sum : ew_max;
			ew_sum = 1;
		}
		if (i < (size - 4*20)) {
			ns_sum = ch11Num[i-1] * ch11Num[(i-1) + 20] *
				 ch11Num[(i-1) + 40] * ch11Num[(i-1) + 60];

			ns_max = ns_sum > ns_max ? ns_sum : ns_max;

			down_diag_sum = ch11Num[i-1] * ch11Num[(i-1) + 21] *
				ch11Num[(i-1) + 42] * ch11Num[(i-1) + 63];

			down_diag_max = down_diag_sum > down_diag_max ?
				down_diag_sum:down_diag_max;
		}
		if (i > 59) {
			up_diag_sum = ch11Num[i-1] * ch11Num[(i-1) - 19] *
				ch11Num[(i-1) - 38] * ch11Num[(i-1) - 57];

			up_diag_max = up_diag_sum > up_diag_max ?
				up_diag_sum:up_diag_max;
		}
		ew_sum *= ch11Num[i-1];
	}
	sum = ns_sum > up_diag_max ? ns_sum : up_diag_max;
	sum = sum > down_diag_max ? sum : down_diag_max;
	sum = sum > ew_max ? sum : ew_max;
	return sum;
}



long challenge_12(void)
/* What is the value of the first triangle number to have over five hundred
 * divisors? */
{
	int triangular_num = 0;
	int n = 1;
	int done = 0;
	int divisors = 0;
	int limit = 0;
        
	while (!done) {
		limit = triangular_num = n*(n+1)>>1; /* rshift to avoid div */
		for (int i = 1; i < limit; i++) {    /* find num-divisors */
			if (triangular_num%i == 0) {
				limit = i==2?triangular_num>>1:
					triangular_num/i; /* 0.02s shaved.. */
				if (limit != i)
					divisors++;
				divisors++;
			}
		}
		n++;
		if (divisors > 500)
			done = 1;
		divisors=0;
	}
	return triangular_num;
}



long challenge_13(void)
/* Work out the first ten digits of the sum of the following one-hundred
 * 50-digit numbers. */
{
        int col = 50;
        int rows = 100;
        char part_sum[52];
        int carry = 0;
        int tmp_sum = 0;
        int overflow = 0;
        char answer[12];
        
        part_sum[51] = '\0';
        
        for (int i = col; i >= 0; i--) {
                for (int j = 0; j < rows; j++)
                        tmp_sum += (ch13num[j][i]-48);
                tmp_sum += carry;
                carry = tmp_sum/10;
                part_sum[i] = ((tmp_sum%10)+48);
                overflow = carry;
                tmp_sum = 0;
        }
        snprintf(answer, 11, "%d%s", overflow, part_sum);
        return atol(answer);
}



long challenge_14(void)
/* Which starting number, under one million, produces the longest chain? */
{
	int old = 0;
	int new = 0;
	int res = 0;

	for (int j = 0; j < 1000000; j++)
		collatzCache[j] = -1;

	for (int i = 1; i < 1000000; i++) {
		new = collatz_seq(i);
		collatzCache[i] = new;
		if (new > old) {
			old = new;
			res = i;
		}
	}
	return res;
}



long challenge_15(void)
{
	return 0;
}


