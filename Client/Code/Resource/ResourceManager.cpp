#include "ResourceManager.h"
#include "Texture/Texture.h"

INIT_INSTACNE(ResourceManager)
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Initialize()
{
	std::shared_ptr<Texture> texture0 = std::make_shared<Texture>();
	texture0->CreateTexture(531, 532);
	if (texture0->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
	{
		texture0.reset();
		return false;
	}
	texture0->SetSize(65, 65);
	mTextureList.emplace("Tile", texture0);

	std::shared_ptr<Texture> texture1 = std::make_shared<Texture>();
	texture1->CreateTexture(65, 76);
	if (texture1->LoadTexture(L"../Resource/Textures/Player.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
	{
		texture1.reset();
		return false;
	}
	texture1->SetSize(65, 76);
	mTextureList.emplace("Player", texture1);

	std::shared_ptr<Texture> texture2 = std::make_shared<Texture>();
	texture2->CreateTexture(78, 79);
	if (texture2->LoadTexture(L"../Resource/Textures/OtherPlayer.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
	{
		texture1.reset();
		return false;
	}
	texture2->SetSize(78, 79);
	mTextureList.emplace("OtherPlayer", texture2);

	std::shared_ptr<Texture> texture3 = std::make_shared<Texture>();
	texture3->CreateTexture(63, 55);
	if (texture3->LoadTexture(L"../Resource/Textures/OrangeMushroom.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
	{
		texture3.reset();
		return false;
	}
	texture3->SetSize(63, 55);
	mTextureList.emplace("Monster", texture3);

	return true;
}

Texture* ResourceManager::FindTexture(const std::string& name)
{
	if (mTextureList.count(name) == false)
	{
		return nullptr;
	}

	return mTextureList[name].get();
}
