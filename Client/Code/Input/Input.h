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

enum KEY_TYPE
{
	NONE = 0,

	UP_KEY, DOWN_KEY, LEFT_KEY, RIGHT_KEY,
	TAB_KEY, ENTER_KEY, CONTROL_KEY, ALT_KEY,

	F1_KEY, F2_KEY, F3_KEY, F4_KEY, F5_KEY, F6_KEY, F7_KEY, F8_KEY, F9_KEY, F10_KEY, F11_KEY, F12_KEY,

	A_KEY, B_KEY, C_KEY, D_KEY, E_KEY, F_KEY, G_KEY, H_KEY, I_KEY, J_KEY, K_KEY, L_KEY, M_KEY, N_KEY, 
	O_KEY, P_KEY, Q_KEY, R_KEY, S_KEY, T_KEY, U_KEY, V_KEY, W_KEY, X_KEY, Y_KEY, Z_KEY,

	MOUSE_LBUTTON, MOUSE_RBUTTON
};

constexpr int MAX_KEY_TYPE = MOUSE_RBUTTON + 1;

struct KeyState
{
	KeyState()
		: keyType(NONE), pushed(false), pushing(false), pop(false) {}

	int keyType;
	// 키를 누를 순간
	bool pushed;
	// 키를 계속 누르고 있을 때
	bool pushing;
	// 키를 떼는 순간
	bool pop;
};

class Input
{
	SINGLE_TONE(Input)
		
public:		
	virtual bool Initialize();

	virtual LRESULT CALLBACK ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);

	LRESULT ProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);
	void ProcessMouseMessage(HWND, UINT, LPARAM);

	const std::pair<int, int>& GetMousePos() const { return mMousePos; }

	bool KeyOnceCheck(KEY_TYPE);
	bool GetIsPushed(KEY_TYPE key)	const { return mKeyStateList[key].pushed; }
	bool GetIsPushing(KEY_TYPE key)	const { return mKeyStateList[key].pushing; }
	bool GetIsPop(KEY_TYPE key)	const { return mKeyStateList[key].pop; }
	void ProcessKeyEvent();

	
private:
	std::array<KeyState, MAX_KEY_TYPE> mKeyStateList;
	std::pair<int, int> mMousePos;
	bool mFlag;
};