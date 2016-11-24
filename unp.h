#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LISTENQ 10
#define SERV_PORT 9877

#define MAXLINE 8192
#define MAXBUF 8192

typedef struct sockaddr SA;

void str_echo(int);
void str_cli(FILE *, int);

void unix_error(char *msg);

char *Fgets(char *s, int size, FILE *stream);
int Fputs(const char *s, FILE *stream);
ssize_t readn(int filedes, void *buff, size_t nbytes);
ssize_t writen(int filedes, const void *buff, size_t nbytes);
ssize_t readline(int filedes, void *buff, size_t maxlen);
ssize_t Readn(int filedes, void *buff, size_t nbytes);
ssize_t Writen(int filedes, const void *buff, size_t nbytes);
ssize_t Readline(int filedes, void *buff, size_t maxlen);


int Socket(int domain, int type, int protocol);
void Setsockopt(int s, int level, int optname, const void *optval, int optlen);
void Bind(int sockfd, struct sockaddr *my_addr, int addrlen);
void Listen(int s, int backlog);
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);
void Connect (int sockfd, struct sockaddr *serv_addr, int addrlen);
void Close(int fd);

//int inet_pton(int family, const char *strptr, void *addrptr);
int Inet_pton(int family, const char *strptr, void *addrptr);
//const char * inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

pid_t Fork(void);

void sig_child(int signo);
