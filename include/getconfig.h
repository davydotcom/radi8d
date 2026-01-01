
/****************************************************************/
/*		FileName: getconfig.h				*/
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
#include<string.h>
#include<stdlib.h>
#ifndef GETCONFIG_H
#define GETCONFIG_H
#define CONFIGFILE "radi8d.cfg"
#define GC(a) GetConfigOption(CONFIGFILE,a)
#define GI(a) atoi(GetConfigOption(CONFIGFILE,a))
char *GetConfigOption(char *configfile,char *option);
#endif
