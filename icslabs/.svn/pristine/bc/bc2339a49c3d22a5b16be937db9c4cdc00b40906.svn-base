/*
 * proxy.c - ICS Web proxy
 * ID: 520030910034
 * Name: Xue Songtao
 */

#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Function prototypes
 */
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
void *thread(void *addr);
void proxy(int connfd,struct sockaddr_in *sockaddr);
ssize_t Rio_readn_w(int fd, void *ptr, size_t nbytes);
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n);
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen);

/* 
 * define a structure to store address and descriptor
 */
typedef struct addr_item{
    int connfd;
    struct sockaddr_in client_addr;
} addr_item_t;

/*
 * semaphores to ensure thread safe
 */
sem_t log_mutex,web_mutex;

/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    int listen_fd;
    socklen_t client_len = sizeof(struct sockaddr_in);
    pthread_t tid;
    addr_item_t *addr;

    /* Check arguments */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    /* ignore signal SIGPIPE */
    Signal(SIGPIPE, SIG_IGN);

    /* init semaphores */
    Sem_init(&log_mutex, 0, 1);
    Sem_init(&web_mutex, 0, 1);

    /* listen port */
    listen_fd = Open_listenfd(argv[1]);
    while(1)
    {
        addr = Malloc(sizeof(addr_item_t));
        addr->connfd = Accept(listen_fd,(SA *)&(addr->client_addr),&client_len);
        Pthread_create(&tid,NULL,thread,addr);
    }
    
    /* free resource */  
    Close(listen_fd);

    exit(0);
}

/*
 * thread function
 */
void *thread(void *addr)
{
    /* detach thread */
    Pthread_detach(Pthread_self());
    addr_item_t *my_addr = (addr_item_t*)addr;
    proxy(my_addr->connfd,&(my_addr->client_addr));
    Close(my_addr->connfd);
    Free(my_addr);
    return NULL;
}

/*
 * proxy function
 */
void proxy(int connfd,struct sockaddr_in *sockaddr)
{
    int clientfd;
    char buf[MAXLINE], request_header[MAXLINE];
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];
    rio_t conn_rio,client_rio;
    size_t size = 0,len = 0,n = 0;

    Rio_readinitb(&conn_rio,connfd);
    if (Rio_readlineb_w(&conn_rio, buf, MAXLINE) == 0) {
        app_error("read request line error\n");
        return;
    }
    if (sscanf(buf, "%s %s %s", method, uri, version) != 3) {
        app_error("get parameters error\n");
        return;
    }
    if (parse_uri(uri, hostname, pathname, port) != 0 ) {
        app_error("parse uri error\n");
        return;
    }

    P(&web_mutex);
    if ((clientfd = Open_clientfd(hostname,port)) < 0) {
        V(&web_mutex);
        fprintf(stderr, "connect server error\n");
        return;
    }
    V(&web_mutex);
    Rio_readinitb(&client_rio,clientfd);

    sprintf(request_header, "%s /%s %s\r\n", method, pathname, version);
    while ((n = Rio_readlineb_w(&conn_rio, buf, MAXLINE)) != 0)
    {
        if(!strncasecmp(buf, "Content-Length", 14))
            sscanf(buf+15, "%zu", &len);
        sprintf(request_header, "%s%s", request_header, buf);
        if(!strncmp(buf, "\r\n", 2)) break;  
    }

    Rio_writen(clientfd, request_header, strlen(request_header));
    for (int i = 0; i < len; i++) 
    {
        if (Rio_readnb_w(&conn_rio, buf, 1) == 0)
            break;
        Rio_writen_w(clientfd, buf, 1);
    }

    while ((n = Rio_readlineb_w(&client_rio,buf,MAXLINE)) != 0)
    {
        size += n;
        if(!strncasecmp(buf, "Content-Length: ", 14))
            sscanf(buf+15, "%zu", &len);
        Rio_writen_w(connfd, buf, strlen(buf));
        if(!strncmp(buf, "\r\n", 2)) break;
    }
    for(int i = 0; i < len; i++)
    {
        if (Rio_readnb_w(&client_rio, buf, 1) == 0) 
            break;
        size++;
        Rio_writen_w(connfd, buf, 1);
    }

    if (size > 0)
        format_log_entry(buf,sockaddr,uri,size);

    P(&log_mutex);
    printf("%s\n", buf);
    V(&log_mutex);

    Close(clientfd);
}


/*
 * wrapper of rio_readn()
 */
ssize_t Rio_readn_w(int fd, void *ptr, size_t nbytes)
{
    ssize_t n;
  
    if ((n = rio_readn(fd, ptr, nbytes)) < 0) {
        fprintf(stderr, "rio_readn error\n");
        return 0;
    }

    return n;
}

/*
 * wrapper of rio_writen()
 */
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n)
{
    ssize_t c;

    if ((c = rio_writen(fd, usrbuf, n)) != n) {
        fprintf(stderr, "rio_writen error\n");
        return 0;
    }
    return c;
}

/*
 * wrapper of rio_readnb()
 */
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n)
{
    ssize_t c;

    if ((c = rio_readnb(rp, usrbuf, n)) < 0) {
        fprintf(stderr, "rio_readnb error\n");
        return 0;
    }

    return n;
}

/*
 * wrapper of rio_readlineb()
 */
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen)
{
    ssize_t n;

    if ((n = rio_readlineb(rp, usrbuf, maxlen)) < 0) {
        fprintf(stderr, "rio_readlineb error\n");
        return 0;
    }
    return n;
}

/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0) {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':') {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    } else {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL) {
        pathname[0] = '\0';
    }
    else {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    char host[INET_ADDRSTRLEN];

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    if (inet_ntop(AF_INET, &sockaddr->sin_addr, host, sizeof(host)) == NULL)
        unix_error("Convert sockaddr_in to string representation failed\n");

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %s %s %zu", time_str, host, uri, size);
}
