#include "Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

Texture::Texture()
{
	mTexPositions.reserve(100);

	//int bpp;      
	mWidth = 0;
	mHeight = 0;
	mNumBytes = 0;
	mBuffer = nullptr;

	mSprite = nullptr;
}

Texture::~Texture()
{
	mTexPositions.clear();
	if (mBuffer != nullptr)
	{
		mBuffer->Release();
	}

	if (mSprite != nullptr)
	{
		mSprite->Release();
	}
}

void Texture::CreateTexture(int width, int height)
{
	// 기본은 1개만 
	mTexPositions.emplace_back(std::make_pair(0, 0));

	mWidth = width;
	mHeight = height;
	mNumBytes = width * height * 2;
	mBuffer = nullptr;

	InitializeSprite();
}

void Texture::CreateTexture(int width, int height, std::vector<std::pair<int, int>>& texPositions)
{
	// 텍스쳐의 포지션이 여러개인 경우
	mTexPositions = texPositions;
	mWidth = width;
	mHeight = height;
	mNumBytes = width * height * 2;
	mBuffer = nullptr;

	InitializeSprite();
}

bool Texture::LoadTexture(const wchar_t* path, int cx, int cy, int mode)
{
	if (mode == BITMAP_EXTRACT_MODE_CELL)
	{
		cx = cx * (mWidth + 1) + 1;
		cy = cy * (mHeight + 1) + 1;
	}

	HRESULT result = D3DXCreateTextureFromFileEx
	(
		GET_INSTANCE(GraphicEngine)->GetDevice(), 
		path,
		mWidth,
		mHeight,
		1,
		0,
		D3DFMT_UNKNOWN, D3DPOOL_DEFAULT,
		D3DX_DEFAULT, D3DX_DEFAULT,
		D3DCOLOR_ARGB(255, 0, 0, 0),
		NULL, NULL, &mBuffer
	);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Texture::InitializeSprite()
{
	HRESULT result = D3DXCreateSprite(GET_INSTANCE(GraphicEngine)->GetDevice(), &mSprite);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
