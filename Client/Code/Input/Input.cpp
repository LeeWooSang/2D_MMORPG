#include "Input.h"
#include <iostream>
#include "../Core/Core.h"
#include "../Scene/SceneManager.h"
#include <Windows.h>
//#include <imm.h>
//#pragma comment(lib,"imm32.lib")

INIT_INSTACNE(Input)
//std::array<KeyState, MAX_KEY_TYPE> Input::mKeyStateList;
std::pair<int, int> Input::mMousePos = std::make_pair(0, 0);
Input::Input()
{
	mFlag = false;
	mMousePos.first = 0;
	mMousePos.second = 0;
}

Input::~Input()
{
}

bool Input::Initialize()
{	
	for (int i = 0; i < MAX_KEY_TYPE; ++i)
	{
		mKeyStateList[i].keyType = KEY_TYPE::NONE;
		mKeyStateList[i].keyState = KEY_STATE::NONE;
		mKeyStateList[i].isPrevPush = false;
	}

	mKeyStateList[KEY_TYPE::NONE].keyType = KEY_TYPE::NONE;
	mKeyStateList[KEY_TYPE::LEFT_KEY].keyType = VK_LEFT;
	mKeyStateList[KEY_TYPE::RIGHT_KEY].keyType = VK_RIGHT;
	mKeyStateList[KEY_TYPE::UP_KEY].keyType = VK_UP;
	mKeyStateList[KEY_TYPE::DOWN_KEY].keyType = VK_DOWN;

	mKeyStateList[KEY_TYPE::TAB_KEY].keyType = VK_TAB;
	mKeyStateList[KEY_TYPE::ENTER_KEY].keyType = VK_RETURN;
	mKeyStateList[KEY_TYPE::CONTROL_KEY].keyType = VK_CONTROL;
	mKeyStateList[KEY_TYPE::ALT_KEY].keyType = VK_MENU;
	mKeyStateList[KEY_TYPE::CAPS_LOCK_KEY].keyType = VK_CAPITAL;

	mKeyStateList[KEY_TYPE::F1_KEY].keyType = VK_F1;
	mKeyStateList[KEY_TYPE::F2_KEY].keyType = VK_F2;
	mKeyStateList[KEY_TYPE::F3_KEY].keyType = VK_F3;
	mKeyStateList[KEY_TYPE::F4_KEY].keyType = VK_F4;
	mKeyStateList[KEY_TYPE::F5_KEY].keyType = VK_F5;
	mKeyStateList[KEY_TYPE::F6_KEY].keyType = VK_F6;
	mKeyStateList[KEY_TYPE::F7_KEY].keyType = VK_F7;
	mKeyStateList[KEY_TYPE::F8_KEY].keyType = VK_F8;
	mKeyStateList[KEY_TYPE::F9_KEY].keyType = VK_F9;
	mKeyStateList[KEY_TYPE::F10_KEY].keyType = VK_F10;
	mKeyStateList[KEY_TYPE::F11_KEY].keyType = VK_F11;
	mKeyStateList[KEY_TYPE::F12_KEY].keyType = VK_F12;

	// 0 ~ 9
	{
		int key = 0x30;
		for (int i = KEY_TYPE::NUM0_KEY; i <= KEY_TYPE::NUM9_KEY; ++i)
		{
			mKeyStateList[i].keyType = key++;
		}
	}
	// numpad0 ~ 9
	{
		int key = 0x60;
		for (int i = KEY_TYPE::NUMPAD0_KEY; i <= KEY_TYPE::NUMPAD9_KEY; ++i)
		{
			mKeyStateList[i].keyType = key++;
		}
	}
	{
		// A~Z키
		int key = 0x41;
		for (int i = KEY_TYPE::A_KEY; i <= KEY_TYPE::Z_KEY; ++i)
		{
			mKeyStateList[i].keyType = key++;
		}
	}

	mKeyStateList[KEY_TYPE::BACK_SPACE].keyType = VK_BACK;
	mKeyStateList[KEY_TYPE::SPACE].keyType = VK_SPACE;
	mKeyStateList[KEY_TYPE::SHIFT_KEY].keyType = VK_SHIFT;

	mKeyStateList[KEY_TYPE::COMMA_KEY].keyType = VK_OEM_COMMA;
	mKeyStateList[KEY_TYPE::DOT_KEY].keyType = VK_OEM_PERIOD;
	mKeyStateList[KEY_TYPE::SLASH_KEY].keyType = VK_OEM_2;
	mKeyStateList[KEY_TYPE::LEFT_BRACKET].keyType = VK_OEM_4;
	mKeyStateList[KEY_TYPE::BACK_SLASH_KEY].keyType = VK_OEM_5;
	mKeyStateList[KEY_TYPE::RIGHT_BRACKET].keyType = VK_OEM_6;
	mKeyStateList[KEY_TYPE::TILDE_KEY].keyType = VK_OEM_3;
	mKeyStateList[KEY_TYPE::SEMICOLON_KEY].keyType = VK_OEM_1;
	mKeyStateList[KEY_TYPE::APOSTROPHE_KEY].keyType = VK_OEM_7;
	mKeyStateList[KEY_TYPE::PLUS_KEY].keyType = VK_OEM_PLUS;
	mKeyStateList[KEY_TYPE::HYPHEN_KEY].keyType = VK_OEM_MINUS;
	
	mKeyStateList[KEY_TYPE::MOUSE_LBUTTON].keyType = VK_LBUTTON;
	mKeyStateList[KEY_TYPE::MOUSE_RBUTTON].keyType = VK_RBUTTON;

	return true;
}

void Input::ProcessWindowMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	switch (msg)
	{
		case WM_IME_COMPOSITION:
		case WM_IME_NOTIFY:		
		case WM_CHAR:				
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			ProcessKeyboardMessage(msg, wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_VSCROLL:
		case WM_MOUSEWHEEL:
			ProcessMouseMessage(msg, wParam, lParam);
			break;

		default:
			break;
	}
}

void Input::ProcessKeyboardMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	if (msg == WM_KEYDOWN)
	{
		if (wParam == VK_ESCAPE)
		{
			GET_INSTANCE(Core)->Quit();
			return;
		}
		
		std::cout << wParam << std::endl;	
	}
}

void Input::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	switch (msg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			mMousePos.first = LOWORD(lParam);
			mMousePos.second = HIWORD(lParam);
			break;
		}

		case WM_MOUSEWHEEL:
		{
			GET_INSTANCE(SceneManager)->ProcessMouseMessage(msg, wParam, lParam);
			break;
		}

		default:
			break;
	}
}

bool Input::KeyOnceCheck(KEY_TYPE key)
{
	//processKeyEvent();

	if (mKeyStateList[key].keyState == KEY_STATE::TAP)
	{
		return true;
	}

	return false;
}

void Input::ProcessKeyEvent()
{
	// 포커스 있을때만 입력처리
	HWND handle = GetFocus();
	if (handle == nullptr)
	{
		return;
	}

	setMousePos();

	for (int i = 0; i < mKeyStateList.size(); ++i)
	{
		// 키가 정의 되어 있지 않으면,
		if (mKeyStateList[i].keyType == KEY_TYPE::NONE)
		{
			continue;
		}

		// 지금 확인하는 키가 눌린 상태
		if (KEY_DOWN(mKeyStateList[i].keyType))
		{
			// 키를 처음 누른 경우
			if (mKeyStateList[i].isPrevPush == true)
			{
				mKeyStateList[i].keyState = KEY_STATE::HOLD;
			}
			// 키를 계속 누르고 있을 때,
			else
			{
				mKeyStateList[i].keyState = KEY_STATE::TAP;
			}
			mKeyStateList[i].isPrevPush = true;
		}

		// 키가 눌리지 않았을 때
		else
		{
			// 키를 눌렀다가 땔 떼
			if (mKeyStateList[i].isPrevPush == true)
			{
				mKeyStateList[i].keyState = KEY_STATE::AWAY;
			}
			// 키를 누르지도 않았을 때
			else
			{
				mKeyStateList[i].keyState = KEY_STATE::NONE;
			}
			mKeyStateList[i].isPrevPush = false;
		}
	}
}

void Input::setMousePos()
{
	POINT mouse;
	::GetCursorPos(&mouse);
	::ScreenToClient(GET_INSTANCE(Core)->GetMainHandle(), &mouse);
	mMousePos.first = mouse.x;
	mMousePos.second = mouse.y;
}
