#ifndef CHAT_USER_H
#define CHAT_USER_H

#include <string>
using namespace std;

class User
{
   public:
   User();
   User(string user, string password);
   string username, password;
};

#endif
