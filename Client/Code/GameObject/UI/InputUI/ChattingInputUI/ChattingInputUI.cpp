#include "ChattingInputUI.h"
#include "../../../../Input/Input.h"
#include "../../../../Network/Network.h"
#include "../../../../GraphicEngine/GraphicEngine.h"
#include "../../ChattingBox/ChattingBox.h"

ChattingInputUI::ChattingInputUI()
	: InputUI()
{
	mOpen = false;
	mChatState = CHAT_STATE::NONE;
}

ChattingInputUI::~ChattingInputUI()
{
}

bool ChattingInputUI::Initialize(int x, int y)
{
	InputUI::Initialize(x, y);

	mCarrotPos = std::make_pair(0, 705);
	Visible();

	return true;
}

void ChattingInputUI::Update()
{
	if (mChatState == CHAT_STATE::NONE)
	{
		mOpen = false;
		NotVisible();
	}

	InputUI::Update();
}

void ChattingInputUI::Render()
{
	InputUI::Render();
}

void ChattingInputUI::MouseOver()
{
}

void ChattingInputUI::MouseLButtonDown()
{
}

void ChattingInputUI::MouseLButtonUp()
{
}

void ChattingInputUI::MouseLButtonClick()
{
}

void ChattingInputUI::OpenChattingBox()
{
	// �����ִٸ�
	if (mOpen == false)
	{
		mChatState = CHAT_STATE::CHATTING;
		mOpen = true;
		Visible();
	}
	else
	{
		if (mChatState == CHAT_STATE::CHATTING && mText.length() <= 0)
		{
			mChatState = CHAT_STATE::NONE;
		}
	}
}

void ChattingInputUI::processInput()
{
	for (int i = KEY_TYPE::NUM0_KEY; i <= KEY_TYPE::NUM9_KEY; ++i)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
		{
			wchar_t wc = i + 26;
			if (mCarrotIndex == mText.length())
			{
				mText += wc;
			}
			else
			{
				mText.insert(mText.begin() + (mCarrotIndex), wc);
			}

			mChatState = CHAT_STATE::CHATTING;
			++mCarrotIndex;
		}
	}

	bool numLock = ::GetKeyState(VK_NUMLOCK);
	if (numLock == true)
	{
		for (int i = KEY_TYPE::NUMPAD0_KEY; i <= KEY_TYPE::NUMPAD9_KEY; ++i)
		{
			if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
			{
				wchar_t wc = i + 16;
				if (mCarrotIndex == mText.length())
				{
					mText += wc;
				}
				else
				{
					mText.insert(mText.begin() + (mCarrotIndex), wc);
				}

				mChatState = CHAT_STATE::CHATTING;
				++mCarrotIndex;
			}
		}
	}

	bool capsLock = ::GetKeyState(VK_CAPITAL);
	int alpha = 'a' - KEY_TYPE::A_KEY;
	if (capsLock == true)
	{
		alpha = 'A' - KEY_TYPE::A_KEY;
	}
	for (int i = KEY_TYPE::A_KEY; i <= KEY_TYPE::Z_KEY; ++i)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
		{
			wchar_t wc = i + alpha;
			if (mCarrotIndex == mText.length())
			{
				mText += wc;
			}
			else
			{
				mText.insert(mText.begin() + (mCarrotIndex), wc);
			}

			mChatState = CHAT_STATE::CHATTING;
			++mCarrotIndex;
		}
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::SPACE) == true)
	{
		wchar_t wc = ' ';
		if (mCarrotIndex == mText.length())
		{
			mText += wc;
		}
		else
		{
			mText.insert(mText.begin() + (mCarrotIndex), wc);
		}

		mChatState = CHAT_STATE::CHATTING;
		++mCarrotIndex;
	}

	if (mText.length() > 0)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
		{
			if (mCarrotIndex > 0)
			{
				--mCarrotIndex;
			}
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
		{
			if (mCarrotIndex < mText.length())
			{
				++mCarrotIndex;
			}
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::BACK_SPACE) == true)
		{
			if (mCarrotIndex >= mText.length())
			{
				mText.pop_back();
				--mCarrotIndex;
			}
			else if (mCarrotIndex > 0)
			{
				mText.erase(mText.begin() + mCarrotIndex - 1);
				--mCarrotIndex;
			}
		}

		if (mChatState == CHAT_STATE::CHATTING)
		{
			// ä�� ��Ŷ �����ϰ�, ��� ä�û���
			if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
			{
				mChatState = CHAT_STATE::CHAT_END;
#ifdef SERVER_CONNECT
				GET_INSTANCE(Network)->SendChatPacket(mText);
				mText.clear();
				mCarrotIndex = 0;
#else
				// ä�� ���
				static_cast<ChattingBox*>(mParentUI)->AddChattingLog(0, mText.c_str());
				mText.clear();
				mCarrotIndex = 0;
#endif 
			}
		}
		setCarrotPos();
	}
	else
	{
		// ä�û��� ���̸鼭, ä�ñ��̰� 0�̰�, ���ʹ��������� ����
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
		{
			if (mChatState == CHAT_STATE::CHAT_END)
			{
				mChatState = CHAT_STATE::NONE;
			}
		}
	}
}
