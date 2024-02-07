#include "ChannelUI.h"
#include "../../../Network/Network.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"

void ChannelClick(const std::string& channel)
{
	std::cout << channel << "채널 버튼" << std::endl;
#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendChangeChannelPacket(std::stoi(channel));
#endif // SERVER_CONNECT
	{
		ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
		ui->OpenChannelUI();
	}
}

ChannelUI::ChannelUI()
	: UI()
{
	mOpen = false;
}

ChannelUI::~ChannelUI()
{
}

bool ChannelUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	{
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(195, 10) == false)
		{
			return false;
		}
		ui->SetRect(200, 60);
		ui->Visible();
		//ui->SetLButtonClickCallback(ChannelClick, std::to_string(channel++));
		AddChildUI("SelectButton", ui);
	}

	{
		int originX = 0;
		int originY = 75;
		int gap = 5;

		int channel = 0;
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 5; ++j)
			{
				ButtonUI* ui = new ButtonUI;
				if (ui->Initialize(originX + gap, originY + gap) == false)
				{
					return false;
				}
				ui->SetRect(74, 30);
				ui->Visible();
				ui->SetLButtonClickCallback(ChannelClick, std::to_string(channel++));
				AddChildUI("Channel", ui);

				originX += (74 + gap);
			}
			originX = 0;
			originY += (30 + gap);
		}
	}

	SetRect(400, 290);
	SetPosition(200, 200);

	return true;
}

void ChannelUI::Update()
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

void ChannelUI::Render()
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

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "하늘색");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderText(L"스카니아", mPos.first + 20, mPos.second + 25, "메이플", "검은색");

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ChannelUI::OpenChannelUI()
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

void ChannelUI::MouseOver()
{
	UI::MouseOver();
}

void ChannelUI::MouseLButtonDown()
{
}

void ChannelUI::MouseLButtonUp()
{
}

void ChannelUI::MouseLButtonClick()
{
}
