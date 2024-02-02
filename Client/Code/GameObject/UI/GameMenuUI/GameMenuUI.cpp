#include "GameMenuUI.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Scene/Scene.h"

void ChannelChangeClick(const std::string& name)
{
	std::cout << "채널 변경" << std::endl;
}

void GameSettingClick(const std::string& name)
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
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(10, 10) == false)
		{
			return false;
		}
		ui->SetRect(230, 100);
		ui->Visible();
		ui->SetLButtonClickCallback(ChannelChangeClick, "ChannelChange");
		AddChildUI("ChannelChangeButton", ui);
	}
	{
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(10, 120) == false)
		{
			return false;
		}
		ui->SetRect(230, 100);
		ui->Visible();
		ui->SetLButtonClickCallback(GameSettingClick, "Setting");
		AddChildUI("SettingButton", ui);
	}
	{
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(10, 230) == false)
		{
			return false;
		}
		ui->SetRect(230, 100);
		ui->Visible();
		ui->SetLButtonClickCallback(GameQuitClick, "Quit");
		AddChildUI("QuitButton", ui);
	}

	SetRect(250, 340);
	SetPosition(275, 200);

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

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	//GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "하늘색");

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
