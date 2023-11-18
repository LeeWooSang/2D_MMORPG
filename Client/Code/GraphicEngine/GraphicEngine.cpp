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

	mFactory = nullptr;
	mWICImagingFactory = nullptr;
	mWriteFactory = nullptr;
	mFontCollection = nullptr;
	mRenderTarget = nullptr;
	mRedBrush = nullptr;
	mFontMap.clear();
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

	for (auto& font : mFontMap)
	{
		font.second.textLayout->Release();
		font.second.font->Release();
	}
	mFontCollection->Release();
	mRedBrush->Release();
	mRenderTarget->Release();
	mWriteFactory->Release();
	mWICImagingFactory->Release();
	mFactory->Release();
}

bool GraphicEngine::Initialize(HWND handle, int width, int height)
{
	/*mDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
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
		TEXT("����"), &mFont);
	if (FAILED(result))
	{
		return false;
	}*/

	mWidth = width;
	mHeight = height;
	//screen_bpp = bpp;
	
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory);
	if (result != S_OK)
	{
		return false;
	}

	D2D1_SIZE_U size = D2D1::SizeU(mWidth, mHeight);
	D2D1_RENDER_TARGET_PROPERTIES d2dRTProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES d2dHwndRTProps = D2D1::HwndRenderTargetProperties(handle, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
	result = mFactory->CreateHwndRenderTarget(d2dRTProps, d2dHwndRTProps, &mRenderTarget);
	if (result != S_OK)
	{
		return false;
	}

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &mRedBrush);
	if (result != S_OK)
	{
		return false;
	}

	result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&mWICImagingFactory);
	if (result != S_OK)
	{
		return false;
	}

	// �ؽ�Ʈ�� ���� Factory ����
	result = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&mWriteFactory);
	if (result != S_OK)
	{
		return false;
	}

	createGameFont();

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

void GraphicEngine::RenderRectangle(const D2D1_RECT_F& pos)
{
	// ������ ���� ����
	mRenderTarget->DrawRectangle(pos, mRedBrush, 3.0);
}

void GraphicEngine::RenderTexture(Texture* texture, const D2D1_RECT_F& pos)
{
	mRenderTarget->DrawBitmap(texture->GetImage(), pos);
}

void GraphicEngine::RenderTexture(Texture* texture, const D2D1_RECT_F& pos, const D2D1_RECT_F& rect)
{
	mRenderTarget->DrawBitmap(texture->GetImage(), pos, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rect);
}

void GraphicEngine::RenderText(const std::wstring& text, int x, int y, const std::string& font)
{
	//RECT rect = { x, y, mWidth, mHeight };
	//mFont->DrawTextW(mSprite, text, -1, &rect, 0, color);

	D2D1_RECT_F rect = {x, y, mWidth, mHeight };
	mRenderTarget->DrawTextW(text.c_str(), static_cast<UINT32>(text.length()), mFontMap[font].font, &rect, mRedBrush);
}

void GraphicEngine::createGameFont()
{
	// ��Ʈ ���
	std::wstring fontPath[] = { L"../Resource/Fonts/a�ǿ��ǵ���.ttf", L"../Resource/Fonts/�����ý��丮.ttf" };

	// ��Ʈ�� ���� ��ġ�Ҷ� ���
	//AddFontResourceEx(fontPath[0].c_str(), FR_PRIVATE, 0);
	//AddFontResourceEx(fontPath[1].c_str(), FR_PRIVATE, 0);

	// ��Ʈ�� ��ġ���� �ʰ�, �޸𸮿� �÷��� ����� ���
	// ���� ����
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT result = mWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);

	IDWriteFontFile* fontFile[MAX_FONT_COUNT];
	IDWriteFontSet* fontSet[MAX_FONT_COUNT];

	std::wstring fontName[MAX_FONT_COUNT];
	unsigned int max_length = 64;

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// �ش��ϴ� ��ο��� ��Ʈ ������ �ε��Ѵ�.
		result = mWriteFactory->CreateFontFileReference(fontPath[i].c_str(), nullptr, &fontFile[i]);
		// ������ ��Ʈ �߰�
		result = pFontSetBuilder->AddFontFile(fontFile[i]);
		result = pFontSetBuilder->CreateFontSet(&fontSet[i]);
		// ��Ʈ Collection�� ��Ʈ �߰� => ��Ʈ Collection���� ���� ����� ��Ʈ�� ����Ǿ� ����
		result = mWriteFactory->CreateFontCollectionFromFontSet(fontSet[i], &mFontCollection);

		// ��Ʈ �̸��� ������ ���
		IDWriteFontFamily* pFontFamily;
		IDWriteLocalizedStrings* pLocalizedFontName;

		result = mFontCollection->GetFontFamily(i, &pFontFamily);
		result = pFontFamily->GetFamilyNames(&pLocalizedFontName);
		// ����Ǿ��ִ� ��Ʈ�� �̸��� ����
		result = pLocalizedFontName->GetString(0, const_cast<wchar_t*>(fontName[i].c_str()), max_length);

		pFontFamily->Release();
		pLocalizedFontName->Release();
	}

	pFontSetBuilder->Release();

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		fontFile[i]->Release();
		fontSet[i]->Release();
	}

	float fontSize = 25.f;

	// ��Ʈ ��ü
	IDWriteTextFormat* pFont[MAX_FONT_COUNT];
	// ��Ʈ ����
	IDWriteTextLayout* pTextLayout[MAX_FONT_COUNT];
	std::wstring wstr = L"TextLayout Initialize";

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// ��Ʈ ��ü ����	
		result = mWriteFactory->CreateTextFormat(fontName[i].c_str(), mFontCollection, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-US", &pFont[i]);

		if (i == 0)
		{
			// ��Ʈ�� �߾ӿ� ���Ľ�Ű��
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			result = mWriteFactory->CreateTextLayout(wstr.c_str(), static_cast<UINT32>(wstr.length()), pFont[i], 1024.0f, 1024.0f, &pTextLayout[i]);

			mFontMap.emplace("�ǿ��ǵ���", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
		else
		{
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			result = mWriteFactory->CreateTextLayout(wstr.c_str(), static_cast<UINT32>(wstr.length()), pFont[i], 4096.0f, 4096.0f, &pTextLayout[i]);

			mFontMap.emplace("������", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
	}
}

INIT_INSTACNE(Camera)
Camera::Camera()
	: mPos(std::make_pair(-4, -4)) 
{
}

Camera::~Camera()
{
}