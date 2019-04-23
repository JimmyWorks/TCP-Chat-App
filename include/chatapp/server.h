/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Header for ChatServer
 */

#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <sstream>
#include <map>
#include <list>
#include "chatapp/constants.h"
#include "chatapp/user.h"
#include "chatapp/chat_enums.h"
#include "chatapp/json_parser.h"
#include "chatapp/message.h"
using namespace std;

// ChatServer
// ChatServer instance is the primary object responsible for setting up
// the service using a TCP socket, binding the address and port, and
// listening for incoming connections.  All client incoming connection
// requests get their own thread for processing and handling messages.
class ChatServer
{
   public:
   int sockfd; // server file descriptor
   struct sockaddr_in serverAddress, clientAddress;

   int setup(int port); // setup the service
   void ready(); // handle received connection requests
   void destroy(); // close all connections

   static std::map<string,User> users; // list of users
   static std::list<int> clientfds; // list of client file descriptors

   private:
   static void Connection(int clientfd); // new connection thread
   static void Send(string msg, int clientfd); // send messages
   static string HandleMessage(string msg, int clientfd); // handle messages
};

#endif
