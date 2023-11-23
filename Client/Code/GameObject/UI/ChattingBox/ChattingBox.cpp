#include "ChattingBox.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../GameTimer/GameTimer.h"

ChattingBox::ChattingBox()
	: UI()
{
	mOpen = false;
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
	mCarrotPos = mPos;
	{
		float x;
		float y;
		DWRITE_HIT_TEST_METRICS metrics;
		GET_INSTANCE(GraphicEngine)->GetFont("메이플").textLayout->HitTestTextPosition(0, false, &x, &y, &metrics);
		std::cout << metrics.width << ", " << metrics.height << std::endl;
	}

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

	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "흰색");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	std::wstring chatting = L"";
	for(auto& chat : mChattings)
	{
		chatting += chat;
	}
	GET_INSTANCE(GraphicEngine)->RenderText(chatting, mPos.first, mPos.second, "메이플", "흰색");
	
	if (mElapsedTime > 0.5)
	{
		D2D1_RECT_F carrotRect;
		carrotRect.left = mCarrotPos.first;
		carrotRect.top = mCarrotPos.second;
		carrotRect.right = carrotRect.left + 2;
		carrotRect.bottom = carrotRect.top + 25;

		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(carrotRect);
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

void ChattingBox::OpenChattingBox()
{
	// 닫혀있다면
	if (mOpen == false)
	{
		mOpen = true;
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
	for (int i = KEY_TYPE::A_KEY; i <= KEY_TYPE::Z_KEY; ++i)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
		{
			wchar_t wc = i + 76;
			if (mCarrotIndex == mChattings.size())
			{
				mChattings.emplace_back(wc);
			}
			else
			{
				mChattings.insert(mChattings.begin() + (mCarrotIndex), wc);
			}

			std::pair<float, float> size = GetTextSize(wc);
			mCarrotPos.first += size.first;
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

		std::pair<float, float> size = GetTextSize(wc);
		mCarrotPos.first += size.first;
		++mCarrotIndex;
	}

	if (mChattings.size() > 0)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
		{
			if (mCarrotIndex > 0)
			{				
				std::pair<float, float> size = GetTextSize(mChattings[mCarrotIndex - 1]);
				mCarrotPos.first -= size.first;

				--mCarrotIndex;
			}
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
		{
			if (mCarrotIndex < mChattings.size())
			{
				++mCarrotIndex;

				std::pair<float, float> size = GetTextSize(mChattings[mCarrotIndex - 1]);
				mCarrotPos.first += size.first;
			}
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::BACK_SPACE) == true)
		{
			if (mCarrotIndex >= mChattings.size())
			{
				std::pair<float, float> size = GetTextSize(mChattings[mCarrotIndex - 1]);
				mCarrotPos.first -= size.first;

				mChattings.pop_back();
				--mCarrotIndex;
			}
			else if(mCarrotIndex > 0)
			{
				std::pair<float, float> size = GetTextSize(mChattings[mCarrotIndex - 1]);
				mCarrotPos.first -= size.first;

				mChattings.erase(mChattings.begin() + mCarrotIndex - 1);
				--mCarrotIndex;
			}
		}
	}
}

std::pair<float, float> ChattingBox::GetTextSize(wchar_t c)
{
	std::pair<float, float> size;

	std::wstring text = L"";
	text += c;
	IDWriteTextLayout* layout;

	HRESULT result = GET_INSTANCE(GraphicEngine)->GetWriteFactory()->CreateTextLayout(
		text.c_str(), 
		static_cast<UINT32>(text.length()), 
		GET_INSTANCE(GraphicEngine)->GetFont("메이플").font, 
		50.0, 50.0f, 
		&layout);

	float x;
	float y;
	DWRITE_HIT_TEST_METRICS metrics;
	layout->HitTestTextPosition(0, false, &x, &y, &metrics);
	size = std::make_pair(metrics.width, metrics.height);

	layout->Release();

	return size;
}
