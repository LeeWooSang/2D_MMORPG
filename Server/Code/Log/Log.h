#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"
#include "../Common/Protocol.h"
#include <iostream>

constexpr int MAX_LOG_BUF_SIZE = 1024;
struct LogData
{
	LogData()
		: type(0), id(-1), ip("")	{ memset(msg, 0, MAX_LOG_BUF_SIZE); }
	LogData(int _type, int _id, const std::string& _ip, char* p)
		: type(_type), id(_id), ip(_ip) { memcpy(msg, p, sizeof(msg)); }
	LogData(int _type, int _id, const std::string& _ip, char* p, int len)
		: type(_type), id(_id), ip(_ip)	{ memcpy(msg, p, len); }
	int type;
	int id;
	std::string ip;
	char msg[MAX_LOG_BUF_SIZE];
};

class Log
{
	SINGLE_TONE(Log)

public:
	void PushLog(LogData& data) { mLogQu.push(data); }

private:
	void saveLog(const char* log, ...);
	void run();

private:
	std::string mPath;
	volatile bool mFlag;
	std::thread mJobThd;
	tbb::concurrent_queue<LogData> mLogQu;
};

