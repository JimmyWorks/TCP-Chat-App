#include "chatapp/user.h"

User::User()
{
  username = "";
  password = "";
  online = false;
}

User::User(string uname, string pword)
{
   username = uname;
   password = pword;
}
