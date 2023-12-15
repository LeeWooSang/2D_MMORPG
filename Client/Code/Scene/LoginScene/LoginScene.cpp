#include "LoginScene.h"
#include "../../Layer/LoginLayer/LoginLayer.h"
#include "../../GameObject/UI/InputUI/InputUI.h"
#include "../../Input/Input.h"
#include "../SceneManager.h"

LoginScene::LoginScene()
	: Scene()
{
	mInputUI = nullptr;
	mLayer = nullptr;
}

LoginScene::~LoginScene()
{
	delete mLayer;
}

bool LoginScene::Initialize()
{
	mIsReady = true;

	mLayer = new LoginLayer;
	if (mLayer->Initialize() == false)
	{
		return false;
	}

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
	mLayer->Update();
	mInputUI->Update();
}

void LoginScene::Render()
{
	mLayer->Render();
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
