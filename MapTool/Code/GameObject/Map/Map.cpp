#include "Map.h"
#include <iostream>
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"

Map::Map()
	: GameObject()
{
	mMouseOver = false;
}

Map::~Map()
{
}

bool Map::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);

	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("WhiteTile");
	SetTexture(data.name);

	mRect.first = data.size.first;
	mRect.second = data.size.second;

	return true;
}

void Map::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	
	if (CheckMouseOver(mousePos.first, mousePos.second) == true)
	{
		CheckMouseClick(mousePos.first, mousePos.second);
	}
}

void Map::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	
	// 이미지 크기
	D2D1_RECT_F rect;
	rect.left = mTexture->GetPos().first;
	rect.top = mTexture->GetPos().second;
	rect.right = mTexture->GetPos().first + mRect.first;
	rect.bottom = mTexture->GetPos().second + mRect.second;

	// 이미지 위치
	D2D1_RECT_F pos;
	pos.left = (mPos.first - cameraPos.first) * mRect.first + 8.0;
	pos.top = (mPos.second - cameraPos.second) * mRect.second + 8.0;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos, rect);

	if (mMouseClick == true)
	{
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "파란색");
	}
	else if (mMouseOver == true)
	{
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "빨간색");
	}
	else
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "검은색");
	}
}

bool Map::CheckMouseOver(int x, int y)
{
	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();

	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = (mPos.first - cameraPos.first) * mRect.first + 8.0;
	collisionBox.top = (mPos.second - cameraPos.second) * mRect.second + 8.0;
	collisionBox.right = collisionBox.left + mRect.first;
	collisionBox.bottom = collisionBox.top + mRect.second;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
		return true;
	}

	mMouseOver = false;
	return false;
}

bool Map::CheckMouseClick(int x, int y)
{
	// 키를 누른 순간
	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::TAP)
	{
		mMouseDown = true;
		mMouseUp = false;
	}
	// 키를 뗀 순간
	else if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::AWAY)
	{
		mMouseUp = true;
		// 유아이 안에서 키를 누르고 뗐을 때만 클릭
		if (mMouseDown == true)
		{
			mMouseClick = true;
		}
		mMouseDown = false;
	}

	return false;
}
