#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <memory>

#include <winsock2.h>
//#pragma comment(lib, "Ws2_32.lib")

#include <tbb/tbb.h>

#ifdef _DEBUG
// �޸� ���� �ִ��� �˷��ش�.
#include <crtdbg.h>

#pragma comment(lib, "tbb_debug.lib")

#else
#pragma comment(lib, "tbb.lib")

#endif

// ����� ���� �ܼ�â ����
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

//#include "Protocol.h"