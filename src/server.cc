#include "chatapp/server.h"

string ChatServer::Message;

int ChatServer::setup(int port)
{
   // Setup a IPv4 TCP connection
   // socket()
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sockfd == -1)
   {
      cout << "Failed to create socket..." << endl;
      exit(SOCK_CREATE_FAILURE);
   }
   else
      cout << "Created socket..." << endl;

   // Zero out server address
   memset(&serverAddress, 0, sizeof(serverAddress));

   // Assign IP address and port
   serverAddress.sin_family = AF_INET;
   serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
   serverAddress.sin_port=htons(port);

   // Bind new socket to IP address and verify successful
   // bind()
   if( bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) != 0)
   {
      cout << "Failed to bind socket..." << endl;
      exit(SOCK_BIND_FAILURE);
   }
   else
      cout << "Socket binded..." << endl;

   // Server now ready to listen for new connections
   // listen()
   if( listen(sockfd, 5) != 0)
   {
      cout << "Listening for connections failed..."  << endl;
      exit(SOCK_LISTEN_FAILURE);
   }
   else
      cout << "Listening for connections..." << endl;

   return 0;
}

void ChatServer::Task(int arg)
{
	int n;
	int newsockfd = arg;
	char msg[MAXPACKET_SIZE];

	while(1)
	{
		n = recv(newsockfd, msg, MAXPACKET_SIZE, 0);
		if(n==0)
		{
		   close(newsockfd);
		   break;
		}
		msg[n]=0;
		Message = string(msg);
                // Do something with message
	 }
}

void ChatServer::ready()
{
	socklen_t sosize  = sizeof(clientAddress);
	while(true)
	{
                // Accept connection-based sockets  
		newsockfd = accept(sockfd,(struct sockaddr*)&clientAddress,&sosize);

		string str = inet_ntoa(clientAddress.sin_addr);
                cout << "Accepted new connection: " << str << 
                        ", Port: " << clientAddress.sin_port << endl;
		std::thread newThread(&ChatServer::Task, newsockfd);
		newThread.detach();
	}
}

string ChatServer::getMessage()
{
	return Message;
}

void ChatServer::Send(string msg)
{
	send(newsockfd,msg.c_str(),msg.length(),0);
}

void ChatServer::clean()
{
	Message = "";
	memset(msg, 0, MAXPACKET_SIZE);
}

void ChatServer::detach()
{
	close(sockfd);
	close(newsockfd);
} 
