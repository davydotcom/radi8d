#include"errhandle.h"
#include"cswrapper.h"
#include"signal.h"
#include"stdstructs.h"
#include"cChannelHandler.h"
#include"cUserHandler.h"//this is a comment
#include"getarg.h"
cChannelHandler *ChannelHandler;
cUserHandler *UserHandler;
cERR *ErrorHandler;
cswrapper *Socket;
int port;
int nsfd;
int lsfd;
bool fail;
bool DAEMONMODE;
bool DEBUG;
bool LOG;
bool HandlersLoaded;

void LoadHandlers(void);
void UnloadHandlers(void);
/*signal(SIGINT,SIG_IGN) 
{
	printf("Ctrl-C Signal Received..Unloading Server...\n");
	if(HandlersLoaded)
		UnloadHandlers();
}*/
/*
signal(SIGQUIT,SIG_IGN) 
{
	printf("Ctrl-\ Signal Received..Unloading Server...\n");
	if(HandlersLoaded)
		UnloadHandlers();	
}
*/


void LoadHandlers(void)
{
//*******************INIT MAIN CLASSES***************
DAEMONMODE=false;
DEBUG=false;
LOG=false;
HandlersLoaded = false;
ErrorHandler = new cERR(GC("ERRORFILE"));
if(!strcmp(GC("DEBUG"),"ON"))
{
	DEBUG=true;
	ErrorHandler->EnableDebug(0);
	info("main()","Debug Enabled");
}
Socket = new cswrapper();
info("main()","Loaded Socket Handler");
UserHandler = new cUserHandler(GC("USERFILE"),GC("PROFILELOCATION"),ErrorHandler,GI("MAXUSERS"),GI("MAXFILESHARES"),GI("MAXFILETRANSFERRATE"),GI("MAXPROFILESIZE"),Socket);
info("main()","Loaded UserHandler");
ChannelHandler = new cChannelHandler(GI("MAXCHANNELS"),GI("MAXCHANUSERS"),GI("DEFAULTCODEC"),UserHandler,Socket,ErrorHandler);
info("main()","Loaded ChannelHandler");
port = -1;
if(!strcmp(GC("DAEMONMODE"),"ON"))
{
DAEMONMODE = true;
}
HandlersLoaded = true;
}
int main(int argc, char *argv[])
{
LoadHandlers();
//=================GET ARGUMENTS=====================
if(argc < 3)
{
invalidarg:
	dbg(1,"main()","Usage: radi8d -p <port>");
	fail = true;
}
else if(fail == false)
{
for(int x=0;x<argc;x++)
{
	if(!strcmp(argv[x],"-p"))
	{
		if(argc != x)
		{
			port = atoi(argv[x+1]);
			x++;
		}
	}
	if(!strcmp(argv[x],"-d"))
	{
		DAEMONMODE = true;
		if(LOG==false)
		{
			DEBUG=false;
		}
	}
	if(!strcmp(argv[x],"--debug"))
	{
		if(DAEMONMODE == false)
		{
			DEBUG=true;
		}
	}
	if(!strcmp(argv[x],"--log"))
	{
		LOG=true;
		DEBUG=true;
	}	
	if(!strcmp(argv[x],"--nodebug"))
		DEBUG=false;
}
if(port == -1)
	goto invalidarg;

//INIT
if(DEBUG == true)
{
	ErrorHandler->SetLogFile("/var/log/radi8d.log");
	ErrorHandler->EnableDebug(LOG);
	info("main()","Debug Enabled");
}
else
{
	printf("Disabling Debug Mode!\n");
	ErrorHandler->DisableDebug();

}
if(DAEMONMODE == true)
{
	printf("Intializing Daemon Mode Now...Standby.\n");
	//TODO:INSERT CODE TO START DAEMON MODE HERE
}

//DONE
//===================================================
//*****************START MAIN CODE HERE*******************
char *channame = new char[25];
for(int d=0;d<2;d++)
{
for(int x=0;x<ChannelHandler->maxchannels;x++)
{

	sprintf(channame,"%d",x);
	printf("%d:Creating Channel %d name=%s\n",ChannelHandler->numchannels,x,channame);
	ChannelHandler->CreateChannel(channame,"1234","The Ultimate Test",0,false,false);
}
info("main()","Max Amount of Channels Created!!!");
for(int x=(ChannelHandler->maxchannels) - 1 ;x>=0;x--)
{
	sprintf(channame,"%d",x);

	printf("%d:Removing Channel %s\n",ChannelHandler->numchannels,channame);
	ChannelHandler->RemoveChannel(channame);
}

}
//DELETE VARS
delete [] channame;

//DELETE CLASSESs
UnloadHandlers();
return 0;
}
}
void UnloadHandlers(void)
{

delete ChannelHandler;
info("main()","Deleted ChannelHandler");
delete UserHandler;
info("main()","Deleted UserHandler");
delete Socket;
info("main()","Deleted Socket Handler");
delete ErrorHandler;
}
