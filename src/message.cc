#include "chatapp/message.h"

string ChatMessage::create(int opcode)
{
   string msg = ChatMessage::create(opcode, ACK, "", "");
   return msg;
}

string ChatMessage::create(int opcode, string user)
{
   string msg = ChatMessage::create(opcode, ACK, user, "");
   return msg;
}

string ChatMessage::create(int opcode, string user, string message)
{
   string msg = ChatMessage::create(opcode, ACK, user, message);
   return msg;
}

string ChatMessage::create(int opcode, int ack, string message)
{
   string msg = ChatMessage::create(opcode, ack, "", message);
   return msg;
}

string ChatMessage::create(int opcode, int ack, string user, string message)
{
   string _user = user;
   int u_size = user.length();

   if(opcode < 0 || opcode >= INVALID_OP)
   {
      cout << "Invalid operation not supported..." << endl;
      exit(INVALID_OPCODE);
   }
   if(ack != ACK && ack != NACK)
   {
      cout << "Invalid ack/nack in message..." << endl;
      exit(MESSAGE_ERROR);
   }
   if(user.length() > USER_SIZE)
   {
      cout << "Username \"" << user << "\" is longer than username limit: " 
           << to_string(USER_SIZE) << endl;
      exit(MESSAGE_ERROR);
   }
   if(message.length() > MAXPACKET_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE)
   {
      cout << "Message length is " << message.length() 
           << " is longer than message size limit: " 
           << MAXPACKET_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE << endl;
      exit(MESSAGE_ERROR);
   }

   // Fill in unused space with '$' for user
   for(int i = 0; i < (USER_SIZE-u_size); i++)
   {
      _user += '$';
   }

   string msg = to_string(opcode) + to_string(ack) + _user + message;
   return msg;
}
