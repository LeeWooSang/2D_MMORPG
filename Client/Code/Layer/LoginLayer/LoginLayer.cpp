#include "LoginLayer.h"

#include "../../Scene/Scene.h"
#include "../../Scene/SceneManager.h"
#include "../../GameObject/UI/UIManager.h"
#include "../../GameObject/UI/UI.h"

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
	}

	{
		UI* ui = new UI;
		if (ui->Initialize(0, 0) == false)
		{
			return false;
		}
		ui->SetTexture("LoginTab0");
		loginUI->AddChildUI("LoginTab0", ui);
	}
	{
		UI* ui = new UI;
		if (ui->Initialize(0, 0) == false)
		{
			return false;
		}
		ui->SetTexture("LoginTab1");
		loginUI->AddChildUI("LoginTab1", ui);
	}

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
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginGameGuide");
		D2D1_RECT_F rect;
		rect.left = 719;
		rect.top = 0;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginInputBackground");
		D2D1_RECT_F rect;
		rect.left = 234;
		rect.top = 259;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	//{
	//	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginTab0");
	//	D2D1_RECT_F rect;
	//	rect.left = 261;
	//	rect.top = 296;
	//	rect.right = rect.left + tex->GetSize().first;
	//	rect.bottom = rect.top + tex->GetSize().second;
	//	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	//}
	//{
	//	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginTab1");
	//	D2D1_RECT_F rect;
	//	rect.left = 400;
	//	rect.top = 296;
	//	rect.right = rect.left + tex->GetSize().first;
	//	rect.bottom = rect.top + tex->GetSize().second;
	//	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	//}

	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdInput0");
		D2D1_RECT_F rect;
		rect.left = 261;
		rect.top = 328;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdRadioButton1");
		D2D1_RECT_F rect;
		rect.left = 462;
		rect.top = 328;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("IdInput1");
		D2D1_RECT_F rect;
		rect.left = 484;
		rect.top = 328;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("PWInput");
		D2D1_RECT_F rect;
		rect.left = 261;
		rect.top = 374;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginButton");
		D2D1_RECT_F rect;
		rect.left = 261;
		rect.top = 430;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginQuitButton");
		D2D1_RECT_F rect;
		rect.left = 500;
		rect.top = 490;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

	GET_INSTANCE(UIManager)->Render();
}
