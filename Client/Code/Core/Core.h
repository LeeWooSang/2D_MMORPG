#pragma once
#include "../Common/Macro.h"
#include "../Common/Defines.h"

class Core
{
	SINGLE_TONE(Core)

public:
	bool Initialize(HWND handle, int width, int height);
	void Run();
	void Quit();

	void WindowProc(HWND handle, unsigned int msg, unsigned long long wParam, long long lParam);
	HWND GetMainHandle() { return mHandle; }

private:
	HWND mHandle;
};