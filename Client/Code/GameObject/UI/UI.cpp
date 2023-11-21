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

	mMouseOver = false;
	mMouseLButtonDown = false;
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

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void UI::MouseOver()
{
	//std::cout << "UI 안에서 MouseOver" << std::endl;
	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;

	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
}

void UI::MouseLButtonDown()
{
	std::cout << "UI 안에서 LButton Down" << std::endl;
}

void UI::MouseLButtonUp()
{
	std::cout << "UI  안에서 LButton Up" << std::endl;
}

void UI::MouseLButtonClick()
{
	std::cout << "UI LButton 클릭" << std::endl;
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

bool UI::CheckContain(int left, int top, int right, int bottom)
{
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
