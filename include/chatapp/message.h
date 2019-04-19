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
   static string create(int opcode, string user);
   static string create(int opcode, string user, string message);
   static string create(int opcode, int ack, string message);
   static string create(int opcode, int ack, string user, string message);
};

#endif
