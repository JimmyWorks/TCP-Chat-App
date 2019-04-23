/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * Constants used by TCP Chat App
 */

#define SERV_PORT 8080 // server port
#define MAXPACKET_SIZE 1450 // max byte size for app messages
#define OPCODE_SIZE 1 // byte size of opcode field
#define ACK_SIZE 1 // byte size of ack field
#define USER_SIZE 15 // byte size of user fields
#define ONLINE_TTL 5 // active user time to live in seconds
// Locations of json files relative to bin folder
#define USER_CONFIG "../json/config/user.conf"
#define USER_DB "../json/server/users.json"
