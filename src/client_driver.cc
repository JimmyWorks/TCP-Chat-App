/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * TCP Chat App client driver file 
 */

#include "chatapp/client.h"

// ChatClient instance
ChatClient chatclient;

// Method signatures
void end_client(int signum);
void heartbeat(int arg);
void receiver(int arg);
void commandprompt();
void exitApp();
bool isOnline(string user);
void handleMessage(string msg);
void printHelp();
void logout();

// List of active users
list<string> activeUsers; 
// Semaphores and mutual exclusion for threads
sem_t login_result, login_mutex; // for login
sem_t logout_result, logout_mutex; // for logout
sem_t sending_message, send_mess_mutex; // for sending message
sem_t online_mutex; // online status mutual exclusion
sem_t active_user_mutex; // for updating active users list
// Error messages
string login_error = "";
string logout_error = "";
string send_mess_error = "";
string user = "";

// Globals
bool user_online = false;
bool talk_mode = false;
string talk_user = "";
bool loggedIn = false;

// Chat Client Driver Main
int main(int argc, char *argv[])
{
   // Signal handling to properly shutdown socket connections
   signal(SIGINT, end_client);

   // Initialize all semaphores
   // Wait/Notify Semaphores:
   if(sem_init(&login_result, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: login_result" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&logout_result, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: logout_result" << endl;
      exit(INIT_ERROR);
   }
   if(sem_init(&sending_message, 0, 0) == -1)
   {
      cout << "Failed to initialize semaphore: sending_message" << endl;
      exit(INIT_ERROR);
   }
   // Mutual Exclusion Semaphores:
   if(sem_init(&login_mutex, 0, 1) == -1)
   {
      cout << "Failed to initialize semaphore: login_mutex" << endl;
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

   // Handle CLI args
   if(argc != 1)
   {
      cout << "Usage:" << endl;
      cout << "./chat_client"  << endl << endl;
      cout << "User and connection settings can be updated in config/user.config" << endl;
      exit(CLI_ERROR);
   }

   // Parse user config
   rapidjson::Document d = JsonParser::Parse(USER_CONFIG);
   string serverAddr = d["server"].GetString();

   // Setup chat client and connect to server
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

   // Welcome message
   cout << "========================================" << endl;
   cout << "      Welcome to Jimmy's Chat App!      " << endl;
   cout << "========================================" << endl << endl;

   string pass;
   while(true)
   {
      // Get user credentials
      cout << "Login: ";
      getline(cin, user);
      cout << "Password: ";
      getline(cin, pass);

      // Send login request
      string msg = ChatMessage::serialize(CONN_REQ, ACK, user, "", pass);
      chatclient.Send(msg);

      // Wait for login response   
      sem_wait(&login_result);

      // Check if login successful
      sem_wait(&online_mutex);
         loggedIn = user_online;
      sem_post(&online_mutex);

      // If successful
      if(loggedIn)
      {
         // Welcome user
         cout << "Welcome, " << user << "!" << endl << endl;
         cout << "==========================" << endl;
         cout << "Enter commands at any time. Some commands:" << endl;
         printHelp();

         // While logged in, get commands from user and process accordingly
         while(loggedIn)
         {
            commandprompt();
         } 
      }
      else
      {  // If unsuccessful, display the error
         sem_wait(&login_mutex);
            cout << login_error << endl;
         sem_post(&login_mutex);
      }

      // Clear active users
      sem_wait(&active_user_mutex);
         activeUsers.clear();
      sem_post(&active_user_mutex);
      // Clear cin
      cin.clear();   
   }
   return 0;
}

// TCP Chat Command Prompt
// Get user input and process input
void commandprompt()
{
   // Get user input
   string line = "";
   getline(cin, line);

   // Get first token
   int index = line.find(" ");
   string token = line.substr(0, index);  
 
   // If currently in talk mode
   if(talk_mode)
   {
      // If command to stop talk mode
      if(token.compare("/stop") == 0)
      {
         cout << "[System] Stopped talking to " << talk_user << endl;
         talk_mode = false;
         talk_user = "";
      }
      // If other user no longer online
      else if(!isOnline(talk_user))
      {
         cout << "[System] Stopped talking to " << talk_user << endl;
         cout << "[System] User no longer active" << endl;
         talk_mode = false;
         talk_user = "";
      }
      // Otherwise, send the message to the other user
      else
      {
         // Send the message
         string msg = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user, talk_user, line);
         chatclient.Send(msg);
    
         // Wait for the message to send
         sem_wait(&sending_message);

         // Check if any errors
         string err = "";
         sem_wait(&send_mess_mutex);
            err = send_mess_error;
            send_mess_error = "";
         sem_post(&send_mess_mutex);
 
         // If error, post error
         if(!err.empty())
            cout << "[System] ERROR: " << err << endl;
      } 
   }
   // If not in talk mode
   else
   {
      // Remove the first token
      line.erase(0, index+1);
             
      // Check active users
      if(token.compare("/active") == 0)
      {
          // Get active user list
          list<string> copy;
          sem_wait(&active_user_mutex);
             copy = activeUsers;
          sem_post(&active_user_mutex);

          // If there are users online
          if(copy.size() > 0)
          {
             // Print the active users
             cout << endl << "Active users:" << endl;
             for(std::list<string>::iterator it = copy.begin(); it != copy.end(); ++it)
                 cout << " - " << *it << endl;
          }
          else
          {
             cout << "No active users to send messages" << endl;
          }
       } 
       // Send message to another user
       else if(token.compare("/whisper") == 0)
       {
           // Get the user
           index = line.find(" ");
           string user2 = line.substr(0, index);  
           line.erase(0, index+1);

           // If user online
           if(isOnline(user2))
           {
               // Send the message
               string msg = ChatMessage::serialize((int)CHAT_MESSAGE, (int)ACK, user, user2, line);
               chatclient.Send(msg);
  
               // Wait for message to send
               sem_wait(&sending_message);

               // Check for any errors
               string err = "";
               sem_wait(&send_mess_mutex);
                  err = send_mess_error;
                  send_mess_error = "";
               sem_post(&send_mess_mutex);
 
               // If error, post error
               if(!err.empty())
                  cout << "[System] ERROR: " << err << endl;
            }
            else
            {
               cout << "[System] User, " << token << ", is not available" << endl;
            }
        }
        // Start a talk session
        else if(token.compare("/talk") == 0)
        {
            // Get the user
            index = line.find(" ");
            string user2 = line.substr(0, index);  
            line.erase(0, index+1);

            // Check if user if online
            if(isOnline(user2))
            {
               // Start talk mode
               cout << "[System] Now talking to " << user2 << endl;
               talk_user = user2;
               talk_mode = true;
            }
            else
            {
               cout << "[System] User is not available: " << user2 << endl;
            }
         }
         // Stop talk session (Invalid command)
         else if(token.compare("/stop") == 0)
         {
            cout << "[System] Not currently talking to a user" << endl;
         }
         // Exit the application
         else if(token.compare("/exit") == 0)
         {
            cout << "[System] Logging out..." << endl;
            logout();
            exitApp();
         }
         // Log out of the system
         else if(token.compare("/logout") == 0)
         {
            cout << "[System] Logging out..." << endl;
            logout(); 
         }
         // Help - Print valid commands
         else if(token.compare("/help") == 0)
         {
            cout << "[Help] Commands:" << endl;
            printHelp();
         }
         // Invalid command
         else
         {
            cout << "[System] Invalid operation" << endl;
         }
         
         // Clear cin
         cin.clear();
         cout << endl;

      }

      // Update logged in status
      sem_wait(&online_mutex);
          loggedIn = user_online;
      sem_post(&online_mutex);
      // Clear cin
      cin.clear();
}

// Log out
// Log out user
void logout()
{
   // Send user connection termination to server
   string msg = ChatMessage::serialize((int)CONN_TERM, (int)ACK, user);
   chatclient.Send(msg);
                  
   // Wait for log out result
   sem_wait(&logout_result);

   // Check for error
   string err;
   sem_wait(&logout_mutex);
      err = logout_error;
      logout_error = ""; 
   sem_post(&logout_mutex);

   // If error, post error
   if(!err.empty())
      cout << "[System] Error logging out on server..." << endl;
      cout << "[System] Ending application..." << endl;
}

// Print Help
// Print all valid commands
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

// Is Online
// Checks if user is online
bool isOnline(string user)
{
   // Get active users
   list<string> copy;
   sem_wait(&active_user_mutex);
      copy = activeUsers;
   sem_post(&active_user_mutex);

   // Check if user is among the active users
   for(std::list<string>::iterator it = copy.begin(); it != copy.end(); ++it)
   {
      if((*it).compare(user) == 0)
         return true;
   }
   return false;
} 

// End Client
void end_client(int signum)
{
   chatclient.destroy();
   exit(0);   
}

// Heartbeat
// Task for thread responsible for sending heartbeats
void heartbeat(int arg)
{
   bool online;
   while(true)
   {
      // Sleep for half of TTL
      usleep(ONLINE_TTL*1000000/2);

      // Get online status
      sem_wait(&online_mutex);
         online = user_online;
      sem_post(&online_mutex);

      // If online
      if(online)
      {
         // Send heartbeat
         string beat = ChatMessage::serialize(HEARTBEAT, ACK, user); 
         chatclient.Send(beat);
      }
   }
}

// Receiver
// Task for thread responsible for receiving messages from the server
void receiver(int arg)
{
   while(true)
   {
      // Block on receiving messages from server
      string rec = chatclient.Receive();
      if( rec != "" )
      {
         // Handle message
         handleMessage(rec);
      }
   }
}

// Exit App Message
void exitApp()
{
   cout << "Thank you for using my chat app!" << endl;
   cout << "Author: Jimmy Nguyen" << endl;
   cout << "Email: Jimmy@Jimmyworks.net" << endl << endl;
   cout << "Goodbye!" << endl;
   exit(0);
}

// Handle Message
// Handles incoming messages from server
void handleMessage(string msg)
{

   // Deserialize message and get values
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
   // Handle based on opcode
   switch(opcode)
   {
      case HEARTBEAT:
      {
         list<string> temp;

         // If message is not empty
         while(!message.empty())
         {
            // Parse based on delimiter
            int index = message.find(";");
            string usr = message.substr(0, index);
            message.erase(0, index+1);
            // If it does not match 
            // the user, add to list
            if(usr.compare(user) != 0)
               temp.push_back(usr);
         }

         // Update active users list
         sem_wait(&active_user_mutex);
            activeUsers.clear();
            activeUsers = temp;
         sem_post(&active_user_mutex);
         break;
      }

      case CONN_REQ:
      {
         // Update based on ack/nack
         if(ack == ACK)
         {
            // Update login status
            sem_wait(&login_mutex);
               login_error = "";
               user_online = true;
            sem_post(&login_mutex);
         }
         else
         {
            // Update error message
            sem_wait(&login_mutex);
               login_error = message;
            sem_post(&login_mutex);
         }

         // Notify login result received
         sem_post(&login_result);
         break;
      }

      case CONN_TERM:
      {
         // If nack
         if(ack == NACK)
         {
            // Update error message
            sem_wait(&logout_mutex);
               logout_error = message;
            sem_post(&logout_mutex);
         }

         // Update login status
         sem_wait(&online_mutex);
            user_online = false;
         sem_post(&online_mutex);

         // Notify logout completed
         sem_post(&logout_result);
         break;
      }
      
      case CHAT_MESSAGE:
      {
         // If nack
         if(ack == NACK)
         {
            // Update error message
            sem_wait(&send_mess_mutex);
               send_mess_error = message;
            sem_post(&send_mess_mutex);
            
         }

         // Notify sent message received
         sem_post(&sending_message);
         break;
      }
       
      case CHAT_SMESSAGE:
      {
         // Print received message
         cout << "[" << user2 << "] " << message << endl;

         // Send ack reply
         string reply = ChatMessage::serialize((int)CHAT_SMESSAGE, (int)ACK, user1, user2, message);
         chatclient.Send(reply);
         break;
      }
      
      // Invalid operation 
      default:
      {
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);    
      }
   }
}
