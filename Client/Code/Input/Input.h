#pragma once
#include "../Common/Macro.h"
#include <array>
#include <vector>

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

	BACK_SPACE, SPACE, SHIFT_KEY,

	// ! " # $ % & '
	EXCLAMATION_MARK_KEY, QUOTATION_MARK_KEY, SHARP_KEY, DOLLAR_KEY, PERCENT_KEY, AMPERSAND_KEY, APOSTROPHE_KEY,
	// ( ) * + , - . /
	LEFT_PARENTHESIS_KEY, RIGHT_PARENTHESIS_KEY, ASTERISK_KEY, PLUS_KEY, COMMA_KEY, HYPHEN_KEY, DOT_KEY, SLASH_KEY,
	// : ; < = > ? @
	COLON_KEY, SEMICOLON_KEY, LEFT_ANGLE_BRACKET, EQUAL_KEY, RIGHT_ANGLE_BRACKET, QUESTION_MARK_KEY, AT_KEY,
	// [ \ ] ^ _ `
	LEFT_BRACKET, BACK_SLASH_KEY, RIGHT_BRACKET, CIRCUMFLEX_KEY, UNDER_SCORE_KEY, GRAVE_ACCENT_KEY, 
	// { | } ~
	LEFT_BRACE, VERTICAL_BAR_KEY, RIGHT_BRACE, TILDE_KEY,

	ESC_KEY,

	MOUSE_LBUTTON, MOUSE_RBUTTON
};

constexpr int MAX_KEY_TYPE = MOUSE_RBUTTON + 1;

enum class KEY_STATE
{
	NONE,	// 누르지 않았고, 이전에도 눌리지 않은 상태
	TAP,		// 막 누른 시점
	HOLD,	// 누르고 있는
	AWAY	// 막 땐 시점
};
struct KeyState
{
	KeyState()
		: keyType(KEY_TYPE::NONE), keyState(KEY_STATE::NONE), isPrevPush(false) {}
	KeyState(int type, KEY_STATE state, bool prevPush)
		: keyType(type), keyState(state), isPrevPush(prevPush) {}

	int keyType;
	KEY_STATE keyState;
	bool isPrevPush;
	//// 키를 누를 순간
	//bool pushed;
	//// 키를 계속 누르고 있을 때
	//bool pushing;
	//// 키를 떼는 순간
	//bool pop;
};

class Input
{
	SINGLE_TONE(Input)
		
public:		
	bool Initialize();

	void ProcessWindowMessage(unsigned int msg, unsigned long long wParam, long long lParam);
	void ProcessKeyboardMessage(unsigned int msg, unsigned long long wParam, long long lParam);
	void ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam);

	const std::pair<int, int>& GetMousePos() const { return mMousePos; }

	bool KeyOnceCheck(KEY_TYPE);
	
	KEY_STATE GetKeyState(KEY_TYPE key) 
	{
		return mKeyStateList[key].keyState; 
	}
	//bool GetIsPushed(KEY_TYPE key)	const { return mKeyStateList[key].pushed; }
	//bool GetIsPushing(KEY_TYPE key)	const { return mKeyStateList[key].pushing; }
	//bool GetIsPop(KEY_TYPE key)	const { return mKeyStateList[key].pop; }
	void ProcessKeyEvent();

private:
	void setMousePos();
	
	void processKorean(long long lParam);

private:
	//static std::array<KeyState, MAX_KEY_TYPE> mKeyStateList;
	std::array<KeyState, MAX_KEY_TYPE> mKeyStateList;

	static std::pair<int, int> mMousePos;
	bool mFlag;

};