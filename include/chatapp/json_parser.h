
#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "chatapp/errorcodes.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class JsonParser
{
   public:
   static rapidjson::Document Parse(string path);
};

#endif

