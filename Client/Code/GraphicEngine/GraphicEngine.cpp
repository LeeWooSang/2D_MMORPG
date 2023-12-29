#include "GraphicEngine.h"
#include "../Resource/Texture/Texture.h"

INIT_INSTACNE(GraphicEngine)
GraphicEngine::GraphicEngine()
{
	mWidth = 0;
	mHeight = 0;
	mFactory = nullptr;
	mWICImagingFactory = nullptr;
	mWriteFactory = nullptr;
	mFontCollection = nullptr;
	mRenderTarget = nullptr;
	mFontMap.clear();
	mBrushColorMap.clear();
}

GraphicEngine::~GraphicEngine()
{
	for (auto& font : mFontMap)
	{
		font.second.textLayout->Release();
		font.second.font->Release();
	}

	for (auto& color : mBrushColorMap)
	{
		color.second->Release();
	}

	mFontCollection->Release();
	mRenderTarget->Release();
	mWriteFactory->Release();
	mWICImagingFactory->Release();
	mFactory->Release();
}

bool GraphicEngine::Initialize(HWND handle, int width, int height)
{
	mWidth = width;
	mHeight = height;
	//screen_bpp = bpp;
	
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mFactory);
	if (result != S_OK)
	{
		return false;
	}

	// �ް�ü �ʱ�ȭ
	result = CoInitialize(nullptr);
	if (result != S_OK)
	{
		return false;
	}

	D2D1_SIZE_U size = D2D1::SizeU(width, height);
	D2D1_RENDER_TARGET_PROPERTIES d2dRTProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES d2dHwndRTProps = D2D1::HwndRenderTargetProperties(handle, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
	result = mFactory->CreateHwndRenderTarget(d2dRTProps, d2dHwndRTProps, &mRenderTarget);
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

	createFont();
	createBrushColor();

	return true;
}

void GraphicEngine::RenderRectangle(const D2D1_RECT_F& pos, const std::string& color)
{
	// ������ ���� ����
	mRenderTarget->DrawRectangle(pos, mBrushColorMap[color], 3.0);
}

void GraphicEngine::RenderFillRectangle(const D2D1_RECT_F& pos, const std::string& color)
{
	mRenderTarget->FillRectangle(pos, mBrushColorMap[color]);
}

void GraphicEngine::RenderTexture(Texture* texture, const D2D1_RECT_F& pos)
{
	mRenderTarget->DrawBitmap(texture->GetImage(), pos);
}

void GraphicEngine::RenderTexture(Texture* texture, const D2D1_RECT_F& pos, const D2D1_RECT_F& rect)
{
	mRenderTarget->DrawBitmap(texture->GetImage(), pos, 1.0, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rect);
}

void GraphicEngine::RenderText(const std::wstring& text, int x, int y, const std::string& color)
{
	D2D1_RECT_F rect = { x, y, x + 100, y + 100 };
	mRenderTarget->DrawTextW(text.c_str(), static_cast<UINT32>(text.length()), mFontMap["����"].font, &rect, mBrushColorMap[color]);
}

void GraphicEngine::RenderText(const std::wstring& text, int x, int y, const std::string& font, const std::string& color)
{
	//RECT rect = { x, y, mWidth, mHeight };
	//mFont->DrawTextW(mSprite, text, -1, &rect, 0, color);

	D2D1_RECT_F rect = {x, y, mWidth + 100, mHeight };
	mRenderTarget->DrawTextW(text.c_str(), static_cast<UINT32>(text.length()), mFontMap[font].font, &rect, mBrushColorMap[color]);

	//float pointX;
	//float pointY;
	//DWRITE_HIT_TEST_METRICS metrics;
	//mFontMap[font].textLayout->HitTestTextPosition(0, false, &pointX, &pointY, &metrics);
}

void GraphicEngine::createFont()
{
	const int MAX_LOAD_FONT_COUNT = 2;

	// ��Ʈ ���
	std::wstring fontPath[] = { L"../Resource/Fonts/a�ǿ��ǵ���.ttf", L"../Resource/Fonts/�����ý��丮.ttf" };

	// ��Ʈ�� ���� ��ġ�Ҷ� ���
	//AddFontResourceEx(fontPath[0].c_str(), FR_PRIVATE, 0);
	//AddFontResourceEx(fontPath[1].c_str(), FR_PRIVATE, 0);

	// ��Ʈ�� ��ġ���� �ʰ�, �޸𸮿� �÷��� ����� ���
	// ���� ����
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT result = mWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);

	IDWriteFontFile* fontFile[MAX_LOAD_FONT_COUNT];
	IDWriteFontSet* fontSet[MAX_LOAD_FONT_COUNT];

	std::wstring fontName[MAX_LOAD_FONT_COUNT];
	unsigned int max_length = 64;

	for (int i = 0; i < MAX_LOAD_FONT_COUNT; ++i)
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

	for (int i = 0; i < MAX_LOAD_FONT_COUNT; ++i)
	{
		fontFile[i]->Release();
		fontSet[i]->Release();
	}

	for (int i = 0; i < MAX_LOAD_FONT_COUNT; ++i)
	{
		// ��Ʈ ��ü
		IDWriteTextFormat* font;
		// ��Ʈ ����
		IDWriteTextLayout* textLayout;
		float fontSize = 25.f;
		std::wstring wstr = L"TextLayout Initialize";

		// ��Ʈ ��ü ����	
		result = mWriteFactory->CreateTextFormat(
			fontName[i].c_str(),
			mFontCollection,
			//DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_WEIGHT_DEMI_BOLD,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-US",
			&font);

		if (i == 0)
		{
			// ��Ʈ�� �߾ӿ� ���Ľ�Ű��
			result = font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			result = font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			result = mWriteFactory->CreateTextLayout(wstr.c_str(), static_cast<UINT32>(wstr.length()), font, 1024.0f, 1024.0f, &textLayout);

			mFontMap.emplace("�ǿ��ǵ���", FontInfo(font, textLayout, fontSize));
		}
		else
		{
			result = font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			result = font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			result = mWriteFactory->CreateTextLayout(wstr.c_str(), static_cast<UINT32>(wstr.length()), font, 0, 0, &textLayout);
			mFontMap.emplace("������", FontInfo(font, textLayout, fontSize));
		}
	}

	{
		IDWriteTextFormat* font;
		IDWriteTextLayout* textLayout;
		float fontSize = 14;
		std::wstring wstr = L"0";

		result = mWriteFactory->CreateTextFormat(
			L"Gulim",
			mFontCollection,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-US",
			&font);

		//result = font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		result = font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		result = font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		result = mWriteFactory->CreateTextLayout(wstr.c_str(), static_cast<UINT32>(wstr.length()), font, 0, 0, &textLayout);
		mFontMap.emplace("����", FontInfo(font, textLayout, fontSize));
	}
}

void GraphicEngine::createBrushColor()
{
	const int MAX_COLOR_COUNT = 9;
	ID2D1SolidColorBrush* color[MAX_COLOR_COUNT];

	int index = 0;
	HRESULT result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Blue, 1.0f), &color[index]);
	mBrushColorMap.emplace("�Ķ���", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Brown, 1.0f), &color[index]);
	mBrushColorMap.emplace("����", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &color[index]);
	mBrushColorMap.emplace("���", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &color[index]);
	mBrushColorMap.emplace("������", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue, 1.0f), &color[index]);
	mBrushColorMap.emplace("�ϴû�", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LavenderBlush, 1.0f), &color[index]);
	mBrushColorMap.emplace("����ȫ��", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &color[index]);
	mBrushColorMap.emplace("������", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f), &color[index]);
	mBrushColorMap.emplace("��Ȳ��", color[index++]);

	result = mRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 0.5f), &color[index]);
	mBrushColorMap.emplace("����������", color[index++]);
}

INIT_INSTACNE(Camera)
Camera::Camera()
	: mPos(std::make_pair(-4, -4)) 
{
}

Camera::~Camera()
{
}