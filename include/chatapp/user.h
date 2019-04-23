#ifndef CHAT_USER_H
#define CHAT_USER_H

#include <string>
#include <iostream>
#include <semaphore.h>
#include "chatapp/chat_enums.h"
using namespace std;

class User
{
   public:
   string username, password;
   sem_t sending_message, send_mess_mutex;
   string send_mess_error;
   bool online;
   int connfd;

   User();
   User(string user, string password);
   void init(string user, string password);
};

#endif
