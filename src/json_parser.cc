/* TCP Chat App
 * Author: Jimmy Nguyen
 * Email: Jimmy@Jimmyworks.net
 * 
 * JsonParser implementation file
 */

#include "chatapp/json_parser.h"

// Parse
// Uses rapidjson libraries to parse json files
rapidjson::Document JsonParser::Parse(string path)
{
   // Open the file to process
   ifstream file;
   file.open(path);
   if(!file.is_open())
   {
      cout << "Failed to open file to parse..." << endl;
      exit(FILE_ACCESS_ERROR);
   }
   else
      cout << "Opened file..." << endl;

   // Get contents of file
   string buffer, line;
   while( getline(file, line))
      buffer += line;
   file.close();

   // Parse the file
   rapidjson::Document d;
   d.Parse(buffer.c_str());
   if(!d.IsObject())
   {
      cout << "Failed to parse file..." << endl;
      exit(FILE_PARSE_ERROR);
   }
   cout << "Successfully parsed file..." << endl;

   return d;
}
