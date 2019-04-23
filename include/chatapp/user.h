/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Header for User 
 */

#ifndef CHAT_USER_H
#define CHAT_USER_H

#include <string>
#include <iostream>
#include <semaphore.h>
#include "chatapp/chat_enums.h"
using namespace std;

// User
// Class used primarily by the ChatServer to
// manage users.  Each user is created at 
// startup in ChatServer::setup() where users
// are created based on the database (simulated
// here using a json file).  Users maintain the
// username, password, online status, connection
// file descriptor, as well as semaphores used by
// the threads to keep track of when messages are
// sent/received and corresponding errors.
class User
{
   public:
   string username, password;
   // semaphore for sending and waiting on message 
   // ack/nack and mutual exclusion for accessing 
   // the send_mess_error message
   sem_t sending_message, send_mess_mutex;
   string send_mess_error;
   bool online;
   int connfd;

   User();
   User(string user, string password);
   void init(string user, string password);
};

#endif
