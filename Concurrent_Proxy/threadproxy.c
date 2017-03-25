/*
 * proxy.c - A Simple  Concurrent Web proxy
 *
 * Course Name: 14:332:456-Network Centric Programming
 * Assignment 3
 * Student Name: Akhilesh bondlela
 *
 * IMPORTANT: Give a high level description of your code here. You
 * must also provide a header comment at the beginning of each
 * function that describes what that function does.
 *
 * The code creates a socket to act as a server to the browser. It creates a
 * listening port to which the browser can connect. The browser connects to the
 * port on the proxy and creates a socket. The socket then recieves a request
 * from the browser and parses it to obtain the necessary items, such as url,
 * port number, path, etc. The proxy then takes the parsed items and recreates
 * another GET request to send to a web server. We take the port number(default 80)
 * and the address and connect through socket to the webserver. The proxy then
 * then sends the modified GET request to the web server. It then reads the
 * response of the web server, and passes the response to the web browser.
 */

#include "csapp.h"

#include <pthread.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Function prototypes
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, int size);
void* process_data(void* arg);
int new_open_clientfd(char *hostname, char *portnum);

/*
* Global Variables
*/
struct sockaddr_in tmp_client_addr;
pthread_mutex_t logfile_mutex = PTHREAD_MUTEX_INITIALIZER;


/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{

        //Managing signals
        signal(SIGPIPE, SIG_IGN);

        /* Check arguments */
        if (argc != 2) {
                fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
                exit(0);
        }


        // socket

        int s=Socket(AF_INET, SOCK_STREAM, 0);

        // bind

        struct sockaddr_in servaddr;
        bzero(&servaddr,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
        servaddr.sin_port=htons(atoi(argv[1]));

        Bind(s, (struct sockaddr*) &servaddr, sizeof(servaddr));

        // listen

        Listen(s, 5);

        while(1) {

                // accept
                struct sockaddr_in client_addr;
                int client_size = sizeof(client_addr);
                int browserfd = Accept(s, (struct sockaddr*) &client_addr, &client_size);

                tmp_client_addr = client_addr;

                int *tmp = malloc(sizeof(tmp));
                *tmp = browserfd;
                pthread_t thread;
                pthread_create(&thread, NULL, process_data, tmp);
                pthread_detach(thread);

        }

        exit(0);
}


/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), and the size in bytes
 * of the response from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, int size)
{
        time_t now;
        char time_str[MAXLINE];
        unsigned long host;
        unsigned char a, b, c, d;

        /* Get a formatted time string */
        now = time(NULL);
        strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

        /*
         * Convert the IP address in network byte order to dotted decimal
         * form. Note that we could have used inet_ntoa, but chose not to
         * because inet_ntoa is a Class 3 thread unsafe function that
         * returns a pointer to a static variable (Ch 13, CS:APP).
         */
        host = ntohl(sockaddr->sin_addr.s_addr);
        a = host >> 24;
        b = (host >> 16) & 0xff;
        c = (host >> 8) & 0xff;
        d = host & 0xff;


        /* Return the formatted log entry string */
        sprintf(logstring, "%s: %d.%d.%d.%d %s %d", time_str, a, b, c, d, uri, size);
}

/*
 * ts_open_clientfd - Thread safe version of open_clientfd.
 */
int new_open_clientfd(char *hostname, char *portnum)
{
	int clientfd;
	int hp;

	/* Initialize addrinfo struct to use getaddrinfo */
	struct addrinfo hints;
	struct addrinfo *results, *rp;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_STREAM;

	/* Fill in the server's IP address and port */
	if ((hp = getaddrinfo(hostname, portnum, &hints, &results) != 0))
		return -2; /* check h_errno for cause of error */
	for(rp = results; rp != NULL; rp = rp ->ai_next) {
		if ((clientfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0)
		continue;

		/* Establish a connection with the server */
		if (connect(clientfd, rp->ai_addr, rp->ai_addrlen) < 0){
			close(clientfd);
			continue;
		}

		break;
	}
	if(rp == NULL){
		fprintf(stderr, "Could not connect\n");
		return -1;
	}
	freeaddrinfo(results);
	return clientfd;
}

void* process_data(void* arg)
{
  struct sockaddr_in client_addr = tmp_client_addr;
  int browserfd = *((int*)arg);
  free(arg);

        // Read request
        char reqbuf[10000];
        bzero(&reqbuf, sizeof(reqbuf));
        Read(browserfd, reqbuf, sizeof(reqbuf));

        // Makes sure that the request is a GET request
        if(strncmp(reqbuf,"GET",3) != 0) {
                return NULL;
        }

        // Parse request from browser
        char* reqp = reqbuf;
        char* firstline = strsep(&reqp, "\r");
        char* portnum = strdup(firstline);
        char* hostname = strdup(firstline);
        char* sitereq = strdup(firstline);
        char* protocol = strdup(firstline);

        // Site extension for the request
        strsep(&sitereq, "/");
        strsep(&sitereq, "/");
        strsep(&sitereq, "/");
        sitereq = strsep(&sitereq, " ");

        // Port number if specified
        strsep(&portnum, ":");
        strsep(&portnum, ":");
        if(portnum == NULL) {
                portnum = "80";
        }
        else{
                portnum = strsep(&portnum, "/");
        }

        // URL or hostname
        strsep(&hostname, "/");
        strsep(&hostname, "/");
        hostname = strsep(&hostname, ":");
        hostname = strsep(&hostname, "/");

        // Protocol in the request
        strsep(&protocol, "/");
        strsep(&protocol, "/");
        strsep(&protocol, " ");

        // Create the client
        int port = atoi(portnum);

        int sitefd = open_clientfd(hostname,port);

        // Write request to the socket communicating with web server
        char getreq[MAXBUF];
        bzero(&getreq, sizeof(getreq));
        sprintf(getreq, "GET /%s %s\nHost: %s\n\n", sitereq, protocol, hostname);
        Write(sitefd, getreq, strlen(getreq));

        // Read reply from the web server into readrep then write the response on to the client server
      	char readrep[MAXBUF];
      	bzero(&readrep, sizeof(readrep));
      	int readval = 0;
      	int len
         = 0;
      	while((readval = Read(sitefd, readrep, sizeof(readrep))) != (size_t)NULL){
      		Write(browserfd, readrep, readval);
      		len = len + readval;
      	}

        //Opens logfile file pointer
        int logfile = open("proxy.log", O_APPEND | O_CREAT | O_RDWR, S_IRWXU, 0777);

        char logstring[500];
        char sitelong[500];
        sprintf(sitelong, "http://%s/%s", hostname, sitereq);
        bzero(logstring, sizeof(logstring));

        //Creates and formats an entry for the log
        pthread_mutex_lock(&logfile_mutex);
        format_log_entry(logstring, &client_addr, sitelong, len);
        write(logfile, logstring, strlen(logstring));
        write(logfile, "\n", 2);


        //Close file pointers

        Close(logfile);
        pthread_mutex_unlock(&logfile_mutex);
        // close socket
        Close(browserfd);
        Close(sitefd);
        pthread_exit(0);
}