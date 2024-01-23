#include "DialogUI.h"
#include "../ButtonUI/ButtonUI.h"
#include "../InputUI/InputUI.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Input/Input.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"
#include "../TradeUI/TradeUI.h"
#include "../Inventory/Inventory.h"

void OkButtonClick(const std::string& name)
{
	// 1. ���̾�α� UI�� ������.
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	DialogUI* dialog = trade->GetMesoDialogUI();
	dialog->OpenDialogUI();

	// 2. ��ȯâ�� ���� �޼Ұ� ��ϵȴ�.
	std::vector<UI*>& v = dialog->FindChildUIs("Input");
	InputUI* input = static_cast<InputUI*>(v.front());
	long long addedMeso = _wtoll(input->GetText().c_str());
	trade->AddMyMeso(addedMeso);

	// 3. �κ��丮�� �ִ� ���� �޼Ҵ� �ø���ŭ ���δ�.
	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	inventory->DeductMeso(addedMeso);
}

void CancelButtonClick(const std::string& name)
{
	// 1. ���̾�α� UI�� ������.
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	DialogUI* dialog = trade->GetMesoDialogUI();
	dialog->OpenDialogUI();
}

DialogUI::DialogUI()
	: UI()
{
	mOpen = false;
}

DialogUI::~DialogUI()
{
}

bool DialogUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("DialogUIBackground0");
		SetTexture(data.name);
	}
	{
		InputUI* ui = new InputUI;
		if (ui->Initialize(0, 20) == false)
		{
			return false;
		}
		ui->Visible();
		AddChildUI("Input", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("DialogUIButton0");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(OkButtonClick);
		AddChildUI("Button", ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("DialogUIButton1");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(CancelButtonClick);
		AddChildUI("Button", ui);
	}

	SetPosition(300, 300);

	return true;
}

void DialogUI::Update()
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

void DialogUI::Render()
{
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderText(L"�ø� �޼Ҹ� �Է����ּ���.", mPos.first, mPos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}

}

void DialogUI::MouseOver()
{
}

void DialogUI::MouseLButtonDown()
{
}

void DialogUI::MouseLButtonUp()
{
}

void DialogUI::MouseLButtonClick()
{
}

void DialogUI::OpenDialogUI()
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

void DialogUI::Visible()
{
	mAttr |= ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Visible();
		}
	}
}

void DialogUI::NotVisible()
{
	mAttr &= ~ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->NotVisible();
		}
	}
}
