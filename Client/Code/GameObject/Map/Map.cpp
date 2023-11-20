#include "Map.h"
#include <iostream>
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"

Map::Map()
	: GameObject()
{
	//for (int i = 0; i < MAX_MARK; ++i)
	//{
	//	mMarks[i].message_time = 0;
	//}
}

Map::~Map()
{
}

bool Map::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	
	//mTileTexture = std::make_shared<Texture>();
	//mTileTexture->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);

	//mBlackTile = std::make_shared<Texture>();
	//mWhiteTile = std::make_shared<Texture>();

	//mReactor->CreateTexture(0, 0, 531, 532);
	//mBlackTile->CreateTexture(0, 0, 531, 532);
	//mWhiteTile->CreateTexture(0, 0, 531, 532);

	//mReactor->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	//mBlackTile->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	//mWhiteTile->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS);

	//mBlackTile->SetPosition(69, 5);
	//mBlackTile->SetSize(TILE_SIZE, TILE_SIZE);

	//mWhiteTile->SetPosition(5, 5);
	//mWhiteTile->SetSize(TILE_SIZE, TILE_SIZE);

	// 간격 표시자 생성
	//int markWidth = TILE_SIZE;
	//int markHeight = TILE_SIZE;

	//int textureId = 1;
	//GET_INSTANCE(GraphicEngine)->LoadTexture(L"../Resource/Textures/Flag.png", textureId, markWidth, markWidth);

	//for (int i = 0; i < MAX_MARK; ++i)
	//{
	//	if (!Create_BOB32(&mMarks[i], 0, 0, markWidth, markHeight, 1, BOB_ATTR_SINGLE_FRAME))
	//	{
	//		return false;
	//	}

	//	Load_Frame_BOB32(&mMarks[i], textureId, 0, 0, 0, BITMAP_EXTRACT_MODE_CELL);
	//	Set_Pos_BOB32(&mMarks[i], 0, 0);
	//}

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

	//D3DXVECTOR3 pos = D3DXVECTOR3(static_cast<float>(mPos.first), static_cast<float>(mPos.second), 0.0);
	//D3DXVECTOR3 pos = D3DXVECTOR3(mPos.first * mTexture->GetSize().first + 7, mPos.second * mTexture->GetSize().second + 7, 0.0);
	//GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	//D3DXVECTOR3 pos = D3DXVECTOR3((mPos.first - cameraPos.first) * 65.0f + 10, (mPos.second - cameraPos.second) * 65.0f + 10, 0.0);
	//D3DXVECTOR3 pos = D3DXVECTOR3
	//(
	//	(mPos.first - cameraPos.first) * mTexture->GetSize().first + 8.0,
	//	(mPos.second - cameraPos.second) * mTexture->GetSize().second + 8.0,
	//	0.0
	//);

	//GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
	//mTexture->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);
	//mTexture->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));	
	//mTexture->GetSprite()->End();

	
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
