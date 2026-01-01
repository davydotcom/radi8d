#include"getconfig.h"
void VoidNewLine(char *buff,int size)
{
	for(int x=0;x<size;x++)
	{
		if(buff[x] == '\n')
		{
			buff[x] = '\0';
			break;
		}

	}	
}
char *GetConfigOption(char *configfile,char *option)
{
	FILE *Config = fopen(configfile,"r");
	if(Config == NULL)
		return NULL;
	char *data = new char[50]; 
	memset(data,0,50);
	char *buffer = new char[255];
	while(!feof(Config))	
	{
		bzero(buffer,255);
		fgets(buffer,255,Config);
		VoidNewLine(buffer,255);
		if(feof(Config))
			break;
		if(!memcmp(buffer,option,strlen(option)))
		{
			strcpy(data,&buffer[strlen(option)+1]);
			break;
		}
	}
	if(data[0] == '\0')
	{
		delete [] data;
		data=NULL;
	}
	delete [] buffer;
	fclose(Config);
	return data;
}

