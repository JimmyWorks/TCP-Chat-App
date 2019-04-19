#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <sstream>
#include <map>
#include "chatapp/constants.h"
#include "chatapp/user.h"
#include "chatapp/chat_enums.h"
#include "chatapp/json_parser.h"
#include "chatapp/message.h"

using namespace std;

class ChatServer
{
   public:
   int sockfd;
   struct sockaddr_in serverAddress, clientAddress;

   int setup(int port);
   void ready();
   void detach();

   static std::map<string,User> users;

   private:
   static void Connection(int clientfd);
   static void Send(string msg, int clientfd);
};

#endif
