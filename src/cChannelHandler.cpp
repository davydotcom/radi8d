/***************************************************
*		Name:cChannelHandler.cpp	   *
*	Author: David Estes(chronom1) 		   *
*		Jason Kielpenski(defiant_pngn)	   *
*						   *
****************************************************/


#include"cChannelHandler.h"
sChannels* cChannelHandler::GetChannel(int num) //Returns Ptr to sChannels
{
	if(num == 0)
		return ChannelList; //Return Primary ChannelList
	sChannels *temp = ChannelList;
	for(int x=0;x<num;x++)
	{
		temp = temp->next;
		if(temp == NULL)
			break;
	}
	return temp; //Return Pointer To A sChannels Object
}
sBanList* cChannelHandler::GetBan(int num,char *ChannelName) //Gets Ptr To a sBanList object
{

sChannels *tempchan = GetChannel(0); //Temporary Channel Pointer
	for(;;) //Find Channel Object With Specified ChannelName
	{
		if(tempchan == NULL) //Channel Does Not Exist
			return NULL;
		if(!strcmp(tempchan->ChannelName,ChannelName)) //Channel Found
			break;
		tempchan = tempchan->next; //Move ptr to next Object
	}

	if(num == 0) //Just Return Primary BanList Object
		return tempchan->BanList;
	sBanList *temp = tempchan->BanList; //Create a temporary banlist pointer
	for(int x=0;x<num;x++) //Loop Until num is found
	{
		temp = temp->next;
		if(temp == NULL)
			break;
	}
	return temp; //Return Pointer
}

sUserList* cChannelHandler::GetUser(int num,char *ChannelName) //Gets sUserList ptr for usernum in Channel
{
sChannels *tempchan; //Temporary Channel Pointer
	tempchan = GetChannel(0); //Set Pointer Equal To ChannelList
	for(;;)
	{
		if(tempchan == NULL) //Channel Doesn't Exist If This Is Hit
			return NULL;
		if(!strcmp(tempchan->ChannelName,ChannelName)) //Channel Found. Break out
			break;
		tempchan = tempchan->next;
	}

	if(num == 0) //If the usernum is 0 than just return UserList
		return tempchan->UserList;
	sUserList *temp = tempchan->UserList; //Otherwise Create temp ptr to UserList and increment.
	for(int x=0;x<num;x++) //Loop through the number specified
	{
		temp = temp->next;
		if(temp == NULL) 
			break;
	}
	return temp; //Return Pointer
}

bool cChannelHandler::CreateChannel(char *ChannelName,char *Password,char *Topic,int chantype,bool ReservedOnly, bool RequireSpeakPermissions)
{
	if(maxchannels < numchannels) //Check To Makser Sure maxchannels has not been reached
		return false;
	if(ChannelName == NULL) //Make Sure A ChannelName was Specified
	{
		return false;
	}
	if(IsAChannel(ChannelName) != -1 || strlen(ChannelName) > 25 || !strcmp(ChannelName,"user")) //Check To Make Sure Channel Does not already exist, the Name does not exceed 25chars, and the ChannelName does not equal "user"
	{
		return false;
	}
	if(ChannelList == NULL) //If No Channels Currently Exist.
	{
		ChannelList = new sChannels; //Create New sChannels Object
		ptrLastChannel = ChannelList; //Set ptr value for the ptr to last channel
	}
	else //If Other Channels Do Exist
	{
		ptrLastChannel->next = new sChannels; //Create New sChannels Object
		ptrLastChannel = ptrLastChannel->next; //Move ptrLastChannel ptr
		
	}
	ptrLastChannel->ChannelName = new char[strlen(ChannelName)+1]; //Create ChannelName
	strcpy(ptrLastChannel->ChannelName,ChannelName); //Copy ChannelName
	if(Password != NULL) //If Password Field Was Not Set To NULL Than Set It
	{
		ptrLastChannel->Password = new char[strlen(Password)+1];
		
		strcpy(ptrLastChannel->Password,Password);
	}
	if(Topic != NULL) //If Topic Field Was Not Set To NULL Than Set It
	{
		ptrLastChannel->Topic = new char[strlen(Topic)+1];
		strcpy(ptrLastChannel->Topic,Topic);
	}
	ptrLastChannel->chantype = chantype; //Set chantype
	ptrLastChannel->ReservedOnly = ReservedOnly; //Set ReservedOnly
	ptrLastChannel->RequireSpeakPermissions = RequireSpeakPermissions; //SetRequireSPeak
	ptrLastChannel->codec = defaultcodec; //Set codec
	numchannels++; //Increment Number of Channels
	return true; //Success!!
}
bool cChannelHandler::RemoveChannel(char *ChannelName) //Used To Remove A Channel
{
	int *channum = new int; //Integer used for Storing Channel Number
	*channum = IsAChannel(ChannelName); //Checks for channels existence. If exists returns channel Number
	if(*channum == -1) //If Channel Does Not Exist Than delete channum and return false
	{	
		delete channum;
		return false;
	}
	sUserList *temp; //Create a Temporary UserList Pointer
	for(;;) //This Loop Loops Through The UserList Objects Until All Users Have Been Kicked From The Current Channel.
	{
		temp = GetUser(0,ChannelName); //Get Pointer To Primary UserList Object
		if(temp == NULL) //If All Users Have Been Kicked Than Exit
			break;

		KickUser(-1,ChannelName,temp->sfd,"Channel is Being Removed"); //Kick The User
	}
	sChannels *tempchan; //Create Temporary Channel Pointers
	sChannels *tempchan2; //Same as Above
	tempchan = GetChannel(*channum); //Get Pointer To Channel Marked For removal
	if(*channum == 0) //If Channel Is First in List
	{
		tempchan = ChannelList->next; //Get ptr to Next Channel
		ChannelList->next=NULL; //Set next ptr to NULL to prevent removal of other channels
		delete ChannelList; //Delete First Channel in List
		ChannelList = tempchan; //Change ChannelList to point to the next Channel
		numchannels--; //Decrement Number of Channels
	}
	else //If It Is SomeWhere Else in The List
	{
	tempchan2 = tempchan->next; //tempchan2 = Object After Marked Channel
	tempchan->next = NULL; //Set next value to NULL to prevent other removals
	delete tempchan; //Delete Channel Object
	GetChannel((*channum) - 1)->next = tempchan2; //Set The previous channel's next ptr to tempchan2
	numchannels--; //Decrement Number of Channels
	}

	tempchan2 = GetChannel(0); //Get Pointer to ChannelList
	if(tempchan2 == NULL) //If it Is NULL than set ptrLastChannel to NULL
		ptrLastChannel = NULL;
	else
	{
	for(;;) //OtherWise Loop Through until LastChannel Is Found
	{
		if(tempchan2->next == NULL) //Last Channel Found
		{
			ptrLastChannel = tempchan2; //Set ptrLastChannel
			break;
		}
		tempchan2 = tempchan2->next; //Increment through Linked List
	}

	}
	delete channum; //Delete channum integer
	return true; //Success
}
int cChannelHandler::JoinChannel(int usersfd,char *ChannelName,char *Password)
{
	info("cChannelHandler::JoinChannel","In Function");
	if(IsAChannel(ChannelName) == -1) //Check If Channel Exists
		return -1; //Channel Does Not Exist
	if(IsInChannel(usersfd,ChannelName) != -1) //Checks If Already In Channel
		return -2; //Already In Channel
	sChannels *temp = GetChannel(0); //Pointer to ChannelList
	for(;;) //Loop Until Channel Object Found
	{
		if(temp == NULL) //If Occurs Than Channel Does Not Exist
			return -4; //Unknown Error
		if(!strcmp(temp->ChannelName,ChannelName)) //Channel Object Found
			break;
		temp = temp->next; //Increment Through Linked List
	}
	if(temp->ReservedOnly) //Checks If The Channel Is A ReservedOnly Channel
	{
		if(!UserHandler->GetReservedStatus(usersfd)) //Gets Users Reserved Status
			return -5; //Not A Reserved User
	}
	if(temp->Password != NULL) //If Not A Public Channel Than Check For Password Value
	{
	if(Password == NULL)
		return -3; //Invalid Password
	if(strcmp(temp->Password,Password))
		return -3; //Invalid Password
	}
	info("cChannelHandler::JoinCHannel","Join is Possible");
	sUserList *tempuser;
	if(temp->UserList == NULL)
	{
		temp->UserList = new sUserList;
		temp->UserList->sfd = usersfd;
		int *newperm = new int;
		*newperm = UserHandler->GetPermissions(usersfd);
		if(*newperm > 777)
			temp->UserList->permissions = *newperm;
		else
			temp->UserList->permissions = 777; //Admin
		delete newperm;
		char *UserName = UserHandler->GetUserName(usersfd);
		info("cChannelHandler::JoinCHannel","Creating Temp Buffer");
		char *buffer = new char[67];
		info("cChannelHandler::JoinChannel","Buffer Created");
		bzero(buffer,67);
		sprintf(buffer,"!usrjoind:%s:%s:%d:%d",ChannelName,UserName,temp->UserList->permissions,UserHandler->GetAcceptVoice(usersfd));
		SendToChannel(usersfd,ChannelName,buffer);
		delete [] buffer;
		return 0;
	}
	for(int x=0;;x++)
	{
		tempuser = GetUser(x,ChannelName);
		if(tempuser->next == NULL)
		{
			tempuser->next = new sUserList;
			tempuser->next->sfd = usersfd;
			int *newperm = new int;
			*newperm = UserHandler->GetPermissions(usersfd);
			tempuser->next->permissions = *newperm;
			delete newperm;
			char *UserName = UserHandler->GetUserName(usersfd);
		char *buffer = new char[strlen(UserName)+17+2+strlen(ChannelName)];
		bzero(buffer,strlen(UserName)+19+strlen(ChannelName));
		sprintf(buffer,"!usrjoind:%s:%s:%d:%d",ChannelName,UserName,tempuser->next->permissions,UserHandler->GetAcceptVoice(usersfd));
		SendToChannel(usersfd,ChannelName,buffer);
		delete [] buffer;
			break;
		}
	}
	return 0;
}
bool cChannelHandler::LeaveChannel(int usersfd,char *ChannelName)
{
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sUserList *temp;
	sUserList *temp2;
sChannels *tempchan = GetChannel(*channum);
	if(tempchan->UserList->sfd == usersfd)
	{
		temp = tempchan->UserList->next;
		tempchan->UserList->next = NULL;
		delete tempchan->UserList;
		tempchan->UserList = temp;
	}
	else
	{
		temp = GetUser(*usernum,ChannelName);
		temp2 = temp->next;
		temp->next = NULL;
		GetUser((*usernum-1),ChannelName)->next = temp2;
		delete temp;
	}
	char *UserName = UserHandler->GetUserName(usersfd);
		char *buffer = new char[strlen(UserName)+19+strlen(ChannelName)];
		bzero(buffer,strlen(UserName)+19+strlen(ChannelName));
		sprintf(buffer,"!usrleft:%s:%s",ChannelName,UserName);
		SendToChannel(-1,ChannelName,buffer);
		delete [] buffer;
	if(GetUser(0,ChannelName) == NULL)
		RemoveChannel(ChannelName);
	delete usernum;
	delete channum;
	return true;
}
int cChannelHandler::IsAChannel(char *ChannelName)
{
	if(GetChannel(0) == NULL)
	{
		return -1;
	}
	sChannels *tempchan;
	tempchan = GetChannel(0);
	for(int x=0;;x++)
	{
		if(tempchan == NULL)
		{
			return -1;
		}
		if(!strcmp(tempchan->ChannelName,ChannelName))
			return x;
		tempchan=tempchan->next;
	}
}
int cChannelHandler::IsInChannel(int usersfd,char *ChannelName)
{
	sUserList *tempuser;
	for(int x=0;;x++)
	{
		tempuser = GetUser(x,ChannelName);
		if(tempuser == NULL)
			return -1;
		if(tempuser->sfd == usersfd)
			return x;
	}
}
bool cChannelHandler::SendToChannel(int usersfd,char *ChannelName,char *msg)
{
	if(msg == NULL)
		return false;
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	if(usersfd != -1)
	{
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = temp->UserList;
	for(;;)
	{
		if(tempuser == NULL)
			break;
		printf("looping\n");
		if(tempuser->sfd != usersfd)
		{
			printf("SFD: %d\n",tempuser->sfd);
			Socket->cs_write(tempuser->sfd,msg,strlen(msg));
			
		}
		
		tempuser = tempuser->next;
		info("cChannelHandler::SendToChannel","Next Object");
	}
	info("cChannelHandler::SendToChannel","Out of Loop");
	delete channum;
	delete usernum;
	info("cChannelHandler::SendToChannel","Returning Success");
	return true;
}
char* cChannelHandler::GetUserList(char *ChannelName,int num)
{
	sUserList *temp = GetUser(num,ChannelName);
	if(temp == NULL)
		return NULL;

	return UserHandler->GetUserName(temp->sfd);
}
char* cChannelHandler::GetChannelList(int num)
{
	sChannels *temp = GetChannel(num);
	if(temp == NULL)
		return NULL;
	return temp->ChannelName;
}
bool cChannelHandler::BanUser(int fromsfd,char *ChannelName,char *username,char *hostname,int minutes,char *reason)
{
	return true;	
}
bool cChannelHandler::UnBanUser(int fromsfd,char *ChannelName,char *username)
{
	return true;
}
bool cChannelHandler::KickUser(int fromsfd,char *ChannelName, int usersfd,char *reason)
{
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sUserList *temp;
	sUserList *temp2;
	if(fromsfd != -1)
	{
		int *fusernum = new int;
		*fusernum = IsInChannel(fromsfd,ChannelName);
		if(*fusernum == -1)
		{
			delete channum;
			delete usernum;
			delete fusernum;
			return false;
		}
		temp = GetUser(*fusernum,ChannelName);
		if(temp->permissions < 777)
		{
			delete channum;
			delete usernum;
			delete fusernum;
			return false;
		}
		delete fusernum;
		

	}
	char *buffer = new char[13+strlen(ChannelName)+strlen(reason)];
	sprintf(buffer,"!die:%s:kick:%s\0",ChannelName,reason);
	Socket->cs_write(usersfd,buffer,strlen(buffer));
	delete [] buffer;

	sChannels *tempchan;
	tempchan=GetChannel(*channum);
	if(tempchan->UserList->sfd == usersfd)
	{
		temp = tempchan->UserList->next;
		tempchan->UserList->next = NULL;
		delete tempchan->UserList;
		tempchan->UserList = temp;
		delete usernum;
		delete channum;
		return true;
	}
	temp = tempchan->UserList;
	for(int x=0;;x++)
	{
		temp2 = temp;
		temp = temp->next;
		if(temp == NULL)
		{
			delete channum;
			delete usernum;
			return false;
		}
		if(temp->sfd == usersfd)
		{
			sUserList *temp3 = temp->next;
			temp->next = NULL;
			temp2->next = temp3;
			break;
		}
	}
	delete channum;
	delete usernum;
	return true;
}
char* cChannelHandler::GetTopic(char *ChannelName)
{
	int channum;
	channum = IsAChannel(ChannelName);
	if(channum == -1)
	{
		return NULL;
	}
	return GetChannel(channum)->Topic;
}
bool cChannelHandler::SetTopic(int usersfd,char *ChannelName,char *Topic)
{
	info("cChannelHandler::SetTopic","Entering Function");
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	info("cChannelHandler::SetTopic","Deleting Temporary Vars");
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	if(strlen(Topic)> 150)
		return false;
	if(temp->Topic != NULL)
		delete [] temp->Topic;
	info("cChannelHandler::SetTopic","Setting Topic In Channel Struct");		
	temp->Topic = new char[strlen(Topic)+1];
	memset(temp->Topic,0,strlen(Topic)+1);
	strcpy(temp->Topic,Topic);

	
	return true;
}
bool cChannelHandler::SetPassword(int usersfd,char *ChannelName,char *Password)
{
		int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	if(temp->Password != NULL)
		delete [] temp->Password;
	temp->Password = new char[strlen(Password)];
	strcpy(temp->Password,Password);

	
	return true;
}
bool cChannelHandler::SetChanType(int usersfd,char *ChannelName,int chantype)
{
		int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	if(chantype >= 0 && chantype < 3)
		temp->chantype = chantype;
	else
		return false;
	
	return true;
}
bool cChannelHandler::SetCodec(int usersfd,char *ChannelName,int codec)
{
		int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	if(codec > 0 && codec < 6)
		temp->codec = codec;
	else
		return false;
	
	return true;
}
bool cChannelHandler::SetMaxUsers(int usersfd,char *ChannelName,int maxusers)
{
	if(maxusers <= 1)
		return false;
	if(maxusers > this->maxusers)
		return false;
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	temp->maxusers = maxusers;
	
	return true;
}
bool cChannelHandler::SetReservedOnly(int usersfd,char *ChannelName,bool ReservedOnly)
{
		int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	temp->ReservedOnly = ReservedOnly;
	
	return true;
}
bool cChannelHandler::SetRequireSpeakPermissions(int usersfd,char *ChannelName,bool RequireSpeakPermissions)
{
	int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1)
	{
		delete channum;
		delete usernum;
		return false;
	}
	sChannels *temp = GetChannel(*channum);
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	delete channum;
	delete usernum;
	if(tempuser->permissions < 777)
	{
		return false;
	}
	temp->RequireSpeakPermissions = RequireSpeakPermissions;
	
	return true;
}
bool cChannelHandler::SetUserPermissions(int fromsfd,char *ChannelName, int usersfd,int permissions)
{
		int *channum = new int;
	*channum = IsAChannel(ChannelName);
	if(*channum == -1)
	{
		delete channum;
		return false;
	}
	int *usernum = new int;
	int *fusernum = new int;
	*fusernum = IsInChannel(fromsfd,ChannelName);
	*usernum = IsInChannel(usersfd,ChannelName);
	if(*usernum == -1 || *fusernum == -1)
	{
		delete fusernum;
		delete channum;
		delete usernum;
		return false;
	}
	sUserList *tempuser = GetUser(*usernum,ChannelName);
	sUserList *tempfuser = GetUser(*fusernum,ChannelName);
	delete channum;
	delete usernum;
	delete fusernum;
	if(tempfuser->permissions < 777)
	{
		return false;
	}
	if(permissions > tempfuser->permissions)
		return false;
		
	tempuser->permissions = permissions;
	
	return true;
}
void cChannelHandler::UserDisconnected(int usersfd)
{
	sChannels *tempchan;
	tempchan = GetChannel(0);
	if(tempchan == NULL)
		return;
	while(tempchan != NULL)
	{

		if(LeaveChannel(usersfd,tempchan->ChannelName))
		{
			tempchan = GetChannel(0);
		}
		else
			tempchan=tempchan->next;
	}
	
}
void cChannelHandler::UserNameChange(int usersfd,char *OldUserName)
{
sChannels *tempchan;
	tempchan = GetChannel(0);
	char *ChanName = new char[255];
	char *NewUserName = UserHandler->GetUserName(usersfd);
	
	char *buffer = new char[61];
	sprintf(buffer,"!name:%s:%s",OldUserName,NewUserName);
	while(tempchan != NULL)
	{
		bzero(ChanName,255);
		strcpy(ChanName,tempchan->ChannelName);
		SendToChannel(-1,ChanName,buffer);
		
		tempchan=tempchan->next;
	}
	delete [] buffer;
	delete [] ChanName;
}
int cChannelHandler::GetNumberUsers(char *ChannelName)
{

	int *channum = new int;
	*channum = IsAChannel(ChannelName);
		if( *channum == -1) //Check If Channel Exists
		{
			delete channum;
			return -1; //Channel Does Not Exist
		}
	sChannels *tempchan = GetChannel(*channum);
	sUserList *tempuser = tempchan->UserList;
	delete channum;
	for(int x=0;;x++)
	{
		if(tempuser == NULL)
		{
			return x;
		}
		tempuser = tempuser->next;
	
	}
	
}
