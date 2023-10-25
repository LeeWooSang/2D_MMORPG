#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>
#include <unordered_map>

using namespace std;
using namespace chrono;

#include <WinSock2.h>
#include <winsock.h>
#pragma comment (lib, "ws2_32.lib")

#include <Windows.h>
#include <math.h>			// Header File For Windows Math Library
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
//#include <gl\glaux.h>		// Header File For The Glaux Library

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")