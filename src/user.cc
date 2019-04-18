#include "chatapp/user.h"

User::User()
{
  username = "";
  password = "";
}

User::User(string uname, string pword)
{
   username = uname;
   password = pword;
}
