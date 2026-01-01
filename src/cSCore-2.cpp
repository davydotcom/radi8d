
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
void cSCore::parsecmd(int usersfd,char *buffer)
{


	if(buffer[0] == '!')
	{
		char *cmd  = GetArg(0,buffer,':');
		info("Command From parsecmd",cmd);
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
	printf("ChName = %s\n",ChName);
	char *Password = GetArg(2,buffer,':');
	if(ChName != NULL)
	{
		if(ChannelHandler->IsAChannel(ChName) == -1)
		{
			info("cSCore::JoinChannel","Creating Channel");
			if(!ChannelHandler->CreateChannel(ChName,Password,NULL,0,false,false))
			{
				Socket->cs_write(usersfd,"!err:jnchn:Unable To Create",27);
				delete [] ChName;
				if(Password != NULL)
					delete [] Password;
				return;
				
			}
			
		}
		info("cSCore::JoinChannel","Joining Channel");
		returnval = ChannelHandler->JoinChannel(usersfd,ChName,Password);
		char *newCommand = new char[strlen(ChName)+10];
		info("cSCore::JoinChannel","Channel Join Called");
		switch(returnval)
		{
		case 0: //Success
			sprintf(newCommand,"!apr:jnchn:%s",ChName);
			Socket->cs_write(usersfd,newCommand,10+strlen(ChName));			
			break;
		case -1: //Channel Does Not Exist
			Socket->cs_write(usersfd,"!err:jnchn:Failed To Create Channel",35);		
			break;
		case -2: //User Already In Channel
			Socket->cs_write(usersfd,"!err:jnchn:Already In Channel",29);		
			break;
		case -3:// Invalid Password
			Socket->cs_write(usersfd,"!err:jnchn:Invalid Password",27);		
			break;
		case -4://Unknown Error (Should Never Happen)
			Socket->cs_write(usersfd,"!err:jnchn:Unknown Error",24);		
			break;
		case -5://Reserved Users Only
			Socket->cs_write(usersfd,"!err:jnchn:This Channel Is For Reserved Users Only",50);		
			break;
		};
		if(Password != NULL)
			delete [] Password;
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:lvchn:Channel Not Specified",32);
}
void cSCore::LeaveChannel(int usersfd,char *buffer)
{
bool returnval;
	char *ChName = GetArg(1,buffer,':');
	printf("ChName = %s\n",ChName);
	
	if(ChName != NULL)
	{

		returnval = ChannelHandler->LeaveChannel(usersfd,ChName);
		if(!returnval)
			Socket->cs_write(usersfd,"!err:lvchn:You Are Not In That Channel",38);
		else
			Socket->cs_write(usersfd,"!apr:lvchn",10);
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:lvchn:Channel Not Specified",32);
	
}
void cSCore::Message(int usersfd,char *buffer)
{
bool returnval;
	char *ChName = GetArg(1,buffer,':');
	printf("ChName = %s\n",ChName);
	
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
					Socket->cs_write(usersfd,"!err:msg:User Does Not Exist",28);
					delete [] TheMsg;
					delete TheirSFD;
				}
				else
				{
				char *FromUser = UserHandler->GetUserName(usersfd);
				info("cSCore::Message","Declaring Buffer");
				char *buffer2 = new char[44 + strlen(TheMsg)];
				memset(buffer2,0,44 + strlen(TheMsg));
				info("cSCore::Message","Buffer Declared");
				sprintf(buffer2,"!usrmsg:user:%s:%s",FromUser,TheMsg);
				Socket->cs_write(*TheirSFD,buffer2,strlen(buffer2));
				delete [] buffer2;
				delete [] TheMsg;
				delete TheirSFD;
				}
				}
				else
				{
					Socket->cs_write(usersfd,"!err:msg:No Message",19);	
				}
				delete[]UserName;
			}
			else
				Socket->cs_write(usersfd,"!err:msg:UserName Not Specified",31);
		}
		else
		{
			char *TheMsg = GetArg(2,buffer,':');
			if(TheMsg != NULL)
			{
				char *FromUser = UserHandler->GetUserName(usersfd);
				
				
				
				
				char *buffer2 = new char[66 + strlen(TheMsg)];
				
				memset(buffer2,0,66 + strlen(TheMsg));
				
				sprintf(buffer2,"!usrmsg:%s:%s:%s",ChName,FromUser,TheMsg);	
				
				returnval = ChannelHandler->SendToChannel(usersfd,ChName,buffer2);
				
				if(!returnval)
					Socket->cs_write(usersfd,"!err:msg:Failed To Send To Channel",34);
				delete [] buffer2;
				delete [] TheMsg;
			}
			else
				Socket->cs_write(usersfd,"!err:msg:No Message",19);	
		}
		
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:msg:Channel Not Specified",30);
}
void cSCore::ChangeName(int usersfd,char *buffer)
{
	int returnval;
	char *NewName = GetArg(1,buffer,':');
	
	
	if(NewName != NULL)
	{
		char *Password = GetArg(2,buffer,':');
		char *Temp = UserHandler->GetUserName(usersfd);
		char *OldName = new char[26];
		memset(OldName,0,26);
		strcpy(OldName,Temp);
		returnval = UserHandler->SetName(usersfd,NewName,Password);
		switch(returnval)
		{
		case 0:
			Socket->cs_write(usersfd,"!apr:name",9);
			ChannelHandler->UserNameChange(usersfd,OldName);
			break;
		case -1:
			Socket->cs_write(usersfd,"!err:name:New Name Not Specified",32);
			break;
		case -2:
			Socket->cs_write(usersfd,"!err:name:New Name Too Long",27);
			break;
		case -3:
			Socket->cs_write(usersfd,"!err:name:Unknown Error",23);
			break;
		case -4:
			Socket->cs_write(usersfd,"!err:name:Invalid Password",26);
			break;
		case -5: 
			Socket->cs_write(usersfd,"!err:name:User Already Connected",32);
			break;
		};
		delete [] OldName;
		delete [] NewName;
		delete [] Password;
	}
	else
		Socket->cs_write(usersfd,"!err:name:New Name Not Specified",32);
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
					Socket->cs_write(usersfd,"!err:emote:User Does Not Exist",30);
					delete [] TheMsg;
					delete TheirSFD;
				}
				else
				{
				char *FromUser = UserHandler->GetUserName(usersfd);
				char *buffer2 = new char[44 + strlen(TheMsg)];
				memset(buffer2,0,44 + strlen(TheMsg));
				sprintf(buffer2,"!usremt:user:%s:%s",FromUser,TheMsg);
				Socket->cs_write(*TheirSFD,buffer2,strlen(buffer2));
				delete [] buffer2;
				delete [] TheMsg;
				delete TheirSFD;
				}
				}
				else
				{
					Socket->cs_write(usersfd,"!err:emote:No Message",21);	
				}
				delete[]UserName;
			}
			else
				Socket->cs_write(usersfd,"!err:emote:UserName Not Specified",33);
		}
		else
		{
			char *TheMsg = GetArg(2,buffer,':');
			if(TheMsg != NULL)
			{
				char *FromUser = UserHandler->GetUserName(usersfd);
				
				
				
				
				char *buffer2 = new char[66 + strlen(TheMsg)];
				
				memset(buffer2,0,66 + strlen(TheMsg));
				
				sprintf(buffer2,"!usremt:%s:%s:%s",ChName,FromUser,TheMsg);	
				
				returnval = ChannelHandler->SendToChannel(usersfd,ChName,buffer2);
				
				if(!returnval)
					Socket->cs_write(usersfd,"!err:emote:Failed To Send To Channel",36);
				delete [] buffer2;
				delete [] TheMsg;
			}
			else
				Socket->cs_write(usersfd,"!err:emote:No Message",21);	
		}
		
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:emote:Channel Not Specified",32);
}
void cSCore::SendMOTD(int usersfd)
{
	info("cSCore::SendMOTD","Sending MOTD to Client!");
	FILE *MOTDFile = NULL;
	MOTDFile = fopen("radi8d.motd","r");
	if(MOTDFile == NULL)
	{
		Socket->cs_write(usersfd,"!motd:No Motd Set",17);
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
	char *chunk = new char[CHUNK_SIZE * 2 + 7]; // Account for escaping overhead
	
	int filepos = 0;
	while(filepos < filesize)
	{
		memset(chunk, 0, CHUNK_SIZE * 2 + 7);
		strcpy(chunk, "!motd:");
		int chunkpos = 6;
		int chunkchars = 0;
		
		// Fill chunk up to CHUNK_SIZE characters (after escaping)
		while(filepos < filesize && chunkchars < CHUNK_SIZE)
		{
			if(filebuffer[filepos] == '\n')
			{
				if(chunkchars + 4 <= CHUNK_SIZE)
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
				if(chunkchars + 7 <= CHUNK_SIZE)
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
				if(chunkchars + 1 <= CHUNK_SIZE)
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
			Socket->cs_write(usersfd, chunk, strlen(chunk));
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
			char *buffer = new char[50];
			memset(buffer,0,50);
			sprintf(buffer,"!chanadd:%s:%d",ChannelName,*NumberUsers);
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			delete [] buffer;
		}
		else
		{
		char *buffer = new char[50 + strlen(Topic)];
			memset(buffer,0,50 + strlen(Topic));
			sprintf(buffer,"!chanadd:%s:%d:%s",ChannelName,*NumberUsers,Topic);
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			delete [] buffer;
		}
		
	}
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

		char *TopicData = new char[strlen(ChName)+7+10];
		memset(TopicData,0,strlen(ChName)+7+10);
		sprintf(TopicData,"!topic:%s:No Topic",ChName);
		Socket->cs_write(usersfd,TopicData,strlen(TopicData));
		delete [] TopicData;
		}
		else
		{
			char *TopicData = new char[strlen(ChName)+9+strlen(Topic)];
			memset(TopicData,0,strlen(ChName)+9+strlen(Topic));
			sprintf(TopicData,"!topic:%s:%s",ChName,Topic);
			Socket->cs_write(usersfd,TopicData,strlen(TopicData));
			delete [] TopicData;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:topic:Channel Not Specified",32);
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
			Socket->cs_write(usersfd,"!err:settopic:No Topic Specified",32);

		
		}
		else
		{
			if(ChannelHandler->SetTopic(usersfd,ChName,Topic))
			{
				info("cSCore::SetTopic","Topic Set Succeeded");
				char *TopicSet = new char[288];
				memset(TopicSet,0,288);
				sprintf(TopicSet,"!topic:%s:%s",ChName,Topic);
				ChannelHandler->SendToChannel(-1,ChName,TopicSet);
				delete [] TopicSet;
			}
			else
			{
				Socket->cs_write(usersfd,"!err:settopic:Unable To Set The Topic. You May Not Have Sufficient Privilages",77);
				info("cSCore::SetTopic","Topic Failed to be set");	
			}
			delete [] Topic;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:settopic:Channel Name Not Specified",40);
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
			sprintf(buffer,"!usrjoind:%s:%s:%d:%d",ChName,UserName,TempUser->permissions,UserHandler->GetAcceptVoice(TempUser->sfd));
			Socket->cs_write(usersfd,buffer,strlen(buffer));
			//!usrjoind:channel:username:permissions:allowvoice
			delete [] buffer;
			TempUser = TempUser->next;
		}
		delete [] ChName;
	}
	else
		Socket->cs_write(usersfd,"!err:userlist:Channel Name Not Specified",40);
}

