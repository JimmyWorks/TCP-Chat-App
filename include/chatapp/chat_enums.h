/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Enumerations used by the TCP Chat App 
 */

#ifndef _CHATAPP_ENUMS_H
#define _CHATAPP_ENUMS_H

// Error Codes
// Used to pass return codes outside the app
enum Error_Codes
{
   SUCCESS,
   SERVER_CONN_FAILED,
   INIT_ERROR,
   CLI_ERROR,
   FILE_ACCESS_ERROR,
   FILE_PARSE_ERROR,
   SOCK_CREATE_FAILURE,
   SOCK_BIND_FAILURE,
   SOCK_LISTEN_FAILURE,
   INVALID_OPCODE,
   MESSAGE_ERROR
};

// Op Codes
// Type of messages the app can pass between
// the client and server
enum Op_Codes
{
   HEARTBEAT,
   CONN_REQ,
   CONN_TERM,
   CHAT_MESSAGE,
   CHAT_SMESSAGE,
   INVALID_OP
};

// Response Code
// Simple enum for seeing when ack/nack is passed
enum Response_Codes
{
   ACK,
   NACK
};

#endif
