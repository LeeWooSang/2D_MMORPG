#include "Test2.h"
#include "../Resource/ResourceManager.h"
#include "../GameObject/UI/InputUI/InputUI.h"
#include "../GameObject/UI/ButtonUI/ButtonUI.h"
#include "../Layer/LoginLayer/LoginLayer.h"

Test22::~Test22()
{
	for (auto& ui : mUIs)
	{
		delete ui.second;
		ui.second = nullptr;
	}
	mUIs.clear();
}

Test2::~Test2()
{
}

bool Test2::Initialize()
{
	{
		InputUI* mInputUI = new InputUI;
		if (mInputUI->Initialize(0, 0) == false)
		{
			return false;
		}
		mInputUI->SetPosition(261, 328);
		static_cast<InputUI*>(mInputUI)->SetText(L"127.0.0.1");
		Add("InputUI", mInputUI);
	}

	UI* loginUI = new UI;
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginInputBackground");
		if (loginUI->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		loginUI->SetTexture(data.name);
		loginUI->Visible();
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginGameGuide");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginTab0");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginTab1");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("IdInput0");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("IdRadioButton1");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("IdInput1");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("PWInput");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginButton");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginQuitButton");
		UI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		loginUI->AddChildUI(data.name, ui);
	}

	loginUI->SetPosition(234, 259);
	Add("LoginUI", loginUI);

	return true;
}

void Test2::Update()
{
}

void Test2::Render()
{
}

void Test2::processKeyboardMessage()
{
}