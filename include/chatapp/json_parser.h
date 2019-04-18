
#ifndef _JSON_PARSER_H
#define _JSON_PARSER_H


#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "chatapp/chat_enums.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;

class JsonParser
{
   public:
   static rapidjson::Document Parse(string path);
};

#endif

