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
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (Collision(mousePos.first, mousePos.second) == true)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::MOUSE_LBUTTON) == true)
		{
			std::cout << "스크롤 배경 클릭" << std::endl;
		}
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
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
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (Collision(mousePos.first, mousePos.second) == true)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::MOUSE_LBUTTON) == true)
		{
			std::cout << "스크롤 바 클릭" << std::endl;
		}
	}
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
	pos.right = pos.left + mTexture->GetSize().first - 5;
	pos.bottom = pos.top + mTexture->GetSize().second - (MAX_INVENTORY_WIDTH_SLOT_SIZE - VIEW_SLOT_HEIGHT) * mAlphaValue;

	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}
