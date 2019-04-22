#include "chatapp/client.h"

ChatClient chatclient;

void end_client(int signum);
void heartbeat(int arg);
void receiver(int arg);
void exitApp();
void handleMessage(string msg);

list<string> activeUsers;
sem_t login_result, online_mutex;
string login_error = "";
string user = "";

int main(int argc, char *argv[])
{
   signal(SIGINT, end_client);

   if(sem_init(&login_result, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: login_result" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&online_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: online_mutex" << endl;
      exit(INIT_ERROR);
   }

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

   if(!chatclient.setup(serverAddr))
   {
      cout << endl;
      cout << "==================================================" << endl;
      cout << " Server connection failed.  Server may be offline." << endl;
      cout << " Please try again later." << endl;
      cout << "==================================================" << endl << endl;
      exit(SERVER_CONN_FAILED);
   }


   // Create client threads
 
   // Create thread for sending heartbeats 
   std::thread heartbeat_thread(&heartbeat, 0);
   heartbeat_thread.detach();

   // Create thread for handling received messages
   std::thread recv_thread(&receiver, 0);
   recv_thread.detach();

   cout << "========================================" << endl;
   cout << "      Welcome to Jimmy's Chat App!      " << endl;
   cout << "========================================" << endl << endl;

   string pass;
   int selection = -1;
   while(true)
   {
      cout << "Login: ";
      getline(cin, user);
      cout << "Password: ";
      getline(cin, pass);

      string msg = ChatMessage::serialize(CONN_REQ, ACK, user, "", pass);
      chatclient.Send(msg);
   
      sem_wait(&login_result);
      if(login_error.empty())
      {
         cout << "Welcome, " << user << "!" << endl << endl;

         while(true)
         {
            cout << "Options:" << endl;
            cout << "==========================" << endl;
            cout << "1. Check users online" << endl;
            cout << "2. Start chat session" << endl;
            cout << "3. Exit application" << endl << endl;
            cout << "> ";
            
            cin >> selection;
            switch(selection)
            {
               case 1:
               {
                  if(activeUsers.empty())
                  {
                     cout << endl << "No active users online." << endl << endl;
                  }
                  else
                  {
                     cout << endl << "Active users:" << endl;
                     for(std::list<string>::iterator it = activeUsers.begin(); 
                         it != activeUsers.end(); ++it)
                        cout << " -  " << *it << endl << endl;
                  }
                  
               }
               break;

               case 2:
               {
                  cout << "Selected to start chat session." << endl;
               }
               break;

               case 3:
               {
                  cout << "Logging out user..." << endl;
                  string msg = ChatMessage::serialize(CONN_TERM, ACK, user);
                  chatclient.Send(msg);
                  sem_wait(&login_result);
                  exitApp();
               }
               break;

               default:
               {
                  cout << "Invalid option selected.  Please try again..." << endl;
               }
            }
            cout << endl;
            cin.clear();
         }
      }
      else
      {
         cout << login_error << endl;
      }
      activeUsers.clear();
      cin.clear();
   }

   return 0;
}

void end_client(int signum)
{
   chatclient.destroy();
   exit(0);   
}

void heartbeat(int arg)
{
   bool online;
   while(true)
   {
      usleep(ONLINE_TTL*1000000/2);

      sem_wait(&online_mutex);
         online = chatclient.online;
      sem_post(&online_mutex);

      if(online)
      {
         // Send heartbeat
         string beat = ChatMessage::serialize(HEARTBEAT, ACK, user); 
         chatclient.Send(beat);
      }
   }
   //return arg;
}

void receiver(int arg)
{
   while(true)
   {
      string rec = chatclient.Receive();
      if( rec != "" )
      {
         handleMessage(rec);
      }
   }
   //return arg;
}

void exitApp()
{
   cout << "Thank you for using my chat app!" << endl;
   cout << "Author: Jimmy Nguyen" << endl;
   cout << "Email: Jimmy@Jimmyworks.net" << endl << endl;
   cout << "Goodbye!" << endl;
   exit(0);
}

void handleMessage(string msg)
{

   ChatMessage obj = ChatMessage::deserialize(msg);
   int opcode = obj.opcode;
   int ack = obj.ack;
   string user1 = obj.user1;
   string user2 = obj.user2;
   string message = obj.message;

/*
   cout << "Handling..." << endl;
   cout << "op: " << opcode << endl;
   cout << "ack: " << ack << endl;
   cout << "user1: " << user1 << endl;
   cout << "user2: " << user2 << endl;
   cout << "message: " << message << endl;
*/
   std::map<string, User>::iterator it;
   std::map<string, User>::iterator it2;
   switch(opcode)
   {
      case HEARTBEAT:
      {
         activeUsers.clear();

         // Get list of users online
         while(!message.empty())
         {
            int index = message.find(";");
            string user = message.substr(0, index);
            message.erase(0, index+1);
            activeUsers.push_back(user);
         }
         break;
      }

      case CONN_REQ:
      {
         if(ack == ACK)
         {
            login_error = "";
            sem_wait(&online_mutex);
               chatclient.online = true;
            sem_post(&online_mutex);
         }
         else
         {
            login_error = message;
         }

         sem_post(&login_result);
         break;
      }

      case CONN_TERM:
      {
         sem_wait(&online_mutex);
            chatclient.online = false;
         sem_post(&online_mutex);
         login_error = "User logged out";
         sem_post(&login_result);
         break;
      }
      
      case CHAT_REQ:
      {
         cout << "Got CHAT_REQ ack/nack..." << endl;
         break;
      }
      
      case CHAT_TERM:
      {
         cout << "Got CHAT_TERM ack/nack..." << endl;
         break;
      }
      
      case CHAT_MESSAGE:
      {
         cout << "Got CHAT_MESSAGE ack/nack..." << endl;
         break;
      }
      
      case CHAT_SREQ:
      {
         cout << "Got new chat request!" << endl;
         break;
      }

      case CHAT_STERM:
      {
         cout << "Chat user terminated the chat" << endl;
         break;
      }
      
      case CHAT_SMESSAGE:
      {
         cout << "Incoming chat message!" << endl;
         break;
      }
       
      default:
      {
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);    
      }
   }
}
