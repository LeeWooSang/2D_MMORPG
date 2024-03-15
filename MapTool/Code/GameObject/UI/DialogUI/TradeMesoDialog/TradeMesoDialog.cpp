#include "TradeMesoDialog.h"
#include "../../../../Network/Network.h"

#include "../../ButtonUI/ButtonUI.h"
#include "../../InputUI/InputUI.h"
#include "../../../../Resource/ResourceManager.h"
#include "../../../../Resource/Texture/Texture.h"
#include "../../../../GraphicEngine/GraphicEngine.h"
#include "../../../../Input/Input.h"

#include "../../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../../Scene/InGameScene/InGameScene.h"
#include "../../TradeUI/TradeUI.h"
#include "../../Inventory/Inventory.h"

void OkButtonClick(const std::string& name)
{
	// 1. 다이얼로그 UI가 꺼진다.
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	DialogUI* dialog = trade->GetMesoDialogUI();
	dialog->OpenDialogUI();

	// 2. 교환창에 나의 메소가 등록된다.
	std::vector<UI*>& v = dialog->FindChildUIs("Input");
	InputUI* input = static_cast<InputUI*>(v.front());
	std::wstring addedMesoStr = input->GetText();
	long long addedMeso = 0;
	if (addedMesoStr != L"")
	{
		addedMeso = _wtoll(addedMesoStr.c_str());
	}
	trade->AddMyMeso(addedMeso);
	// 입력창 초기화
	input->SetText(L"");
	input->SetCarrotPos();

	// 3. 인벤토리에 있는 나의 메소는 올린만큼 깍인다.
	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	inventory->DeductMeso(addedMeso);

#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendAddTradeMeso(trade->GetTradeUserId(), addedMeso);
#endif // SERVER_CONNECT
}

void CancelButtonClick(const std::string& name)
{
	// 1. 다이얼로그 UI가 꺼진다.
	TradeUI* trade = static_cast<TradeUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("TradeUI"));
	DialogUI* dialog = trade->GetMesoDialogUI();
	dialog->OpenDialogUI();

	// 2. 교환창에 나의 메소가 초기화
	std::vector<UI*>& v = dialog->FindChildUIs("Input");
	InputUI* input = static_cast<InputUI*>(v.front());
	input->SetText(L"");
	input->SetCarrotPos();
}

TradeMesoDialog::TradeMesoDialog()
	: DialogUI()
{
}

TradeMesoDialog::~TradeMesoDialog()
{
}

bool TradeMesoDialog::Initialize(int x, int y)
{
	DialogUI::Initialize(x, y);

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("DialogUIBackground0");
		SetTexture(data.name);
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
	
	InputUI* ui = new InputUI;
	if (ui->Initialize(0, 20) == false)
	{
		return false;
	}
	ui->SetRect(230, 30);
	ui->Visible();
	AddChildUI("Input", ui);
	
	SetPosition(300, 300);
	ui->SetCarrotPos();

	return true;
}

void TradeMesoDialog::Update()
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

void TradeMesoDialog::Render()
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
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderText(L"올릴 메소를 입력해주세요.", mPos.first, mPos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void TradeMesoDialog::MouseOver()
{
}

void TradeMesoDialog::MouseLButtonDown()
{
}

void TradeMesoDialog::MouseLButtonUp()
{
}

void TradeMesoDialog::MouseLButtonClick()
{
}
