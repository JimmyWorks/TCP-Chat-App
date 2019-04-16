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
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>

using namespace std;

class ChatClient
{
   private:
      int sockfd;
      std::string address;
      int port;
      struct sockaddr_in server;

   public:
     ChatClient();
     bool setup(string address, int port);
     bool sendme(string message);
     string receive(int size = MAXPACKET_SIZE); 
     string read();
     void destroy();
};

#endif
