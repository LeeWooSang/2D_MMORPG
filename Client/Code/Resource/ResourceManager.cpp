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
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(531, 532);
		if (texture->LoadTexture(L"../Resource/Textures/Map.bmp", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		texture->SetSize(65, 65);
		mTextureList.emplace("Tile", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Player.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(65, 76);
		mTextureList.emplace("Player", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/OtherPlayer.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(78, 79);
		mTextureList.emplace("OtherPlayer", texture);
	}	
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/OrangeMushroom.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(63, 55);
		mTextureList.emplace("Monster", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Sword.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Sword", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Ax.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Ax", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(300, 453);
		if (texture->LoadTexture(L"../Resource/Textures/Inventory.png", 0, 0, BITMAP_EXTRACT_MODE_ABS) == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(65, 65);
		mTextureList.emplace("Inventory", texture);
	}


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
