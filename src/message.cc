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

   int message_size = MAXPACKET_SIZE - USER_SIZE - 2;

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
      cout << "Username \"" << user << "\" is longer than username limit: " << to_string(USER_SIZE) << endl;
      exit(MESSAGE_ERROR);
   }
   if(message.length() > message_size)
   {
      cout << "Message is longer than message size limit: " << to_string(message_size) << endl;
      exit(MESSAGE_ERROR);
   }

   for(int i = 0; i < (USER_SIZE-user.length()); i++)
   {
      user += '$';
   }

   string msg = to_string(opcode) + to_string(ack) + user + message;
/* 
   switch(opcode)
   {
      case HEARTBEAT:
         break;
      case CONN_REQ:
         break;
      case CONN_TERM:
         break;
      case CHAT_REQ:
         break;
      case CHAT_TERM:
         break;
      case CHAT_MESSAGE:
         break;
      case default:
         cout << "Invalid operation not supported..." << endl;
         exit(INVALID_OPCODE);
   }
*/
   return msg;
}
