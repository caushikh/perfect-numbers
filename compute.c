/* Author: Hari Caushik
 * Date Last Modified: 3/6/13
 * Description: Computes perfect numbers 
*/

#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	unsigned int num;
	unsigned int i;
	unsigned int sum = 0;
	unsigned long nmod = 0;
	float nsec;
	float mod_per_sec;

	if (!argc)
	{
		printf("Please enter a number to determine whether");
		printf(" it is perfect.\n");
	}
	
	num = atoi(argv[1]);
	
	clock_t start = clock(), diff;
	for (i = 1; i <= num/2; i++)
	{
		if (!(num % i))
			sum += i;
		nmod++;
	}
	diff = clock() - start;

	nsec = (float) (diff * 1000) / CLOCKS_PER_SEC;
	printf("number of modulos is %ld\n", nmod);
	printf("Number of milliseconds is %10f\n", nsec);
	mod_per_sec = nmod / nsec;

	if (sum == num)
		printf("%u is a perfect number.\n", num);
	else
		
		printf("%u is not a perfect number.\n", num);
	
	printf("Compute completed %f operations per second.\n", mod_per_sec);
	return 0;
}
