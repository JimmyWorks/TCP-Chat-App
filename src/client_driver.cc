#include "chatapp/client.h"

ChatClient chatclient;

void end_client(int signum)
{
   chatclient.destroy();
   exit(0);   
}

int main(int argc, char *argv[])
{
   signal(SIGINT, end_client);

   if(argc != 1)
   {
      cout << "Usage:" << endl;
      cout << "chat_client [port number]"  << endl << endl;
      exit(CLI_ERROR);
   }

   ifstream userConfig;
   userConfig.open(USER_CONFIG);
   if(!userConfig.is_open())
   {
      cout << "Failed to open user config file..." << endl;
      exit(CONFIG_FILE_ERROR);
   }
   else
      cout << "Opened user config file..." << endl;

   string buffer, line;
   while( getline(userConfig, line))
      buffer += line;

   userConfig.close();
   rapidjson::Document d;
   d.Parse(buffer.c_str());
   if(!d.IsObject())
   {
      cout << "Failed to parse user config..." << endl;
   }
   cout << "Successfully parsed config..." << endl;

   string username = (d["user"].GetObject())["name"].GetString();
   string serverAddr = d["server"].GetString();

   chatclient.setup(serverAddr, SERV_PORT);

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
