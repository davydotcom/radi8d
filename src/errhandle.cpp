#include"errhandle.h"
int cERR::EnableDebug(bool log)
{
	if(log && logfile == NULL)
	{
		islog = false;
		return -2;
	}
	else
	{
		islog = log;
	}
	if(log)
		isANSI=false;
	enabled = true;
	return 0;
}
int cERR::ProduceError(int errnum,char *function,char *reason)
{
	
	if(!enabled)
		return 0; //DEBUG DISABLED BUT SYSTEM CAN CONTINUE
	if(errfile == NULL)	//IF NO ERRFILE SPECIFIED
		return -1; //ERROR FILE NOT SET
	FILE *ErrorFile = NULL;
	FILE *output = NULL;
	ErrorFile = fopen(errfile,"r"); //OPEN ERR FILE
	if(ErrorFile == NULL)
		return -2; //Error Openning FILE
	if(islog && logfile != NULL)
	{
		output = fopen(logfile,"a");
		if(output == NULL)
		{
			islog = false;
			output = stderr;
		}

	}
	else
	{
		output = stderr;
	}

	char *buffer = new char[512];
	char *errorNum = new char[5];
	int *x = new int;
	bool *complete = new bool;
	*complete = false;
	while(!feof(ErrorFile))
	{

		bzero(errorNum,5);
		bzero(buffer,512);
		if(feof(ErrorFile))
			break;
		fgets(buffer,512,ErrorFile);

		for((*x)=0;buffer[*x] != '\0' && buffer[*x] != ':' && (*x) < 5;(*x)++)
		{

			errorNum[*x] = buffer[*x];
		}

		if(errnum == atoi(errorNum))
		{

			buffer[strlen(buffer)-1] = '\0';
			if(isANSI){
			if(errnum != 0)
			fprintf(output,"%sErr%d in fn %s: %s\n%sReason:%s\n%s",FORERED,errnum,function,&buffer[(*x)+1],FOREGREEN,reason,NORMAL);
			else
				fprintf(output,"%s%sINFORMATIVE in fn %s: %s\n%sReason:%s%s\n",FOREWHITE,BACKBLUE,function,&buffer[(*x)+1],FOREGREEN,reason,NORMAL);
			(*complete)=true;
			}
			else
			{

			fprintf(output,"Err%d in fn %s: %s\nReason:%s\n",errnum,function,&buffer[(*x)+1],reason);
			(*complete)=true;
			}
			break;
		}
	}

	if(!(*complete))
		fprintf(output,"Err %d not Found\n",errnum);
	delete [] buffer;

	delete [] errorNum;


	delete complete;

	delete x;

	if(output != stderr)
		fclose(output);

	fclose(ErrorFile);
//	delete output;

//	delete ErrorFile;
	return 0;

}
int cERR::DisableDebug(void)
{
	enabled = false;
	return 0;
}
int cERR::SetLogFile(char *file)
{
	if(file == NULL)
	{
		return -1; //no filename passed
	}
	if(logfile != NULL)
	{
		char *temp = logfile;
		delete [] temp;
	}
	logfile = new char[strlen(file)+1];
	strcpy(logfile,file);

	return 0;
}
int cERR::SetErrFile(char *file)
{
	if(file == NULL)
	{
		return -1; //no filename passed
	}
	if(errfile != NULL)
	{
		char *temp = errfile;
		delete [] temp;
	}
	errfile = new char[strlen(file)+1];
	strcpy(errfile,file);
	return 0;
}

cERR::cERR(char *file)
{
	isANSI=true;
	memusage = 0;
	maxmemusage = 0;
	islog=false;
	enabled=false;
	errfile = file;
	logfile=NULL;
}
