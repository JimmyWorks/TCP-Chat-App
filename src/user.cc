/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * User implementation file
 */

#include "chatapp/user.h"

// Overloaded Constructors
User::User()
{
   init("", "");
}
User::User(string uname, string pword)
{
   init(uname, pword);
}

// Init
// Method for initializing members for overloaded constructors
void User::init(string uname, string pword)
{
   // Semaphore for notifying server when sent message is received
   if(sem_init(&sending_message, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: sending_message" << endl;
      exit(INIT_ERROR);
   }

   // Mutual exclusion semaphore for accessing the send_mess_error
   if(sem_init(&send_mess_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: send_mess_mutex" << endl;
      exit(INIT_ERROR);
   }

   // Initializing members
   username = uname;
   password = pword;
   send_mess_error = "";
   online = false;
   connfd = -1;
}

