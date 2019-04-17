#include "chatapp/server.h"

ChatServer chatserver;

int main()
{
   // Setup the server and once ready, listen for new connections
   if( chatserver.setup(SERV_PORT) == 0 )
      chatserver.ready();

   return 0;
}
