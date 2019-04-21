#include "chatapp/message.h"

string ChatMessage::create(int opcode)
{
   string msg = ChatMessage::create(opcode, ACK, "", "", "");
   return msg;
}

string ChatMessage::create(int opcode, int ack)
{
   string msg = ChatMessage::create(opcode, ack, "", "", "");
   return msg;
}

string ChatMessage::create(int opcode, int ack, string user)
{
   string msg = ChatMessage::create(opcode, ack, user, "", "");
   return msg;
}

string ChatMessage::create(int opcode, int ack, string user1, string user2)
{
   string msg = ChatMessage::create(opcode, ack, user1, user2, "");
   return msg;
}

string ChatMessage::create(int opcode, int ack, string user1, string user2, string message)
{
   string _user1 = user1;
   string _user2 = user2;

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
   if(user1.length() > USER_SIZE)
   {
      cout << "Username \"" << user1 << "\" is longer than username limit: " 
           << to_string(USER_SIZE) << endl;
      exit(MESSAGE_ERROR);
   }
   if(user2.length() > USER_SIZE)
   {
      cout << "Username \"" << user2 << "\" is longer than username limit: "
           << to_string(USER_SIZE) << endl;
   }

   if(message.length() > MAXPACKET_SIZE-USER_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE)
   {
      cout << "Message length is " << message.length() 
           << " is longer than message size limit: " 
           << MAXPACKET_SIZE-USER_SIZE-USER_SIZE-OPCODE_SIZE-ACK_SIZE << endl;
      exit(MESSAGE_ERROR);
   }

   // Fill in unused space with '$' for user
   _user1 = ChatMessage::addPad(_user1);
   _user2 = ChatMessage::addPad(_user2);   

   string msg = to_string(opcode) + to_string(ack) + _user1 + _user2 + message;
   return msg;
}

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

string ChatMessage::removePad(string user)
{
   string moduser = user;
   for(int i = moduser.length()-1; i > -1; i--)
   {
      if(moduser[i] == '$')
         moduser.erase(i);
   }
   return moduser;
}
