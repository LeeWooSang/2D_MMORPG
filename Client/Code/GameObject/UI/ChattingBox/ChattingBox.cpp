#include "ChattingBox.h"
#include "../../../Network/Network.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../GameTimer/GameTimer.h"
#include "../../../Resource/ResourceManager.h"

ChattingBox::ChattingBox()
	: UI()
{
	mOpen = false;
	mChatState = CHAT_STATE::NONE;

	mChattingLog.clear();
	mChattingLog.reserve(100);

	mChattings.clear();

	mCarrotIndex = 0;
	mCarrotPos = std::make_pair(0.0, 0.0);
	mElapsedTime = 0.0;
}

ChattingBox::~ChattingBox()
{
}

bool ChattingBox::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetPosition(0, 500);

	mCarrotPos = std::make_pair(mPos.first, mPos.second + 200);

	return true;
}

void ChattingBox::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	mElapsedTime += GET_INSTANCE(GameTimer)->GetElapsedTime();
	if (mElapsedTime > 1.0)
	{
		mElapsedTime = 0.0;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);
	//SetCarrotPos();

	if (mOpen == true)
	{
		processInput();
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void ChattingBox::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + 400;
	pos.bottom = pos.top + 200;

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "흰색");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}
	
	for (int i = 0; i < mChattingLog.size(); ++i)
	{
		int x = mPos.first;
		//int y = mPos.second + 200 - (GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize * (mChattingLog.size() - i));
		int y = mPos.second + 200 - ((GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize + 1) * (mChattingLog.size() - i));

		int left = x;
		int top = y;
		int right = left + 400;
		int bottom = top + (GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize) + 1;
		if (CheckContain(left, top, right, bottom) == false)
		{
			continue;
		}

		GET_INSTANCE(GraphicEngine)->RenderText(mChattingLog[i].chatting, x, y, "메이플", "검은색");
	}

	if (mElapsedTime > 0.5)
	{
		D2D1_RECT_F carrotRect;
		carrotRect.left = mCarrotPos.first;
		carrotRect.top = mCarrotPos.second;
		carrotRect.right = carrotRect.left + 2;
		carrotRect.bottom = carrotRect.top + 25;

		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(carrotRect);
	}

	std::wstring chatting = L"";
	for (auto& chat : mChattings)
	{
		chatting += chat;
	}
	if (chatting.size() > 0)
	{
		GET_INSTANCE(GraphicEngine)->RenderText(chatting, mPos.first, mCarrotPos.second, "메이플", "갈색");
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ChattingBox::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + 400;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + 200;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void ChattingBox::MouseOver()
{
}

void ChattingBox::MouseLButtonDown()
{
}

void ChattingBox::MouseLButtonUp()
{
}

void ChattingBox::MouseLButtonClick()
{
	std::cout << "채팅창 클릭" << std::endl;
}

bool ChattingBox::CheckContain(int left, int top, int right, int bottom)
{
	D2D1_RECT_F rect;
	rect.left = mPos.first;
	rect.top = mPos.second;
	rect.right = rect.left + 400;
	rect.bottom = rect.top + 200;

	if (left < rect.left || right > rect.right || top < rect.top || bottom > rect.bottom)
	{
		return false;
	}

	return true;
}

void ChattingBox::OpenChattingBox()
{
	if (mChatState != CHAT_STATE::NONE)
	{
		return;
	}

	// 닫혀있다면
	if (mOpen == false)
	{
		mOpen = true;
		mChatState = CHAT_STATE::CHATTING;
		Visible();
	}
	// 열려있다면
	else
	{
		mOpen = false;
		NotVisible();
	}
}

void ChattingBox::processInput()
{
	for (int i = KEY_TYPE::NUM0_KEY; i <= KEY_TYPE::NUM9_KEY; ++i)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
		{
			wchar_t wc = i + 26;
			if (mCarrotIndex == mChattings.size())
			{
				mChattings.emplace_back(wc);
			}
			else
			{
				mChattings.insert(mChattings.begin() + (mCarrotIndex), wc);
			}

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
				if (mCarrotIndex == mChattings.size())
				{
					mChattings.emplace_back(wc);
				}
				else
				{
					mChattings.insert(mChattings.begin() + (mCarrotIndex), wc);
				}

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
			if (mCarrotIndex == mChattings.size())
			{
				mChattings.emplace_back(wc);
			}
			else
			{
				mChattings.insert(mChattings.begin() + (mCarrotIndex), wc);
			}
		
			++mCarrotIndex;
		}
	}
	
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::SPACE) == true)
	{
		wchar_t wc = ' ';
		if (mCarrotIndex == mChattings.size())
		{
			mChattings.emplace_back(wc);
		}
		else
		{
			mChattings.insert(mChattings.begin() + (mCarrotIndex), wc);
		}

		++mCarrotIndex;
	}

	if (mChattings.size() > 0)
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
			if (mCarrotIndex < mChattings.size())
			{
				++mCarrotIndex;
			}
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::BACK_SPACE) == true)
		{
			if (mCarrotIndex >= mChattings.size())
			{
				mChattings.pop_back();
				--mCarrotIndex;
			}
			else if(mCarrotIndex > 0)
			{
				mChattings.erase(mChattings.begin() + mCarrotIndex - 1);
				--mCarrotIndex;
			}
		}

		if (mChatState == CHAT_STATE::CHATTING)
		{
			// 채팅 패킷 전송하고, 계속 채팅상태
			if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
			{
				std::wstring chatting;
				for (auto& chat : mChattings)
				{
					chatting += chat;
				}

				// 채팅 기록
				mChattingLog.emplace_back(ChattingLog(chatting, 0, 0));

				mChattings.clear();
				mCarrotIndex = 0;
#ifdef SERVER_CONNECT
				GET_INSTANCE(Network)->SendChatPacket(chatting);
#endif 
			}
		}

		SetCarrotPos();
	}
	else
	{
		// 채팅상태 중이면서, 채팅길이가 0이고, 엔터눌렀을때는 종료
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
		{
			mChatState = CHAT_STATE::NONE;
		}
	}
}

void ChattingBox::SetCarrotPos()
{
	std::wstring text = L"";
	for (int i = 0; i < mCarrotIndex; ++i)
	{
		text += mChattings[i];
	}

	IDWriteTextLayout* layout;

	HRESULT result = GET_INSTANCE(GraphicEngine)->GetWriteFactory()->CreateTextLayout(
		text.c_str(),
		static_cast<UINT32>(text.length()),
		GET_INSTANCE(GraphicEngine)->GetFont("메이플").font,
		50.0, 50.0f,
		&layout);

	mCarrotPos.first = 0.0;
	for (int i = 0; i < text.length(); ++i)
	{
		float x;
		float y;
		DWRITE_HIT_TEST_METRICS metrics;
		layout->HitTestTextPosition(i, false, &x, &y, &metrics);
		mCarrotPos.first += metrics.width;

		//mCarrotPos.second = mPos.second + metrics.height;
	}

	layout->Release();
}