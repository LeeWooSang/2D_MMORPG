#include "ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Input/Input.h"
#include "../UIManager.h"
#include "../Inventory/Inventory.h"

ButtonUI::ButtonUI()
	: UI()
{
}

ButtonUI::~ButtonUI()
{
}

bool ButtonUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

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
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + 30;
	pos.bottom = pos.top + 30;

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

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
	static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"))->OpenInventory();
}