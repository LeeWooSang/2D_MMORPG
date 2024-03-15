#include "Scroll.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"
#include "../Inventory/Inventory.h"

Scroll::Scroll()
	: UI()
{
	mAlphaValue = 0;
}

Scroll::~Scroll()
{
}

bool Scroll::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	ScrollBar* scrollBar = new ScrollBar;
	
	scrollBar->Initialize(3, 0);

	AddChildUI("ScrollBar", scrollBar);

	return true;
}

void Scroll::Update()
{
	UI::Update();
}

void Scroll::Render()
{
	UI::Render();
}

void Scroll::SetAlphaValue(int value)
{
	mAlphaValue = value;

	for (auto& child : mChildUIs["ScrollBar"])
	{
		ScrollBar* scrollBar = static_cast<ScrollBar*>(child);
		scrollBar->SetAlphaValue(value);
	}
}

ScrollBar::ScrollBar()
	: UI()
{
	mAlphaValue = 0;
}

ScrollBar::~ScrollBar()
{
}

bool ScrollBar::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetTexture("ScrollBar");
	Visible();

	return true;
}

void ScrollBar::Update()
{
	UI::Update();
}

void ScrollBar::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second - (MAX_INVENTORY_WIDTH_SLOT_SIZE - VIEW_SLOT_HEIGHT) * mAlphaValue;
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ScrollBar::MouseOver()
{
}

void ScrollBar::MouseLButtonDown()
{
}

void ScrollBar::MouseLButtonUp()
{
}

void ScrollBar::MouseLButtonClick()
{
}

void ScrollBar::MouseOverCollision(int x, int y)
{
	if (IsVisible() == false)
	{
		return;
	}

	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = collisionBox.left + mTexture->GetSize().first;
	collisionBox.top = mPos.second;
	collisionBox.bottom = collisionBox.top + mTexture->GetSize().second - (MAX_INVENTORY_WIDTH_SLOT_SIZE - VIEW_SLOT_HEIGHT) * mAlphaValue;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}

	//for (auto& child : mChildUIs)
	//{
	//	for (int i = 0; i < child.second.size(); ++i)
	//	{
	//		child.second[i]->MouseOverCollision(x, y);
	//	}
	//}
}
