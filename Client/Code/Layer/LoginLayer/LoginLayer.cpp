#include "LoginLayer.h"

#include "../../Scene/Scene.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/UI/UIManager.h"
#include "../../GameObject/UI/UI.h"
#include "../../GameObject/UI/ButtonUI/ButtonUI.h"

#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

LoginLayer::LoginLayer()
{
}

LoginLayer::~LoginLayer()
{
}

bool LoginLayer::Initialize()
{
	UI* loginUI = new UI;
	{
		if (loginUI->Initialize(0, 0) == false)
		{
			return false;
		}
		loginUI->SetTexture("LoginInputBackground");
		loginUI->Visible();
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(485, -259) == false)
		{
			return false;
		}
		ui->SetTexture("LoginGameGuide");
		ui->Visible();
		loginUI->AddChildUI("LoginGameGuide", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(27, 37) == false)
		{
			return false;
		}
		ui->SetTexture("LoginTab0");
		ui->Visible();
		loginUI->AddChildUI("LoginTab0", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(166, 37) == false)
		{
			return false;
		}
		ui->SetTexture("LoginTab1");
		ui->Visible();
		loginUI->AddChildUI("LoginTab1", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(27, 69) == false)
		{
			return false;
		}
		ui->SetTexture("IdInput0");
		ui->Visible();
		loginUI->AddChildUI("IdInput0", ui);
	}
	{
		UI* ui = new ButtonUI;
		if (ui->Initialize(228, 69) == false)
		{
			return false;
		}
		ui->SetTexture("IdRadioButton1");
		ui->Visible();
		loginUI->AddChildUI("IdRadioButton1", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(250, 69) == false)
		{
			return false;
		}
		ui->SetTexture("IdInput1");
		ui->Visible();
		loginUI->AddChildUI("IdInput1", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(27, 115) == false)
		{
			return false;
		}
		ui->SetTexture("PWInput");
		ui->Visible();
		loginUI->AddChildUI("PWInput", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(27, 115) == false)
		{
			return false;
		}
		ui->SetTexture("PWInput");
		ui->Visible();
		loginUI->AddChildUI("PWInput", ui);
	}
	{
		UI* ui = new ButtonUI;
		if (ui->Initialize(27, 171) == false)
		{
			return false;
		}
		ui->SetTexture("LoginButton");
		ui->Visible();
		loginUI->AddChildUI("LoginButton", ui);
	}
	{
		UI* ui = new ButtonUI;
		if (ui->Initialize(266, 231) == false)
		{
			return false;
		}
		ui->SetTexture("LoginQuitButton");
		ui->Visible();
		loginUI->AddChildUI("LoginQuitButton", ui);
	}

	loginUI->SetPosition(234, 259);
	GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::LOGIN_SCENE)->AddSceneUI("LoginUI", loginUI);

	return true;
}

void LoginLayer::Update()
{
	GET_INSTANCE(UIManager)->Update();
}

void LoginLayer::Render()
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginBackground");
		D2D1_RECT_F rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	GET_INSTANCE(UIManager)->Render();
}
