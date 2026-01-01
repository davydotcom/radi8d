
/****************************************************************/
/*		FileName: errhandle.h				*/
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
#include"ansicolor.h"
#ifndef ERRORCLASS
#define ERRORCLASS
#define ERRORFILELOCATION "data.err"
#define dbg(a,b,c) ErrorHandler->ProduceError(a,b,c)
#define info(a,b) ErrorHandler->ProduceError(0,a,b)
class cERR
{
private:
char *logfile;
char *errfile;
public:
int memusage;
int maxmemusage;
bool isANSI;
bool islog;
bool enabled;
int EnableDebug(bool log);
int ProduceError(int errnum,char *function,char *reason);
int DisableDebug(void);

int SetLogFile(char *file);
int SetErrFile(char *file);
cERR(char *file);
~cERR()
{
	if(logfile != NULL)
		delete [] logfile;
	if(errfile != NULL)
		delete [] errfile;
};
};
#endif
