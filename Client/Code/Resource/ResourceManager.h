#pragma once
#include "../Common/Macro.h"
#include <unordered_map>
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

struct TextureData
{
	TextureData()
		: path(""), name(""), size(std::make_pair(0, 0)), origin(std::make_pair(0, 0)), texId(0), icon(false), motion(-1) {}

	TextureData(const std::string& _path, const std::string& _name, int width, int height, int originX, int originY, int _texId, bool _icon, int _motion)
		: path(_path), name(_name), size(std::make_pair(width, height)), origin(std::make_pair(originX, originY)),
		texId(_texId), icon(_icon), motion(_motion)
	{
	}

	std::string path;
	std::string name;
	std::pair<int, int> size;
	std::pair<int, int> origin;
	int texId;
	bool icon;
	int motion;
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

	TextureData& GetTextureData(const std::string name);
	std::vector<TextureData>& GetTextureDatas(int texId) { return mTextureIds[texId]; }
	TextureData& GetTextureDataIcon(int texId);

private:
	bool loadTextureDatas();

private:
	// 폰트, 폰트사이즈, 문자, 텍스트 크기
	std::unordered_map<std::string, std::unordered_map<int, std::unordered_map<wchar_t, TextInfo>>> mTextInfos;

	std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureList;
	std::unordered_map<std::string, TextureData> mTextureDatas;
	// 텍스쳐 아이디로 텍스쳐정보를 찾을 수 있어야함
	std::unordered_map<int, std::vector<TextureData>> mTextureIds;
};