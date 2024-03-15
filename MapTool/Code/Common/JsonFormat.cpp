#define _CRT_SECURE_NO_WARNINGS
#include "JsonFormat.h"
#include <fstream>
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

INIT_INSTACNE(JsonFormat)
JsonFormat::JsonFormat()
{
}

JsonFormat::~JsonFormat()
{
}

bool JsonFormat::LoadCashItems(rapidjson::Document& doc)
{
    std::string path = "../Resource/CashItems.json";
    if(LoadJsonFile(doc, path) == false)
    {
        return false;
    }

    return true;
}

bool JsonFormat::LoadJsonFile(rapidjson::Document& doc, const std::string& path)
{
    FILE* in = fopen(path.c_str(), "rb");
    if(!in)
    {
        return false;
    }

    char buf[1024] = { 0, };
    rapidjson::FileReadStream is(in, buf, sizeof(buf));
    doc.ParseStream(is);

    fclose(in);
    return true;
}

bool JsonFormat::SaveJsonFile(rapidjson::Document& doc, const std::string& path)
{
    FILE* out = nullptr;

    std::ifstream in(path);
    if (!in)
    {
        out = fopen(path.c_str(), "w");
    }
    else
    {
        out = fopen(path.c_str(), "a");
    }

    char buf[1024] = { 0, };
    rapidjson::FileWriteStream os(out, buf, sizeof(buf));

    rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
    doc.Accept(writer);

    fclose(out);
    return true;
}

void JsonFormat::makeJsonFormatString(std::string& format, rapidjson::Document& doc)
{
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    format = strbuf.GetString();
}
