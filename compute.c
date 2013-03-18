/* Author: Hari Caushik
 * Date Last Modified: 3/6/13
 * Description: Computes perfect numbers 
*/

#include <stdio.h>
#include <time.h>
#include <math.h>

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
#define SERV_COMP_PORT 9880
#define SERV_COMP_PORT_STR "9880"
#define SERV_CONT_PORT 9882
#define SERV_CONT_PORT_STR "9882"
#define MAX_PERFECT 15

static int sockfd;
static int contfd;

void cpbuf(char sendline[MAXLINE], char *buf);

static void chandler(int sig)
{
	close(sockfd);
	_exit(EXIT_FAILURE);
}
static void phandler(int sig)
{
	wait(NULL);
	close(contfd);
	_exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
	/* compute perfect number variables */
	unsigned int num;
	uint32_t i, j, k, n;
	int len;
	
	uint32_t sum = 0;
	unsigned long nmod = 0;
	float nsec;
	float mod_per_sec;
	uint32_t start;
	uint32_t end;
	uint32_t perfect[MAX_PERFECT];
	clock_t t;
	pid_t compute;
	struct sigaction csa;
	struct sigaction psa;

	/* socket variables */
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];
	char *buf = '\0';

	if (!argc)
	{
		printf("Please enter the server address\n");
		exit(EXIT_FAILURE);
	}

	/* create server sockets */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/* install compute signal handlers */
	sigemptyset(&csa.sa_mask);
	csa.sa_flags = 0;
	csa.sa_handler = chandler;
	if (sigaction(SIGINT, &csa, NULL) == -1)
		perror("sigaction failed\n");
	if (sigaction(SIGQUIT, &csa, NULL) == -1)
		perror("sigaction failed\n");
	if (sigaction(SIGHUP, &csa, NULL) == -1)
		perror("sigaction failed\n");
	if (sigaction(SIGUSR1, &csa, NULL) == -1)
		perror("sigaction failed\n");

	contfd = socket(AF_INET, SOCK_STREAM, 0);

	/* connect to compute server */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_COMP_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	write(sockfd, "1", 1);

	/* connect to control server */
	servaddr.sin_port = htons(SERV_CONT_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(contfd, (struct sockaddr *)&servaddr, sizeof(servaddr));


	/* one process monitors control socket, other does computation */

#if 0
	/* monitoring control - parent */
	if (!fork())
	{
		/* got signal from manage */
		if ((n = read(contfd, recvline, MAXLINE)) != 0)
		{
			/* stop computation and shut down cleanly */
		}
		else
		{
			/* server unexpectedly quit */
		}
	}
#endif
	compute = fork();

	if (compute != 0)
	{
		/* do perfect number computation */
		while (read(sockfd, recvline, MAXLINE) != 0)
		{
			/* get start and end numbers */
			bzero(sendline, MAXLINE);
			len = strlen(recvline);
			start = (uint32_t) atol(recvline);
			end = (uint32_t) atol(recvline + len + 1);
			printf("%s, %s\n", recvline, recvline + len + 1);
			printf("start: %d\n", start);
			printf("end: %d\n", end);
			bzero(&perfect, sizeof(perfect));
			k = 0;
	
			/* find perfect numbers and time it */
			t = clock();
			printf("begin time: %d\n", t);
			sum = 0;
			for (i = start; i <= end; i++)
			{
				for (j = 1; j <= i/2; j++)
				{
					if (!(i % j))
						sum += j;
					nmod++;
				}
				
				if (i == 6)
					printf("6 sum = %d\n", sum);
	
				/* number is a perfect number */
				if (sum == i)
				{
					perfect[k] = i;
					printf("Perfect Number Found: %d\n", i);
					k++;
				}
				sum = 0;
			}
			t = clock() - t;
			printf("diff time: %d\n", t);
			/* scale time and compute performance*/
			nsec = (float) t / CLOCKS_PER_SEC;
			mod_per_sec = nmod / nsec;
	
			/* send perfect numbers and performance characteristics to
			  server */
			n = sprintf(sendline, "%f;", mod_per_sec);
			for (i = 0; (i < k) && (n < MAXLINE); i++)
			{
				n += sprintf(&sendline[n], "%d, ", perfect[i]); 
			}
			write(sockfd, sendline, n);
			bzero(recvline, MAXLINE);
		
			/* send to server */
	
	#if 0
			write(sockfd, sendline, strlen(sendline) + 1);
	
	
			/* receive from server */
			if (read(sockfd, recvline, MAXLINE) == 0)
			{
				perror("problem reading");
				exit(EXIT_FAILURE);
			}
			fputs(recvline, stdout);
			fputc('\n', stdout);
//		}
	#endif
		}
	}
	else
	{
		/* monitor control socket */
		union sigval val;
		val.sival_int = 0;
		/* install control signal handler */
		sigemptyset(&psa.sa_mask);
		psa.sa_flags = 0;
		psa.sa_handler = phandler;
		if (sigaction(SIGINT, &psa, NULL) == -1)
			perror("sigaction failed\n");
		if (sigaction(SIGQUIT, &psa, NULL) == -1)
			perror("sigaction failed\n");
		if (sigaction(SIGHUP, &psa, NULL) == -1)
			perror("sigaction failed\n");
	
		read(contfd, recvline, MAXLINE);
		sigqueue(compute, SIGUSR1, val);
		wait(NULL);
		close(contfd);
		exit(EXIT_SUCCESS);
	}
	return 0;
}

void cpbuf(char sendline[MAXLINE], char *buf)
{
	int i;
	int len = strlen(buf);
	for (i = 0; i < len; i++)
	{
		sendline[i] = buf[i];
	}
	sendline[i] = '\0';
}
