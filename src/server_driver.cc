#include <iostream>
#include "chatapp/server.h"

ChatServer chatserver;

void read_messages()
{
   while(true)
   {
      srand(time(NULL));
      char ch = 'a' + rand() % 26;
      string s(1,ch);
      string str = chatserver.getMessage();
      if( str != "" )
      {
         cout << "Message:" << str << endl;
	 chatserver.Send(" [client message: "+str+"] "+s);
	 chatserver.clean();
      }
      usleep(1000);
   }
}

int main()
{

   // Create thread for printing out current messages read from connections
   std::thread msg_reader(&read_messages);
   msg_reader.detach();

   // Setup the server and once ready, listen for new connections
   if( chatserver.setup(SERV_PORT) == 0 )
      chatserver.ready();

   return 0;
}
