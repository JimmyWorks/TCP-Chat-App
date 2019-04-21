#include "chatapp/client.h"

ChatClient chatclient;

void end_client(int signum)
{
   chatclient.destroy();
   exit(0);   
}

void heartbeat(int arg)
{
   string beat = ChatMessage::create(HEARTBEAT); 
   while(true)
   {
      usleep(ONLINE_TTL*1000000/2);
      if(chatclient.online)
      {
         // SEND HEARTBEAT
         cout << beat << endl;
         chatclient.Send(beat);
      }
   }
}

void receiver(int arg)
{
   while(true)
   {
      string rec = chatclient.Receive();
      if( rec != "" )
      {
      cout << "Response:" << rec << endl;
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
   std::thread recv_thread(&receiver, 0);
   recv_thread.detach();

   cout << "========================================" << endl;
   cout << "      Welcome to Jimmy's Chat App!      " << endl;
   cout << "========================================" << endl << endl;

   string inStr, user, pass;
   while(true)
   {
      cout << "Login: ";
      getline(cin, user);
      cout << "Password: ";
      getline(cin, pass);

      string msg = ChatMessage::create(CONN_REQ, ACK, user, "", pass);
      chatclient.Send(msg);
   
      usleep(5000);
      cout << "again..." << endl;
   }

   return 0;
}
