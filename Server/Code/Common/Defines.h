#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <memory>
#include <random>

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
