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
#include <string>
#include <vector>
#include <unordered_map>

struct FontInfo
{
	FontInfo()
		: font(nullptr), textLayout(nullptr), fontSize(0.f) {}
	FontInfo(IDWriteTextFormat* _font, IDWriteTextLayout* _layout, float _size)
		: font(_font), textLayout(_layout), fontSize(_size) {}

	// 폰트 객체
	IDWriteTextFormat* font{ nullptr };
	// 폰트 형식
	IDWriteTextLayout* textLayout{ nullptr };

	float fontSize;
};

class Texture;
class GraphicEngine
{
	SINGLE_TONE(GraphicEngine)

public:
	bool Initialize(HWND handle, int width, int height);
	void RenderRectangle(const D2D1_RECT_F& pos, const std::string& color = "빨간색");
	void RenderFillRectangle(const D2D1_RECT_F& pos, const std::string& color = "빨간색");

	void RenderTexture(Texture* texture, const D2D1_RECT_F& pos);
	void RenderTexture(Texture* texture, const D2D1_RECT_F& pos, const D2D1_RECT_F& rect);

	void RenderText(const std::wstring& text, int x, int y, const std::string& color = "검은색");
    void RenderText(const std::wstring& text, int x, int y, const std::string& font, const std::string& color);

	void createFont();
	void createBrushColor();

	IWICImagingFactory* GetWICImagingFactory() { return mWICImagingFactory; }
	ID2D1HwndRenderTarget* GetRenderTarget() { return mRenderTarget; }
	
	IDWriteFactory5* GetWriteFactory() { return mWriteFactory; }
	FontInfo& GetFont(const std::string& key) { return mFontMap[key]; }


private:
	int mWidth;
	int mHeight;

	ID2D1Factory3* mFactory;
	IWICImagingFactory* mWICImagingFactory;
	IDWriteFactory5* mWriteFactory;
	IDWriteFontCollection1* mFontCollection;
	ID2D1HwndRenderTarget* mRenderTarget;

	std::unordered_map<std::string, FontInfo> mFontMap;
	std::unordered_map<std::string, ID2D1SolidColorBrush*> mBrushColorMap;
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
