#include "GraphicEngine.h"
#include "../Resource/Texture/Texture.h"

INIT_INSTACNE(GraphicEngine)
GraphicEngine::GraphicEngine()
{
	mWidth = 0;
	mHeight = 0;

	mDirect3D = nullptr;
	mDevice = nullptr;
	mSprite = nullptr;
	mFont = nullptr;
	mTextures.clear();
	mTextures.reserve(MAX_TEXTURE);
}

GraphicEngine::~GraphicEngine()
{
	for (auto& tex : mTextures)
	{
		tex->Release();
	}

	if (mFont != nullptr)
	{
		mFont->Release();
	}
	if (mSprite != nullptr)
	{
		mSprite->Release();
	}
	if (mDevice != nullptr)
	{
		mDevice->Release();
	}
	if (mDirect3D != nullptr)
	{
		mDirect3D->Release();
	}

	mRenderTarget->Release();
	mFactory->Release();
}

bool GraphicEngine::Initialize(HWND handle, int width, int height)
{
	mDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (mDirect3D == nullptr)
	{
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = mDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, handle, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &mDevice);
	if (FAILED(result))
	{
		return false;
	}

	result = D3DXCreateSprite(mDevice, &mSprite);
	if (FAILED(result))
	{
		return false;
	}

	result = D3DXCreateFontW(mDevice, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
		TEXT("±¼¸²"), &mFont);
	if (FAILED(result))
	{
		return false;
	}

	mWidth = width;
	mHeight = height;
	//screen_bpp = bpp;
	
	result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory);
	if (result != S_OK)
		return false;

	D2D1_SIZE_U size = D2D1::SizeU(mWidth, mHeight);
	D2D1_RENDER_TARGET_PROPERTIES d2dRTProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES d2dHwndRTProps = D2D1::HwndRenderTargetProperties(handle, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
	result = mFactory->CreateHwndRenderTarget(d2dRTProps, d2dHwndRTProps, &mRenderTarget);
	if (result != S_OK)
		return false;

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &mRedBrush);


	result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&mWICImagingFactory);
	if (result != S_OK)
		return false;

	return true;
}

void GraphicEngine::Flip()
{
	mDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

void GraphicEngine::FillSurface(D3DCOLOR color)
{
	mDevice->Clear(0, nullptr, D3DCLEAR_TARGET, color, 1.0f, 0);
}

void GraphicEngine::RenderStart()
{
	FillSurface(D3DCOLOR_ARGB(255, 0, 0, 0));

	mDevice->BeginScene();

	mSprite->Begin(D3DXSPRITE_ALPHABLEND);


	//mSprite->Begin(D3DXSPRITE_SORT_TEXTURE);
}

void GraphicEngine::RenderEnd()
{
	mSprite->End();

	mDevice->EndScene();

	Flip();
}

void GraphicEngine::RenderText(const wchar_t* text, int x, int y, D3DCOLOR color)
{
	RECT rectTemp = { x, y, mWidth, mHeight };
	mFont->DrawTextW(mSprite, text, -1, &rectTemp, 0, color);
}

INIT_INSTACNE(Camera)
Camera::Camera()
	: mPos(std::make_pair(-4, -4)) 
{
}

Camera::~Camera()
{
}