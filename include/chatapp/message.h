/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Header for ChatMessage
 */

#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <iostream>
#include "chatapp/chat_enums.h"
#include "chatapp/constants.h"
using namespace std;

// ChatMessage
// Class used to create ChatMessages deserialized from received serialized messages 
// and serialize new messages to send.
class ChatMessage
{
   public:
   int opcode, ack;
   string user1, user2, message;

   // Overloaded Constructors
   ChatMessage();
   ChatMessage(int opcode);
   ChatMessage(int opcode, int ack);
   ChatMessage(int opcode, int ack, string user1);
   ChatMessage(int opcode, int ack, string user1, string user2);
   ChatMessage(int opcode, int ack, string user1, string user2, string message);

   // Overloaded serialized ChatMessages
   static string serialize(int opcode);
   static string serialize(int opcode, int ack);
   static string serialize(int opcode, int ack, string user);
   static string serialize(int opcode, int ack, string user1, string user2);
   static string serialize(int opcode, int ack, string user1, string user2, string message);
   
   // Deserialize
   static ChatMessage deserialize(string msg);

   // Internal methods used to verify and format arguments
   private:
   void init(int opcode, int ack, string user1, string user2, string message);
   static void verifyOperation(int opcode);
   static void verifyAck(int ack);
   static void verifyUser(string user1);
   static void verifyMessage(string message);
   static string addPad(string user);
   static string removePad(string user);
};

#endif
