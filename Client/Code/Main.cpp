#define WIN32_LEAN_AND_MEAN  
#define INITGUID

#include "./Common/Defines.h"
#include "Core/Core.h"

// defines for windows 
#define WINDOW_CLASS_NAME L"Client"  // class name

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	GET_INSTANCE(Core)->WindowProc(hwnd, msg, wparam, lparam);

	PAINTSTRUCT	ps;		 
	HDC	hdc;	 
	
	switch (msg)
	{
	case WM_CREATE:
		return 0; 

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

	default:
		break;
	} 

	return (DefWindowProc(hwnd, msg, wparam, lparam));
} 

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASS winclass;	
	HWND	 hwnd;		
	MSG		 msg;		

	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = nullptr;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&winclass))
		return(0);

	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
		L"Client",	 // title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,	   // x,y
		WINDOW_WIDTH,  // width
		WINDOW_HEIGHT, // height
		nullptr,	   // handle to parent 
		nullptr,	   // handle to menu
		hinstance,// instance
		nullptr)))	// creation parms
		return(0);

	//main_window_handle = hwnd;
	//main_instance = hinstance;

	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// 게임 초기화
	if (GET_INSTANCE(Core)->Initialize(hwnd, width, height) == false)
	{
		GET_INSTANCE(Core)->Release();
		return false;
	}

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} 

		GET_INSTANCE(Core)->Run();
	} 

	GET_INSTANCE(Core)->Release();

	return(msg.wParam);
} 