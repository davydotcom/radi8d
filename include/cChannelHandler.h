
/****************************************************************/
/*		FileName: cChannelHandler.h			*/
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
#include"errhandle.h"
#include"cswrapper.h"
#ifndef cCHANNELHANDLER_H
#define cCHANNELHANDLER_H
class cChannelHandler
{
private:
sChannels* GetChannel(int num);
sBanList* GetBan(int num,char *ChannelName);

public:
	sUserList* GetUser(int num,char *ChannelName);
	int err;
	cswrapper *Socket;
	cUserHandler *UserHandler;
	cERR *ErrorHandler;
	sChannels *ChannelList;
	int maxusers;
	int maxchannels;
	int numchannels;
	sChannels *ptrLastChannel;
	int defaultcodec;

	//Functions
	bool CreateChannel(char *ChannelName,char *Password,char *Topic,int chantype,bool ReservedOnly, bool RequireSpeakPermissions);
	bool RemoveChannel(char *ChannelName);
	int JoinChannel(int usersfd,char *ChannelName,char *Password);
	bool LeaveChannel(int usersfd,char *ChannelName);
	int IsAChannel(char *ChannelName);
	int IsInChannel(int usersfd,char *ChannelName);
	bool SendToChannel(int usersfd,char *ChannelName,char *msg);
	char* GetUserList(char *ChannelName,int num);
	char* GetChannelList(int num);
	bool BanUser(int fromsfd,char *ChannelName,char *username,char *hostname,int minutes,char *reason);
	bool UnBanUser(int fromsfd,char *ChannelName,char *username);
	bool KickUser(int fromsfd,char *ChannelName, int usersfd,char *reason);
	char* GetTopic(char *ChannelName);
	bool SetTopic(int usersfd,char *ChannelName,char *Topic);
	bool SetPassword(int usersfd,char *ChannelName,char *Password);
	bool SetChanType(int usersfd,char *ChannelName,int chantype);
	bool SetCodec(int usersfd,char *ChannelName,int codec);
	bool SetMaxUsers(int usersfd,char *ChannelName,int maxusers);
	bool SetReservedOnly(int usersfd,char *ChannelName,bool ReservedOnly);
	bool SetRequireSpeakPermissions(int usersfd,char *ChannelName,bool RequireSpeakPermissions);
	bool SetUserPermissions(int fromsfd,char *ChannelName, int usersfd,int permissions);
	void UserDisconnected(int usersfd);
	void UserNameChange(int usersfd,char *OldUserName);
	int GetNumberUsers(char *ChannelName);
	
	cChannelHandler(unsigned int maxchans,unsigned int maxusrs,unsigned short int defcodec,cUserHandler *userdata,cswrapper *Wrapper,cERR *errhandle)
	{
		maxchannels = maxchans;
		numchannels = 0;
		maxusers = maxusrs;
		defaultcodec = defcodec;
		UserHandler = userdata;
		ErrorHandler = errhandle;
		Socket = Wrapper;
		ChannelList = NULL;
		ptrLastChannel = NULL;
	};
	~cChannelHandler() //Destructor
	{
		ErrorHandler = NULL;
		UserHandler = NULL;
		if(ChannelList != NULL)
			delete ChannelList;

	};
};

#endif
