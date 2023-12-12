#include "Map.h"
#include <iostream>
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"

Map::Map()
	: GameObject()
{
}

Map::~Map()
{
}

bool Map::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	
	return true;
}

void Map::Update()
{
}

void Map::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	RECT src;
	src.left = mTexture->GetPos().first;
	src.top = mTexture->GetPos().second;
	src.right = mTexture->GetPos().first + mTexture->GetSize().first;
	src.bottom = mTexture->GetPos().second + mTexture->GetSize().second;

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	
	// 이미지 크기
	D2D1_RECT_F rect;
	rect.left = mTexture->GetPos().first;
	rect.top = mTexture->GetPos().second;
	rect.right = mTexture->GetPos().first + mTexture->GetSize().first;
	rect.bottom = mTexture->GetPos().second + mTexture->GetSize().second;

	// 이미지 위치
	D2D1_RECT_F pos;
	pos.left = (mPos.first - cameraPos.first) * mTexture->GetSize().first + 8.0;
	pos.top = (mPos.second - cameraPos.second) * mTexture->GetSize().second + 8.0;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;

	//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos, rect);
}
