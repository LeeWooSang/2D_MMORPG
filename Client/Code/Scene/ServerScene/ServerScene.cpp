#include "ServerScene.h"
#include "../../Network/Network.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Manager/UIManager/UIManager.h"
#include "../../GameObject/UI/ChannelUI/ChannelUI.h"
#include "../../GameObject/UI/ButtonUI/ButtonUI.h"

#include "../../Manager/SceneMangaer/SceneManager.h"

void ServerSelectClick(const std::string& name)
{
	ServerScene* scene = static_cast<ServerScene*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE));
	int type = scene->GetServer(name);
	std::cout << "서버 : " << name << ", " << type << std::endl;

	ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("ChannelUI"));
	ui->OpenChannelUI();
}

ServerScene::ServerScene()
	: Scene()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
	mUIsMap.clear();
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

	UI* serverUI = new UI;
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ServerUIBackground1");
		if (serverUI->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		serverUI->SetTexture(data.name);
		serverUI->Visible();
		AddSceneUI("ServerUI", serverUI);
	}

	int i = 0;
	for(auto& server : mServerList)
	{
		std::string name = "ServerUIButton" + std::to_string(i++);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(ServerSelectClick, server.first);
		serverUI->AddChildUI(server.first, ui);
	}
	serverUI->SetPosition(660, 0);

	{
		LoginChannelUI* ui = new LoginChannelUI;
		ui->Initialize(0, 0);
		ui->SetPosition(200, 200);
		AddSceneUI("ChannelUI", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("Quit");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(0, 0) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetPosition(10, 600);
		ui->SetLButtonClickCallback(GameQuitClick, "Quit");
		AddSceneUI("QuitButton", ui);
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
		std::string name = "ServerUIBackground0";
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		D2D1_RECT_F rect;
		rect.left = data.origin.first;
		rect.top = data.origin.second;
		rect.right = rect.left + data.size.first;
		rect.bottom = rect.top + data.size.second;
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	GET_INSTANCE(UIManager)->Render();
}

void ServerScene::processKeyboardMessage()
{
}
