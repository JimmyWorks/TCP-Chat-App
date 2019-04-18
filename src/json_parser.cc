
#include "chatapp/json_parser.h"

rapidjson::Document JsonParser::Parse(string path)
{

   ifstream file;
   file.open(path);
   if(!file.is_open())
   {
      cout << "Failed to open file to parse..." << endl;
      exit(FILE_ACCESS_ERROR);
   }
   else
      cout << "Opened file..." << endl;

   string buffer, line;
   while( getline(file, line))
      buffer += line;

   file.close();
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
