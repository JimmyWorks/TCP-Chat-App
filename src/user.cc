#include "chatapp/user.h"

User::User()
{
  username = "";
  password = "";
  online = false;
  connfd = -1;
  chatting = "";
}

User::User(string uname, string pword)
{
   username = uname;
   password = pword;
   online = false;
   connfd = -1;
   chatting = "";
}
