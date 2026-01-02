
/****************************************************************/
/*		FileName: cSCore.cpp				*/
/*		Author: David Estes(Chrono)			*/
/*		CopyRight 2004,2005 David Estes			*/
/*		Under the GPL License				*/
/*		FOR NON-COMMERCIAL USE ONLY			*/
/****************************************************************/
//====================
//Revisement History |
//====================
//02/23/2005 -- Moved Several Functions into cSCore-2.cpp to make file smaller
//
//
//====================
#include"cSCore.h"
#include"getarg.h"
void cSCore::runcore(void)
{
	/*Lets Initialize Some Variables*/
	int *lsfd = new int; //Listening File Descriptor
	int *nsfd = new int; //Sfd For New Connections
	int returnval;
	int *maxfd = new int;
	char *buffer; //Temporary Message Buffer
	char *charbuffer = new char[1];
	struct sockaddr_in *newc = new sockaddr_in;
	fd_set *cset = new fd_set;
	fd_set *master = new fd_set;
	memset(charbuffer,0,1);
	*lsfd = Socket->cs_serve(Port);
	int *optval = new int;
	*optval = 1;
	setsockopt(*lsfd,SOL_SOCKET,SO_REUSEADDR,optval,sizeof(*optval));
	//FD_ZERO(cset);
	FD_ZERO(master);
	FD_ZERO(cset);
	FD_SET(*lsfd, master); //Add the listening sfd to the FD set
	*maxfd = *lsfd;
	sMainUsers *tempuser;
	while(enabled)
	{

		*cset = *master;	
		select((*maxfd)+1,cset,NULL,NULL,NULL);
		if(FD_ISSET(*lsfd, cset)) //If It is a New Connection
		{
			*nsfd = accept(*lsfd,NULL,(socklen_t *)sizeof(*newc));
			ErrorHandler->ProduceError(0,"cSCore::runcore()","New Connection Received");
			
			// Handle SSL handshake if SSL is enabled
			if(Socket->is_ssl_enabled())
			{
				if(Socket->cs_accept_ssl(*lsfd, *nsfd) < 0)
				{
					ErrorHandler->ProduceError(0,"cSCore::runcore()","SSL handshake failed");
					close(*nsfd);
					continue;
				}
			}
			
			if(!UserHandler->AddUser(*nsfd,"unsupported"))
			{
				ErrorHandler->ProduceError(0,"cSCore::runcore()","Failed To Add User");
				if(Socket->is_ssl_enabled())
					Socket->cs_close_ssl(*nsfd);
				close(*nsfd);
			}
			else
			{
				info("cSCore::runcore()","Adding to FD Set");
			FD_SET(*nsfd,master);
			if(*nsfd > *maxfd)
				*maxfd = *nsfd;
			}
				
		}
		else
		{
			tempuser = UserHandler->GetUser(0);
			//ErrorHandler->ProduceError(0,"cSCore::runcore()","UserPtr Caught");
			while(tempuser != NULL)
			{
				//ErrorHandler->ProduceError(0,"cSCore::runcore()","looping list");
				/*A User wants to send something*/
				if(FD_ISSET(tempuser->sfd,cset) && tempuser->sfd != *lsfd)
				{
					*nsfd = tempuser->sfd;
					//ErrorHandler->ProduceError(0,"cSCore::runcore()","reading data");
					returnval = Socket->cs_read(*nsfd,charbuffer,1);
					//Check For Disconnects
					if(returnval == 0)
					{
						
						
						ChannelHandler->UserDisconnected(*nsfd);
						info("cSCore::runcore()","User Disconnected From Channels");
						UserHandler->RemoveUser(*nsfd);
						info("cSCore::runcore()","User Disconnected");
						
						if(Socket->is_ssl_enabled())
							Socket->cs_close_ssl(*nsfd);
						close(*nsfd);
						FD_CLR(*nsfd,master);
						if(UserHandler->numusers == 0)
						{
							FD_ZERO(master);
							FD_SET(*lsfd,master);
						}
						break;
						
					}
					else
					{
            if(tempuser->isWebSocketProtocol)
            {
             if(tempuser->webSocketByteCount == 0 && tempuser->UserBuffer->strsize == 7)
             {
               printf("We Have the Challenge!");
              tempuser->webSocketByteCount = 7;
             }
             {
             }
            }
            if(tempuser->webSocketByteCount == 8 && int(charbuffer[0]) == 0)
            {
							(tempuser->UserBuffer->done) = true;
            }
            else if((tempuser->webSocketByteCount < 0 || tempuser->webSocketByteCount == 8) && (int(charbuffer[0]) == -1 || charbuffer[0] == '\n') ) //Byte 255
						{
							  
							  this->parsecmd(*nsfd,tempuser->UserBuffer->buffer);
              if(tempuser->UserBuffer->strsize == 1 && tempuser->isWebSocketProtocol && tempuser->webSocketByteCount < 0)
              {
                tempuser->webSocketByteCount = 0;
              }
							(tempuser->UserBuffer->done) = true;

						}
						else if((tempuser->UserBuffer->strsize) < ((tempuser->UserBuffer->buffersize) - 1))
						{
							tempuser->UserBuffer->buffer[(tempuser->UserBuffer->strsize)] = charbuffer[0];
							(tempuser->UserBuffer->strsize)++;
						}
					else if((tempuser->UserBuffer->buffersize) < 131072)
					{
						(tempuser->UserBuffer->buffersize) += 512;
							buffer = new char[(tempuser->UserBuffer->buffersize)];
							memset(buffer,0,(tempuser->UserBuffer->buffersize));
							char *temp = tempuser->UserBuffer->buffer;
							strcpy(buffer,temp);
							buffer[(tempuser->UserBuffer->strsize)] = charbuffer[0];
							(tempuser->UserBuffer->strsize)++;
							tempuser->UserBuffer->buffer = buffer;
							delete [] temp;
							
						}
						else
						{
							(tempuser->UserBuffer->done) = true;
						}
            if(tempuser->isWebSocketProtocol && tempuser->webSocketByteCount == 7)
            {
              tempuser->webSocketByteCount = 8;
              printf("TIME TO PARSE CHALLENGE: %s\n",tempuser->UserBuffer->buffer); 
              UserHandler->sendWebSocketHandshake(tempuser->sfd,tempuser->UserBuffer->buffer);
							(tempuser->UserBuffer->done) = true;
            }
						
						if((tempuser->UserBuffer->done) == true)
						{
							char *temp = tempuser->UserBuffer->buffer;
							tempuser->UserBuffer->buffer = new char[10];
							memset(tempuser->UserBuffer->buffer,0,10);
							(tempuser->UserBuffer->strsize) = 0;
							(tempuser->UserBuffer->buffersize) = 10;
							(tempuser->UserBuffer->done) = false;
							delete [] temp;
						}
					
					
					}
					break;
				}
				tempuser=tempuser->next;
			}

		}
		
	}
	close(*lsfd);
	delete master;
	delete cset;
	delete nsfd;
	delete maxfd;
	delete lsfd;
	delete optval;
	delete newc;
	delete [] charbuffer;
}

