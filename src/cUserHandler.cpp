#include"cUserHandler.h"
#include<openssl/md5.h>

sMainUsers* cUserHandler::GetUser(int num)
{
	if(num == 0)
		return MainUsers;
	sMainUsers *temp = MainUsers;
	for(int x=0;x<num;x++)
	{
		temp = temp->next;
		if(temp == NULL)
			break;
	}
	return temp;
}
sBanList* cUserHandler::GetBan(int num)
{
	if(num == 0)
		return BanList;
	sBanList *temp = BanList;
	for(int x=0;x<num;x++)
	{
		temp = temp->next;
		if(temp == NULL)
			break;
	}
	return temp;
}

//Private Functions For Reserved Users
bool cUserHandler::ValidPassword(char *username, char*password)
{
	bool IsValid = false;
	if(username == NULL || password == NULL)
		return false;
	FILE *Users = NULL;
	Users = fopen(userfile,"r");
	char *buffer = new char[255];
	while(!feof(Users))
	{
		memset(buffer,0,255);
		fgets(buffer,255,Users);
		if(feof(Users))
		{
			break;
		}
		if(buffer[0] != '#')
		{
			buffer[strlen(buffer)-1] = '\0';
			char *ArgumentValue;
			char *PasswordValue;
			ArgumentValue = GetArg(0,buffer,':');
			PasswordValue = GetArg(1,buffer,':');
			
			if(!strcmp(ArgumentValue,username))
			{
				if(!strcmp(PasswordValue,password))
				{
					IsValid = true;
					delete [] ArgumentValue;
					delete [] PasswordValue;
					break;
				}
				else
				{
					IsValid = false;
					delete [] ArgumentValue;
					delete [] PasswordValue;
					break;
				}	
			}
			delete [] ArgumentValue;
			delete [] PasswordValue;
		}
	}
	delete [] buffer;
	fclose(Users);
	return IsValid;
}
bool cUserHandler::GetUserSettings(char *username,int *DefaultPermissions,bool *acceptvoice)
{
	
	if(username == NULL)
		return false;
	FILE *Users = NULL;
	Users = fopen(userfile,"r");
	char *buffer = new char[255];
	while(!feof(Users))
	{
		memset(buffer,0,255);
		fgets(buffer,255,Users);
		if(feof(Users))
		{
			break;
		}
		if(buffer[0] != '#')
		{
			buffer[strlen(buffer)-1] = '\0';
			char *ArgumentValue;
			
			char *Permissions;
			char *AcceptVoice;
			ArgumentValue = GetArg(0,buffer,':');
			
			Permissions = GetArg(2,buffer,':');
			AcceptVoice = GetArg(3,buffer,':');
			
			if(!strcmp(ArgumentValue,username))
			{
				*DefaultPermissions = atoi(Permissions);
				//sscanf(Permissions,"%d",*DefaultPermissions);
				//*acceptvoice = (bool)atoi(AcceptVoice);
				delete [] ArgumentValue;
				
				delete [] Permissions;
				delete [] AcceptVoice;
				
				break;
			}
			delete [] ArgumentValue;
			
			delete [] Permissions;
			delete [] AcceptVoice;
		}
	}
	delete [] buffer;
	fclose(Users);
	return true;
}
bool cUserHandler::SaveUserSettings(char *username)
{
	return true;
}

bool cUserHandler::AddUser(int usersfd, char *hostname)
{
	info("cUserHandler::AddUser()","In AddUser Function");
	if(maxusers < numusers) //Check To Make Sure maxusers has not been reached
		return false;
	if(hostname == NULL) //Make Sure A HostName Was Specified
		return false;
	sMainUsers *tempuser;
	if(MainUsers == NULL)
	{
		info("cUserHandler::AddUser()","Adding First User");
		MainUsers = new sMainUsers;
		tempuser = MainUsers;
	}
	else
	{
		info("cUserHandler::AddUser()","Adding Another User");
		tempuser = GetUser(0);
		while(tempuser->next != NULL)
		{
			tempuser=tempuser->next;
		}
		tempuser->next = new sMainUsers;
		tempuser = tempuser->next;
	}
	info("cUserHandler::AddUser()","Setting Sfd");
	tempuser->sfd = usersfd;
	memset(tempuser->UserBuffer->buffer,0,10);
	info("cUserHandler::AddUser()","Setting hostname");
	tempuser->hostname = new char[100];
	memset(tempuser->hostname,0,100);
	strcpy(tempuser->hostname,hostname);
	
	info("cUserHandler::AddUser()","Setting username to \"unverified\"");
	char *tempstr = new char[11];
	memset(tempstr,0,11);
	strcpy(tempstr,"unverified");
	
	tempuser->username = tempstr;
	
	numusers++;
	return true;
}
bool cUserHandler::RemoveUser(int usersfd)
{

	sMainUsers *tempuser;
	sMainUsers *tempuser2;
	//sMainUsers *tempuser3;
		tempuser = GetUser(0);
		if(tempuser->sfd == usersfd)
		{
			tempuser=MainUsers->next;
			MainUsers->next = NULL;

			delete MainUsers;
			MainUsers = tempuser;
			
			numusers--;
		}
		else
		{
			tempuser = tempuser->next;
			for(int x=1;tempuser != NULL;x++)
			{
				
				if(tempuser->sfd == usersfd)
				{
					tempuser2 = tempuser->next;
					tempuser->next = NULL;

					delete tempuser;
					GetUser(x-1)->next = tempuser2;
					numusers--;
					break;
				}
				//tempuser3 = tempuser;
				tempuser = tempuser->next;
			}
		}	
		return true;
}
int cUserHandler::SetName(int usersfd, char *username,char *Password)
{
	info("cUserHandler::SetName","In Function");
	if(username == NULL)
		return -1; //No Username Specified
	if(strlen(username) > 25)
		return -2; // Username To Long
	if(GetSFD(username) != -1)
		return -5; //User Already Logged in
	info("cUserHandler::SetName","NewUserName Valid");
	int *DefaultPermissions = new int;
	*DefaultPermissions = 0;
	bool *AcceptVoice = new bool;
	*AcceptVoice = true;
	if (IsReservedUserName(username))
	{
		info("cUserHandler::SetName","UserName is Reserved");
		if(Password == NULL)
		{
			delete DefaultPermissions;
			delete AcceptVoice;
			return -4; //Invalid Password
		}
		if(!ValidPassword(username,Password))
		{	
			delete DefaultPermissions;
			delete AcceptVoice;
			return -4;
		}
		info("cUserHandler::SetName","Password is Valid");
		GetUserSettings(username,DefaultPermissions,AcceptVoice);
		printf("Def Permissions = %d\n",*DefaultPermissions);
	}
	sMainUsers *tempuser = GetUser(0);
	info("cUserHandler::SetName","UserList Ptr Acquired");
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
		{
			info("cUserHandler::SetName","UserSFD Found");
			if(tempuser->username != NULL)
				delete [] tempuser->username;
			tempuser->username = new char[26];
			info("cUserHandler::SetName","New UserName String Created");
			memset(tempuser->username,0,26);
			info("cUserHandler::SetName","New UserName String Cleaned");
			strcpy(tempuser->username,username);
			info("cUserHandler::SetName","UserName Set");
			tempuser->DefaultPermissions = *DefaultPermissions;
			tempuser->acceptvoice = *AcceptVoice;
			delete DefaultPermissions;
			delete AcceptVoice;
			return 0; //Success
		}
			
		tempuser = tempuser->next;
	}
	delete DefaultPermissions;
	delete AcceptVoice;
	return -3; //Unknown Error
}
void cUserHandler::setWebSocketHost(int usersfd, char *host)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->webSocketHost = host;
}
void cUserHandler::setWebSocketChallenge(int usersfd,char *challenge)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->webSocketChallenge = challenge;
}
void cUserHandler::setWebSocketProtocol(int usersfd,char *protocol)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->webSocketProtocol = protocol;

}
void cUserHandler::setWebSocketOrigin(int usersfd,char *origin)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->webSocketOrigin = origin;

}

void cUserHandler::setSecWebSocketKey1(int usersfd,char *key)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->secWebSocketKey1 = key;
  printf("Key1: %s\n",key);
}
void cUserHandler::setSecWebSocketKey2(int usersfd,char *key)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->secWebSocketKey2 = key;
  printf("Key2: %s\n",key);
}
bool cUserHandler::isWebSocketEnabled(int usersfd)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return false;
  }
  return currentUser->isWebSocketProtocol;
}
void cUserHandler::setWebSocketEnabled(int usersfd,bool enabled)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    return;
  }
  currentUser->isWebSocketProtocol = enabled;
}
void cUserHandler::sendWebSocketHandshake(int usersfd,char *challenge)
{
  sMainUsers *currentUser = getUserByFD(usersfd);
  if(currentUser == NULL)
  {
    printf("user not found!\n");
    return;
  }

  if(currentUser->secWebSocketKey1 == NULL || currentUser->secWebSocketKey2 == NULL || challenge == NULL)
  {
    printf("Key blank!\n");
    return;
  }


  int numberOfSpacesInKey1,numberOfSpacesInKey2;
  numberOfSpacesInKey1 = 0;
  numberOfSpacesInKey2 = 0;

  for(int counter=0;counter < strlen(currentUser->secWebSocketKey1);counter++)
  {
    if(currentUser->secWebSocketKey1[counter] == ' ')
    {
      numberOfSpacesInKey1++;
    }
  }
  for(int counter=0;counter < strlen(currentUser->secWebSocketKey2);counter++)
  {
    if(currentUser->secWebSocketKey2[counter] == ' ')
    {
      numberOfSpacesInKey2++;
    }
  }
printf("key1: %s - spaces: %d\n",currentUser->secWebSocketKey1,numberOfSpacesInKey1);
printf("key2: %s - spaces: %d\n",currentUser->secWebSocketKey2,numberOfSpacesInKey2);
  char *key1_numbers = new char[strlen(currentUser->secWebSocketKey1)];
  memset(key1_numbers,0,strlen(currentUser->secWebSocketKey1));
  char *key2_numbers = new char[strlen(currentUser->secWebSocketKey2)];
  memset(key2_numbers,0,strlen(currentUser->secWebSocketKey2));

  for(int counter=0;counter < strlen(currentUser->secWebSocketKey1);counter++)
  {
    if(int(currentUser->secWebSocketKey1[counter]) >= int('0') && int(currentUser->secWebSocketKey1[counter]) <= int('9'))
    {
      int char_count=0;
      while(key1_numbers[char_count] != '\0') { char_count++;}
      key1_numbers[char_count] = currentUser->secWebSocketKey1[counter];
    }
  }
  for(int counter=0;counter < strlen(currentUser->secWebSocketKey2);counter++)
  {
    if(int(currentUser->secWebSocketKey2[counter]) >= int('0') && int(currentUser->secWebSocketKey2[counter]) <= int('9'))
    {
      int char_count=0;
      while(key2_numbers[char_count] != '\0') { char_count++;}
      key2_numbers[char_count] = currentUser->secWebSocketKey2[counter];
    }
  }


  unsigned int key1Integer = atoi(key1_numbers)/numberOfSpacesInKey1;
  unsigned int key2Integer = atoi(key2_numbers)/numberOfSpacesInKey2;
  printf("Key1 Numbers: %d\n",key1Integer);
  printf("Key2 Numbers: %d\n",key2Integer);
  delete [] key1_numbers;
  delete [] key2_numbers;

  unsigned char md5data[16];
  memset(&md5data,0,16);
  md5data[0] = ((unsigned char *)&key1Integer)[3];
  md5data[1] = ((unsigned char *)&key1Integer)[2];
  md5data[2] = ((unsigned char *)&key1Integer)[1];
  md5data[3] = ((unsigned char *)&key1Integer)[0];
  md5data[4] = ((unsigned char *)&key2Integer)[3];
  md5data[5] = ((unsigned char *)&key2Integer)[2];
  md5data[6] = ((unsigned char *)&key2Integer)[1];
  md5data[7] = ((unsigned char *)&key2Integer)[0];
  memcpy(&(md5data[8]),challenge,8);
  unsigned char *hash = MD5(&(md5data[0]),16,NULL);

  printf("HASH CALCULATED!\n");
  char *buffer = new char[1028];
  memset(buffer,0,1028);
  sprintf(buffer,"HTTP/1.1 101 WebSocket Protocol Handshake\r\nUpgrade: WebSocket\r\nConnection: Upgrade\r\nSec-WebSocket-Origin: %s\r\nSec-WebSocket-Location: ws://%s/\r\nSec-WebSocket-Protocol: sample\r\n\r\n",currentUser->webSocketOrigin,currentUser->webSocketHost,currentUser->webSocketProtocol);
  int buffer_size = strlen(buffer);
  memcpy(&(buffer[strlen(buffer)]),hash,16);
  printf("WebSocket Out: \n%s",buffer);
  Socket->cs_write(currentUser->sfd,buffer,buffer_size+16);
  delete [] buffer;
  
}
sMainUsers* cUserHandler::getUserByFD(int usersfd)
{
	sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser;
		tempuser = tempuser->next;
	}
	
	return NULL;
}
bool cUserHandler::SetPermissions(int fromsfd,int usersfd,int DefaultPermissions)
{
	
	return true;
}
bool cUserHandler::SetAcceptVoice(int usersfd,bool acceptvoice)
{
	return true;
}
bool cUserHandler::SetProfile(int usersfd,char *text,bool append)
{
	return true;
}
bool cUserHandler::IsConnected(char *username)
{
	if(username == NULL)
		return false;
	sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(!strcmp(username,tempuser->username))
			return true;
		tempuser = tempuser->next;
	}
	
	return false;
}
int cUserHandler::GetSFD(char *username)
{
//if user does not exist return -1;
	if(username == NULL)
		return -1;
	sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(!strcmp(username,tempuser->username))
			return tempuser->sfd;
		tempuser = tempuser->next;
	}
	return -1;
}
char* cUserHandler::GetUserName(int usersfd)
{
	
	sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser->username;
		tempuser = tempuser->next;
	}
	
	return NULL;
}
char* cUserHandler::GetHostName(int usersfd)
{
		sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser->hostname;
		tempuser = tempuser->next;
	}
	
	return NULL;
}
int cUserHandler::GetPermissions(int usersfd)
{
	sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser->DefaultPermissions;
		tempuser = tempuser->next;
	}
	
	return 0;
}
char* cUserHandler::GetProfile(int usersfd,int num)
{
	return NULL;
}
bool cUserHandler::GetAcceptVoice(int usersfd)
{		sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser->acceptvoice;
		tempuser = tempuser->next;
	}
	
	return false;
}
bool cUserHandler::GetReservedStatus(int usersfd)
{		sMainUsers *tempuser = GetUser(0);
	for(;;)
	{
		if(tempuser == NULL)
			break;
		if(tempuser->sfd == usersfd)
			return tempuser->reserved;
		tempuser = tempuser->next;
	}
	
	return false;
}
bool cUserHandler::SendMessage(char *username,char *msg)
{
	if(username == NULL)
		return false;
	int *TheirSFD = new int;
	*TheirSFD = GetSFD(username);
	if(*TheirSFD == -1)
		return false;
	Socket->cs_write(*TheirSFD,msg,strlen(msg));
	
	return true;
}

//Kicking and Banning Functions
bool cUserHandler::BanUser(int fromsfd,int usersfd,int minutes,char *reason)
{
	return true;
}
bool cUserHandler::UnBanUser(int fromsfd,char *username)
{
	return true;
}
bool cUserHandler::KickUser(int fromsfd,int usersfd,char *reason)
{
	return true;
}

//File Share Functions
int cUserHandler::GetTotalFiles(int usersfd)
{
	return 0;
}
void cUserHandler::GetFileList(int usersfd,int *FileID,char *Filename,float *size,int num)
{
	return;
}
int cUserHandler::GetFileID(int usersfd,char *Filename)
{
	return 0;
}
bool cUserHandler::AddFile(int usersfd,char *Filename,float size)
{
	return true;
}
bool cUserHandler::RemoveFile(int usersfd,char *Filename)
{
	return true;
}

//Reserved User Management Functions
bool cUserHandler::AddReservedUser(int fromsfd,char *username,char *password,int DefaultPermissions,bool acceptvoice)
{
	return true;
}
bool cUserHandler::RemoveReservedUser(int fromsfd,char *username)
{
	return true;
}
bool cUserHandler::IsReservedUserName(char *username)
{

	if(username == NULL)
		return false;
	info("cUserHandler::IsReservedUserName","In Function");
	FILE *Users = NULL;
	Users = fopen(userfile,"r");
	info("cUserHandler::IsReservedUserName","UserFile Opened");
	char *buffer = new char[255];
	info("cUserHandler::IsReservedUserName","Buffer Created");
	while(!feof(Users))
	{
		info("cUserHandler::IsReservedUserName","Cleaning Buffer");
		memset(buffer,0,255);
		info("cUserHandler::IsReservedUserName","Reading Line");
		fgets(buffer,255,Users);
		if(feof(Users))
		{
			break;
		}		

		printf("Buffer = %s\n",buffer);
		if(buffer[0] != '#')
		{
			info("cUserHandler::IsReservedUserName","Parsing Line");
			buffer[strlen(buffer)-1] = '\0';
			
			char *ArgumentValue = GetArg(0,buffer,':');
			info("cUserHandler::IsReservedUserName","UserName stored to Temporary String");
			
			if(!strcmp(ArgumentValue,username))
			{
				info("cUserHandler::IsReservedUserName","User Is Reserved");
				delete [] ArgumentValue;
				delete [] buffer;
				return true;
			}
			delete [] ArgumentValue;
			info("cUserHandler::IsReservedUserName","Temporary String Deleted");
		}
	}
	delete [] buffer;
	info("cUserHandler::IsReservedUserName","Buffer Deleted");
	fclose(Users);
	info("cUserHandler::IsReservedUserName","File Closed");
	
	return false;
}
bool cUserHandler::SetPassword(int usersfd,char *newpassword)
{
	return true;
}
