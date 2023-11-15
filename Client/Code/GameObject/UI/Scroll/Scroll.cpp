#include "Scroll.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"

Scroll::Scroll()
	: UI()
{
}

Scroll::~Scroll()
{
}

bool Scroll::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	ScrollBar* scrollBar = new ScrollBar;
	scrollBar->Initialize(1210, 200);

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

ScrollBar::ScrollBar()
	: UI()
{
}

ScrollBar::~ScrollBar()
{
}

bool ScrollBar::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetTexture("ScrollBar");
	Visible();

	//mTexture->InitializeSprite();
	D3DXMATRIX matrix;
	mTexture->GetSprite()->GetTransform(&matrix);
	D3DXMatrixScaling(&matrix, 0.2, 0.2, 0.0);
	mTexture->GetSprite()->SetTransform(&matrix);	

	//UI::Initialize(x, y);

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

	RECT src;
	src.left = mTexture->GetPos(mCurrFrame).first;
	src.top = mTexture->GetPos(mCurrFrame).second;
	src.right = mTexture->GetPos(mCurrFrame).first + mTexture->GetSize().first;
	src.bottom = mTexture->GetPos(mCurrFrame).second + mTexture->GetSize().second;

	D3DXVECTOR3 pos = D3DXVECTOR3(mPos.first, mPos.second, 0.0);
	//GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	mTexture->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
	mTexture->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
	mTexture->GetSprite()->End();

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}
