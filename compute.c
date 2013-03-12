/* Author: Hari Caushik
 * Date Last Modified: 3/6/13
 * Description: Computes perfect numbers 
*/

#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/unistd.h>
#include <sys/wait.h>
#include <sys/select.h>

#define LISTENQ 1024
#define MAXLINE 4096
#define MAXSOCKADDR 128
#define BUFFSIZE 4096
#define SERV_PORT 9878
#define SERV_PORT_STR "9878"

int main(int argc, char *argv[])
{
	/* compute perfect number variables */
	unsigned int num;
	unsigned int i;
	unsigned int sum = 0;
	unsigned long nmod = 0;
	float nsec;
	float mod_per_sec;

	/* socket variables */
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

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

	while (fgets(sendline, MAXLINE, stdin) != NULL)
	{
		bzero(recvline, MAXLINE);
	
		write(sockfd, sendline, strlen(sendline) + 1);

		if (read(sockfd, recvline, MAXLINE) == 0)
		{
			perror("problem reading");
			exit(EXIT_FAILURE);
		}
		fputs(recvline, stdout);
	}

	return 0;
}
