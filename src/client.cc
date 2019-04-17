#include "chatapp/client.h"

// Chat Client Constructor
ChatClient::ChatClient()
{
   sockfd = -1;
   port = 0;
   address = "";
}

// Chat client 
bool ChatClient::setup(string address, int port)
{
  // For uninitialized client, setup endpoint for communication
  if(sockfd == -1)
  {
     // Setup a IPv4 TCP connection
     // socket()
     sockfd = socket(AF_INET , SOCK_STREAM , 0);

     // Check if socket creation successful
     if (sockfd < 0)
     {
   	cout << "Failed to create client socket." << endl;
        return false;
     }
     cout << "Created a new TCP socket with file descriptor: " << sockfd << endl;
  }

  cout << "Creating struct for socket address and port: " 
       << address << ":" << port << endl;
  int result = inet_aton(address.c_str(), &server.sin_addr);  

  // Check if successful
  if(result == 0)
  {
     cout << "Failed to convert address string to IPv4" << endl;
  }

  // Define server sockaddr_in members
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  // connect()
  int connfd = connect(sockfd, (struct sockaddr *)&server , sizeof(server));

  // Check if connection successful
  if(connfd < 0)
  {
     cout << "Failed to create connection to server." << endl;
     return false;
  }
  cout << "Connected to server!" << endl;

  return true;
}

//  
bool ChatClient::sendme(string message)
{
   if(sockfd != -1)
   {
      if(send(sockfd, message.c_str(), strlen(message.c_str()), 0) != -1)
         return true;
   }
   return false;
}

// Receive
string ChatClient::receive(int size)
{
        char buffer[size];
	memset(&buffer[0], 0, sizeof(buffer));

  	string reply;
	if( recv(sockfd , buffer , size, 0) < 0)
  	{
	    	cout << "receive failed!" << endl;
		return nullptr;
  	}
	buffer[size-1]='\0';
  	return buffer;
}

// Read
string ChatClient::read()
{
   char buffer[1] = {};
   string reply;

   while (buffer[0] != '\n') 
   {
      if( recv(sockfd , buffer , sizeof(buffer) , 0) < 0)
      {
         cout << "receive failed!" << endl;
	 return nullptr;
      }
      reply += buffer[0];
   }
   return reply;
}

void ChatClient::destroy()
{
   cout << "Closing client and socket..." << endl;
   close(sockfd);
}
