#include "chatapp/user.h"

User::User()
{
   init("", "");
}

User::User(string uname, string pword)
{
   init(uname, pword);
}

void User::init(string uname, string pword)
{
   if(sem_init(&sending_message, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: sending_message" << endl;
      exit(INIT_ERROR);
   }

   if(sem_init(&send_mess_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: send_mess_mutex" << endl;
      exit(INIT_ERROR);
   }


   username = uname;
   password = pword;
   send_mess_error = "";
   online = false;
   connfd = -1;
}

