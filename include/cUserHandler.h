
/****************************************************************/
/*		FileName: cUserHandler.h			*/
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
#include"getarg.h"
#include"errhandle.h"
#include"cswrapper.h"
#ifndef USERHANDLER_H
#define USERHANDLER_H
class cUserHandler
{
private:
	
	sBanList* GetBan(int num);
	char* HashPassword(char *password);
	bool ValidPassword(char *username, char*password);
	bool GetUserSettings(char *username,int *DefaultPermissions,bool *acceptvoice);
	bool SaveUserSettings(char *username);

public:
	int err;
	int maxusers;
	int numusers;
	int maxfiles;
	
	int MaxFileTransferRate; //0 for unlimited
	int maxprofilesize;
	cERR *ErrorHandler;
	cswrapper *Socket;
	sMainUsers *MainUsers;
	sBanList *BanList;
	char *userfile;
	char *profilelocation;
	//FUNCTIONS
	sMainUsers* GetUser(int num);
	bool AddUser(int usersfd, char *hostname);
	bool RemoveUser(int usersfd);
	int SetName(int usersfd, char *username,char *Password);
	bool SetPermissions(int fromsfd,int usersfd,int DefaultPermissions);
	bool SetAcceptVoice(int usersfd,bool acceptvoice);
	bool SetProfile(int usersfd,char *text,bool append);
	bool IsConnected(char *username);
	
	int GetSFD(char *username);
  sMainUsers* getUserByFD(int usersfd);
	char* GetUserName(int usersfd);
	char* GetHostName(int usersfd);

	int GetPermissions(int usersfd);
	char* GetProfile(int usersfd,int num);
	bool GetAcceptVoice(int usersfd);
	bool GetReservedStatus(int usersfd);
	bool SendMessage(char *username,char *msg);
  //WebSocket Functions
  void setSecWebSocketKey1(int usersfd,char *key);
  void setSecWebSocketKey2(int usersfd,char *key);
  void setWebSocketHost(int usersfd,char *host);
  void setWebSocketOrigin(int usersfd,char *origin);
  void setWebSocketProtocol(int usersfd, char *protocol);
  void setWebSocketChallenge(int usersfd, char *challenge);
  void setWebSocketEnabled(int usersfd,bool enabled);
  bool isWebSocketEnabled(int usersfd);
  void sendWebSocketHandshake(int usersfd,char *challenge);
	//Kicking and Banning Functions
	bool BanUser(int fromsfd,int usersfd,int minutes,char *reason);
	bool UnBanUser(int fromsfd,char *username);
	bool KickUser(int fromsfd,int usersfd,char *reason);

	//File Share Functions
	int GetTotalFiles(int usersfd);
	void GetFileList(int usersfd,int *FileID,char *Filename,float *size,int num);
	int GetFileID(int usersfd,char *Filename);
	bool AddFile(int usersfd,char *Filename,float size);
	bool RemoveFile(int usersfd,char *Filename);

	//Reserved User Management Functions
	bool AddReservedUser(int fromsfd,char *username,char *password,int DefaultPermissions,bool acceptvoice);
	bool RemoveReservedUser(int fromsfd,char *username);
	bool IsReservedUserName(char *username);
	bool SetPassword(int usersfd,char *newpassword);
	cUserHandler(char *userfile2,char *profilelocation2,cERR *errhandle,int maxusers2,int maxfiles2,int MaxFileTransferRate2,int maxprofilesize2,cswrapper *Wrapper)
	{
		userfile = userfile2;
		profilelocation = profilelocation2;
		ErrorHandler = errhandle;
		Socket = Wrapper;
		maxusers = maxusers2;
		numusers = 0;
		maxfiles = maxfiles2;
		MaxFileTransferRate = MaxFileTransferRate;
		maxprofilesize = maxprofilesize2;
		MainUsers = NULL;
		BanList = NULL;
	};
	~cUserHandler()
	{
		if(userfile != NULL)
			delete [] userfile;
		if(profilelocation != NULL)
			delete [] profilelocation;
		if(MainUsers != NULL)
			delete MainUsers;
		if(BanList != NULL)
			delete BanList;
	};
};
#endif

