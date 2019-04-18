#ifndef CHAT_USER_H
#define CHAT_USER_H

#include <string>
using namespace std;

class User
{
   public:
   string username, password;

   User();
   User(string user, string password);

   private:
   bool online;
};

#endif
