#include "ServerScene.h"
#include "../../Network/Network.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Manager/UIManager/UIManager.h"
#include "../../GameObject/UI/ButtonUI/ButtonUI.h"

#include "../../Manager/SceneMangaer/SceneManager.h"

void ServerSelectClick(const std::string& name)
{
	ServerScene* scene = static_cast<ServerScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE));
	int type = scene->GetServer(name);
	std::cout << "서버 : " << name << ", " << type << std::endl;
}

ServerScene::ServerScene()
	: Scene()
{
}

ServerScene::~ServerScene()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
	mUIsMap.clear();
}

bool ServerScene::Initialize()
{
	mServerList.emplace("스카니아", GAME_SERVER_TYPE::SCANIA);
	mServerList.emplace("루나", GAME_SERVER_TYPE::LUNA);
	mServerList.emplace("엘리시움", GAME_SERVER_TYPE::ELYSIUM);
	mServerList.emplace("크로아", GAME_SERVER_TYPE::CROA);
	mServerList.emplace("버닝", GAME_SERVER_TYPE::BURNING);

	int x = 590;
	int y = 0;
	int gap = 30;

	for(auto& server : mServerList)
	{
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(0, 0) == false)
		{
			return false;
		}
		ui->SetRect(120, 50);
		ui->Visible();
		ui->SetPosition(x + gap, y + gap);
		ui->SetLButtonClickCallback(ServerSelectClick, server.first);
		AddSceneUI(server.first, ui);
		y += 50 + gap;
	}

	return true;
}

void ServerScene::Update()
{
	GET_INSTANCE(UIManager)->Update();
}

void ServerScene::Render()
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("ServerBackground");
		D2D1_RECT_F pos;
		pos.left = 0;
		pos.top = 0;
		pos.right = pos.left + tex->GetSize().first;
		pos.bottom = pos.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, pos);
	}
	{
		D2D1_RECT_F pos;
		pos.left = 615;
		pos.top = 25;
		pos.right = pos.left + 130;
		pos.bottom = pos.top + 450;
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "흰색");
	}

	GET_INSTANCE(UIManager)->Render();
}

void ServerScene::processKeyboardMessage()
{
}
