#pragma once
#include "../GameObject.h"
#include <memory>

// ���� ǥ����
constexpr int MAX_MARK = 12;
// Ÿ�� ũ��
constexpr int TILE_SIZE = 65;
// Ÿ�� ����
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
	// ���� ǥ����
	//BOB mMarks[MAX_MARK];
	// ��
	//std::shared_ptr<Texture> mBlackTile;
	//std::shared_ptr<Texture>mWhiteTile;
};
