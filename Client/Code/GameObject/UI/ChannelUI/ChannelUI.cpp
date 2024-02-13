#include "ChannelUI.h"
#include "../../../Network/Network.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../Input/Input.h"
#include "../../../Resource/ResourceManager.h"

#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"

void ChannelClick(const std::string& channel)
{
	std::cout << channel << "ä�� ��ư" << std::endl;
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
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUIBackground0");
		UI::Initialize(x, y);		
		SetTexture(data.name);
		//SetRect(data.size.first, data.size.second);
	}

	for(int i = 1; i < 3; ++i)
	{
		std::string name = "ChannelUIBackground" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		UI* ui = new UI;
		ui->Initialize(data.origin.first, data.origin.second);
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Background", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUILogo0");
		UI* ui = new UI;
		ui->Initialize(data.origin.first, data.origin.second);
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Logo", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUIButton0");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("SelectButton", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUIButton1");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("CancelButton", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUISelect0");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("SelectedChannel", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUISelect1");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("CurrentChannel0", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUICh0");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		//ui->SetRect(74, 30);
		ui->Visible();
		ui->SetLButtonClickCallback(ChannelClick, "0");
		AddChildUI("Channel", ui);
	}

	//{
	//	int originX = 0;
	//	int originY = 75;
	//	int gap = 5;

	//	int channel = 0;
	//	for (int i = 0; i < 6; ++i)
	//	{
	//		for (int j = 0; j < 5; ++j)
	//		{
	//			ButtonUI* ui = new ButtonUI;
	//			if (ui->Initialize(originX + gap, originY + gap) == false)
	//			{
	//				return false;
	//			}
	//			ui->SetRect(74, 30);
	//			ui->Visible();
	//			ui->SetLButtonClickCallback(ChannelClick, std::to_string(channel++));
	//			AddChildUI("Channel", ui);

	//			originX += (74 + gap);
	//		}
	//		originX = 0;
	//		originY += (30 + gap);
	//	}
	//}

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

	//GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "�ϴû�");
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderText(L"��ī�Ͼ�", mPos.first + 20, mPos.second + 25, "������", "������");

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
