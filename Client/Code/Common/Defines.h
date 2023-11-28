#pragma once

#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#include <crtdbg.h>
#endif // DEBUG

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment (lib, "ws2_32.lib")

#include <Windows.h>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   800

#define SERVER_CONNECT

#define LOGIN_ID					"test_id"
#define LOGIN_PASSWORD		"test_password"
