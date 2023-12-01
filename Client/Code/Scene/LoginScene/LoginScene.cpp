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
	
	static_cast<InputUI*>(mInputUI)->SetText(L"127.0.0.1");

	return true;
}

void LoginScene::Update()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::ENTER_KEY) == true)
	{
		GET_INSTANCE(SceneManager)->ChangeScene(SCENE_TYPE::INGAME_SCENE);
	}

	mInputUI->Update();
}

void LoginScene::Render()
{
	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("LoginScene");

	D2D1_RECT_F rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + tex->GetSize().first;
	rect.bottom = rect.top + tex->GetSize().second;
	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);

	mInputUI->Render();
}
