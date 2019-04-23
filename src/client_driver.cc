#include "chatapp/client.h"

ChatClient chatclient;

void end_client(int signum);
void heartbeat(int arg);
void receiver(int arg);
void commandprompt();
void exitApp();
bool isOnline(string user);
void handleMessage(string msg);
void printHelp();
void logout();

list<string> activeUsers;
sem_t login_result, login_mutex;
sem_t logout_result, logout_mutex;
sem_t online_mutex;
sem_t sending_message, send_mess_mutex;
sem_t active_user_mutex;
string login_error = "";
string logout_error = "";
string send_mess_error = "";
string user = "";

bool talk_mode = false;
string talk_user = "";
bool loggedIn = false;

int main(int argc, char *argv[])
{
   signal(SIGINT, end_client);

   if(sem_init(&login_result, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: login_result" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&login_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: login_mutex" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&logout_result, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: logout_result" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&logout_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: logout_mutex" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&online_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: online_mutex" << endl;
      exit(INIT_ERROR);
   }

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

   if(sem_init(&active_user_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: active_user_mutex" << endl;
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
   while(true)
   {
      cout << "Login: ";
      getline(cin, user);
      cout << "Password: ";
      getline(cin, pass);

      string msg = ChatMessage::serialize(CONN_REQ, ACK, user, "", pass);
      chatclient.Send(msg);
   
      sem_wait(&login_result);

      sem_wait(&online_mutex);
         loggedIn = chatclient.online;
      sem_post(&online_mutex);

      if(loggedIn)
      {
         cout << "Welcome, " << user << "!" << endl << endl;
         cout << "==========================" << endl;
         cout << "Enter commands at any time. Some commands:" << endl;
         printHelp();

         while(loggedIn)
         {
            commandprompt();
         } 
      }
      else
      {
         sem_wait(&login_mutex);
            cout << login_error << endl;
         sem_post(&login_mutex);
      }
      sem_wait(&active_user_mutex);
         activeUsers.clear();
      sem_post(&active_user_mutex);
      cin.clear();   
   }
   return 0;
}

void commandprompt()
{
            string line = "";
            getline(cin, line);
            int index = line.find(" ");
            string token = line.substr(0, index);  
 
            if(talk_mode)
            {
               if(token.compare("/stop") == 0)
               {
                  cout << "[System] Stopped talking to " << talk_user << endl;
                  talk_mode = false;
                  talk_user = "";
               }
               else if(!isOnline(talk_user))
               {
                  cout << "[System] Stopped talking to " << talk_user << endl;
                  cout << "[System] User no longer active" << endl;
                  talk_mode = false;
                  talk_user = "";
               }
               else
               {
                  string msg = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user, talk_user, line);
                  chatclient.Send(msg);
                  sem_wait(&sending_message);

                  string err = "";
                  sem_wait(&send_mess_mutex);
                     err = send_mess_error;
                     send_mess_error = "";
                  sem_post(&send_mess_mutex);
 
                  if(!err.empty())
                     cout << "[System] ERROR: " << err << endl;
               } 
            }
            else
            {
               line.erase(0, index+1);
             
               if(token.compare("/active") == 0)
               {
                  list<string> copy;
                  sem_wait(&active_user_mutex);
                     copy = activeUsers;
                  sem_post(&active_user_mutex);
                  if(copy.size() > 0)
                  {
                     cout << endl << "Active users:" << endl;
                     for(std::list<string>::iterator it = copy.begin(); it != copy.end(); ++it)
                         cout << " - " << *it << endl;
                  }
                  else
                  {
                     cout << "No active users to send messages" << endl;
                  }
               } 
               else if(token.compare("/whisper") == 0)
               {
                  index = line.find(" ");
                  string user2 = line.substr(0, index);  
                  line.erase(0, index+1);

                  // Check if user if online
                  if(isOnline(user2))
                  {
                     string msg = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user, user2, line);
                     chatclient.Send(msg);
                     sem_wait(&sending_message);

                     string err = "";
                     sem_wait(&send_mess_mutex);
                        err = send_mess_error;
                        send_mess_error = "";
                     sem_post(&send_mess_mutex);
 
                     if(!err.empty())
                        cout << "[System] ERROR: " << err << endl;
                  }
                  else
                  {
                     cout << "[System] User, " << token << ", is not available" << endl;
                  }
               }
               else if(token.compare("/talk") == 0)
               {
                  index = line.find(" ");
                  string user2 = line.substr(0, index);  
                  line.erase(0, index+1);

                  // Check if user if online
                  if(isOnline(user2))
                  {
                     cout << "[System] Now talking to " << user2 << endl;
                     talk_user = user2;
                     talk_mode = true;
                  }
                  else
                  {
                     cout << "[System] User is not available: " << user2 << endl;
                  }
               }
               else if(token.compare("/stop") == 0)
               {
                  cout << "[System] Not currently talking to a user" << endl;
               }
               else if(token.compare("/exit") == 0)
               {
                  cout << "[System] Logging out..." << endl;
                  logout();
                  exitApp();
               }
               else if(token.compare("/logout") == 0)
               {
                  cout << "[System] Logging out..." << endl;
                  logout(); 
               }
               else if(token.compare("/help") == 0)
               {
                  cout << "[Help] Commands:" << endl;
                  printHelp();
               }
               else
               {
                  cout << "[System] Invalid operation" << endl;
               }
               
               cin.clear();

            } // if not talk_mode

            cout << endl;
            sem_wait(&online_mutex);
               loggedIn = chatclient.online;
            sem_post(&online_mutex);
            cin.clear();
}

void logout()
{
   string msg = ChatMessage::serialize((int)CONN_TERM, (int)ACK, user);
   chatclient.Send(msg);
                  
   sem_wait(&logout_result);

   string err;
   sem_wait(&logout_mutex);
      err = logout_error;
      logout_error = ""; 
   sem_post(&logout_mutex);

   if(!err.empty())
      cout << "[System] Error logging out on server..." << endl;
      cout << "[System] Ending application..." << endl;
}

void printHelp()
{
   cout << "/active - Displays all active users" << endl;
   cout << "/whisper <user> <message> - Send message to user" << endl;
   cout << "/talk <user> - Send all following messages to user" << endl;
   cout << "/stop - Stop talking to user" << endl;
   cout << "/exit - Exit the chat app" << endl;
   cout << "/logout - Log out to switch users" << endl;
   cout << "/help - Display these options again" << endl << endl;
}

bool isOnline(string user)
{
   list<string> copy;
   sem_wait(&active_user_mutex);
      copy = activeUsers;
   sem_post(&active_user_mutex);

   for(std::list<string>::iterator it = copy.begin(); it != copy.end(); ++it)
   {
      if((*it).compare(user) == 0)
         return true;
   }
   return false;
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
         list<string> temp;

         // Get list of users online
         while(!message.empty())
         {
            int index = message.find(";");
            string usr = message.substr(0, index);
            message.erase(0, index+1);
            if(usr.compare(user) != 0)
               temp.push_back(usr);
         }

         sem_wait(&active_user_mutex);
            activeUsers.clear();
            activeUsers = temp;
         sem_post(&active_user_mutex);
         break;
      }

      case CONN_REQ:
      {
         if(ack == ACK)
         {
            sem_wait(&login_mutex);
               login_error = "";
               chatclient.online = true;
            sem_post(&login_mutex);
         }
         else
         {
            sem_wait(&login_mutex);
               login_error = message;
            sem_post(&login_mutex);
         }

         sem_post(&login_result);
         break;
      }

      case CONN_TERM:
      {
         if(ack == NACK)
         {
         sem_wait(&logout_mutex);
            logout_error = message;
         sem_post(&logout_mutex);
         }

         sem_wait(&online_mutex);
            chatclient.online = false;
         sem_post(&online_mutex);

         sem_post(&logout_result);
         break;
      }
      
      case CHAT_MESSAGE:
      {
         if(ack == NACK)
         {
            sem_wait(&send_mess_mutex);
               send_mess_error = message;
            sem_post(&send_mess_mutex);
            
         }

         sem_post(&sending_message);
         break;
      }
       
      case CHAT_SMESSAGE:
      {
         cout << "[" << user2 << "] " << message << endl;
         string reply = ChatMessage::serialize((int)CHAT_SMESSAGE, (int)ACK, user1, user2, message);
         chatclient.Send(reply);
         break;
      }
       
      default:
      {
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);    
      }
   }
}
