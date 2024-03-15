#include "ButtonUI.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Input/Input.h"
#include "../Inventory/Inventory.h"

ButtonUI::ButtonUI()
	: UI()
{
	mFunc = nullptr;
	mName.clear();
}

ButtonUI::~ButtonUI()
{
}

bool ButtonUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetRect(50, 50);

	return true;
}

void ButtonUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void ButtonUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	if (mTexture != nullptr)
	{
		GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	}
	else
	{
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "흰색");
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ButtonUI::MouseOver()
{
}

void ButtonUI::MouseLButtonDown()
{
}

void ButtonUI::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void ButtonUI::MouseLButtonClick()
{
	if (mFunc != nullptr)
	{
		mFunc(mName);
	}
}