#include "GameMenuUI.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Scene/Scene.h"

void ChannelChangeClick(const std::string& name)
{
	std::cout << "ä�� ����" << std::endl;
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
	// �����ִٸ�
	if (mOpen == false)
	{
		mOpen = true;
		Visible();
	}
	// �����ִٸ�
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
	// ���̴� �͸� ����
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
	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "�ϴû�");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
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
