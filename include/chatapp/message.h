#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <iostream>
#include "chatapp/chat_enums.h"
#include "chatapp/constants.h"

using namespace std;

class ChatMessage
{
   public:
   static string create(int opcode);
   static string create(int opcode, int ack);
   static string create(int opcode, int ack, string user);
   static string create(int opcode, int ack, string user1, string user2);
   static string create(int opcode, int ack, string user1, string user2, string message);
   static string addPad(string user);
   static string removePad(string user);

};

#endif
