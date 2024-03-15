#pragma once
#include "Macro.h"
#include <string>
#include <unordered_set>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
using namespace rapidjson;

class JsonFormat
{
    SINGLE_TONE(JsonFormat)

    bool LoadCashItems(rapidjson::Document& doc);
    bool LoadJsonFile(rapidjson::Document& doc, const std::string& path);
    bool SaveJsonFile(rapidjson::Document& doc, const std::string& path);

private:

    void makeJsonFormatString(std::string& format, rapidjson::Document& doc);
};

