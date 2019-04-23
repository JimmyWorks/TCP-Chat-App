/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * ChatMessage implementation file
 */

#include "chatapp/message.h"

// Constructors
ChatMessage::ChatMessage()
{   
   init(HEARTBEAT, ACK, "", "", "");
}
ChatMessage::ChatMessage(int opcode)
{   
   init(opcode, ACK, "", "", "");
}
ChatMessage::ChatMessage(int opcode, int ack)
{   
   init(opcode, ack, "", "", "");
}
ChatMessage::ChatMessage(int opcode, int ack, string user1)
{   
   init(opcode, ack, user1, "", "");
}
ChatMessage::ChatMessage(int opcode, int ack, string user1, string user2)
{   
   init(opcode, ack, user1, user2, "");
}

ChatMessage::ChatMessage(int opcode, int ack, string user1, string user2, string message)
{
   init(opcode, ack, user1, user2, message);
}

// Init
// Initialization for all overloaded constructors
void ChatMessage::init(int opcode, int ack, string user1, string user2, string message)
{
   // Verify all values in proper range and length
   ChatMessage::verifyOperation(opcode);
   ChatMessage::verifyAck(ack);
   ChatMessage::verifyUser(user1);
   ChatMessage::verifyUser(user2);
   ChatMessage::verifyMessage(message);

   // Assign values
   this->opcode = opcode;
   this->ack = ack;   
   this->user1 = user1;
   this->user2 = user2;
   this->message = message;
}

// Deserialize
// Deserialized a serialize message string into a ChatMessage object
ChatMessage ChatMessage::deserialize(string msg)
{
   // Get each element based on message size parameters of the string
   int opcode = stoi(msg.substr(0, OPCODE_SIZE));
   int ack = stoi(msg.substr(0+OPCODE_SIZE, ACK_SIZE));
   string user1 = msg.substr(OPCODE_SIZE+ACK_SIZE, USER_SIZE);
   string user2 = msg.substr(OPCODE_SIZE+ACK_SIZE+USER_SIZE, USER_SIZE);
   string message = msg.substr((OPCODE_SIZE+ACK_SIZE+USER_SIZE+USER_SIZE), 
                    (msg.length() - USER_SIZE - USER_SIZE - ACK_SIZE - OPCODE_SIZE));
   
   // Remove user padding
   user1 = ChatMessage::removePad(user1);
   user2 = ChatMessage::removePad(user2);

   // Create new ChatMessage object
   ChatMessage obj = ChatMessage(opcode, ack, user1, user2, message);
   return obj;
}

// Serialize
// Overloaded serialize method for creating serialized message strings
// Shorter versions are simply wrapper methods
string ChatMessage::serialize(int opcode)
{
   string msg = ChatMessage::serialize(opcode, ACK, "", "", "");
   return msg;
}
string ChatMessage::serialize(int opcode, int ack)
{
   string msg = ChatMessage::serialize(opcode, ack, "", "", "");
   return msg;
}
string ChatMessage::serialize(int opcode, int ack, string user)
{
   string msg = ChatMessage::serialize(opcode, ack, user, "", "");
   return msg;
}
string ChatMessage::serialize(int opcode, int ack, string user1, string user2)
{
   string msg = ChatMessage::serialize(opcode, ack, user1, user2, "");
   return msg;
}

// Serialize
// Full serialize method the wrapper methods use to create a serialized message
string ChatMessage::serialize(int opcode, int ack, string user1, string user2, string message)
{
   // For holding user with padding
   string _user1 = user1;
   string _user2 = user2;

   // Verify value range and length
   verifyOperation(opcode);
   verifyAck(ack);
   verifyUser(user1);
   verifyUser(user2);
   verifyMessage(message);

   // Fill in unused space with '$' for user
   _user1 = ChatMessage::addPad(_user1);
   _user2 = ChatMessage::addPad(_user2);   

   string msg = to_string(opcode) + to_string(ack) + _user1 + _user2 + message;
   return msg;
}

// Add Padding
// Add padding to user to meet user length
string ChatMessage::addPad(string user)
{
   string moduser = user;
   int size = moduser.length();
   // Fill in unused space with '$' for user
   for(int i = 0; i < (USER_SIZE-size); i++)
   {
      moduser += '$';
   }
   return moduser;
}

// Remove Padding
// Remove padding from user to get username
string ChatMessage::removePad(string user)
{
   string moduser = user;
   // Remove '$' from padded user
   for(int i = moduser.length()-1; i > -1; i--)
   {
      if(moduser[i] == '$')
         moduser.erase(i);
   }
   return moduser;
}

// Verify Operation
// Check if opcode is valid 
void ChatMessage::verifyOperation(int opcode)
{
   if(opcode < 0 || opcode >= INVALID_OP)
   {
      cout << "Invalid operation not supported..." << endl;
      exit(INVALID_OPCODE);
   }
}

// Verify Ack
// Check if ack is valid
void ChatMessage::verifyAck(int ack)
{
   if(ack != ACK && ack != NACK)
   {
      cout << "Invalid ack/nack in message..." << endl;
      exit(MESSAGE_ERROR);
   }
}

// Verify User
// Check username size
void ChatMessage::verifyUser(string user)
{
   if(user.length() > USER_SIZE)
   {
      cout << "Username \"" << user << "\" is longer than username limit: " 
           << to_string(USER_SIZE) << endl;
      exit(MESSAGE_ERROR);
   }
}

// Verify Message
// Check message size
void ChatMessage::verifyMessage(string message)
{
   if(message.length() > MAXPACKET_SIZE-USER_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE)
   {
      cout << "Message length is " << message.length() 
           << " is longer than message size limit: " 
           << MAXPACKET_SIZE-USER_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE << endl;
      exit(MESSAGE_ERROR);
   }
}
