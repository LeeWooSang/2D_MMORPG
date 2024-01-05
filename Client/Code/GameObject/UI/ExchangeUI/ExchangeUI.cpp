#include "ExchangeUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"

ExchangeUI::ExchangeUI()
	: UI()
{
}

ExchangeUI::~ExchangeUI()
{
}

bool ExchangeUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);



	return true;
}

void ExchangeUI::Update()
{
	UI::Update();
}

void ExchangeUI::Render()
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
	pos.bottom = pos.top + 300;

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "흰색");
	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	//if (mMouseLButtonDown)
	//{
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	//}
	//else if (mMouseOver)
	//{
	//	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	//}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ExchangeUI::MouseOverCollision(int x, int y)
{
}

void ExchangeUI::MouseOver()
{
}

void ExchangeUI::MouseLButtonDown()
{
}

void ExchangeUI::MouseLButtonUp()
{
}

void ExchangeUI::MouseLButtonClick()
{
}
