#include "chatapp/server.h"

std::map<string, User> ChatServer::users;

int ChatServer::setup(int port)
{
   rapidjson::Document d = JsonParser::Parse(USER_DB);

   const rapidjson::Value& userObjs = d["users"];
   for(rapidjson::SizeType i = 0; i < userObjs.Size(); i++)
   {
      string uname = userObjs[i].GetObject()["username"].GetString();
      string pword = userObjs[i].GetObject()["password"].GetString();
      
      users[uname] = User(uname, pword); 
   }

   cout << "Users:" << endl;
   for(std::map<string, User>::iterator it = users.begin(); it != users.end(); ++it)
   {
      cout << it->first <<  " - " << it->second.password << endl;
   } 


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

void ChatServer::Connection(int arg)
{
	int n;
	int clientfd = arg;
	char msg[MAXPACKET_SIZE];

	while(1)
	{
                // Clear previous message
	        memset(msg, 0, MAXPACKET_SIZE);
                // Get new message
		n = recv(clientfd, msg, MAXPACKET_SIZE, 0);
		if(n==0)
		{
                   cout << "CLOSING THIS CLIENT: " << to_string(clientfd) << endl;
		   close(clientfd);
		   break;
		}
                // Make sure buffer is null terminated
		msg[n]=0;
                // Print message received from client
                cout << string(msg) << endl;

                // HANDLE THE MESSAGE
                
                // Send reply 
                stringstream reply;
                reply << std::this_thread::get_id() << ": ACK";
                string str = reply.str();
                ChatServer::Send(str, clientfd);

	 }
}

void ChatServer::ready()
{
	socklen_t sosize  = sizeof(clientAddress);
	while(true)
	{
                // Accept connection-based sockets  
		int temp = accept(sockfd,(struct sockaddr*)&clientAddress,&sosize);

		string str = inet_ntoa(clientAddress.sin_addr);
                cout << "Accepted new connection: " << str << 
                        ", Port: " << clientAddress.sin_port << endl;
		std::thread newThread(&ChatServer::Connection, temp);
		newThread.detach();
	}
}

// Static method for sending messages to client
void ChatServer::Send(string msg, int clientfd)
{
	send(clientfd,msg.c_str(),msg.length(),0);
}

void ChatServer::detach()
{
	close(sockfd);
        //TODO: Need to close each connection fd
} 
