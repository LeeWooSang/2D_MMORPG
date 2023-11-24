#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>

#include <d2d1helper.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>
#pragma comment( lib, "d2d1.lib" )
#pragma comment(lib, "dwrite.lib")

#include "../Client/Code/Common/JsonFormat.h"

bool InitializeFont(const std::wstring& fontPath, std::wstring& fontName);
IDWriteTextFormat* CreateMyFont(const std::wstring& fontName, float fontSize);
std::pair<float, float> GetTextSize(IDWriteTextFormat* font, wchar_t wc);
void SaveJsonFile(const std::string& fontName);
void LoadJsonFile();

IDWriteFactory5* writeFactory = nullptr;
IDWriteFontCollection1* fontCollection = nullptr;

constexpr int MAX_FONT_COUNT = 2;
std::unordered_map<std::string, IDWriteTextFormat*> gFontMap;

struct TextData
{
	TextData(int _wc, float _width, float _height)
		: wc(_wc), width(_width), height(_height) {}

	int wc;
	float width;
	float height;
};

std::vector<std::pair<float, std::vector<TextData>>> gTextDatas;



int main()
{
	HRESULT result = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&writeFactory);
	if (result != S_OK)
	{
		return false;
	}

	std::wstring fontPath[] = { L"../Client/Resource/Fonts/a피오피동글.ttf", L"../Client/Resource/Fonts/메이플스토리.ttf" };
	{
		std::wstring fontName;
		if (InitializeFont(fontPath[1], fontName) == false)
		{
			return -1;
		}

		rapidjson::Document doc;
		doc.SetObject();
		for (int fontSize = 25; fontSize <= 30; ++fontSize)
		{
			IDWriteTextFormat* font = CreateMyFont(fontName, fontSize);
			if (gFontMap.count("메이플") == false)
			{
				gFontMap.emplace("메이플", font);
			}

			std::vector<TextData> v;
			for (int i = ' '; i <= '~'; ++i)
			{
				std::pair<float, float> size = GetTextSize(font, i);
				v.emplace_back(TextData(i, size.first, size.second));
			}
			gTextDatas.emplace_back(std::make_pair(fontSize, v));
		}

		SaveJsonFile("Maple");
		
		//LoadJsonFile();
	}

	for (auto& font : gFontMap)
	{
		font.second->Release();
	}
	fontCollection->Release();
	writeFactory->Release();
}

bool InitializeFont(const std::wstring& fontPath, std::wstring& fontName)
{
	static int fontIndex = 0;

	// 폰트를 직접 설치할때 사용
	//AddFontResourceEx(fontPath[0].c_str(), FR_PRIVATE, 0);
	//AddFontResourceEx(fontPath[1].c_str(), FR_PRIVATE, 0);

	// 폰트를 설치하지 않고, 메모리에 올려서 사용할 경우
	// 빌더 생성
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT result = writeFactory->CreateFontSetBuilder(&pFontSetBuilder);

	IDWriteFontFile* fontFile;
	IDWriteFontSet* fontSet;

	unsigned int max_length = 64;

	// 해당하는 경로에서 폰트 파일을 로드한다.
	result = writeFactory->CreateFontFileReference(fontPath.c_str(), nullptr, &fontFile);
	// 빌더에 폰트 추가
	result = pFontSetBuilder->AddFontFile(fontFile);
	result = pFontSetBuilder->CreateFontSet(&fontSet);
	// 폰트 Collection에 폰트 추가 => 폰트 Collection에서 내가 사용할 폰트가 저장되어 있음
	result = writeFactory->CreateFontCollectionFromFontSet(fontSet, &fontCollection);

	// 폰트 이름을 얻어오는 방법
	IDWriteFontFamily* pFontFamily;
	IDWriteLocalizedStrings* pLocalizedFontName;

	result = fontCollection->GetFontFamily(fontIndex++, &pFontFamily);
	result = pFontFamily->GetFamilyNames(&pLocalizedFontName);
	
	// 저장되어있는 폰트의 이름을 얻어옴
	result = pLocalizedFontName->GetString(0, const_cast<wchar_t*>(fontName.c_str()), max_length);

	pFontFamily->Release();
	pLocalizedFontName->Release();
	
	pFontSetBuilder->Release();
	fontFile->Release();
	fontSet->Release();

	return true;
}

IDWriteTextFormat* CreateMyFont(const std::wstring& fontName, float fontSize)
{
	IDWriteTextFormat* font = nullptr;
	// 폰트 객체 생성	
	HRESULT result = writeFactory->CreateTextFormat(fontName.c_str(), fontCollection, DWRITE_FONT_WEIGHT_DEMI_BOLD,
		DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-US", &font);

	result = font->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	result = font->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	return font;
}

std::pair<float, float> GetTextSize(IDWriteTextFormat* font, wchar_t wc)
{
	std::pair<float, float> size = std::make_pair(0.00f, 0.00f);
	// 폰트 형식
	IDWriteTextLayout* layout = nullptr;
	std::wstring text = L"";
	text += wc;

	HRESULT result = writeFactory->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.length()), font, 0, 0, &layout);

	float x;
	float y;
	DWRITE_HIT_TEST_METRICS metrics;
	layout->HitTestTextPosition(0, false, &x, &y, &metrics);

	size = std::make_pair(metrics.width, metrics.height);

	return size;
}

void SaveJsonFile(const std::string& fontName)
{
	rapidjson::Document doc;
	doc.SetObject();

	doc.AddMember("font", Value().SetString(fontName.c_str(), doc.GetAllocator()), doc.GetAllocator());
	rapidjson::Value arrayData0(rapidjson::kArrayType);
	for (int i = 0; i < gTextDatas.size(); ++i)
	{
		rapidjson::Value data1;
		data1.SetObject();
		data1.AddMember("fontSize", Value().SetFloat(gTextDatas[i].first), doc.GetAllocator());

		rapidjson::Value arrayData1(rapidjson::kArrayType);
		for (int j = 0; j < gTextDatas[i].second.size(); ++j)
		{
			int wc = gTextDatas[i].second[j].wc;
			float width = gTextDatas[i].second[j].width;
			float height = gTextDatas[i].second[j].height;

			rapidjson::Value data;
			data.SetObject();

			data.AddMember("wc", Value().SetInt(wc), doc.GetAllocator());
			data.AddMember("width", Value().SetFloat(width), doc.GetAllocator());
			data.AddMember("height", Value().SetFloat(height), doc.GetAllocator());
			arrayData1.PushBack(data, doc.GetAllocator());
		}
		data1.AddMember("data", arrayData1, doc.GetAllocator());
		arrayData0.PushBack(data1, doc.GetAllocator());
	}
	doc.AddMember("fontData", arrayData0, doc.GetAllocator());

	GET_INSTANCE(JsonFormat)->SaveJsonFile(doc, "../Client/Resource/fontData.json");
}

void LoadJsonFile()
{
	rapidjson::Document doc;
	GET_INSTANCE(JsonFormat)->LoadJsonFile(doc, "../Client/Resource/fontData.json");

	std::string fontName = doc["font"].GetString();
	float fontSize = doc["fontData"]["fontSize"].GetFloat();
	int wc = doc["fontData"]["data"][0]["wc"].GetInt();
	float width = doc["fontData"]["data"][0]["width"].GetFloat();
	float height = doc["fontData"]["data"][0]["height"].GetFloat();
	std::cout << fontName << std::endl;
}
