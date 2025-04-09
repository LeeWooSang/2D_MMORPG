#include "Log.h"
#include <fstream>
#include <sys/stat.h>
#include <direct.h>
#include <cstdio>

#include "../../../Client/Code/Common/Utility.h"

INIT_INSTACNE(Log)
Log::Log()
{
	mPath = "../LOG/";
	struct stat info;
	// 폴더가 존재하지 않으면 생성
	if (stat(mPath.c_str(), &info) != 0)
	{
		_mkdir(mPath.c_str());
	}

	mFlag = true;
	mJobThd = std::thread(&Log::run, this);
}

Log::~Log()
{
	mFlag = false;
	if (mJobThd.joinable() == true)
	{
		mJobThd.join();
	}
}

void Log::saveLog(const char* log, ...)
{
	char buf[MAX_LOG_BUF_SIZE] = { 0, };

	va_list va;
	va_start(va, log);
	vsnprintf(buf, sizeof(buf), log, va);
	va_end(va);

	std::string fileName = mPath + GetCurrentDate() + ".txt";
	std::ofstream file;
	// 파일을 추가(append) 모드
	file.open(fileName, std::ios::app);
	if (file.is_open() == true)
	{
		std::string logMsg = GetCurrentDateTime() + " - " + buf;
		file << logMsg << std::endl;
		file.close();
	}
}

using namespace std;
void Log::run()
{
	while (mFlag == true)
	{
		std::this_thread::sleep_for(10ms);
		while (true)
		{
			LogData data;
			if (mLogQu.try_pop(data) == false)
			{
				break;
			}

			int clientId = data.id;
			const char* ip = data.ip.c_str();
			if (data.type == 0)
			{
				saveLog("[LOG] clientId : %d, ip : %s, msg : %s", clientId, ip, data.msg);
				continue;
			}
			else
			{
				switch (data.msg[1])
				{
					case CS_PACKET_TYPE::CS_LOGIN:
					{
						CSLoginPacket* packet = reinterpret_cast<CSLoginPacket*>(data.msg);
						saveLog("[CS_LOGIN] clientId : %d, ip : %s, id : %s, password : %s", clientId, ip, packet->loginId, packet->loginPassword);
						break;
					}
					case CS_PACKET_TYPE::CS_MOVE:
					{
						CSMovePacket* packet = reinterpret_cast<CSMovePacket*>(data.msg);
						saveLog("[CS_MOVE] clientId : %d, ip : %s, direction : %d", clientId, ip, packet->direction);
						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
	}
}
