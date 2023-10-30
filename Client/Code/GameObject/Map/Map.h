#pragma once
#include "../GameObject.h"
#include <memory>

// 간격 표시자
constexpr int MAX_MARK = 12;
// 타일 크기
constexpr int TILE_SIZE = 65;
// 타일 개수
constexpr int TILE_COUNT = 10;

class Texture;
class Map : public GameObject
{
public:
	Map();
	virtual ~Map();

	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

private:
	// 간격 표시자
	//BOB mMarks[MAX_MARK];
	// 맵
	//std::shared_ptr<Texture> mBlackTile;
	//std::shared_ptr<Texture>mWhiteTile;
};
