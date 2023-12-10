#pragma once
#include "../Common/Macro.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

struct TextInfo
{
	TextInfo()
		: width(0.0), height(0.0) {}
	TextInfo(float _width, float _height)
		: width(_width), height(_height) {}
	float width;
	float height;
};

class Texture;
class ResourceManager
{
	SINGLE_TONE(ResourceManager)

public:
	bool Initialize();
	Texture* FindTexture(const std::string& name);

	float GetTextWidth(const std::string& font, int fontSize, wchar_t wc)  { return mTextInfos[font][fontSize][wc].width; }
	float GetTextHeight(const std::string& font, int fontSize, wchar_t wc)  { return mTextInfos[font][fontSize][wc].height; }
	bool LoadJsonFile();

private:
	bool loginTexture();
	bool idleTexture();
	bool walkTexture();
	bool jumpTexture();

	bool monsterIdleTexture();
	bool monsterJumpTexture();

private:
	// 폰트, 폰트사이즈, 문자, 텍스트 크기
	std::unordered_map<std::string, std::unordered_map<int, std::unordered_map<wchar_t, TextInfo>>> mTextInfos;

	std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureList;
};