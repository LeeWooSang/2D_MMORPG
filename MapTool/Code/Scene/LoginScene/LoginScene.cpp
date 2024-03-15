#include "LoginScene.h"

#include "../../Core/Core.h"

#include "../../Manager/UIManager/UIManager.h"
#include "../../GameObject/UI/UI.h"
#include "../../GameObject/UI/InputUI/InputUI.h"
#include "../../GameObject/UI/ButtonUI/ButtonUI.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"
#include "../../GraphicEngine/GraphicEngine.h"

#include "../../Manager/SceneMangaer/SceneManager.h"

void LoginClick(const std::string& name)
{
	GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::SERVER_SCENE);
}

LoginScene::LoginScene()
	: Scene()
{
	mInputUI = nullptr;
}

LoginScene::~LoginScene()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
	mUIsMap.clear();

	delete mInputUI;
}

bool LoginScene::Initialize()
{
	mIsReady = true;

	mInputUI = new InputUI;
	if (mInputUI->Initialize(0, 0) == false)
	{
		return false;
	}
	mInputUI->SetRect(201, 45);
	mInputUI->SetPosition(261, 328);
	static_cast<InputUI*>(mInputUI)->SetText(L"127.0.0.1");

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
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(LoginClick, "Login");
		loginUI->AddChildUI(data.name, ui);
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("LoginQuitButton");
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		ui->SetLButtonClickCallback(GameQuitClick, "Quit");
		loginUI->AddChildUI(data.name, ui);
	}

	loginUI->SetPosition(234, 259);
	AddSceneUI("LoginUI", loginUI);

	return true;
}

void LoginScene::Update()
{
	GET_INSTANCE(UIManager)->Update();
	mInputUI->Update();
}

void LoginScene::Render()
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
	mInputUI->Render();

	processKeyboardMessage();
}

void LoginScene::processKeyboardMessage()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
	{
		GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::SERVER_SCENE);
	}
}
