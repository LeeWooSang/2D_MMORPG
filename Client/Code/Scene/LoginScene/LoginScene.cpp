#include "LoginScene.h"
#include "../../Input/Input.h"
#include "../SceneManager.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GameObject/UI/InputUI/InputUI.h"

LoginScene::LoginScene()
	: Scene()
{
	mInputUI = nullptr;
}

LoginScene::~LoginScene()
{
}

bool LoginScene::Initialize()
{
	mIsReady = true;

	mInputUI = new InputUI;
	if (mInputUI->Initialize(0, 0) == false)
	{
		return false;
	}
	mInputUI->SetPosition(261, 328);
	static_cast<InputUI*>(mInputUI)->SetText(L"127.0.0.1");

	return true;
}

void LoginScene::Update()
{
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
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginTab0");
		D2D1_RECT_F rect;
		rect.left = 261;
		rect.top = 296;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginTab1");
		D2D1_RECT_F rect;
		rect.left = 400;
		rect.top = 296;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	}

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
	mInputUI->Render();

	processKeyboardMessage();
}

void LoginScene::processKeyboardMessage()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
	{
		GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::INGAME_SCENE);
	}
}
