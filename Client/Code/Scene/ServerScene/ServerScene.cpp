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

	LoginChannelUI* ui = static_cast<LoginChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("ChannelUI"));
	ui->ChangeLogo(type);

#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendServerSelect(static_cast<GAME_SERVER_TYPE>(type));
#else
	ui->OpenChannelUI();
#endif // SERVER_CONNECT
}

void NoticeYesClick(const std::string& name)
{
	UI* ui = GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("NoticeUI");
	ui->NotVisible();
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
	std::list<std::string>serverList{ "스카니아", "루나", "엘리시움", "크로아", "버닝" };
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
	for(auto& server : serverList)
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
		ui->SetLButtonClickCallback(ServerSelectClick, server);
		serverUI->AddChildUI(server, ui);
	}
	serverUI->SetPosition(660, 0);

	{
		LoginChannelUI* ui = new LoginChannelUI;
		ui->Initialize(0, 0);
		ui->SetPosition(200, 200);
		AddSceneUI("ChannelUI", ui);
	}
	{
		UI* noticeUI = new UI;
		{
			TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("NoticeUIBackground");
			noticeUI->Initialize(data.origin.first, data.origin.second);
			noticeUI->SetTexture(data.name);
		}
		{
			TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("NoticeUIText0");
			UI* ui = new UI;
			ui->Initialize(data.origin.first, data.origin.second);
			ui->SetTexture(data.name);
			ui->Visible();
			noticeUI->AddChildUI("Text", ui);
		}
		{
			TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("NoticeUIButton0");
			ButtonUI* ui = new ButtonUI;
			ui->Initialize(data.origin.first, data.origin.second);
			ui->SetTexture(data.name);
			ui->SetLButtonClickCallback(NoticeYesClick, "");
			ui->Visible();
			noticeUI->AddChildUI("Text", ui);
		}
		noticeUI->SetPosition(275, 300);
		AddSceneUI("NoticeUI", noticeUI);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ServerUIQuit");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(0, 0) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetPosition(0, 700);
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

void ServerScene::ProcessServerSelect(short* channelUserSize)
{
	LoginChannelUI* ui = static_cast<LoginChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("ChannelUI"));
	std::vector<UI*>& v = ui->FindChildUIs("Slot");
	for (int i = 0; i < v.size(); ++i)
	{
		LoginChannelUISlot* slot = static_cast<LoginChannelUISlot*>(v[i]);
		slot->SetChannelUserSize(channelUserSize[i]);
	}

	ui->OpenChannelUI();
}

void ServerScene::ProcessChannelLogin(int channel)
{
	if (channel == -1)
	{
		GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("NoticeUI")->Visible();
		return;
	}

	LoginChannelUI* ui = static_cast<LoginChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::SERVER_SCENE)->FindUI("ChannelUI"));

	ui->ResetSelectChannel();
	ui->SetCurrentChannel(channel);

	GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::INGAME_SCENE);
	ChannelUI* channelUI = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
	channelUI->SetCurrentChannel(channel);
}
