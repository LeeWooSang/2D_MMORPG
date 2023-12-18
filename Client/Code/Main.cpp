#define WIN32_LEAN_AND_MEAN  

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

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		{
			return 0;
		}

	default:
		break;
	} 

	//return 0;
	return (DefWindowProc(hwnd, msg, wparam, lparam));
} 

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASS winclass;
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

	// 윈도우 기본스타일 | 최소화 버튼 | X 버튼 | 바로 출력되도록 
	DWORD dwStyle = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE;
	HWND hwnd = CreateWindow(
		WINDOW_CLASS_NAME,
		L"Client",
		dwStyle,
		0,
		0,
		WINDOW_WIDTH,  // width
		WINDOW_HEIGHT, // height
		nullptr,	   // handle to parent 
		nullptr,	   // handle to menu
		hinstance,// instance
		nullptr);

	RECT rect;
	GetClientRect(hwnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// 초기화
	if (GET_INSTANCE(Core)->Initialize(hwnd, width, height) == false)
	{
		GET_INSTANCE(Core)->Release();
		return false;
	}

	MSG msg;
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