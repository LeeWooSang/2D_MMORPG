#pragma once
#include "../Common/Macro.h"
#include <Windows.h>
#include <array>

constexpr int MAX_LEN = 256;
#define KEY_DOWN(VK_CODE)	((GetAsyncKeyState(VK_CODE) & 0x8000) ? 1 : 0)
#define KEY_UP(VK_CODE)			((GetAsyncKeyState(VK_CODE) & 0x8000) ? 0 : 1)

//struct TextInfo
//{
//	TextInfo(unsigned char key, const wstring& text)
//		: m_Key(key), m_Text(text) {}
//
//	unsigned char m_Key;
//	wstring m_Text;
//};

constexpr int MAX_KEY_TYPE = 11;
enum KEY_TYPE
{
	NONE = 0,

	KEYBOARD_UP,
	KEYBOARD_DOWN,
	KEYBOARD_LEFT,
	KEYBOARD_RIGHT,

	KEYBOARD_TAB,
	KEYBOARD_ENTER,

	KEYBOARD_CONTROL,
	KEYBOARD_ALT,

	MOUSE_LBUTTON,
	MOUSE_RBUTTON
};

struct KeyState
{
	KeyState()
		: m_keyType(NONE), m_pushed(false), m_pushing(false), m_pop(false) {}

	int m_keyType;
	// 키를 누를 순간
	bool m_pushed;
	// 키를 계속 누르고 있을 때
	bool m_pushing;
	// 키를 떼는 순간
	bool m_pop;
};

class Input
{
	SINGLE_TONE(Input)
		
public:		
	virtual bool Initialize();

	virtual LRESULT CALLBACK ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);

	LRESULT ProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);
	void ProcessMouseMessage(HWND, UINT, LPARAM);

	const POINT& GetMousePos() const { return m_mousePos; }

	bool KeyOnceCheck(KEY_TYPE);
	bool GetIsPushed(KEY_TYPE key)	const { return m_keyStateList[key].m_pushed; }
	bool GetIsPushing(KEY_TYPE key)	const { return m_keyStateList[key].m_pushing; }
	bool GetIsPop(KEY_TYPE key)	const { return m_keyStateList[key].m_pop; }
	void ProcessKeyEvent();

private:

	POINT m_mousePos;
	std::array<KeyState, MAX_KEY_TYPE> m_keyStateList;
	bool m_flag;
};