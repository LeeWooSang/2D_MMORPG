#pragma once

#include <iostream>
#include <vector>
#include <thread>

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#include <tbb/tbb.h>
#ifdef _DEBUG
// ����� ���� �ܼ�â ����
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib, "tbb_debug.lib")
#else
#pragma comment(lib, "tbb.lib")
#endif

#include "Protocol.h"