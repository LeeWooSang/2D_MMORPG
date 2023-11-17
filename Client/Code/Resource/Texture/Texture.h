#pragma once
#include <iostream>
#include <vector>
#include <d3dx9tex.h>

#define BITMAP_ID            0x4D42 // universal id for a bitmap

#define BITMAP_EXTRACT_MODE_CELL  0
#define BITMAP_EXTRACT_MODE_ABS   1

class ID2D1Bitmap;
class Texture
{	
public:
	Texture();
	virtual ~Texture();

	void CreateTexture(int width, int height);
	void CreateTexture(int width, int height, std::vector<std::pair<int, int>>& texPositions);
	bool LoadTexture(const wchar_t* path, int cx, int cy, int mode);

	std::pair<int, int> GetPos(int currFrame = 0)		const { return mTexPositions[currFrame]; }
	std::pair<int, int> GetSize() const { return std::make_pair(mWidth, mHeight); };
	void SetSize(int width, int height) { mWidth = width; mHeight = height; }

	LPDIRECT3DTEXTURE9 GetBuffer() { return mBuffer; }

	bool InitializeSprite();
	LPD3DXSPRITE GetSprite() { return mSprite; }

	bool Init(const wchar_t* path);
	ID2D1Bitmap* GetImage() { return mImage; }

protected:
	//int bpp;            // bits per pixel
	std::vector<std::pair<int, int>> mTexPositions;
	int mWidth;
	int mHeight;  
	int mNumBytes;								// total bytes of bitmap
	LPDIRECT3DTEXTURE9 mBuffer;      // pixels of bitmap

	LPD3DXSPRITE mSprite;

	ID2D1Bitmap* mImage;
};

