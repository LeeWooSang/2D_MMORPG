#include "ChattingInputUI.h"
#include "../../../../Input/Input.h"
#include "../../../../Network/Network.h"
#include "../../../../GraphicEngine/GraphicEngine.h"
#include "../../ChattingBox/ChattingBox.h"
#include "../../ButtonUI/ChattingMenuUI/ChattingMenuUI.h"

#include "../../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../../Scene/InGameScene/InGameScene.h"
#include "../../TradeUI/TradeUI.h"
#include "../../../Character/Character.h"

ChattingInputUI::ChattingInputUI()
	: InputUI()
{
	mOpen = false;
	mChatState = CHAT_STATE::NONE;
	mWhispering = false;
	mTrading = false;
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
	// 닫혀있다면
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

		// 귓속말 명령어인지 확인
		if (mText == L"/r ")
		{
			mWhispering = true;
		}
		else if (mText == L"/t ")
		{
			mTrading = true;
		}
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::SLASH_KEY) == true)
	{
		wchar_t wc = '/';
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
			// 채팅 패킷 전송하고, 계속 채팅상태
			if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
			{
				// 귓속말 명령어가 입력됬다면,
				if (mWhispering == true)
				{
					std::wstring temp = L"";
					for (int i = 3; i < mText.length(); ++i)
					{
						temp += mText[i];
					}

					int id = _wtoi(temp.c_str());
					ChattingMenuUI::SetWhispering(id);
					mText.clear();
					mCarrotIndex = 0;
					mWhispering = false;
					mTrading = false;
				}

				else if (mTrading == true)
				{
					std::wstring temp = L"";
					for (int i = 3; i < mText.length(); ++i)
					{
						temp += mText[i];
					}

					mText.clear();
					mCarrotIndex = 0;
					mWhispering = false;
					mTrading = false;

					int id = _wtoi(temp.c_str());
					InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE));
#ifdef SERVER_CONNECT
					// 나자신에게 교환을 신청한게 아니라면
					if (scene->GetPlayer()->GetId() != id)
					{
						// 교환창 오픈
						TradeUI* ui = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
						ui->OpenTradeUI();
						ui->SetTradeUserId(id);
						// 상대방한테도 교환창 오픈
						GET_INSTANCE(Network)->SendRequestTradePacket(id);
					}
#else
					// 교환창 오픈
					TradeUI* ui = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
					ui->OpenTradeUI();
					ui->SetTradeUserId(id);
#endif
				}

				else
				{
					mChatState = CHAT_STATE::CHAT_END;
#ifdef SERVER_CONNECT
					if (ChattingMenuUI::GetChattingType() == static_cast<int>(CHATTING_TYPE::WHISPERING))
					{
						GET_INSTANCE(Network)->SendWhisperingChatPacket(ChattingMenuUI::GetWhisperingId(), mText);
					}
					else
					{
						GET_INSTANCE(Network)->SendBroadCastingChatPacket(mText);
					}
					mText.clear();
					mCarrotIndex = 0;
#else
					// 채팅 기록
					static_cast<ChattingBox*>(mParentUI)->AddChattingLog(0, mText.c_str());
					mText.clear();
					mCarrotIndex = 0;
#endif 
				}
			}
		}
		SetCarrotPos();
	}
	else
	{
		// 채팅상태 중이면서, 채팅길이가 0이고, 엔터눌렀을때는 종료
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
		{
			if (mChatState == CHAT_STATE::CHAT_END)
			{
				mChatState = CHAT_STATE::NONE;
			}
		}
	}
}
