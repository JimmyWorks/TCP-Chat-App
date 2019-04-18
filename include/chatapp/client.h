#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "chatapp/server.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <unistd.h>
#include "chatapp/json_parser.h"
#include "chatapp/chat_enums.h"

using namespace std;

class ChatClient
{
   private:
     int sockfd;
     struct sockaddr_in server;

   public:
     bool online;

     ChatClient();
     bool setup(string address, int port = SERV_PORT);
     bool sendme(string message);
     string receive(int size = MAXPACKET_SIZE); 
     string read();
     void destroy();
};

#endif
