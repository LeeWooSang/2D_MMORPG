#include "Map.h"
#include <iostream>
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

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
	//for (int i = 0; i < MAX_MARK; ++i)
	//{
	//	Destroy_BOB32(&mMarks[i]);
	//}
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

	// ���� ǥ���� ����
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
	// ���̴� �͸� ����
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
	D3DXVECTOR3 pos = D3DXVECTOR3(mPos.first * mTexture->GetSize().first + 7, mPos.second * mTexture->GetSize().second + 7, 0.0);
	GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	// ���� ǥ���� �׸���	 
	//for (int i = 0; i < MAX_MARK; ++i)
	//{
	//	// ���� ǥ���� ��ġ
	//	mMarks[i].x = 8 * i;
	//	mMarks[i].y = 8 * i;
	//	mMarks[i].attr |= BOB_ATTR_VISIBLE;
	//	Draw_BOB32(&mMarks[i]);
	//}

}