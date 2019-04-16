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
#include "chatapp/constants.h"
using namespace std;

enum Error_Codes
{
   SUCCESS,
   CONFIG_FILE_ERROR,
   SOCK_CREATE_FAILURE,
   SOCK_BIND_FAILURE,
   SOCK_LISTEN_FAILURE
};

class ChatServer
{
   public:
   int sockfd, newsockfd, n, pid;
   struct sockaddr_in serverAddress;
   struct sockaddr_in clientAddress;
   pthread_t serverThread;
   char msg[MAXPACKET_SIZE];


   static string Message;
   int setup(int port);
   void ready();
   string getMessage();
   void Send(string msg);
   void clean();
   void detach();
   private:
   static void Task(int arg);
};

#endif
