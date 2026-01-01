#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"getarg.h"
int main()
{
char *buffer = new char[255];
bzero(buffer,255);
strcpy(buffer,"1:This is:what is this:please work:i am done");
printf("buffer = %s\n",buffer);

for(int x=0;x<5;x++)
{
printf("Argument %d = %s\n",x,GetArg(x,buffer,':'));
}
return 0;
}
