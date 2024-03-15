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
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	
	// �̹��� ũ��
	D2D1_RECT_F rect;
	rect.left = mTexture->GetPos().first;
	rect.top = mTexture->GetPos().second;
	rect.right = mTexture->GetPos().first + mRect.first;
	rect.bottom = mTexture->GetPos().second + mRect.second;

	// �̹��� ��ġ
	D2D1_RECT_F pos;
	pos.left = (mPos.first - cameraPos.first) * mRect.first + 8.0;
	pos.top = (mPos.second - cameraPos.second) * mRect.second + 8.0;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos, rect);

	if (mMouseClick == true)
	{
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver == true)
	{
		GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "������");
	}
	else
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "������");
	}
}

bool Map::CheckMouseOver(int x, int y)
{
	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();

	// ui�� rect�� ��Ҵ°�?
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
	// Ű�� ���� ����
	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::TAP)
	{
		mMouseDown = true;
		mMouseUp = false;
	}
	// Ű�� �� ����
	else if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::AWAY)
	{
		mMouseUp = true;
		// ������ �ȿ��� Ű�� ������ ���� ���� Ŭ��
		if (mMouseDown == true)
		{
			mMouseClick = true;
		}
		mMouseDown = false;
	}

	return false;
}
