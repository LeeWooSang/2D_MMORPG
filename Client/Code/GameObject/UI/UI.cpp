#include "UI.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Input/Input.h"

UI::UI()
	: GameObject()
{
	mParentUI = nullptr;
	mChildUIs.clear();
}

UI::~UI()
{
	for (auto& child : mChildUIs)
	{
		delete child;
	}

	mChildUIs.clear();
}

bool UI::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);

	return true;
}

void UI::Update()
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
			std::cout << "UI 마우스 좌 클릭" << std::endl;
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::MOUSE_RBUTTON) == true)
		{
			std::cout << "UI 마우스 우 클릭" << std::endl;
		}
	}

	for (auto& child : mChildUIs)
	{
		child->Update();
	}
}

void UI::Render()
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
	GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	for (auto& child : mChildUIs)
	{
		child->Render();
	}
}

void UI::SetPosition(int x, int y)
{
	// 최상위 부모UI 라면,
	if (mParentUI == nullptr)
	{
		mPos = std::make_pair(x, y);
	}

	else
	{
		std::pair<int, int> pos = mParentUI->mPos;
		mPos.first += pos.first;
		mPos.second += pos.second;
	}

	for (auto& child : mChildUIs)
	{
		child->SetPosition(x, y);
	}
}

bool UI::Collision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + mTexture->GetSize().first;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + mTexture->GetSize().second;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		return true;
	}

	return false;
}

void UI::AddChildUI(UI* ui)
{
	mChildUIs.emplace_back(ui);
	ui->mParentUI = this;
}
