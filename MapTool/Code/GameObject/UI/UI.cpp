#include "UI.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Input/Input.h"

UI::UI()
	: GameObject()
{
	mParentUI = nullptr;
	mChildUIs.clear();
	mRenderChildUIs.clear();
	mRenderIndex = -1;

	mOriginX = 0;
	mOriginY = 0;

	mMouseOver = false;
	mMouseLButtonDown = false;
	mMouseLButtonClick = false;

	mDragStartPos = std::make_pair(0, 0);
}

UI::~UI()
{
	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			delete child.second[i];
			child.second[i] = nullptr;
		}
		child.second.clear();
	}

	mChildUIs.clear();
	mRenderChildUIs.clear();
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
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

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

void UI::MouseOver()
{
}

void UI::MouseLButtonDown()
{
	mDragStartPos = GET_INSTANCE(Input)->GetMousePos();
}

void UI::MouseLButtonUp()
{
}

void UI::MouseLButtonClick()
{
	mMouseLButtonClick = true;
	std::cout << "UI LButton 클릭" << std::endl;
}

void UI::Move(int mouseX, int mouseY)
{
	int x = mouseX - mDragStartPos.first;
	int y = mouseY - mDragStartPos.second;

	mPos.first += x;
	mPos.second += y;

	mDragStartPos = GET_INSTANCE(Input)->GetMousePos();

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->SetPosition(x, y);
		}
	}
}

void UI::Click(int mouseX, int mouseY)
{
	mPos.first = mouseX;
	mPos.second = mouseY;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->SetPosition(mouseX, mouseY);
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

void UI::MouseOverCollision(int x, int y)
{
	if (IsVisible() == false)
	{
		return;
	}

	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + mRect.first;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + mRect.second;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
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

		mRenderChildUIs.emplace_back(v);
		mRenderIndex = mRenderChildUIs.size() - 1;
	}
	else
	{
		mChildUIs[key].emplace_back(ui);		
		mRenderChildUIs[mRenderIndex].emplace_back(ui);
	}
	ui->mParentUI = this;
}

std::unordered_map<std::string, std::vector<UI*>>& UI::GetChildUIs()
{
	return mChildUIs; 
}

bool UI::GetMouseOver()	const
{
	return mMouseOver;
}

void UI::SetMouseOver(bool mouseOver)
{
	mMouseOver = mouseOver; 
}

bool UI::GetMouseLButtonDown() const
{
	return mMouseLButtonDown; 
}

void UI::SetMouseLButtonDown(bool lButtonDown)
{
	mMouseLButtonDown = lButtonDown;
}
