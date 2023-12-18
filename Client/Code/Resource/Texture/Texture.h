#pragma once
#include <iostream>
#include <vector>

#define BITMAP_ID            0x4D42 // universal id for a bitmap

#define BITMAP_EXTRACT_MODE_CELL  0
#define BITMAP_EXTRACT_MODE_ABS   1

struct ID2D1Bitmap;
class Texture
{	
public:
	Texture();
	virtual ~Texture();

	void CreateTexture(int width, int height);
	void CreateTexture(int width, int height, std::vector<std::pair<int, int>>& texPositions);
	bool LoadTexture(const wchar_t* path);

	std::pair<int, int> GetPos(int currFrame = 0)		const { return mTexPositions[currFrame]; }
	std::pair<int, int> GetSize() const { return std::make_pair(mWidth, mHeight); };
	void SetSize(int width, int height) { mWidth = width; mHeight = height; }

	std::pair<int, int> GetOrigin()	const { return mOrigin; }
	void SetOrigin(int x, int y) { mOrigin = std::make_pair(x, y); }

	ID2D1Bitmap* GetImage() { return mImage; }

protected:
	std::vector<std::pair<int, int>> mTexPositions;
	int mWidth;
	int mHeight;
	std::pair<int, int> mOrigin;

	ID2D1Bitmap* mImage;
};

