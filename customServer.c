#include "unp.h"
#include "sqStack.h"
#include <pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>

void init();
void doit(int sockfd);
void inter2client(int);
void checkHeart(int);
int isOperator(char);
int calcu(int, int, char);
void printPcontent(char *pcontent, int length);
void calPrefixExpression(char *expression, int recvLength, char *result);
int BtoI(char *);
void ItoB(int, char*);
int make_socket_non_blocking(int sfd);

int lifetime = 10;

#define heartLength 6
#define MAXEVENTS 64

char heartPackage[6];
char messageWrapper[6];

int main(int argc, char *argv[])
{
	int listenfd, connfd;
	socklen_t clilen;
	int efd;
	struct epoll_event event;
	struct epoll_event *events;
	
	struct sockaddr_in cliaddr, servaddr;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	
	Listen(listenfd, LISTENQ);
	
	make_socket_non_blocking(listenfd);

	efd = epoll_create(10);
	if(efd == -1)
	{
		unix_error("epoll_create");
	}
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event) == -1)
	{
		unix_error("epoll_ctl");
	}
	
	events = calloc(MAXEVENTS, sizeof (event));
	
	init();

	for( ; ; )
	{
		int n, i;
		
		n = epoll_wait(efd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++)
		{
			if ((events[i].events & EPOLLERR) ||
				(events[i].events & EPOLLHUP) ||
				(!(events[i].events & EPOLLIN)))
			{
				fprintf(stderr, "epoll error: socket close\n");
				Close(events[i].data.fd);
				continue;
			}

			else if (listenfd == events[i].data.fd)
			{
				while (1)
				{
					clilen = sizeof(cliaddr);
					printf("waiting..\n");
					connfd = accept(listenfd, (SA *) &cliaddr, &clilen);
					if (connfd == -1)
					{
						if ((errno == EAGAIN) ||
							(errno == EWOULDBLOCK))
						{
							break;
						}
						else
						{
							unix_error("accept");
						}
					}
					printf("connect...\n");
					make_socket_non_blocking(connfd);

					event.data.fd = connfd;
					event.events = EPOLLIN | EPOLLET;
					if ((epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event)) == -1)
						unix_error("epoll_ctl");					
					doit(connfd);
				}
				printf("jump of for?");
				continue;
			}
			else 
			{
				doit(events[i].data.fd);
			}
		}
	}
}

void init()
{
	char heartPcontentLength[4],msgPcontentLength[4];
	int heartPlength = 1, msgPlength = 4;
	
	memset(heartPackage, 0, heartLength);
	memset(messageWrapper, 0, 6);
	
	ItoB(heartPlength, heartPcontentLength);
	ItoB(msgPlength, msgPcontentLength);

	heartPackage[0] = 'p';
	messageWrapper[0] = 'p';
	heartPackage[5] = 'h';
	messageWrapper[5] = 'm';

	memcpy(&heartPackage[1], heartPcontentLength, 4);
	memcpy(&messageWrapper[1], msgPcontentLength, 4);
	
	//printPcontent(heartPackage, 6);
	//printPcontent(messageWrapper, 6);
}

/**
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

}
**/

void doit(int sockfd)
{
	inter2client(sockfd);
}

void inter2client(int sockfd)
{
	char phead;
	char ptype;
	char pcontent[MAXLINE];
	int plength;
	char lengthByte[4];
	char result[4];
	char replyline[MAXLINE];
	int rc;

	char discardPackage[18];
	
	memcpy(replyline, messageWrapper, 6);

	while (readn(sockfd, &phead, 1) > 0)
	{
		memset(&replyline[6], 0, MAXLINE - 6);
		
		if(phead == 'G')
		{
			readn(sockfd, discardPackage, 17);
			return ;
		}
		if(phead != 'p')
		{
			printf("wrong package!\n");
			return ;
		}

		rc = readn(sockfd, lengthByte, 4);
		if (rc != 4)
		{
			printf("wrong lengthByte!\n");
			return ;
		}
		plength = BtoI(lengthByte);
		
		rc = readn(sockfd, &ptype, 1);
		if (rc != 1)
		{
			printf("wrong ptype!\n");
			return ;
		}
		if (ptype == 'h')
		{
			lifetime = 5;
			//设置与心跳线程的共享变量
		} else if (ptype == 'm')
		{
			rc = readn(sockfd, pcontent, plength);
			if( rc != plength)
			{
				printf("package loss content!\n");
				return ;
			}
			printPcontent(pcontent, plength);
			calPrefixExpression(pcontent, plength, result);
			memcpy(&replyline[6], result, 4);
			printPcontent(replyline, 10);
			if( writen(sockfd, replyline, 10) < 0)
			{
				printf("client is close!");
				return ;
			}
		} else
		{
			printf("wrong package!\n");
			return ;
		}
	}	
	printf("client is close--inter\n");
	return ;
	//shutdown(sockfd, 2);
}

void checkHeart(int sockfd)
{
	while(lifetime > 0)
	{
		sleep(1);
		printf("lifetime = %d\n", lifetime);
		if (writen(sockfd, heartPackage, heartLength) < 0)
		{
			printf("client is close --heart\n");
			break;
		}
		lifetime --;
	}
	printf("shutdown client\n");
	//shutdown(sockfd, 2);
}

void printPcontent(char *pcontent, int length)
{
	int i ;
	for (i = 0; i < length; i++)
	{
		printf("%d ",pcontent[i]);
	}
	printf("\n");
}

void calPrefixExpression(char *expression, int recvLength, char *resultByte)
{	
	int i;
	char numstr[4];
	int num;
	int leftNum, rightNum;
	int result;

	sqStack s;
	
	initStack(&s);

	clearStack(&s);	
	for(i = 0; i < recvLength; i ++)	
		printf("%d ", expression[i]);
	printf("\n");

	for(i = recvLength - 1; i >= 0;)
	{
		if(isOperator(expression[i]))
		{
			pop(&s, &leftNum);
			pop(&s, &rightNum);
			result = calcu(leftNum, rightNum, expression[i]);
			printf("%d %c %d\n", leftNum, expression[i], rightNum);
			push(&s, result);
			i--;	
		}
		else
		{
			i -= 3;
			memcpy(numstr, &expression[i], 4);
			num = (BtoI(numstr));
			printf("%d\n", num);
			push(&s, num);	
			i--;
		}
	}
	pop(&s, &result);
	ItoB(result, numstr);
	printf("结果为: %d\n", result);
	memcpy(resultByte, numstr, 4);
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

int make_socket_non_blocking(int sfd)
{
	int flags, s;
	
	flags = fcntl(sfd, F_GETFL, 0);
	if(flags == -1)
	{
		unix_error("fcntl get");
	}
	
	flags |= O_NONBLOCK;
	s = fcntl(sfd, F_SETFL, flags);
	if (s == -1)
	{
		unix_error("fcntl set");
	}
		
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
