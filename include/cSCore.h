
/****************************************************************/
/*		FileName: cSCore.h				*/
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
#include"stdstructs.h"
#include"cUserHandler.h"
#include"cChannelHandler.h"
#include"errhandle.h"
#include"cswrapper.h"
#include"getarg.h"
#ifndef CORE_MODULE
#define CORE_MODULE
class cSCore
{
private:
int Port;
cUserHandler *UserHandler;
cChannelHandler *ChannelHandler;
cswrapper *Socket;
cERR *ErrorHandler;
//Functions For The parsecmd
void JoinChannel(int usersfd,char *buffer);
void LeaveChannel(int usersfd,char *buffer);
void Message(int usersfd,char *buffer);
void ChangeName(int usersfd,char *buffer);
void Emote(int usersfd,char *buffer);
void ChannelList(int usersfd);
void UserList(int usersfd, char *buffer);
void SendMOTD(int usersfd);
void SetTopic(int usersfd,char *buffer);
void GetTopic(int usersfd,char *buffer);
public:
bool enabled;
void runcore(void);
void parsecmd(int usersfd,char *buffer);
cSCore(int port,cswrapper *Sock,cUserHandler *Uh,cChannelHandler *Ch,cERR *Errh)
{
	Port = port;
	Socket = Sock;
	UserHandler = Uh;
	ChannelHandler = Ch;
	ErrorHandler = Errh;
	enabled = true;
};
~cSCore()
{

};

};
#endif
