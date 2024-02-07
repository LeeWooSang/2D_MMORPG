#include "ServerScene.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"

ServerScene::ServerScene()
	: Scene()
{
}

ServerScene::~ServerScene()
{
}

bool ServerScene::Initialize()
{
	return true;
}

void ServerScene::Update()
{
}

void ServerScene::Render()
{
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture("ServerBackground");
		D2D1_RECT_F pos;
		pos.left = 0;
		pos.top = 0;
		pos.right = pos.left + tex->GetSize().first;
		pos.bottom = pos.top + tex->GetSize().second;
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, pos);

	}
}

void ServerScene::processKeyboardMessage()
{
}
