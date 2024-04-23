#include "InputUI.h"
#include "../../../GameTimer/GameTimer.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Input/Input.h"

InputUI::InputUI()
	: UI()
{
	mText.clear();
	mCarrotIndex = 0;
	mCarrotPos = std::make_pair(0.0, 0.0);
	mTextSize = std::make_pair(0.0, 0.0);
	mElapsedTime = 0.0;
}

InputUI::~InputUI()
{
}

bool InputUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetRect(300, 30);
	//SetPosition(x, 200);
	mCarrotPos = std::make_pair(mPos.first, mPos.second);

	Visible();

	return true;
}

void InputUI::Update()
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

	processInput();
}

void InputUI::Render()
{
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	//pos.right = pos.left + 400;
	//pos.bottom = pos.top + 30;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "����������");

	if (mElapsedTime > 0.5)
	{
		D2D1_RECT_F carrotRect;
		carrotRect.left = mCarrotPos.first;
		carrotRect.top = mCarrotPos.second;
		carrotRect.right = carrotRect.left + 2;
		carrotRect.bottom = carrotRect.top + 25;

		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(carrotRect, "���");
	}
	
	if (mText.length() > 0)
	{
		D2D1_RECT_F rect;
		rect.left = mPos.first;
		rect.top = mPos.second;
		rect.right = rect.left + mTextSize.first;
		rect.bottom = rect.top + mTextSize.second;

		GET_INSTANCE(GraphicEngine)->RenderRectangle(rect, "������");

		if (checkTextCollision() == true)
		{
			GET_INSTANCE(GraphicEngine)->RenderText(mText, mPos.first, mCarrotPos.second, "������", "��Ȳ��");
		}
		else
		{
			GET_INSTANCE(GraphicEngine)->RenderText(mText, mPos.first, mCarrotPos.second, "������", "���");
		}
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void InputUI::MouseOver()
{
}

void InputUI::MouseLButtonDown()
{
}

void InputUI::MouseLButtonUp()
{
}

void InputUI::MouseLButtonClick()
{
}

void InputUI::SetText(const std::wstring& text)
{
	mText = text;
	mCarrotIndex = mText.length();
	SetCarrotPos();
}

void InputUI::processInput()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::DOT_KEY) == true)
	{
		wchar_t wc = '.';
		if (mCarrotIndex == mText.length())
		{
			mText += wc;
		}
		else
		{
			mText.insert(mText.begin() + (mCarrotIndex), wc);
		}
		++mCarrotIndex;
	}

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
				if (mCarrotIndex == mText.size())
				{
					mText += wc;
				}
				else
				{
					mText.insert(mText.begin() + (mCarrotIndex), wc);
				}

				++mCarrotIndex;
			}
		}
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

		SetCarrotPos();
	}
}

bool InputUI::checkTextCollision()
{
	D2D1_RECT_F rect;
	rect.left = mPos.first;
	rect.top = mPos.second;
	rect.right = rect.left + mTextSize.first;
	rect.bottom = rect.top + mTextSize.second;

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (mousePos.first >= rect.left && mousePos.first <= rect.right && mousePos.second >= rect.top && mousePos.second <= rect.bottom)
	{
		return true;
	}

	return false;
}

void InputUI::SetCarrotPos()
{
	std::wstring text = L"";
	for (int i = 0; i < mCarrotIndex; ++i)
	{
		text += mText[i];
	}

	IDWriteTextLayout* layout = nullptr;

	HRESULT result = GET_INSTANCE(GraphicEngine)->GetWriteFactory()->CreateTextLayout(
		text.c_str(),
		static_cast<UINT32>(text.length()),
		GET_INSTANCE(GraphicEngine)->GetFont("������").font,
		50.0, 50.0f,
		&layout);

	mCarrotPos.first = mPos.first;
	mCarrotPos.second = mPos.second;

	mTextSize = std::make_pair(0.0, 0.0);
	for (int i = 0; i < text.length(); ++i)
	{
		float x = 0.0;
		float y = 0.0;
		DWRITE_HIT_TEST_METRICS metrics = { 0 };
		result =layout->HitTestTextPosition(i, false, &x, &y, &metrics);
		mCarrotPos.first += metrics.width;
		//mCarrotPos.second = mPos.second + metrics.height;
		mTextSize.first += metrics.width;
		mTextSize.second = metrics.height;
	}

	layout->Release();
}
