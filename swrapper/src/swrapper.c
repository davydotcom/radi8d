#include "swrapper.h"

#define 	USE_TCP
#define		USE_IPV4
#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 255
#endif
#ifdef USE_IPV4

#ifdef USE_TCP

/*
s_connect()
Creates a socket and connects it to a host. Also checks host to see whether it is a hostname or IP address, and converts it accordingly.
*/
int s_connect (char *host, int port) {

	/*Variable initialization*/
	struct sockaddr_in dst;
	int len, sfd;
	int family = AF_INET;
	struct hostent *addr;
	char str[INET6_ADDRSTRLEN]; /*Use the largest of the two*/
	sprintf(str, "%s", host);

	/*If it is a hostname*/
	if (inet_pton(family, str, &dst.sin_addr) <= 0) {
		bzero(&str, sizeof(str));
		addr = gethostbyname(host);
		
		inet_ntop(	family,
				addr->h_addr,
				str,
				sizeof(str)	);
	
		/*Now that we definately have an IP in there...*/
		inet_pton(family, str, &dst.sin_addr);
	}

	/*Setup sockaddr structure*/
	dst.sin_family = family;
	dst.sin_port = htons(port);
	len = sizeof(dst);

	/*Create socket & err check*/
	sfd = socket(family, SOCK_STREAM, 0);
	if (sfd < 0) {
		perror("tcp_connect >> socket()");
		return -1;
	}

	/*Connect socket & err check*/
	if ((connect(sfd, (struct sockaddr *)&dst, len)) < 0) {
		perror("tcp_connect >> connect()");
		return -1;
	}

	return sfd;
} 


/*
s_serve()
Creates a listening TCP socket, and returns the file descriptor.
*/
int s_serve (int port) {

	/*Initialize Variables*/
	struct sockaddr_in host;
	int sfd, t;
	int family = AF_INET;

	/*Create socket & err check*/
	sfd = socket(family, SOCK_STREAM, 0);
	if (sfd < 0) {
		perror("s_serve >> socket()");
		return -1;
	}

	/*Setup sockaddr structure*/
	host.sin_family = family;
	host.sin_port = htons(port);
	host.sin_addr.s_addr = INADDR_ANY;

	/*Bind socket to port & err check*/
	t = bind(sfd, (struct sockaddr *)&host, sizeof(host));
	if (t < 0) {
		perror("s_serve >> bind()");
		return -1;
	}

	/*Set socket to listen for incoming connections*/
	t = listen(sfd, CONNMAX);
	if (t < 0) {
		perror("s_serve >> listen()");
		return -1;
	}
	
return sfd;
}


/*
s_write()
Writes given data to specified socket and checks to make sure it all was sent.
*/
int s_write (int sfd, char *str,int len) {
	int x, t;
	for (x = 0; x <= (len-1); x += 0) {
		t = 0;
		t = write(sfd, str, len); 
		if (t < 0) {
			perror("s_write >> write()");
			return -1;
		}
		x += t;
	}
	return 0;
}
#endif

#ifdef USE_UDP

/*
TODO
Fill all these in with UDP code
*/
int s_connect(char *host, int port) { }
int s_serve(int port) { }
int s_write(int sfd, char *str,int len) { }
#endif

#endif /*End ifdef USE_IPV4*/


/*
TODO
Add IPV6 Code
*/


