#pragma once
#include "../Common/Macro.h"
#include <array>

#define KEY_DOWN(VK_CODE)	((GetAsyncKeyState(VK_CODE) & 0x8000) ? 1 : 0)
#define KEY_UP(VK_CODE)			((GetAsyncKeyState(VK_CODE) & 0x8000) ? 0 : 1)

enum KEY_TYPE
{
	NONE = 0,

	UP_KEY, DOWN_KEY, LEFT_KEY, RIGHT_KEY,
	TAB_KEY, ENTER_KEY, CONTROL_KEY, ALT_KEY, CAPS_LOCK_KEY,

	F1_KEY, F2_KEY, F3_KEY, F4_KEY, F5_KEY, F6_KEY, F7_KEY, F8_KEY, F9_KEY, F10_KEY, F11_KEY, F12_KEY,
	NUM0_KEY, NUM1_KEY, NUM2_KEY, NUM3_KEY, NUM4_KEY, NUM5_KEY, NUM6_KEY, NUM7_KEY, NUM8_KEY, NUM9_KEY,
	NUMPAD0_KEY, NUMPAD1_KEY, NUMPAD2_KEY, NUMPAD3_KEY, NUMPAD4_KEY, NUMPAD5_KEY, NUMPAD6_KEY, NUMPAD7_KEY, NUMPAD8_KEY, NUMPAD9_KEY,

	A_KEY, B_KEY, C_KEY, D_KEY, E_KEY, F_KEY, G_KEY, H_KEY, I_KEY, J_KEY, K_KEY, L_KEY, M_KEY, N_KEY, 
	O_KEY, P_KEY, Q_KEY, R_KEY, S_KEY, T_KEY, U_KEY, V_KEY, W_KEY, X_KEY, Y_KEY, Z_KEY,

	BACK_SPACE, SPACE,

	MOUSE_LBUTTON, MOUSE_RBUTTON
};

constexpr int MAX_KEY_TYPE = MOUSE_RBUTTON + 1;

enum class KEY_STATE
{
	NONE,	// ������ �ʾҰ�, �������� ������ ���� ����
	TAP,		// �� ���� ����
	HOLD,	// ������ �ִ�
	AWAY	// �� �� ����
};
struct KeyState
{
	KeyState()
		: keyType(KEY_TYPE::NONE), keyState(KEY_STATE::NONE), isPrevPush(false) {}

	int keyType;
	KEY_STATE keyState;
	bool isPrevPush;
	//// Ű�� ���� ����
	//bool pushed;
	//// Ű�� ��� ������ ���� ��
	//bool pushing;
	//// Ű�� ���� ����
	//bool pop;
};

class Input
{
	SINGLE_TONE(Input)
		
public:		
	virtual bool Initialize();

	void ProcessWindowMessage(unsigned int msg, unsigned long long wParam, long long lParam);
	void ProcessKeyboardMessage(unsigned int msg, unsigned long long wParam, long long lParam);
	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

	void SetMousePos(std::pair<int, int> pos) { mMousePos = pos; }
	const std::pair<int, int>& GetMousePos() const { return mMousePos; }

	bool KeyOnceCheck(KEY_TYPE);
	
	KEY_STATE GetKeyState(KEY_TYPE key) const { return mKeyStateList[key].keyState; }
	//bool GetIsPushed(KEY_TYPE key)	const { return mKeyStateList[key].pushed; }
	//bool GetIsPushing(KEY_TYPE key)	const { return mKeyStateList[key].pushing; }
	//bool GetIsPop(KEY_TYPE key)	const { return mKeyStateList[key].pop; }
	void ProcessKeyEvent();

	
private:
	std::array<KeyState, MAX_KEY_TYPE> mKeyStateList;
	std::pair<int, int> mMousePos;
	bool mFlag;

};