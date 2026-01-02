
/****************************************************************/
/*		FileName: cSCore-2.cpp				*/
/*		Author: David Estes(Chrono)			*/
/*		CopyRight 2004,2005 David Estes			*/
/*		Under the GPL License				*/
/*		FOR NON-COMMERCIAL USE ONLY			*/
/****************************************************************/
//====================
//Revisement History |
//====================
//2/23/2005 -- File Created in Order to shrink cSCore.cpp
//
//
//====================
#include"cSCore.h"
#include"getarg.h"
#include <fstream>
void cSCore::parsecmd(int usersfd,char *buffer)
{
	

	if(buffer[0] == '!')
	{
	char *cmd  = GetArg(0,buffer,':');
	// info("Command From parsecmd",cmd);
		if(cmd != NULL)
		{
			if(!strcmp(cmd,"!name")) //Name Change Requested
			{
				ChangeName(usersfd,buffer);
			}
			
			else if(!strcmp(UserHandler->GetUserName(usersfd),"unverified"))
			{
				info("cSCore::parsecmd()","Unverified User Attempted Command\n");
			}
			else if(!strcmp(cmd,"!jnchn")) //Join Channel Requested
			{
				JoinChannel(usersfd,buffer);	
			}
			else if(!strcmp(cmd,"!lvchn")) //Leave Channel Requested
			{
				LeaveChannel(usersfd,buffer);
			}
			else if(!strcmp(cmd,"!msg")) //Message Transmit Requested
			{
				Message(usersfd,buffer);
			}
			else if(!strcmp(cmd,"!emote")) //Emote Transmit Requested
			{
				Emote(usersfd,buffer);
			}
			else if(!strcmp(cmd,"!topic"))
			{
				GetTopic(usersfd,buffer);
			}
			else if(!strcmp(cmd,"!settopic"))
			{
				SetTopic(usersfd,buffer);
			}
			

			else if(!strcmp(cmd,"!chanlist")) //Name Change Requested
			{
				ChannelList(usersfd);
			}
			else if(!strcmp(cmd,"!userlist")) //Name Change Requested
			{
				UserList(usersfd,buffer);
			}
			else if(!memcmp(cmd,"!motd",5))
			{
				SendMOTD(usersfd);
			}
			else if(!strcmp(cmd,"!kick"))
			{
				KickUser(usersfd,buffer);
			}
			delete [] cmd;
		}

	}
  else
  {
    if(!memcmp(buffer,"GET",3))
    {
      printf("WebSocket Request Detected: \n%s\n",buffer);
      
    }
    else if(!memcmp(buffer,"Sec-WebSocket-Key1:",19))
    {
      char *key = new char[strlen(buffer)-19];
      memset(key,0,strlen(buffer)-20);
      memcpy(key,&(buffer[20]),strlen(buffer)-21);
      UserHandler->setSecWebSocketKey1(usersfd,key);
    }
    else if(!memcmp(buffer,"Sec-WebSocket-Key2:",19))
    {
      char *key = new char[strlen(buffer)-19];
      memset(key,0,strlen(buffer)-20);
      memcpy(key,&(buffer[20]),strlen(buffer)-21);
      UserHandler->setSecWebSocketKey2(usersfd,key);
      UserHandler->setWebSocketEnabled(usersfd,true);
    }
    else if(!memcmp(buffer,"Host:",5))
    {
      char *host = new char[strlen(buffer) -5];
      memset(host,0,strlen(buffer) - 6);
      memcpy(host,&(buffer[6]),strlen(buffer)-7);
      UserHandler->setWebSocketHost(usersfd,host);
    }
    else if(!memcmp(buffer,"Origin:",7))
    {
      char *host = new char[strlen(buffer) -7];
      memset(host,0,strlen(buffer) - 8);
      memcpy(host,&(buffer[8]),strlen(buffer)-9);
      UserHandler->setWebSocketOrigin(usersfd,host);
    }
    else if(!memcmp(buffer,"Sec-WebSocket-Protocol:",23))
    {
      char *host = new char[strlen(buffer) -23];
      memset(host,0,strlen(buffer) - 24);
      memcpy(host,&(buffer[24]),strlen(buffer)-25);
      UserHandler->setWebSocketProtocol(usersfd,host);
    }
  }
}

void cSCore::JoinChannel(int usersfd,char *buffer)
{
	int returnval;

	char *ChName = GetArg(1,buffer,':');
	info("cSCore::JoinChannel",ChName);
	char *Password = GetArg(2,buffer,':');
	if(ChName != NULL)
	{
		if(ChannelHandler->IsAChannel(ChName) == -1)
		{
			info("cSCore::JoinChannel","Creating Channel");
			if(!ChannelHandler->CreateChannel(ChName,Password,NULL,0,false,false))
			{
				Socket->cs_write(usersfd,"!err:jnchn:Unable To Create\n",28);
				delete [] ChName;
				if(Password != NULL)
					delete [] Password;
				return;
				
			}
			
		}
	info("cSCore::JoinChannel","Joining Channel");
	returnval = ChannelHandler->JoinChannel(usersfd,ChName,Password);
	info("cSCore::JoinChannel","Channel Join Called");
	switch(returnval)
	{
case 0: //Success
	{
		char *newCommand = new char[strlen(ChName)+13];
		sprintf(newCommand,"!apr:jnchn:%s\n",ChName);
		Socket->cs_write(usersfd,newCommand,12+strlen(ChName));
		delete [] newCommand;
	}
		break;
	case -1: //Channel Does Not Exist
		Socket->cs_write(usersfd,"!err:jnchn:Failed To Create Channel\n",36);
		break;
	case -2: //User Already In Channel
		Socket->cs_write(usersfd,"!err:jnchn:Already In Channel\n",30);
		break;
	case -3:// Invalid Password
		Socket->cs_write(usersfd,"!err:jnchn:Invalid Password\n",28);
		break;
	case -4://Unknown Error (Should Never Happen)
		Socket->cs_write(usersfd,"!err:jnchn:Unknown Error\n",25);
		break;
	case -5://Reserved Users Only
		Socket->cs_write(usersfd,"!err:jnchn:This Channel Is For Reserved Users Only\n",51);
		break;
	};
	if(Password != NULL)
		delete [] Password;
	if(ChName != NULL)
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:lvchn:Channel Not Specified\n",33);
}
void cSCore::LeaveChannel(int usersfd,char *buffer)
{
bool returnval;
	char *ChName = GetArg(1,buffer,':');
	info("cSCore::LeaveChannel",ChName);
	
	if(ChName != NULL)
	{

		returnval = ChannelHandler->LeaveChannel(usersfd,ChName);
		if(!returnval)
			Socket->cs_write(usersfd,"!err:lvchn:You Are Not In That Channel\n",39);
		else
			Socket->cs_write(usersfd,"!apr:lvchn\n",11);
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:lvchn:Channel Not Specified\n",33);
	
}
void cSCore::Message(int usersfd,char *buffer)
{
bool returnval;
	char *ChName = GetArg(1,buffer,':');
//	dbg(1,"cSCore::Message",ChName);
	if(ChName != NULL)
	{
		if(!strcmp(ChName,"user"))
		{
			char *UserName = GetArg(2,buffer,':');
			if(UserName != NULL)
			{
				char *TheMsg = GetArg(3,buffer,':');
				if(TheMsg != NULL)
				{
				
				int *TheirSFD = new int;
				*TheirSFD = UserHandler->GetSFD(UserName);
				if(*TheirSFD == -1)
				{
					Socket->cs_write(usersfd,"!err:msg:User Does Not Exist\n",29);
					delete [] TheMsg;
					delete TheirSFD;
				}
			else
			{
			char *FromUser = UserHandler->GetUserName(usersfd);
			info("cSCore::Message","Declaring Buffer");
			int buffer2_size = 19 + strlen(FromUser) + strlen(TheMsg);
			char *buffer2 = new char[buffer2_size];
			memset(buffer2,0,buffer2_size);
			info("cSCore::Message","Buffer Declared");
				sprintf(buffer2,"!usrmsg:user:%s:%s\n",FromUser,TheMsg);
				Socket->cs_write(*TheirSFD,buffer2,strlen(buffer2));
				delete [] buffer2;
				delete [] TheMsg;
				delete TheirSFD;
				}
				}
				else
				{
					Socket->cs_write(usersfd,"!err:msg:No Message\n",20);
				}
				delete[]UserName;
			}
			else
				Socket->cs_write(usersfd,"!err:msg:UserName Not Specified\n",32);
		}
		else
		{
		char *TheMsg = GetArg(2,buffer,':');
		if(TheMsg != NULL)
		{
			char *FromUser = UserHandler->GetUserName(usersfd);
			
			
			
			
			int buffer2_size = 13 + strlen(ChName) + strlen(FromUser) + strlen(TheMsg);
			char *buffer2 = new char[buffer2_size];
			
			memset(buffer2,0,buffer2_size);
			
			sprintf(buffer2,"!usrmsg:%s:%s:%s\n",ChName,FromUser,TheMsg);
				
				returnval = ChannelHandler->SendToChannel(usersfd,ChName,buffer2);
				
				if(!returnval)
					Socket->cs_write(usersfd,"!err:msg:Failed To Send To Channel\n",35);
				delete [] buffer2;
				delete [] TheMsg;
			}
			else
				Socket->cs_write(usersfd,"!err:msg:No Message\n",20);
		}
		
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:msg:Channel Not Specified\n",31);
}
void cSCore::ChangeName(int usersfd,char *buffer)
{
	int returnval;
	char *NewName = GetArg(1,buffer,':');
	
	
	if(NewName != NULL)
	{
		// Log the name change request
		char *logMsg = new char[256];
		snprintf(logMsg, 256, "User requesting name change to: %s", NewName);
		info("cSCore::ChangeName", logMsg);
		delete [] logMsg;
		char *Password = GetArg(2,buffer,':');
		char *Temp = UserHandler->GetUserName(usersfd);
		char *OldName = new char[26];
		memset(OldName,0,26);
		strcpy(OldName,Temp);
		returnval = UserHandler->SetName(usersfd,NewName,Password);
	switch(returnval)
		{
		case 0:
			Socket->cs_write(usersfd,"!apr:name\n",10);
			ChannelHandler->UserNameChange(usersfd,OldName);
			break;
		case -1:
			Socket->cs_write(usersfd,"!err:name:New Name Not Specified\n",33);
			break;
		case -2:
			Socket->cs_write(usersfd,"!err:name:New Name Too Long\n",28);
			break;
		case -3:
			Socket->cs_write(usersfd,"!err:name:Unknown Error\n",24);
			break;
		case -4:
			Socket->cs_write(usersfd,"!err:name:Invalid Password\n",27);
			break;
		case -5: 
			Socket->cs_write(usersfd,"!err:name:User Already Connected\n",33);
			break;
		};
		delete [] OldName;
		delete [] NewName;
		delete [] Password;
	}
	else
		Socket->cs_write(usersfd,"!err:name:New Name Not Specified\n",33);
}
void cSCore::Emote(int usersfd,char *buffer)
{
bool returnval;
	char *ChName = GetArg(1,buffer,':');
	
	
	if(ChName != NULL)
	{
		if(!strcmp(ChName,"user"))
		{
			char *UserName = GetArg(2,buffer,':');
			if(UserName != NULL)
			{
				char *TheMsg = GetArg(3,buffer,':');
				if(TheMsg != NULL)
				{
				
				int *TheirSFD = new int;
				*TheirSFD = UserHandler->GetSFD(UserName);
				if(*TheirSFD == -1)
				{
					Socket->cs_write(usersfd,"!err:emote:User Does Not Exist\n",31);
					delete [] TheMsg;
					delete TheirSFD;
				}
			else
			{
			char *FromUser = UserHandler->GetUserName(usersfd);
			int buffer2_size = 19 + strlen(FromUser) + strlen(TheMsg);
			char *buffer2 = new char[buffer2_size];
			memset(buffer2,0,buffer2_size);
			sprintf(buffer2,"!usremt:user:%s:%s\n",FromUser,TheMsg);
				Socket->cs_write(*TheirSFD,buffer2,strlen(buffer2));
				delete [] buffer2;
				delete [] TheMsg;
				delete TheirSFD;
				}
				}
				else
				{
					Socket->cs_write(usersfd,"!err:emote:No Message\n",22);
				}
				delete[]UserName;
			}
			else
				Socket->cs_write(usersfd,"!err:emote:UserName Not Specified\n",34);
		}
		else
		{
		char *TheMsg = GetArg(2,buffer,':');
		if(TheMsg != NULL)
		{
			char *FromUser = UserHandler->GetUserName(usersfd);
			
			
			
			
			int buffer2_size = 13 + strlen(ChName) + strlen(FromUser) + strlen(TheMsg);
			char *buffer2 = new char[buffer2_size];
			
			memset(buffer2,0,buffer2_size);
			
			sprintf(buffer2,"!usremt:%s:%s:%s\n",ChName,FromUser,TheMsg);
				
				returnval = ChannelHandler->SendToChannel(usersfd,ChName,buffer2);
				
				if(!returnval)
					Socket->cs_write(usersfd,"!err:emote:Failed To Send To Channel\n",37);
				delete [] buffer2;
				delete [] TheMsg;
			}
			else
				Socket->cs_write(usersfd,"!err:emote:No Message\n",22);
		}
		
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:emote:Channel Not Specified\n",33);
}
void cSCore::SendMOTD(int usersfd)
{
	info("cSCore::SendMOTD","Sending MOTD to Client!");
	FILE *MOTDFile = NULL;
	MOTDFile = fopen("radi8d.motd","r");
	if(MOTDFile == NULL)
	{
		Socket->cs_write(usersfd,"!motd:No Motd Set\n",18);
		return;
	}
	
	// Read entire file
	fseek(MOTDFile, 0, SEEK_END);
	long filesize = ftell(MOTDFile);
	rewind(MOTDFile);
	
	char *filebuffer = new char[filesize + 1];
	memset(filebuffer, 0, filesize + 1);
	fread(filebuffer, 1, filesize, MOTDFile);
	fclose(MOTDFile);
	
	// Process and send in chunks with escaping
	const int CHUNK_SIZE = 128;
	const int BUFFER_SIZE = 6 + CHUNK_SIZE + 50; // prefix + max content + safety margin
	char *chunk = new char[BUFFER_SIZE];
	
	int filepos = 0;
	while(filepos < filesize)
	{
		memset(chunk, 0, BUFFER_SIZE);
		strcpy(chunk, "!motd:");
		int chunkpos = 6;
		int chunkchars = 0;
		
		// Fill chunk up to CHUNK_SIZE characters (after escaping)
		while(filepos < filesize && chunkchars < CHUNK_SIZE)
		{
			if(filebuffer[filepos] == '\n')
			{
				if(chunkchars + 4 <= CHUNK_SIZE && chunkpos + 4 < BUFFER_SIZE)
				{
					strcpy(&chunk[chunkpos], "<nl>");
					chunkpos += 4;
					chunkchars += 4;
					filepos++;
				}
				else
					break;
			}
			else if(filebuffer[filepos] == '\r')
			{
				// Skip carriage returns
				filepos++;
			}
			else if(filebuffer[filepos] == ':')
			{
				if(chunkchars + 7 <= CHUNK_SIZE && chunkpos + 7 < BUFFER_SIZE)
				{
					strcpy(&chunk[chunkpos], "<colon>");
					chunkpos += 7;
					chunkchars += 7;
					filepos++;
				}
				else
					break;
			}
			else
			{
				if(chunkchars + 1 <= CHUNK_SIZE && chunkpos + 1 < BUFFER_SIZE)
				{
					chunk[chunkpos] = filebuffer[filepos];
					chunkpos++;
					chunkchars++;
					filepos++;
				}
				else
					break;
			}
		}
		
		if(chunkpos > 6)
		{
			chunk[chunkpos] = '\n';
			Socket->cs_write(usersfd, chunk, chunkpos+1);
		}
	}
	
	delete [] filebuffer;
	delete [] chunk;
}
void cSCore::ChannelList(int usersfd)
{
	char *ChannelName;
	char *Topic;
	int *NumberUsers = new int;
	
	for(int x=0;;x++)
	{
		ChannelName = ChannelHandler->GetChannelList(x);
		if(ChannelName == NULL)
			break;
		Topic = ChannelHandler->GetTopic(ChannelName);
		*NumberUsers = ChannelHandler->GetNumberUsers(ChannelName);
		if(Topic == NULL)
		{
			char *buffer = new char[64];
			memset(buffer,0,64);
			sprintf(buffer,"!chanadd:%s:%d\n",ChannelName,*NumberUsers);
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			delete [] buffer;
		}
		else
		{
			// Include topic in the same line and terminate with newline
			char *buffer = new char[64 + strlen(Topic)];
			memset(buffer,0,64 + strlen(Topic));
			sprintf(buffer,"!chanadd:%s:%d:%s\n",ChannelName,*NumberUsers,Topic);
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			delete [] buffer;
		}
		
	}
	// Per docs: send channels until #EOF received by client
	Socket->cs_write(usersfd,"#EOF\n",5);
	delete NumberUsers;
}
void cSCore::GetTopic(int usersfd,char *buffer)
{
	char *ChName = GetArg(1,buffer,':');
	if(ChName != NULL)
	{
		char *Topic = ChannelHandler->GetTopic(ChName); //Do not delete this var, it is just a pointer to a struct object
		if(Topic == NULL)
		{

		char *TopicData = new char[strlen(ChName)+7+10+1];
		memset(TopicData,0,strlen(ChName)+7+10+1);
		sprintf(TopicData,"!topic:%s:No Topic\n",ChName);
		Socket->cs_write(usersfd,TopicData,strlen(TopicData));
		delete [] TopicData;
		}
		else
		{
			char *TopicData = new char[strlen(ChName)+9+strlen(Topic)+1];
			memset(TopicData,0,strlen(ChName)+9+strlen(Topic)+1);
			sprintf(TopicData,"!topic:%s:%s\n",ChName,Topic);
			Socket->cs_write(usersfd,TopicData,strlen(TopicData));
			delete [] TopicData;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:topic:Channel Not Specified\n",33);
}
void cSCore::SetTopic(int usersfd,char *buffer)
{
	info("cSCore::SetTopic","Entering Function");
	char *ChName = GetArg(1,buffer,':');
	if(ChName != NULL)
	{
		
		char *Topic = GetArg(2,buffer,':');
		if(Topic == NULL)
		{
			Socket->cs_write(usersfd,"!err:settopic:No Topic Specified\n",33);

		
		}
		else
		{
			if(ChannelHandler->SetTopic(usersfd,ChName,Topic))
			{
				info("cSCore::SetTopic","Topic Set Succeeded");
				char *TopicSet = new char[288];
				memset(TopicSet,0,288);
				sprintf(TopicSet,"!topic:%s:%s\n",ChName,Topic);
				ChannelHandler->SendToChannel(-1,ChName,TopicSet);
				delete [] TopicSet;
			}
			else
			{
				Socket->cs_write(usersfd,"!err:settopic:Unable To Set The Topic. You May Not Have Sufficient Privilages\n",78);
				info("cSCore::SetTopic","Topic Failed to be set");	
			}
			delete [] Topic;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:settopic:Channel Name Not Specified\n",41);
}
void cSCore::UserList(int usersfd, char *buffer)
{

	char *ChName = GetArg(1,buffer,':');
	
	
	if(ChName != NULL)
	{
		sUserList *TempUser = ChannelHandler->GetUser(0,ChName);
		for(;;)
		{
			if(TempUser == NULL)
				break;
			
				
			char *UserName = UserHandler->GetUserName(TempUser->sfd);
			char *buffer = new char[67];
			memset(buffer,0,67);
			sprintf(buffer,"!usrjoind:%s:%s:%d:%d\n",ChName,UserName,TempUser->permissions,UserHandler->GetAcceptVoice(TempUser->sfd));
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			//!usrjoind:channel:username:permissions:allowvoice
			delete [] buffer;
			TempUser = TempUser->next;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:userlist:Channel Name Not Specified\n",41);
}
void cSCore::KickUser(int usersfd, char *buffer)
{
	char *ChName = GetArg(1,buffer,':');
	if(ChName == NULL)
	{
		Socket->cs_write(usersfd,"!err:kick:Channel Not Specified\n",32);
		return;
	}
	
	char *UserName = GetArg(2,buffer,':');
	if(UserName == NULL)
	{
		Socket->cs_write(usersfd,"!err:kick:Username Not Specified\n",33);
		delete [] ChName;
		return;
	}
	
	char *Reason = GetArg(3,buffer,':');
	if(Reason == NULL)
	{
		// Default reason if none provided
		Reason = new char[8];
		strcpy(Reason,"Kicked");
	}
	
	// Get the socket fd of the user to be kicked
	int targetSfd = UserHandler->GetSFD(UserName);
	if(targetSfd == -1)
	{
		Socket->cs_write(usersfd,"!err:kick:User Not Found\n",25);
		delete [] ChName;
		delete [] UserName;
		delete [] Reason;
		return;
	}
	
	// Check if user is in the channel
	if(ChannelHandler->IsInChannel(targetSfd, ChName) == -1)
	{
		Socket->cs_write(usersfd,"!err:kick:User Not In Channel\n",30);
		delete [] ChName;
		delete [] UserName;
		delete [] Reason;
		return;
	}
	
	// Log the kick attempt
	char *logMsg = new char[256];
	char *kickerName = UserHandler->GetUserName(usersfd);
	snprintf(logMsg, 256, "User %s attempting to kick %s from %s", kickerName, UserName, ChName);
	info("cSCore::KickUser", logMsg);
	delete [] logMsg;
	
	// Attempt to kick the user (this will also broadcast !usrleft to the channel)
	bool result = ChannelHandler->KickUser(usersfd, ChName, targetSfd, Reason);
	
	if(result)
	{
		Socket->cs_write(usersfd,"!apr:kick\n",10);
	}
	else
	{
		Socket->cs_write(usersfd,"!err:kick:Permission Denied or Unable to Kick\n",46);
	}
	
	delete [] ChName;
	delete [] UserName;
	delete [] Reason;
}

