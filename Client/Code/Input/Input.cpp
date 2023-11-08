#include "Input.h"
#include <iostream>
//#include <imm.h>
//#pragma comment(lib,"imm32.lib")

INIT_INSTACNE(Input)
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
	mKeyStateList[KEY_TYPE::NONE].keyType = KEY_TYPE::NONE;
	mKeyStateList[KEY_TYPE::LEFT_KEY].keyType = VK_LEFT;
	mKeyStateList[KEY_TYPE::RIGHT_KEY].keyType = VK_RIGHT;
	mKeyStateList[KEY_TYPE::UP_KEY].keyType = VK_UP;
	mKeyStateList[KEY_TYPE::DOWN_KEY].keyType = VK_DOWN;

	mKeyStateList[KEY_TYPE::TAB_KEY].keyType = VK_TAB;
	mKeyStateList[KEY_TYPE::ENTER_KEY].keyType = VK_RETURN;
	mKeyStateList[KEY_TYPE::CONTROL_KEY].keyType = VK_CONTROL;
	mKeyStateList[KEY_TYPE::ALT_KEY].keyType = VK_MENU;

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

	mKeyStateList[KEY_TYPE::MOUSE_LBUTTON].keyType = VK_LBUTTON;
	mKeyStateList[KEY_TYPE::MOUSE_RBUTTON].keyType = VK_RBUTTON;

	return true;
}

LRESULT Input::ProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND handle = GetFocus();
	if (handle == nullptr)
	{
		return 0;
	}

	switch (message)
	{
		case WM_IME_COMPOSITION:
		case WM_IME_NOTIFY:		
		case WM_CHAR:				
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			ProcessKeyboardMessage(hWnd, message, wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
			ProcessMouseMessage(hWnd, message, lParam);
			break;

		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0;
}

LRESULT Input::ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Scene* scene = GET_INSTANCE(SceneManager)->GetScene();
	//if (scene != nullptr)
	//	scene->ProcessKeyboardMessage(hWnd, message, wParam, lParam);

	return 0;
}

void Input::ProcessMouseMessage(HWND hWnd, UINT message, LPARAM lParam)
{
	switch (message)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		{
			mMousePos.first = LOWORD(lParam);
			mMousePos.second = HIWORD(lParam);

			//std::cout << "X : " << mMousePos.first << ", Y : " << mMousePos.second << std::endl;
			//cout << "mX : " << mouse.x << ", mY : " << mouse.y << endl;
			break;
		}

		default:
			break;
	}
}

bool Input::KeyOnceCheck(KEY_TYPE key)
{
	//processKeyEvent();

	if (mKeyStateList[key].pop == true)
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

	mFlag = false;

	for (auto& key : mKeyStateList)
	{
		// 키가 정의 되어 있지 않으면,
		if (key.keyType == KEY_TYPE::NONE)
		{
			continue;
		}

		// 지금 확인하는 키가 눌린 상태
		if (KEY_DOWN(key.keyType))
		{
			// 키를 처음 누른 경우
			if (key.pushing == false)
			{
				key.pushed = true;
			}
			// 키를 계속 누르고 있을 때,
			else
			{
				key.pushed = false;
			}

			key.pushing = true;
			mFlag = true;
		}

		// 키가 눌리지 않았을 때
		else
		{
			// 키를 눌렀다가 땔 떼
			if (key.pushing == true)
			{
				key.pop = true;
			}
			// 키를 누르지도 않았을 때
			else
			{
				key.pop = false;
			}

			key.pushing = false;
			key.pushed = false;
		}
	}
}