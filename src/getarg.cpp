#include "getarg.h"

char *GetArg(int arg,char *ArgString,char Delim)
{
	if(ArgString == NULL)
		return NULL;

	char *returnarg;
	int x;
	int *argnum = new int;
	int *DelimPos1 = new int;
	int *DelimPos2 = new int;
	*DelimPos1 = 0;
	*DelimPos2 = 0;
	if(arg == 0)
	{
		for(x=0;ArgString[x+1] != Delim && ArgString[x+1] != '\0';x++){}
		if(ArgString[x] == '\0')
		{
			returnarg = new char[strlen(ArgString)+1];
			bzero(returnarg,strlen(ArgString)+1);
			strcpy(returnarg,ArgString);
		}
		else
		{
			returnarg = new char[x+2];
			bzero(returnarg,x+2);
			memcpy(returnarg,ArgString,(x)+1);
			returnarg[x+1] = '\0';
		}
	}
	else
	{
		*argnum = -1;
		for(x=0;ArgString[x] != '\0';x++)
		{
			if(ArgString[x] == Delim)
			{
				*DelimPos1 = *DelimPos2;
				*DelimPos2 = x;
				(*argnum)++;
			}
			if(*argnum == arg)
				break;
		}
		if(ArgString[x] == '\0')
		{
			*DelimPos1 = *DelimPos2;
			*DelimPos2 = x;
			(*argnum)++;
		}
		if(*DelimPos2 <= *DelimPos1)
		{
			delete DelimPos1;
			delete DelimPos2;
			delete argnum;
			
			return NULL;
		}
		if(*argnum == arg && (*DelimPos2) > ((*DelimPos1)+1))
		{
			char *temp = &ArgString[(*DelimPos1)+1];
			int len = (*DelimPos2)-(*DelimPos1)-1;
			returnarg = new char[len+1];
			bzero(returnarg,len+1);
			memcpy(returnarg,temp,len);
			returnarg[len] = '\0';
		}
		else
			returnarg = NULL;
	}

	delete DelimPos1;
	delete DelimPos2;
	
	delete argnum;
	return returnarg;
}
