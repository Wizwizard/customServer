#include "unp.h"
#include "sqStack.h"
#include <pthread.h>

void doit(int sockfd);
void inter2client(int);
void checkHeart(int);
int isOperator(char);
int calcu(int, int, char);
int BtoI(char *);
void ItoB(int, char*);

int lifetime = 10;

int main(int argc, char *argv[])
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	
	Listen(listenfd, LISTENQ);

	for( ; ; )
	{
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		
		if ( (childpid = Fork()) == 0)
		{
			Close(listenfd);
			doit(connfd);
			exit(0);
		}
		Close(connfd);
	}
}
void doit(int sockfd)
{
	pthread_t idInter, idHeart;
	int ret;

	ret = pthread_create(&idInter, NULL, (void *)inter2client, (void *)sockfd);
	if (ret  != 0)
		unix_error("create inter");
	
	ret = pthread_create(&idHeart, NULL, (void *)checkHeart, (void *)sockfd);
	if (ret != 0)
		unix_error("create heart");

	pthread_join(idInter, NULL);
}
void inter2client(int sockfd)
{
	printf("enter in inter\n");
	char readline[MAXLINE];
	int recvLength;
	
	int i;
	char numstr[4];
	int num;
	int leftNum, rightNum;
	int result;

	sqStack s;
	
	initStack(&s);

	while(1)
	{
	clearStack(&s);

	recvLength = read(sockfd , readline, MAXLINE);
	lifetime = 10;
	if(recvLength <= 0)
	{
		printf("client is close!\n");
		exit(0);
	}
	
	printf("recvlength = %d\n", recvLength);
	for(i = 0; i < recvLength; i ++)
		printf("%d ", readline[i]);
	printf("\n");

	for(i = recvLength - 1; i >= 0;)
	{
		if(isOperator(readline[i]))
		{
			pop(&s, &leftNum);
			pop(&s, &rightNum);
			result = calcu(leftNum, rightNum, readline[i]);
			printf("%d %c %d\n", leftNum, readline[i], rightNum);
			push(&s, result);
			i--;	
		}
		else
		{
			i -= 3;
			memcpy(numstr, &readline[i], 4);
			num = (BtoI(numstr));
			printf("%d\n", num);
			push(&s, num);	
			i--;
		}
	}
	pop(&s, &result);
	ItoB(result, numstr);
	printf("结果为: %d\n", result);
	Writen(sockfd, numstr, 4);
	}
}

void checkHeart(int sockfd)
{
	printf("enter in heart\n");
	while(lifetime > 0)
	{
		sleep(1);
		printf("lifetime = %d\n", lifetime);
		lifetime --;
	}
	printf("shutdown\n");
	shutdown(sockfd, 2);
}

int isOperator(char c)
{
	if (c == '+')
		return 1;
	else if (c == '-')
		return 1;
	else if (c == '*')
		return 1;
	else if (c == '/')
		return 1;
	return 0;
} 

// 用值-结果方法更好
int calcu(int leftNum, int rightNum, char c)
{
	if (c == '+')
		return leftNum + rightNum;
	else if (c == '-')
		return leftNum - rightNum;
	else if (c == '*')
		return leftNum * rightNum;
	else if (c == '/')
		return leftNum / rightNum;
	return 0;
}

int BtoI(char *arry)
{
	return (arry[0] | arry[1]<<8 | arry[2]<<16 | arry[3]<<24);
}

void ItoB(int num, char *arry)
{
	arry[0] = (char) (num & 0xff);
	arry[1] = (char) ((num & 0xff00) >> 8);
	arry[2] = (char) ((num & 0xff0000) >> 16);
	arry[3] = (char) ((num >> 24) & 0xff);
}
