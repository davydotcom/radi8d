#include"errhandle.h"
#include"cswrapper.h"
#include"cSCore.h"
#include"signal.h"
#include"stdstructs.h"
#include"cChannelHandler.h"
#include"cUserHandler.h"//this is a comment

cChannelHandler *ChannelHandler;
cSCore *MainCore;
cUserHandler *UserHandler;
cERR *ErrorHandler;
cswrapper *Socket;
int port;
bool fail;
bool DAEMONMODE;
bool DEBUG;
bool LOG;
bool HandlersLoaded;
bool USE_SSL;
char *SSL_CERT;
char *SSL_KEY;
void sig_broken_pipe(int a)
{
	printf("Broken Pipe Detected...\n");
}
void sig_shutdown(int a)
{
	printf("Shutting Down Server\n");
	MainCore->enabled = false;
	Socket->cs_connect("localhost",port);
}
void LoadHandlers(void);
void UnloadHandlers(void);


void LoadHandlers(void)
{
//*******************INIT MAIN CLASSES***************
signal(SIGPIPE,sig_broken_pipe);
signal(SIGINT,sig_shutdown);

DAEMONMODE=false;
DEBUG=false;
LOG=false;
HandlersLoaded = false;
USE_SSL=false;
SSL_CERT=NULL;
SSL_KEY=NULL;
ErrorHandler = new cERR(GC("ERRORFILE"));
char *DebugMode = GC("DEBUG");
if(!strcmp(DebugMode,"ON"))
{
	DEBUG=true;
	ErrorHandler->EnableDebug(0);
	info("main()","Debug Enabled");
}
delete [] DebugMode;
Socket = new cswrapper();
info("main()","Loaded Socket Handler");
char *UserFile = GC("USERFILE");
char *ProfileLocation = GC("PROFILELOCATION");
char *MaxUsers = GC("MAXUSERS");
char *MaxFileShares = GC("MAXFILESHARES");
char *MaxPROFileSize = GC("MAXPROFILESIZE");

int iMaxUsers = atoi(MaxUsers);
int iMaxFileShares = atoi(MaxFileShares);
int iMaxFileTransferRate = 58;
int iMaxProfileSize = atoi(MaxPROFileSize);


delete [] MaxUsers;
delete [] MaxFileShares;
delete [] MaxPROFileSize;
UserHandler = new cUserHandler(UserFile,ProfileLocation,ErrorHandler,iMaxUsers,iMaxFileShares,iMaxFileTransferRate,iMaxProfileSize,Socket);
info("main()","Loaded UserHandler");
char *MaxChannels = GC("MAXCHANNELS");
char *MaxChanUsers = GC("MAXCHANUSERS");
char *DefaultCodec = GC("DEFAULTCODEC");
int iMaxChannels = atoi(MaxChannels);
int iMaxChanUsers = atoi(MaxChanUsers);
int iDefaultCodec = atoi(DefaultCodec);

delete [] MaxChannels;
delete [] MaxChanUsers;
delete [] DefaultCodec;

ChannelHandler = new cChannelHandler(iMaxChannels,iMaxChanUsers,iDefaultCodec,UserHandler,Socket,ErrorHandler);
info("main()","Loaded ChannelHandler");
port = -1;
char *DaemonMode = GC("DAEMONMODE");


if(!strcmp(DaemonMode,"ON"))
{
DAEMONMODE = true;
}
delete [] DaemonMode;
HandlersLoaded = true;
}
int main(int argc, char *argv[])
{
LoadHandlers();
//=================GET ARGUMENTS=====================
if(argc < 3)
{
invalidarg:
	dbg(1,"main()","Usage: radi8d -p <port> [--ssl] [--cert <file>] [--key <file>]");
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
	if(!strcmp(argv[x],"--ssl"))
	{
		USE_SSL=true;
	}
	if(!strcmp(argv[x],"--cert"))
	{
		if(argc > x+1)
		{
			SSL_CERT = argv[x+1];
			x++;
		}
	}
	if(!strcmp(argv[x],"--key"))
	{
		if(argc > x+1)
		{
			SSL_KEY = argv[x+1];
			x++;
		}
	}
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
	daemon(1,0);
}

// Initialize SSL if requested
if(USE_SSL)
{
	info("main()","Initializing SSL/TLS support");
	if(Socket->init_ssl(SSL_CERT, SSL_KEY) < 0)
	{
		printf("Failed to initialize SSL\n");
		fail = true;
		return 1;
	}
}

//DONE
//===================================================
//*****************START MAIN CODE HERE*******************
char *ThreadedMode = GC("THREADED");

if(!strcmp(ThreadedMode,"OFF"))
{
MainCore = new cSCore(port,Socket,UserHandler,ChannelHandler,ErrorHandler);
info("main()","Running Core!");
MainCore->runcore();
delete  MainCore;
delete [] ThreadedMode;

}



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
