
/****************************************************************/
/*		FileName: stdstructs.h				*/
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
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"getconfig.h"
//#include"getarg.h"
#ifndef STDSTRUCT_H
#define STDSTRUCT_H

struct sUserBuffers //Storage For User Reading Buffers
{
	char *buffer;
	bool done;
	int buffersize;
	int strsize;
	sUserBuffers *next;
	sUserBuffers() //Constructor
	{
		buffer = new char[10];

		next = NULL;
		done = false;
		buffersize = 10;
		strsize = 0;
	};
	~sUserBuffers() //Destructor
	{
		if(buffer != NULL)
			delete [] buffer;
		if(next != NULL)
			delete next;
		
	};
};
struct sUserList //User List For Users In Channels...Not Main User Struct
{
	int sfd; //The Socket ID of the User
	int permissions; //The Permissions In Channel 0-Normal 1-speakperm(for moderated channels) 2-Moderator Permissions 3-Admin Permissions 4-SuperAdmin(Global Administrator)
	sUserList *next; //Pointer for linked List
	sUserList() //Constructor
	{
		permissions = 0;
		next = NULL;
	};
	~sUserList() //Destructor
	{
		if(next != NULL)
			delete next;
	};
};

struct sBanList //List of Users Banned From Channel and or Global Bans
{
	char *username; //UserName to Ban
	char *hostname; //HostName to Ban
	int minutes; //0=unlimited if minutes > 0 than is time limit
	sBanList *next; //Pointer For Linked List
	sBanList() //Constructor
	{
		username = NULL; //Create in Handler
		hostname = NULL; //Create in Handler
		minutes = 0; //Default Unlimited
		next = NULL;
	};
	~sBanList() //Destructor
	{
		if(username != NULL)
			delete [] username;
		if(hostname != NULL)
			delete [] hostname;
		if(next != NULL)
			delete next;
	};
};

struct sFiles //Users File Shares
{
	int FileID; //FileID used for file request when connecting to user
	char *filename; //FileName
	float size; //Size in Bytes of File
	sFiles *next; //Pointer For Linked List
	sFiles() //Constructor
	{
		filename = NULL; //Create in Handler
		size = 0;
		next = NULL;
	};
	~sFiles() //Destructor
	{
		if(filename != NULL)
			delete [] filename;
		if(next != NULL)
			delete next;
	};
};

//*************MAINUSERS STRUCT*****************
struct sMainUsers //Stores User Information For The Currently Connected Users
{
	int sfd; //Socket Id
	sUserBuffers *UserBuffer;
	int totalfiles; //File Sharing Number of Files
	char *username; //Username
	int DefaultPermissions;
	bool acceptvoice; //Determines wether user accepts voice comm or not
  bool isWebSocketProtocol;
  char *secWebSocketKey1;
  char *secWebSocketKey2;
  char *webSocketChallenge;
  char *webSocketProtocol;
  char *webSocketOrigin;
  char *webSocketHost;
  int webSocketByteCount;
	char *hostname; //User HostName
	bool reserved; //Is This A Reserved User
	sMainUsers *next; //Pointer for Linked List
	sMainUsers() //Constructor
	{
		username = NULL; //Set in Handler
		DefaultPermissions = 0; //Normal User
		acceptvoice = true; //Default to AcceptVoiceComm
		hostname = NULL; //Set in Handler
    secWebSocketKey1 = NULL;
    secWebSocketKey2 = NULL;
    webSocketProtocol = NULL;
    webSocketOrigin = NULL;
    webSocketHost = NULL;
    isWebSocketProtocol = false;
    webSocketChallenge = NULL;
    webSocketByteCount = -1;
		reserved = false; //By Default is Not Reserved
		totalfiles=0;
		next = NULL;
		UserBuffer = new sUserBuffers;
	};
	~sMainUsers() //Destructor
	{
		if(username != NULL)
			delete [] username;
		if(hostname != NULL)
			delete [] hostname;
		if(next != NULL)
			delete next;
		if(UserBuffer != NULL)
			delete UserBuffer;
    if(secWebSocketKey1 != NULL)
      delete [] secWebSocketKey1;
    if(secWebSocketKey2 != NULL)
      delete [] secWebSocketKey2;
    if(webSocketChallenge != NULL)
      delete [] webSocketChallenge;
    if(webSocketProtocol != NULL)
      delete [] webSocketProtocol;
    if(webSocketOrigin != NULL)
      delete [] webSocketOrigin;
    if(webSocketHost != NULL)
      delete [] webSocketHost;


	};
	
};

//*************Channels Structure*************

struct sChannels //Structure Used For Storing Channel Information
{
	sUserList *UserList; //Pointer To Channel UserList
	sBanList *BanList; //List of Banned Users
	char *ChannelName; //Name of Channel
	char *Password; //Channel Password
	int chantype; //0-Normal 1-Voice 2-Both 1&2
	bool ReservedOnly; //Channel For Reserved Users Only
	int codec; //Codec 1=Low 2=Mid-Low 3-Mid 4-Mid-High 5-High
	char *Topic; //The Channel Topic
	int maxusers; //Max Users Allowed in Channel
	bool RequireSpeakPermissions; //Determines if SpeakPermissions Are Req
	sChannels *next; //Pointer for Linked List
	sChannels() //Constructor
	{
		UserList = NULL;
		BanList = NULL;
		ChannelName = NULL; //Set in Channel Handler
		Password = NULL; //Set in Channel Handler NULL=public
		chantype = 0; //Normal Channel
		ReservedOnly = false; //Public Channel
		codec = 1; //Low Quality Default
		Topic = NULL; //Set in Channel Handler
		maxusers = 0; //Set in Channel Handler
		RequireSpeakPermissions = false; //Default to False
		next = NULL;
	};
	~sChannels() //Destructor
	{
		if(UserList != NULL)
			delete UserList;
		if(BanList != NULL)
			delete BanList;
		if(ChannelName != NULL)
			delete [] ChannelName;
		if(Password != NULL)
			delete [] Password;
		if(Topic != NULL)
			delete [] Topic;
		if(next != NULL)
			delete next;
	};
};
#endif
