/*I finally succeded.

Server.c
Code:
*/
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#define version 0x002
#define MYPORT 3490
#define bufsize 128
int main()
{
	WSADATA wsaData;
	SOCKET socketfd,new_fd;
	struct sockaddr_in my_addr,client_sockaddr; 
	char message[bufsize];
	int size;
	if(WSAStartup(MAKEWORD(version,2),&wsaData))
	{
		fprintf(stderr,"\nError intializing winsock library!");
		return 1;
	}
	my_addr.sin_family=AF_INET;
	my_addr.sin_port=htons(MYPORT);
	my_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	memset(&(my_addr.sin_zero),'\0',8);
	
	socketfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
	if(socketfd==INVALID_SOCKET)
	{
		fprintf(stderr,"\nError creating socket!");
		return 1;
	}

	if(bind(socketfd,(const struct sockaddr*)&my_addr,sizeof(struct sockaddr))==SOCKET_ERROR)
	{
		fprintf(stderr,"Error binding");
		return 1;
	}

	if(listen(socketfd,5)==SOCKET_ERROR)
	{
		fprintf(stderr,"Error listening!");
		return 1;
	}
	
	while(1)
	{
		int get; 

		printf("\nWaiting for data on port TCP: %u\n",MYPORT);
		
		size=sizeof(struct sockaddr);
		new_fd=accept(socketfd,(struct sockaddr*)&client_sockaddr,&size);
		if(new_fd==INVALID_SOCKET)
		{
			fprintf(stderr,"\nError accepting connection!");
			return 1;
		}
		
		
		get=recv(new_fd,message,bufsize-1,0);

		if(get==0)
			break;
		else
			if(get==SOCKET_ERROR)
			{
				fprintf(stderr,"\nError receiving data!");
				return 1;
			}
			else
			{
				message[get]=0;
				fprintf(stdout,"%s ",message);
			}

	}
	closesocket(new_fd);
	closesocket(socketfd);
	WSACleanup();
}



/*
Client.c
Code:
*/
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#define version 0x002
#define SERVERPORT 3490
int main()
{
	WSADATA wsaData;
	SOCKET socketfd;
	struct sockaddr_in server_addr; 
	char message[]="Hello FANTA!";
	struct hostent *h;
	if(WSAStartup(MAKEWORD(version,2),&wsaData))
	{
		fprintf(stderr,"\nError intializing winsock library!");
		return 1;
	}

	h=gethostbyname("127.0.0.1");
	server_addr.sin_family=h->h_addrtype;
	server_addr.sin_port=htons(SERVERPORT);
	server_addr.sin_addr.S_un.S_addr=inet_addr(h->h_name);
	memset(&(server_addr.sin_zero),'\0',8);
	
	socketfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);  
	if(socketfd==INVALID_SOCKET)
	{
		fprintf(stderr,"\nError creating socket!");
		return 1;
	}

	if(connect(socketfd,(const struct sockaddr*)&server_addr,sizeof(struct sockaddr)))
	{
		fprintf(stdout,"Could not be connected!");
		return 1;
	}
	if(send(socketfd,message,lstrlen(message),0)==SOC  KET_ERROR)
	{
		fprintf(stderr,"Data could not be send!");
		return 1;
	}

	closesocket(socketfd);
	WSACleanup();

}


/*I manage to get it work accidentaly.
Changing code in server.c (while(1))
if line is:
Code:

h=gethostbyname(INADDR_ANY);

which I thought is a good way to obtain host infos, client simply couldn't connect.
then I try not to use hopstent at all an to try with
Code:

server_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);


but that doesn't work either.

It started working only after I put:
h=gethostbyname("127.0.0.1");

I have no experience nor enough knowledge about networks, but as far as I know IP 127.0.0.1 is IP of my computer
when I'm offline and don't have LAN (in that case 192.168...).
So what is the purpose of INADDR_ANY,it seems that is "0.0.0.0" but that is not problem in server.c
In Beej's tutorial I read
Quote:

bind(), I should mention that some of the process of getting your own IP address and/or port
can can be automated:
my_addr.sin_port = 0; // choose an unused port at random
my_addr.sin_addr.s_addr = INADDR_ANY; // use my IP address
but that makes me problem in client.c

Can you explain me this, or at least recommended some good link.
I'm felling that I'm very close to understand this*/