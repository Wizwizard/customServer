#include "unp.h"

void unix_error(char *msg)
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(0);
}

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nread = read(fd, ptr, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else 
				return(-1);
		} else if (nread == 0)
		{
			break;
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft);
}
ssize_t Readn(int fd, void *vptr, size_t n)
{
	ssize_t rc;

	if ( (rc = readn(fd, vptr, n)) < 0)
		unix_error("readn");
	return rc;
}

ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
		
	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ( (nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else 
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
ssize_t Writen(int fd, const void *vptr, size_t n)
{
	ssize_t rc;

	if ( (rc = writen(fd, vptr, n)) < 0)
		unix_error("writen");
	return rc;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char 	c, *ptr;
	
	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ( (rc = read(fd, &c, 1)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		} else if (rc == 0) {
			*ptr = 0;
			return (n-1);
		} else {
			if (errno == EINTR)
				continue;
			return (-1);
		}
	}
	*ptr = 0;
	return (n);
}
ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t rc;

	if ( (rc = readline(fd, vptr, maxlen)) < 0)
		unix_error("readline");
	return rc;
}

int Socket( int family, int type, int protocol)
{
	int rc;
	
	if( (rc = socket(family, type, protocol)) < 0)
		unix_error("socket");
	return rc;
}

void Bind(int sockfd, struct sockaddr *my_addr, int addrlen)
{
	int rc;
		
	if ((rc = bind(sockfd, my_addr, addrlen)) < 0)
		unix_error("Bind error");
}

void Listen(int s, int backlog)
{
	int rc;
	
	if ((rc = listen(s, backlog)) < 0)
		unix_error("Listen error");
}

int Accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
	int rc;

	if ((rc = accept(s, addr, addrlen)) < 0)
		unix_error("Accept error");
	return rc;
}

void Connect(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
	int rc;

	if ((rc = connect(sockfd, serv_addr, addrlen)) < 0)
		unix_error("Conncet error");
}

void Close (int fd)
{
	int rc;
	
	if ((rc = close(fd)) < 0)
	 unix_error("Close error");
}
/*
int inet_pton(int family, const char *strptr, void *addrptr)
{
	if (family == AF_INET)
	{
		struct in_addr in_val;
		
		if (inet_aton(strptr, &in_val))
		{
			memcpy(addrptr, &in_val, sizeof(struct in_addr));
			return (1);
		}
		return (0);
	}
	errno = EAFNOSUPPORT;
	return (-1);
}
*/
int Inet_pton(int family, const char *strptr, void *addrptr)
{
	int rc;

	if ( (rc = inet_pton(family, strptr, addrptr)) < 0)
		unix_error("inet_pton");
	return rc;
}
/*
const char * inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	const u_char *p = (const u_char *) addrptr;
	
	if (family == AF_INET)
	{
		char temp[INET_ADDRSTRLEN];
		snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
		if (strlen(temp) >= len)
		{
			errno = ENOSPC;
			return (NULL);
		}
		strcpy(strptr, temp);
		return (strptr);
	}
	errno = EAFNOSUPPORT;
	return (NULL);
}
*/
const char * Inet_ntop(int family, const void *addrptr, char *strptr, size_t len)
{
	char *rc;
	
	if ( (rc = (char *)inet_ntop(family, addrptr, strptr, len)) == NULL)
		unix_error("inet_ntop");
	return rc;
}

char *Fgets(char *s, int size, FILE *stream)
{
	char *rc;

	if ( (rc = fgets(s,size,stream)) == NULL)
		unix_error("fgets");
	return rc;
}

int Fputs(const char *s, FILE *stream)
{
	int rc;
	if ( (rc = fputs(s,stream)) == EOF)
		unix_error("fputs");
	return rc;
}

pid_t Fork(void)
{
	pid_t rc;

	if ( (rc = fork()) < 0)
		unix_error("fork");
	return rc;
}
