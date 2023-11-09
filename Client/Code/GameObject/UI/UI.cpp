#include "UI.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Input/Input.h"

UI::UI()
	: GameObject()
{
	mParentUI = nullptr;
	mChildUIs.clear();
	mOriginX = 0;
	mOriginY = 0;
}

UI::~UI()
{
	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			delete child.second[i];
		}
	}

	mChildUIs.clear();
}

bool UI::Initialize(int x, int y)
{
	mOriginX = x;
	mOriginY = y;

	GameObject::Initialize(0, 0);

	return true;
}

void UI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
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
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void UI::SetPosition(int x, int y)
{
	// 최상위 부모UI 라면,
	if (mParentUI == nullptr)
	{
		mPos.first = mOriginX + x;
		mPos.second = mOriginY + y;
	}
	else
	{
		std::pair<int, int> pos = mParentUI->mPos;
		mPos.first = mOriginX + pos.first;
		mPos.second = mOriginY + pos.second;
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->SetPosition(x, y);
		}
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

void UI::AddChildUI(std::string key, UI* ui)
{
	if (mChildUIs.count(key) == false)
	{
		std::vector<UI*> v;
		v.emplace_back(ui);
		mChildUIs.emplace(key, v);
	}
	else
	{
		mChildUIs[key].emplace_back(ui);
	}
	ui->mParentUI = this;
}
