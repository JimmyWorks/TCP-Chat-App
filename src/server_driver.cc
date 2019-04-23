/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * TCP Chat App server driver file 
 */

#include "chatapp/server.h"

// Function signatures
void end_server(int signum);

// ChatServer instance
ChatServer chatserver;

// Server main
int main()
{
   // Signal Handler
   signal(SIGINT, end_server);

   // Setup the server and once ready, listen for new connections
   if( chatserver.setup(SERV_PORT) == 0 )
      chatserver.ready();

   return 0;
}

// End Server
void end_server(int signum)
{
   // Close all open connections
   chatserver.destroy();
   exit(0);   
}
