#include "ResourceManager.h"
#include "Texture/Texture.h"
#include "../Common/JsonFormat.h"

INIT_INSTACNE(ResourceManager)
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	mTextInfos.clear();
	mTextureDatas.clear();
	mTextureList.clear();
	GET_INSTANCE(JsonFormat)->Release();
}

bool ResourceManager::Initialize()
{
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(531, 532);
		if (texture->LoadTexture(L"../Resource/Textures/Map.bmp") == false)
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
		if (texture->LoadTexture(L"../Resource/Textures/Player.png") == false)
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
		if (texture->LoadTexture(L"../Resource/Textures/OtherPlayer.png") == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(78, 79);
		mTextureList.emplace("OtherPlayer", texture);
	}	
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(330, 453);
		if (texture->LoadTexture(L"../Resource/Textures/InventoryBackground.png") == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(65, 65);
		mTextureList.emplace("Inventory", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(30, 30);
		if (texture->LoadTexture(L"../Resource/Textures/Slot.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("InventorySlot", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(12, 241);
		if (texture->LoadTexture(L"../Resource/Textures/ScrollBackground.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("ScrollBackground", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(11, 241);
		if (texture->LoadTexture(L"../Resource/Textures/ScrollBar.png") == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(20, 100);
		mTextureList.emplace("ScrollBar", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(30, 30);
		if (texture->LoadTexture(L"../Resource/Textures/XButton.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("XButton", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(100, 30);
		if (texture->LoadTexture(L"../Resource/Textures/Slot.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Slot", texture);
	}

	if (loadTextureDatas() == false)
	{
		return false;
	}

	//if (LoadJsonFile() == false)
	//{
	//	return false;
	//}

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

bool ResourceManager::LoadJsonFile()
{
	rapidjson::Document doc;
	if (GET_INSTANCE(JsonFormat)->LoadJsonFile(doc, "../Resource/fontData.json") == false)
	{
		return false;
	}

	std::string font = doc["font"].GetString();
	std::unordered_map<int, std::unordered_map<wchar_t, TextInfo>> um0;
	for (int i = 0; i < doc["fontData"].Size(); ++i)
	{
		int fontSize = doc["fontData"][i]["fontSize"].GetFloat();

		std::unordered_map<wchar_t, TextInfo> um1;
		for (int j = 0; j < doc["fontData"][i]["data"].Size(); ++j)
		{
			wchar_t wc = doc["fontData"][i]["data"][j]["wc"].GetInt();
			float width = doc["fontData"][i]["data"][j]["width"].GetFloat();
			float height = doc["fontData"][i]["data"][j]["height"].GetFloat();
			um1.emplace(wc, TextInfo(width, height));
		}
		um0.emplace(fontSize, um1);
	}
	mTextInfos.emplace(font, um0);

	return true;
}

TextureData& ResourceManager::GetTextureData(const std::string name)
{
	return mTextureDatas[name];
}

TextureData& ResourceManager::GetTextureDataIcon(int texId)
{
	int index = -1;
	std::vector<TextureData>& v = mTextureIds[texId];
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i].icon == true)
		{
			index = i;
			break;
		}
	}

	return v[index];
}

bool ResourceManager::loadTextureDatas()
{
	rapidjson::Document doc;
	if (GET_INSTANCE(JsonFormat)->LoadJsonFile(doc, "../Resource/TextureDatas.json") == false)
	{
		return false;
	}

	for (int i = 0; i < doc.Size(); ++i)
	{
		TextureData data;
		data.path = doc[i]["Path"].GetString();
		data.name = doc[i]["Name"].GetString();
		data.size = std::make_pair(doc[i]["Width"].GetInt(), doc[i]["Height"].GetInt());
		data.origin = std::make_pair(doc[i]["OriginX"].GetInt(), doc[i]["OriginY"].GetInt());

		if (doc[i].HasMember("TextureId") == true)
		{
			data.texId = doc[i]["TextureId"].GetInt();
		}
		if (doc[i].HasMember("Icon") == true)
		{
			data.icon = doc[i]["Icon"].GetBool();
		}
		if (doc[i].HasMember("Motion") == true)
		{
			data.motion = doc[i]["Motion"].GetInt();
		}
		if (doc[i].HasMember("Parts") == true)
		{
			data.parts = doc[i]["Parts"].GetString();
		}
		if (doc[i].HasMember("EquipSlotType") == true)
		{
			data.equipSlotType = doc[i]["EquipSlotType"].GetInt();
		}

		mTextureDatas.emplace(data.name, data);
		
		if (data.texId == 0)
		{
			continue;
		}

		if (mTextureIds.count(data.texId) == false)
		{
			std::vector<TextureData> v;
			v.reserve(1000);
			v.emplace_back(data);
			mTextureIds.emplace(data.texId, v);
		}
		else
		{
			mTextureIds[data.texId].emplace_back(data);
		}
	}
	
	// 텍스쳐 로드
	for (auto& texData : mTextureDatas)
	{
		TextureData& data = texData.second;
		std::wstring path;
		path.assign(data.path.begin(), data.path.end());

		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(data.size.first, data.size.second);
		if (texture->LoadTexture(path.c_str()) == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(data.origin.first, data.origin.second);
		mTextureList.emplace(data.name, texture);
	}

	return true;
}
