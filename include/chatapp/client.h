/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Header for ChatClient class 
 */

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
#include <list>
#include <semaphore.h>
#include "chatapp/json_parser.h"
#include "chatapp/chat_enums.h"
#include "chatapp/message.h"

using namespace std;

// ChatClient
// Instance created by chat client driver in main()
// which established connection to server via TCP
// as well as an interface for sending and receiving
// messages from the server
class ChatClient
{
   private:
     int sockfd; // file descriptor
     struct sockaddr_in server; // server's sock addr

   public:
     ChatClient();
     bool setup(string address, int port = SERV_PORT);
     bool Send(string message);
     string Receive(int size = MAXPACKET_SIZE); 
     void destroy();
};

#endif
