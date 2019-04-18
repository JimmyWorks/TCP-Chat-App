#include "chatapp/client.h"

ChatClient chatclient;

void end_client(int signum)
{
   chatclient.destroy();
   exit(0);   
}

void heartbeat(int arg)
{
   while(true)
   {
      usleep(ONLINE_TTL*1000000/2);
      if(chatclient.online)
      {
         // SEND HEARTBEAT
         cout << "Pulse..." << endl;
      }
   }
}

int main(int argc, char *argv[])
{
   signal(SIGINT, end_client);

   if(argc != 1)
   {
      cout << "Usage:" << endl;
      cout << "./chat_client"  << endl << endl;
      cout << "User and connection settings can be updated in config/user.config" << endl;
      exit(CLI_ERROR);
   }

   rapidjson::Document d = JsonParser::Parse(USER_CONFIG);

   string username = (d["user"].GetObject())["name"].GetString();
   string serverAddr = d["server"].GetString();

   chatclient.setup(serverAddr);

   std::thread heartbeat_thread(&heartbeat, 0);
   heartbeat_thread.detach();

   cout << "Main thread sleeping 5 seconds..." << endl;
   usleep(5*1000);
   cout << "Main thread turning user online..." << endl;
   chatclient.online = true;
   
   while(true)
   {
      	srand(time(NULL));
        string newMsg = to_string(rand()%25000);
        cout << "Sending: " << newMsg << endl;
	chatclient.sendme(newMsg);
	string rec = chatclient.receive();
	if( rec != "" )
	{
		cout << "Response:" << rec << endl;
	}
	sleep(1);
   }
   return 0;
}
