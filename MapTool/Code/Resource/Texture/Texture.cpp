#include "Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

Texture::Texture()
{
	mTexPositions.reserve(100); 
	mWidth = 0;
	mHeight = 0;

	mOrigin = std::make_pair(0, 0);
	mImage = nullptr;
}

Texture::~Texture()
{
	mTexPositions.clear();

	if (mImage != nullptr)
	{
		mImage->Release();
	}
}

void Texture::CreateTexture(int width, int height)
{
	// 기본은 1개만 
	mTexPositions.emplace_back(std::make_pair(0, 0));

	mWidth = width;
	mHeight = height;
}

void Texture::CreateTexture(int width, int height, std::vector<std::pair<int, int>>& texPositions)
{
	// 텍스쳐의 포지션이 여러개인 경우
	mTexPositions = texPositions;
	mWidth = width;
	mHeight = height;
}

bool Texture::LoadTexture(const wchar_t* path)
{
	IWICBitmapDecoder* pBitmapDecoder;
	HRESULT result = GET_INSTANCE(GraphicEngine)->GetWICImagingFactory()->CreateDecoderFromFilename(path, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pBitmapDecoder);
	if (result != S_OK)
		return false;

	IWICBitmapFrameDecode* pFrameDecode;
	result = pBitmapDecoder->GetFrame(0, &pFrameDecode);
	if (result != S_OK)
		return false;

	IWICBitmapSource* wicSource = pFrameDecode;

	// 이미지 뒤집기
	IWICBitmapFlipRotator* flipRotator = nullptr;
	//result = GET_INSTANCE(GraphicEngine)->GetWICImagingFactory()->CreateBitmapFlipRotator(&flipRotator);
	//if (result != S_OK)
	//	return false;

	//result = flipRotator->Initialize(wicSource, WICBitmapTransformFlipHorizontal);
	//if (result != S_OK)
	//	return false;
	//wicSource = flipRotator;

	IWICFormatConverter* pFormatConverter;
	result = GET_INSTANCE(GraphicEngine)->GetWICImagingFactory()->CreateFormatConverter(&pFormatConverter);
	if (result != S_OK)
		return false;
	//result = pFormatConverter->Initialize(pFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	result = pFormatConverter->Initialize(wicSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
	if (result != S_OK)
		return false;

	result = GET_INSTANCE(GraphicEngine)->GetRenderTarget()->CreateBitmapFromWicBitmap(pFormatConverter, &mImage);
	if (result != S_OK)
		return false;

	if (pBitmapDecoder)
		pBitmapDecoder->Release();

	if (pFrameDecode)
		pFrameDecode->Release();

	if (pFormatConverter)
		pFormatConverter->Release();

	if (flipRotator)
		flipRotator->Release();

	return true;
}