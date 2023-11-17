#pragma once
#include "../Common/Macro.h"
#include <d3d9.h>
#include <d3dx9tex.h>
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "../Lib/d3dx9.lib")

#include <d2d1helper.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#pragma comment( lib, "d2d1.lib" )
#pragma comment(lib, "dwrite.lib")
//using namespace D2D1;

#include <Windows.h>
#include <vector>

constexpr int MAX_TEXTURE = 16;
constexpr int MAX_BOB_FRAMES = 64;   // maximum number of bob frames
constexpr int MAX_BOB_ANIMATIONS = 16;   // maximum number of animation sequeces
constexpr int BOB_ATTR_VISIBLE = 16;  // bob is visible

class Texture;
class GraphicEngine
{
	SINGLE_TONE(GraphicEngine)

public:
	bool Initialize(HWND handle, int width, int height);
	void Flip();
	void FillSurface(D3DCOLOR color);

	void RenderStart();
	void RenderEnd();
    void RenderText(const wchar_t* text, int x, int y, D3DCOLOR color);

    const LPDIRECT3DDEVICE9 GetDevice() const { return mDevice; }
    const LPD3DXSPRITE GetSprite() const { return mSprite; }

	IWICImagingFactory* GetWICImagingFactory() { return mWICImagingFactory; }
	ID2D1HwndRenderTarget* GetRenderTarget() { return mRenderTarget; }
	ID2D1SolidColorBrush* GetRedBrush() { return mRedBrush; }

private:
	int mWidth;
	int mHeight;

	LPDIRECT3D9 mDirect3D;
	LPDIRECT3DDEVICE9 mDevice;
	LPD3DXSPRITE mSprite;
	LPD3DXFONT mFont;
	std::vector<LPDIRECT3DTEXTURE9> mTextures;

	ID2D1Factory3* mFactory;
	IWICImagingFactory* mWICImagingFactory;
	ID2D1HwndRenderTarget* mRenderTarget;
	ID2D1SolidColorBrush* mRedBrush;
};

class Camera
{
    SINGLE_TONE(Camera)

public:

    const std::pair<int, int>& GetPosition()    const { return mPos; }
    void SetPosition(int x, int y) { mPos = std::make_pair(x - 4, y - 4); }

private:
    std::pair<int, int> mPos;
};
