#include "GameMenuUI.h"
#include "../../../Input/Input.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/Scene.h"
#include "../ChannelUI/ChannelUI.h"

void ChannelChangeMenuClick(const std::string& name)
{
	{
		ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
		ui->OpenChannelUI();
	}
	{
		InGameMenuUI* ui = static_cast<InGameMenuUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("InGameMenuUI"));
		ui->OpenGameMenuUI();
	}
}

void GameOptionMenuClick(const std::string& name)
{
}

GameMenuUI::GameMenuUI()
	: UI()
{
	mOpen = false;
}

GameMenuUI::~GameMenuUI()
{
}

bool GameMenuUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	
	return true;
}

void GameMenuUI::Update()
{
}

void GameMenuUI::Render()
{
}

void GameMenuUI::MouseOver()
{
	UI::MouseOver();
}

void GameMenuUI::MouseLButtonDown()
{
}

void GameMenuUI::MouseLButtonUp()
{
}

void GameMenuUI::MouseLButtonClick()
{
}

void GameMenuUI::OpenGameMenuUI()
{
	// 닫혀있다면
	if (mOpen == false)
	{
		mOpen = true;
		Visible();
	}
	// 열려있다면
	else
	{
		mOpen = false;
		NotVisible();
	}
}

InGameMenuUI::InGameMenuUI()
	: GameMenuUI()
{
}

InGameMenuUI::~InGameMenuUI()
{
}

bool InGameMenuUI::Initialize(int x, int y)
{
	GameMenuUI::Initialize(x, y);


	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelChange");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->SetRect(data.size.first, data.size.second);
		ui->Visible();
		ui->SetLButtonClickCallback(ChannelChangeMenuClick, "ChannelChangeMenu");
		AddChildUI("ChannelChangeButton", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("Option");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->SetRect(data.size.first, data.size.second);
		ui->Visible();
		ui->SetLButtonClickCallback(GameOptionMenuClick, "GameOptionMenu");
		AddChildUI("GameOptionButton", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("Quit");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->SetRect(data.size.first, data.size.second);
		ui->Visible();
		ui->SetLButtonClickCallback(GameQuitClick, "Quit");
		AddChildUI("QuitButton", ui);
	}

	int width = 0;
	int height = 0;
	for (int i = 0; i < 3; ++i)
	{
		std::string name = "MenuUIBackground" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		width = data.size.first;
		height += data.size.second;
	}

	SetRect(width, height);
	SetPosition(500, 500);

	return true;
}

void InGameMenuUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void InGameMenuUI::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	for (int i = 0; i < 3; ++i)
	{
		std::string name = "MenuUIBackground" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);

		D2D1_RECT_F pos;
		pos.left = mPos.first + data.origin.first;
		pos.top = mPos.second + data.origin.second;
		pos.right = pos.left + data.size.first;
		pos.bottom = pos.top + data.size.second;

		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture(data.name), pos);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("MenuUITitle");
		D2D1_RECT_F pos;
		pos.left = mPos.first + data.origin.first;
		pos.top = mPos.second + data.origin.second;
		pos.right = pos.left + data.size.first;
		pos.bottom = pos.top + data.size.second;

		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture(data.name), pos);
	}

	//GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "하늘색");

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}
