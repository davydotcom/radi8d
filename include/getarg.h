
/****************************************************************/
/*		FileName: getarg.h				*/
/*		Author: David Estes(Chrono)			*/
/*		CopyRight 2004,2005 David Estes			*/
/*		Under the GPL License				*/
/*		FOR NON-COMMERCIAL USE ONLY			*/
/****************************************************************/
//====================
//Revisement History |
//====================
//
//
//
//====================
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#ifndef GETARG_H
#define GETARG_H
char *GetArg(int arg,char *ArgString,char Delim);
#endif
//DESCRIPTION OF FILE
/*



char *GetArg(int arg,char *ArgString,char Delim)
------------------------------------------------
This function is used to get a part of a delimited string. For Example
Say you had the string "abc:123:hello:world"
What if you wanted to get the 3rd argument in the string.
You would simply type this;

char *value = GetArg(2,"abc:123:hello:world",':');
now value should contain the word "hello"
If youwonder why I passed the argnum as 2 rather than 3 since I needed the 3rd arg it is because the first arg actually is 0. This function may prove very useful in gathering the data sent from a remote connection to the server.

*/
