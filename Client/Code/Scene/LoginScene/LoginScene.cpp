#include "LoginScene.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"

LoginScene::LoginScene()
	: Scene()
{
}

LoginScene::~LoginScene()
{
}

bool LoginScene::Initialize()
{
	mIsReady = true;
	return true;
}

void LoginScene::Update()
{
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
}
