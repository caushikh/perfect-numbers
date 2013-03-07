/* Author: Hari Caushik
 * Date Last Modified: 3/6/13
 * Description: Computes perfect numbers 
*/

#include <stdio.h>

int main(int argc, char *argv[])
{
	unsigned int num;
	unsigned int i;
	unsigned int sum = 0;

	if (!argc)
	{
		printf("Please enter a number to determine whether");
		printf(" it is perfect.\n");
	}
	
	num = atoi(argv[1]);

	for (i = 1; i <= num/2; i++)
	{
		if (!(num % i))
			sum += i;
	}

	if (sum == num)
		printf("%u is a perfect number.\n", num);
	else
		
		printf("%u is not a perfect number.\n", num);
	return 0;
}
