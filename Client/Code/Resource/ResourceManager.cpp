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
	GET_INSTANCE(JsonFormat)->Release();
}

bool ResourceManager::Initialize()
{
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		//texture->CreateTexture(1920, 1080);
		texture->CreateTexture(800, 800);
		if (texture->LoadTexture(L"../Resource/Textures/LoginScene.jpg") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("LoginScene", texture);
	}
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
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/OrangeMushroom.png") == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetSize(63, 55);
		mTextureList.emplace("Monster", texture);
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
		texture->CreateTexture(64, 64);
		if (texture->LoadTexture(L"../Resource/Textures/Slot.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Slot", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(30, 360);
		if (texture->LoadTexture(L"../Resource/Textures/ScrollBackground.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("ScrollBackground", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(25, 360);
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
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Sword2.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Sword", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Ax.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Ax", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Club.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Club", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(65, 65);
		if (texture->LoadTexture(L"../Resource/Textures/Dagger.png") == false)
		{
			texture.reset();
			return false;
		}
		mTextureList.emplace("Dagger", texture);
	}
	
	if (idleTexture() == false)
	{
		return false;
	}

	if (walkTexture() == false)
	{
		return false;
	}

	if (jumpTexture() == false)
	{
		return false;
	}

	if (LoadJsonFile() == false)
	{
		return false;
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

bool ResourceManager::idleTexture()
{
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(39, 35);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Head/FrontHead0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(0, 0);
		mTextureList.emplace("FrontHead0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(21, 31);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Idle/IdleBody0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(5, 34);
		mTextureList.emplace("IdleBody0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(22, 31);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Idle/IdleBody1.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(4, 34);
		mTextureList.emplace("IdleBody1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(23, 31);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Idle/IdleBody2.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(3, 34);
		mTextureList.emplace("IdleBody2", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(10, 19);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Idle/IdleArm0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(21, 35);
		mTextureList.emplace("IdleArm0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(10, 19);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Idle/IdleArm1.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(21, 35);
		mTextureList.emplace("IdleArm1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(11, 19);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Idle/IdleArm2.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(21, 35);
		mTextureList.emplace("IdleArm2", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 13);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Idle/IdleWeapon0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-8, 47);
		mTextureList.emplace("IdleWeapon0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 13);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Idle/IdleWeapon0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-7, 48);
		mTextureList.emplace("IdleWeapon1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 13);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Idle/IdleWeapon0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-6, 48);
		mTextureList.emplace("IdleWeapon2", texture);
	}

	return true;
}

bool ResourceManager::walkTexture()
{
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(27, 32);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Walk/WalkBody0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(2, 33);
		mTextureList.emplace("WalkBody0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(26, 32);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Walk/WalkBody1.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(5, 32);
		mTextureList.emplace("WalkBody1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(24, 32);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Walk/WalkBody2.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(2, 33);
		mTextureList.emplace("WalkBody2", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(29, 31);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Walk/WalkBody3.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(1, 33);
		mTextureList.emplace("WalkBody3", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(12, 17);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Walk/WalkArm0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(22, 35);
		mTextureList.emplace("WalkArm0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(13, 17);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Walk/WalkArm1.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(15, 36);
		mTextureList.emplace("WalkArm1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(12, 17);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Walk/WalkArm2.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(22, 35);
		mTextureList.emplace("WalkArm2", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(14, 15);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Walk/WalkArm3.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(22, 35);
		mTextureList.emplace("WalkArm3", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(43, 8);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Walk/WalkWeapon0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-6, 45);
		mTextureList.emplace("WalkWeapon0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 14);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Walk/WalkWeapon1.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-14, 39);
		mTextureList.emplace("WalkWeapon1", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(43, 8);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Walk/WalkWeapon2.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-6, 45);
		mTextureList.emplace("WalkWeapon2", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 13);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Walk/WalkWeapon3.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-2, 43);
		mTextureList.emplace("WalkWeapon3", texture);
	}

	return true;
}

bool ResourceManager::jumpTexture()
{
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(35, 30);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Body/Jump/JumpBody0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(-2, 34);
		mTextureList.emplace("JumpBody0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(13, 11);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Arm/Jump/JumpArm0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(15, 35);
		mTextureList.emplace("JumpArm0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(8, 9);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Hand/Jump/JumpLeftHand0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(4, 43);
		mTextureList.emplace("JumpLeftHand0", texture);
	}
	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(9, 9);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Hand/Jump/JumpRightHand0.png") == false)
		{
			texture.reset();
			return false;
		}
		texture->SetOrigin(0, 47);
		mTextureList.emplace("JumpRightHand0", texture);
	}

	{
		std::shared_ptr<Texture> texture = std::make_shared<Texture>();
		texture->CreateTexture(41, 13);
		if (texture->LoadTexture(L"../Resource/Textures/Character/Weapon/Idle/IdleWeapon0.png") == false)
		{
			texture.reset();
			return false;
		}
		//texture->SetOrigin(-8, 47);
		mTextureList.emplace("JumpWeapon0", texture);
	}

	return true;
}
