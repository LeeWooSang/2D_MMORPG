#include "GameObject.h"
#include "../../../Server/Code/Common/Protocol.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Texture/Texture.h"

Object::Object()
    
{
}

Object::~Object()
{
}


GameObject::GameObject()
    : mPos(std::make_pair(0, 0)), mDir(1)
{
    mAttr = ATTR_STATE_TYPE::DEFAULT;
    mTexture = nullptr;
    mWorldPos = std::make_pair(0.0, 0.0);
    mRect = std::make_pair(0, 0);
}

GameObject::~GameObject()
{
    Reset();
}

bool GameObject::Initialize(int x, int y)
{
    mAttr = ATTR_STATE_TYPE::DEFAULT;
    mPos.first = x;
    mPos.second = y;
    mDir = 1;
    mTexture = nullptr;

    return true;
}

void GameObject::Update()
{
}

void GameObject::Render()
{
}

void GameObject::Reset()
{
    mAttr = ATTR_STATE_TYPE::DEFAULT;
    mPos = std::make_pair(0, 0);
    mDir = 0;
    mTexture = nullptr;
}

bool GameObject::SetTexture(const std::string& name)
{
    Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
    if (tex == nullptr)
    {
        return false;
    }

    mTexture = tex;
    SetRect(mTexture->GetSize().first, mTexture->GetSize().second);

    return true;
}

bool GameObject::CheckDistance(int x, int y)
{
    if (VIEW_DISTANCE < std::abs(mPos.first - x))
        return false;

    if (VIEW_DISTANCE < std::abs(mPos.second - y))
        return false;

    return true;
}

bool GameObject::IsVisible()
{
    // 보이는 것만 렌더
    if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
    {
        return false;
    }

    return true;
}