#include "Input.h"
//#include <imm.h>
//#pragma comment(lib,"imm32.lib")

INIT_INSTACNE(Input)
Input::Input()
{
	m_mousePos.x = 0;
	m_mousePos.y = 0;
}

Input::~Input()
{
}

bool Input::Initialize()
{
	m_keyStateList[KEY_TYPE::NONE].m_keyType = KEY_TYPE::NONE;
	m_keyStateList[KEY_TYPE::KEYBOARD_LEFT].m_keyType = VK_LEFT;
	m_keyStateList[KEY_TYPE::KEYBOARD_RIGHT].m_keyType = VK_RIGHT;
	m_keyStateList[KEY_TYPE::KEYBOARD_UP].m_keyType = VK_UP;
	m_keyStateList[KEY_TYPE::KEYBOARD_DOWN].m_keyType = VK_DOWN;

	m_keyStateList[KEY_TYPE::KEYBOARD_TAB].m_keyType = VK_TAB;
	m_keyStateList[KEY_TYPE::KEYBOARD_ENTER].m_keyType = VK_RETURN;

	m_keyStateList[KEY_TYPE::KEYBOARD_CONTROL].m_keyType = VK_CONTROL;
	m_keyStateList[KEY_TYPE::KEYBOARD_ALT].m_keyType = VK_MENU;

	m_keyStateList[KEY_TYPE::MOUSE_LBUTTON].m_keyType = VK_LBUTTON;
	m_keyStateList[KEY_TYPE::MOUSE_RBUTTON].m_keyType = VK_RBUTTON;

	return true;
}

LRESULT Input::ProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
	m_mousePos.x = LOWORD(lParam);
	m_mousePos.y = HIWORD(lParam);

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:

		//cout << "X : " << x << ", Y : " << y << endl;
		//cout << "mX : " << mouse.x << ", mY : " << mouse.y << endl;
		break;

	default:
		break;
	}
}

bool Input::KeyOnceCheck(KEY_TYPE key)
{
	//processKeyEvent();

	if (m_keyStateList[key].m_pop == true)
	{
		return true;
	}

	return false;
}

void Input::ProcessKeyEvent()
{
	m_flag = false;

	for (auto& key : m_keyStateList)
	{
		// 키가 정의 되어 있지 않으면,
		if (key.m_keyType == KEY_TYPE::NONE)
		{
			continue;
		}

		// 지금 확인하는 키가 눌린 상태
		if (KEY_DOWN(key.m_keyType))
		{
			// 키를 처음 누른 경우
			if (key.m_pushing == false)
			{
				key.m_pushed = true;
			}
			// 키를 계속 누르고 있을 때,
			else
			{
				key.m_pushed = false;
			}

			key.m_pushing = true;
			m_flag = true;
		}

		// 키가 눌리지 않았을 때
		else
		{
			// 키를 눌렀다가 땔 떼
			if (key.m_pushing == true)
			{
				key.m_pop = true;
			}
			// 키를 누르지도 않았을 때
			else
			{
				key.m_pop = false;
			}

			key.m_pushing = false;
			key.m_pushed = false;
		}
	}
}