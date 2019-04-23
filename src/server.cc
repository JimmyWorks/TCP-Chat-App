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

   cout << "Demo users:" << endl;
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

                // Handle the request
                string reply = ChatServer::HandleMessage(msg, clientfd);
                if(!reply.empty())
                   ChatServer::Send(reply, clientfd);

	 }
}

string ChatServer::HandleMessage(string msg, int clientfd)
{
   string reply = "";

   ChatMessage obj = ChatMessage::deserialize(msg);
   int opcode = obj.opcode;
   int ack = obj.ack;
   string user1 = obj.user1;
   string user2 = obj.user2;
   string message = obj.message;

   cout << endl << "New packet:" << endl;
   cout << msg << endl;
   cout << "======================" << endl;
   cout << "op: " << opcode << endl;
   cout << "ack: " << ack << endl;
   cout << "user1: " << user1 << endl;
   cout << "user2: " << user2 << endl;
   cout << "message: " << message << endl;

   std::map<string, User>::iterator it;
   std::map<string, User>::iterator it2;
   switch(opcode)
   {
      case HEARTBEAT:
      {
         // Get list of users online
         string onlineusers = "";
         for(it = users.begin(); it != users.end(); ++it)
         {
            if(it->second.online)
            {
               onlineusers += it->first + ";";
            }
         } 
         reply = ChatMessage::serialize((int)HEARTBEAT, (int)ACK, user1, "", onlineusers);
         break;
      }

      case CONN_REQ:
      {
         // Check user and password
         cout << "Searching for user: " << user1 << endl;
         it = users.find(user1);
        
         if(it == users.end())
         {
            cout << "Could not find user..." << endl;
            reply = ChatMessage::serialize((int)CONN_REQ, (int)NACK, user1, "", "Invalid username.  Please try again...");
         }
         else if(it->second.password != message)
         {
            cout << "Invalid password..." << endl; 
            reply = ChatMessage::serialize((int)CONN_REQ, (int)NACK, user1, "", "Invalid password.  Please try again...");
         }
         else
         {
            cout << "Valid user login..." << endl;
            it->second.online = true;
            it->second.connfd = clientfd;
            reply = ChatMessage::serialize((int)CONN_REQ, (int)ACK); 
         }
         break;
      }

      case CONN_TERM:
      {
         // Turn user offline
         it = users.find(user1);

         if(it == users.end())
         {
            // CRITICAL ERROR
            cout << "CRITICAL ERROR: USER NOT FOUND FOR CONNECTION TERMINATION..." << endl;
            reply = ChatMessage::serialize((int)CONN_TERM, (int)NACK);
            break;
         }

         // Make user offline
         it->second.online = false;

         reply = ChatMessage::serialize((int)CONN_TERM, (int)ACK);
         break;
      }

      case CHAT_MESSAGE:
      {
         it2 = users.find(user2);

         if(it2 == users.end())
         {
            cout << "CHAT ERROR: User2 does not exist..." << endl;
            reply = user2 + " does not exist";
            reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)NACK, user1, user2, reply);
            break;
         }
         else if(!(it2->second.online))
         {
            cout << "User2 is not online..." << endl;
            reply = user2 + " is no longer online";
            reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)NACK, user1, user2, reply);
         }
         else
         {
            string msg = ChatMessage::serialize((int)CHAT_SMESSAGE, (int)ACK, user2, user1, message);
            ChatServer::Send(msg, it2->second.connfd);

            sem_wait(&(it2->second.sending_message));

            string err;
            sem_wait(&(it2->second.send_mess_mutex));
               err = it2->second.send_mess_error;
               it2->second.send_mess_error = "";
            sem_post(&(it2->second.send_mess_mutex));
            if(err.empty())
            {
               reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user1, user2, message);
            }
            else
            {
               reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user1, user2, err);
            }
         }
         break;
      }

      case CHAT_SMESSAGE:
      {
         // Find the receiving user
         it = users.find(user1);

         if(it == users.end())
         {
            // CRITICAL ERROR
            cout << "CRITICAL ERROR: User1 not found..." << endl;
            sem_wait(&(it->second.send_mess_mutex));
               it->second.send_mess_error = user1 + " not found";
            sem_post(&(it->second.send_mess_mutex));

         }
         else if(ack == NACK)
         {
            sem_wait(&(it->second.send_mess_mutex));
               it->second.send_mess_error = message;
            sem_post(&(it->second.send_mess_mutex));
         }
 
         cout << "Got message ack from " << user1 << endl;
         sem_post(&(it->second.sending_message));
         break;
      }

      default:
      {
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);
      }
   }

   return reply;
}

void ChatServer::ready()
{
        cout << "Ready for new connections..." << endl;
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
