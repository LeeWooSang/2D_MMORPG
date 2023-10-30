#pragma once
#include "../Common/Macro.h"
#include <unordered_map>
#include <string>
#include <memory>

class Texture;
class ResourceManager
{
	SINGLE_TONE(ResourceManager)

public:
	bool Initialize();
	Texture* FindTexture(const std::string& name);

private:
	std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureList;
};