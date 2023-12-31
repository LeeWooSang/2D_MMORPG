#include "LoginLayer.h"
#include "../../Scene/Scene.h"
#include "../../Manager/SceneMangaer/SceneManager.h"
#include "../../Manager/UIManager/UIManager.h"
#include "../../GameObject/UI/UI.h"
#include "../../GameObject/UI/ButtonUI/ButtonUI.h"

#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

LoginLayer::LoginLayer()
{
	mUI = nullptr;
}

LoginLayer::~LoginLayer()
{
	if (mUI != nullptr)
	{
		delete mUI;
	}
}

bool LoginLayer::Initialize()
{
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
	//GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::LOGIN_SCENE)->AddSceneUI("LoginUI", loginUI);
	mUI = loginUI;

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
