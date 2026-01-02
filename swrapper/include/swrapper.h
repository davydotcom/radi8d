/*
sWrapper Library
Basic networking functions which simplify the task of using sockets.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/*
Used by s_serve for a parameter needed in the listen() function.
Specified how many clients can be connected at once.
*/
#define CONNMAX 100000

int s_connect (char *host,int port);
int s_serve (int port);
int s_write(int sfd, char *str,int len);
