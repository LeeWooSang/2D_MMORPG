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
	std::cout << "현재 선택한 채널 : " << channel << std::endl;
	ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
	ui->SetSelectChannel(std::stoi(channel));
}

void ChannelChangeClick(const std::string& name)
{
	ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->GetCurScene()->FindUI("ChannelUI"));
	ui->OpenChannelUI();

	int newChannel = ui->GetSelectChannel();
	std::cout << "선택된 새로운 채널 : " << newChannel << std::endl;
#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendChangeChannelPacket(newChannel);
#else
	ui->SetCurrentChannel(newChannel);
#endif // SERVER_CONNECT
}

void ChannelCancelClick(const std::string& name)
{
	ChannelUI* ui = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
	ui->OpenChannelUI();
	ui->ResetSelectChannel();
}

void ChannelLoginClick(const std::string& name)
{
	LoginChannelUI* ui = static_cast<LoginChannelUI*>(GET_INSTANCE(SceneManager)->GetCurScene()->FindUI("ChannelUI"));
	int selectChannel = ui->GetSelectChannel();
#ifdef SERVER_CONNECT
	GET_INSTANCE(Network)->SendChannelLoginPacket(selectChannel);
#else
	ui->OpenChannelUI();
	ui->ResetSelectChannel();
	ui->SetCurrentChannel(selectChannel);
	GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::INGAME_SCENE);
	{
		ChannelUI* channelUI = static_cast<ChannelUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChannelUI"));
		channelUI->SetCurrentChannel(selectChannel);
	}
#endif // SERVER_CONNECT

}

ChannelUI::ChannelUI()
	: UI()
{
	mOpen = false;
	mCurrentChannel = 0;
	mSelectChannel = 0;
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
		ui->SetLButtonClickCallback(ChannelChangeClick, "dd");
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
		ui->SetLButtonClickCallback(ChannelCancelClick, "dd");
		AddChildUI("CancelButton", ui);
	}

	TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("ChannelUISelect");
	int originX = data.origin.first;
	int originY = data.origin.second;

	for (int i = 0; i < MAX_CHANNEL; ++i)
	{
		ChannelUISlot* slot = new ChannelUISlot;
		if (slot->Initialize(originX, originY) == false)
		{
			return false;
		}
		slot->SetChannel(i);
		slot->SetRect(data.size.first, data.size.second);
		slot->SetSlotName("ChannelUICh" + std::to_string(i));
		AddChildUI("Slot", slot);

		if ((i + 1) % 5 == 0)
		{
			originX = data.origin.first;
			originY += 20;
		}
		else
		{
			originX += 70;
		}
	}

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

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	for (auto& child : mRenderChildUIs)
	{
		for (int i = 0; i < child.size(); ++i)
		{
			child[i]->Render();
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

void ChannelUI::SetCurrentChannel(int channel)
{
	std::vector<UI*> slots = FindChildUIs("Slot");

	// 이전의 현재 채널은 기본상태로 변경
	ChannelUISlot* oldSlot = static_cast<ChannelUISlot*>(slots[mCurrentChannel]);
	oldSlot->SetChannelType(CHANNEL_TYPE::NONE);

	mCurrentChannel = channel;
	mSelectChannel = channel;

	ChannelUISlot* slot = static_cast<ChannelUISlot*>(slots[mCurrentChannel]);
	slot->SetChannelType(CHANNEL_TYPE::SELECT_CHANNEL);
}

void ChannelUI::SetSelectChannel(int channel)
{
	if (channel == mCurrentChannel)
	{
		return;
	}

	std::vector<UI*> slots = FindChildUIs("Slot");

	// 이전에 선택한 채널은 기본상태로 변경
	ChannelUISlot* oldSlot = static_cast<ChannelUISlot*>(slots[mSelectChannel]);
	oldSlot->SetChannelType(CHANNEL_TYPE::NONE);

	mSelectChannel = channel;

	ChannelUISlot* currentSlot = static_cast<ChannelUISlot*>(slots[mCurrentChannel]);
	currentSlot->SetChannelType(CHANNEL_TYPE::CURRENT_CHANNEL);
	
	ChannelUISlot* selectSlot = static_cast<ChannelUISlot*>(slots[mSelectChannel]);
	selectSlot->SetChannelType(CHANNEL_TYPE::SELECT_CHANNEL);	
}

void ChannelUI::ResetSelectChannel()
{
	std::vector<UI*> slots = FindChildUIs("Slot");

	if (mSelectChannel != mCurrentChannel)
	{
		std::cout << mSelectChannel << ", " << mCurrentChannel << std::endl;
		// 선택했던 채널은 기본상태로 변경
		ChannelUISlot* slot = static_cast<ChannelUISlot*>(slots[mSelectChannel]);
		slot->SetChannelType(CHANNEL_TYPE::NONE);
	}

	// 현재 채널은 선택 상태로 변경
	ChannelUISlot* slot = static_cast<ChannelUISlot*>(slots[mCurrentChannel]);
	slot->SetChannelType(CHANNEL_TYPE::SELECT_CHANNEL);
	mSelectChannel = mCurrentChannel;
}

LoginChannelUI::LoginChannelUI()
	: ChannelUI()
{
}

LoginChannelUI::~LoginChannelUI()
{
}

bool LoginChannelUI::Initialize(int x, int y)
{
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginChBackground");
		UI::Initialize(x, y);
		SetTexture(data.name);
	}

	{
		mLogoName = "LoginChLogo0";
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(mLogoName);
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("LoginChLogo", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginChButton");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(ChannelLoginClick, "dd");
		AddChildUI("LoginChButton", ui);
	}

	for(int i = 0; i < MAX_CHANNEL; ++i)
	{
		std::string name = "LoginCh" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		LoginChannelUISlot* ui = new LoginChannelUISlot;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetChannel(i);
		ui->SetTexture(data.name);
		AddChildUI("Slot", ui);
	}

	return true;
}

void LoginChannelUI::ChangeLogo(int type)
{
	switch (type)
	{
	case GAME_SERVER_TYPE::SCANIA:		mLogoName = "LoginChLogo0";	break;
	case GAME_SERVER_TYPE::LUNA:			mLogoName = "LoginChLogo1";	break;
	case GAME_SERVER_TYPE::ELYSIUM:	mLogoName = "LoginChLogo2";	break;
	case GAME_SERVER_TYPE::CROA:		mLogoName = "LoginChLogo3";	break;
	default:													mLogoName = "LoginChLogo4";	break;
	}
	std::vector<UI*>& v = FindChildUIs("LoginChLogo");
	v.front()->SetTexture(mLogoName);
}

ChannelUISlot::ChannelUISlot()
	: UI()
{
	mChannel = 0;
	mSlotName.clear();
	mChannelType = CHANNEL_TYPE::NONE;
}

ChannelUISlot::~ChannelUISlot()
{
}

bool ChannelUISlot::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	Visible();

	return true;
}

void ChannelUISlot::Update()
{
	UI::Update();
}

void ChannelUISlot::Render()
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

	if (mChannelType == CHANNEL_TYPE::CURRENT_CHANNEL)
	{
		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture("ChannelUICurrent"), pos);
	}
	else if (mChannelType == CHANNEL_TYPE::SELECT_CHANNEL)
	{
		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture("ChannelUISelect"), pos);
	}

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(mSlotName);
		D2D1_RECT_F numRect;
		numRect.left = mPos.first + data.origin.first;
		numRect.top = mPos.second + data.origin.second;
		numRect.right = numRect.left + data.size.first;
		numRect.bottom = numRect.top + data.size.second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture(mSlotName), numRect);
	}
}

void ChannelUISlot::MouseLButtonClick()
{
	ChannelUI* parent = static_cast<ChannelUI*>(mParentUI);
	parent->SetSelectChannel(mChannel);
}

LoginChannelUISlot::LoginChannelUISlot()
	: ChannelUISlot()
{
	mChannelUserSize = 0;
}

LoginChannelUISlot::~LoginChannelUISlot()
{
}

bool LoginChannelUISlot::Initialize(int x, int y)
{
	ChannelUISlot::Initialize(x, y);
	mChannelUserSize = 0;

	return true;
}

void LoginChannelUISlot::Render()
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

	LoginChannelUI* parent = static_cast<LoginChannelUI*>(mParentUI);
	if (parent->GetSelectChannel() == mChannel)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "주황색");
	}
	else
	{
		if (mMouseLButtonDown)
		{
			GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
		}
		else if (mMouseOver)
		{
			GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
		}
	}
	
	{
		float rate = static_cast<float>(mChannelUserSize) / MAX_CHANNEL_USER;

		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginChGauge");
		D2D1_RECT_F gaugeRect;
		gaugeRect.left = mPos.first + data.origin.first;
		gaugeRect.top = mPos.second + data.origin.second;
		gaugeRect.right = gaugeRect.left + data.size.first * rate;
		gaugeRect.bottom = gaugeRect.top + data.size.second;

		GET_INSTANCE(GraphicEngine)->RenderTexture(GET_INSTANCE(ResourceManager)->FindTexture("LoginChGauge"), gaugeRect);
	}
}

void LoginChannelUISlot::MouseLButtonClick()
{
	LoginChannelUI* parent = static_cast<LoginChannelUI*>(mParentUI);
	parent->SetSelectChannel(mChannel);
}