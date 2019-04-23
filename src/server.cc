/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * ChatServer implementation file
 */

#include "chatapp/server.h"

// Static map of users known by server
std::map<string, User> ChatServer::users;

// Static list of open client file descriptors 
std::list<int> ChatServer::clientfds;

// Setup
// Setup server by parsing users from json file
// and creating a TCP endpoint
int ChatServer::setup(int port)
{
   // Parse json file for persistent user data
   rapidjson::Document d = JsonParser::Parse(USER_DB);
   const rapidjson::Value& userObjs = d["users"];

   // For each user, extract data
   for(rapidjson::SizeType i = 0; i < userObjs.Size(); i++)
   {
      string uname = userObjs[i].GetObject()["username"].GetString();
      string pword = userObjs[i].GetObject()["password"].GetString();
      
      users[uname] = User(uname, pword); 
   }

   // Print demo users and passwords
   cout << endl << "Demo users:" << endl;
   cout << "================================" << endl;
   for(std::map<string, User>::iterator it = users.begin(); it != users.end(); ++it)
   {
      cout << it->first <<  " - " << it->second.password << endl;
   } 
   cout << endl;

   // Create a IPv4 TCP socket
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

// Connection
// Method for newly created threads to handle incoming
// client request messages.  Arg value passed in is the
// client file descriptor
void ChatServer::Connection(int arg)
{
	int nbytes; // number of bytes received
	int clientfd = arg; // client file descriptor
	char msg[MAXPACKET_SIZE]; // message buffer

	while(1)
	{
                // Clear previous message
	        memset(msg, 0, MAXPACKET_SIZE);

                // Get new message
		nbytes = recv(clientfd, msg, MAXPACKET_SIZE, 0);

                // If none received, close the client
		if(nbytes==0)
		{
                   cout << "CLOSING THIS CLIENT: " << to_string(clientfd) << endl;
		   close(clientfd);
                   
                   // Remove the clientfd from the list
                   clientfds.remove(clientfd);
		   break;
		}

                // Make sure buffer is null terminated
		msg[nbytes]=0;

                // Handle the request
                string reply = ChatServer::HandleMessage(msg, clientfd);

                // If no reply necessary, do not send message
                if(!reply.empty())
                   ChatServer::Send(reply, clientfd);
	 }
}

// Handle Message
// Handle incoming messages from clients based on opcode
string ChatServer::HandleMessage(string msg, int clientfd)
{
   // Reply string
   string reply = "";

   // Deserialize message and extract data
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

   // Iterators for user1 and user2
   std::map<string, User>::iterator it;
   std::map<string, User>::iterator it2;

   // Handle based on opcode
   switch(opcode)
   {
      // Heartbeat message
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

         // Ack message and return active users
         reply = ChatMessage::serialize((int)HEARTBEAT, (int)ACK, user1, "", onlineusers);
         break;
      }

      // User wants to login
      case CONN_REQ:
      {
         // Find user1
         cout << "Searching for user: " << user1 << endl;
         it = users.find(user1);
        
         // If user not found
         if(it == users.end())
         {
            cout << "Could not find user..." << endl;
            reply = ChatMessage::serialize((int)CONN_REQ, (int)NACK, user1, "", "Invalid username.  Please try again...");
         }
         // If password is invalid
         else if(it->second.password != message)
         {
            cout << "Invalid password..." << endl; 
            reply = ChatMessage::serialize((int)CONN_REQ, (int)NACK, user1, "", "Invalid password.  Please try again...");
         }
         // Valid user and password
         else
         {
            cout << "Valid user login..." << endl;
            // Set user online status and connfd
            it->second.online = true;
            it->second.connfd = clientfd;
            // Return ack message for successful login
            reply = ChatMessage::serialize((int)CONN_REQ, (int)ACK); 
         }
         break;
      }

      // User wants to log out
      case CONN_TERM:
      {
         // Find user
         it = users.find(user1);

         // If user not found
         if(it == users.end())
         {
            // CRITICAL ERROR
            cout << "CRITICAL ERROR: USER NOT FOUND FOR CONNECTION TERMINATION..." << endl;
            reply = ChatMessage::serialize((int)CONN_TERM, (int)NACK);
            break;
         }

         // Make user offline
         it->second.online = false;

         // Ack the request
         reply = ChatMessage::serialize((int)CONN_TERM, (int)ACK);
         break;
      }

      // Message request from client1 to server for client2
      case CHAT_MESSAGE:
      {
         // Find user2
         it2 = users.find(user2);

         // If user2 not found
         if(it2 == users.end())
         {
            cout << "CHAT ERROR: User2 does not exist..." << endl;
            reply = user2 + " does not exist";
            reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)NACK, user1, user2, reply);
            break;
         }
         // If user2 is not online
         else if(!(it2->second.online))
         {
            cout << "User2 is not online..." << endl;
            reply = user2 + " is no longer online";
            reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)NACK, user1, user2, reply);
         }
         // If user2 is online
         else
         {
            // Forward the message to user2
            string msg = ChatMessage::serialize((int)CHAT_SMESSAGE, (int)ACK, user2, user1, message);
            ChatServer::Send(msg, it2->second.connfd);

            // Wait for message to send
            sem_wait(&(it2->second.sending_message));

            // Check for errors
            string err;
            sem_wait(&(it2->second.send_mess_mutex));
               err = it2->second.send_mess_error;
               it2->second.send_mess_error = "";
            sem_post(&(it2->second.send_mess_mutex));

            // If no errors, ack the message
            if(err.empty())
            {
               reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user1, user2, message);
            }
            // Otherwise, nack the message and include the error
            else
            {
               reply = ChatMessage::serialize((int)CHAT_MESSAGE, (int)NACK, user1, user2, err);
            }
         }
         break;
      }

      // Client2 responds to message forwarded by server from client1
      case CHAT_SMESSAGE:
      {
         // Find the receiving user
         it = users.find(user1);

         // If receiving user does not exist
         if(it == users.end())
         {
            // CRITICAL ERROR
            cout << "CRITICAL ERROR: User1 not found..." << endl;
            sem_wait(&(it->second.send_mess_mutex));
               it->second.send_mess_error = user1 + " not found";
            sem_post(&(it->second.send_mess_mutex));

         }
         // If there is a problem receiving the message
         else if(ack == NACK)
         {
            sem_wait(&(it->second.send_mess_mutex));
               it->second.send_mess_error = message;
            sem_post(&(it->second.send_mess_mutex));
         }

         // Notify that the message has been ack 
         sem_post(&(it->second.sending_message));
         break;
      }

      // Invalid operation
      default:
      {
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);
      }
   }

   return reply;
}

// Ready
// Method for a detached thread to receive and accept new connection
// requests to the server.  Each new connection gets assigned a new
// thread which processes that connection's requests.
void ChatServer::ready()
{
        cout << "Ready for new connections..." << endl;
	socklen_t sosize  = sizeof(clientAddress);
	while(true)
	{
                // Accept connection-based sockets  
		int newclientfd = accept(sockfd,(struct sockaddr*)&clientAddress,&sosize);

		string str = inet_ntoa(clientAddress.sin_addr);
                cout << "Accepted new connection: " << str << 
                        ", Port: " << clientAddress.sin_port << endl;

                // Add new file descriptor to client list fd
                clientfds.push_back(newclientfd);
           
                // Create new thread to handle that connection's requests
		std::thread newThread(&ChatServer::Connection, newclientfd);
		newThread.detach();
	}
}

// Send
// Static method for sending messages to client given client file descriptor
void ChatServer::Send(string msg, int clientfd)
{
	send(clientfd,msg.c_str(),msg.length(),0);
}

// Destroy
// Close all open connections
void ChatServer::destroy()
{
   // Close all client connections
   for (std::list<int>::iterator it = clientfds.begin(); it != clientfds.end(); ++it)
      close(*it);
   close(sockfd);
} 
