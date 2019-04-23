/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * ChatClient implementation file
 */

#include "chatapp/client.h"

// Constructor
ChatClient::ChatClient()
{
   sockfd = -1;
}

// Setup
// Setup a TCP socket and attempt to connect to the server
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

  // Adding server address and port to server sockaddr_in
  if(inet_aton(address.c_str(), &server.sin_addr) == 0)
  {
     cout << "Failed to convert address string to IPv4" << endl;
     return false;
  }
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  // Connecting to the server
  // connect()
  cout << "Establishing connection to the server..." << endl;
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

// Send
// Method for sending messages from this ChatClient
bool ChatClient::Send(string message)
{
   // If file descriptor established
   if(sockfd != -1)
   {
      // Return true if send message successful
      if(send(sockfd, message.c_str(), strlen(message.c_str()), 0) != -1)
         return true;
   }
   return false;
}

// Receive
// Method for receiving messages at this ChatClient
string ChatClient::Receive(int size)
{
        // Create buffer for message
        char buffer[size];
	memset(&buffer[0], 0, sizeof(buffer));

        // Receive the message or return null pointer
  	string reply;
	if(recv(sockfd , buffer , size, 0) < 0)
  	{
	    	cout << "receive failed!" << endl;
		return nullptr;
  	}
	buffer[size-1]='\0';
  	return buffer;
}

// Destroy
// Properly closes open sockets
void ChatClient::destroy()
{
   cout << "Closing client and socket..." << endl;
   close(sockfd);
}
