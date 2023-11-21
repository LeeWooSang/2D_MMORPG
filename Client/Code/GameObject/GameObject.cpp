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
    : mPos(std::make_pair(0, 0)), mDir(0)
{
    mAttr = ATTR_STATE_TYPE::DEFAULT;
    mTexture = nullptr;

    mAnimationState = 0;
    mCurrFrame = 0;
    mFrameSize = 0;
    mCurrAnimation = 0;
    mAnimationCounter = 0;
    mAnimationIndex = 0;
    mAnimationCountMax = 0;
    for (int i = 0; i < MAX_ANIMATIONS; ++i)
    {
        mAnimations[i] = nullptr;
    }
}

GameObject::~GameObject()
{
    for (int i = 0; i < MAX_ANIMATIONS; ++i)
    {
        if (mAnimations[i] != nullptr)
        {
            delete mAnimations[i];
            mAnimations[i] = nullptr;
        }
    }

    Reset();
}

bool GameObject::Initialize(int x, int y)
{
    mAttr = ATTR_STATE_TYPE::DEFAULT;
    mPos.first = x;
    mPos.second = y;
    mDir = 0;
    mTexture = nullptr;

    mAnimationState = 0;
    mCurrFrame = 0;
    mFrameSize = 0;
    mCurrAnimation = 0;
    mAnimationCounter = 0;
    mAnimationIndex = 0;
    mAnimationCountMax = 0;
    for (int i = 0; i < MAX_ANIMATIONS; ++i)
    {
        mAnimations[i] = nullptr;
    }

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

    mAnimationState = 0;
    mCurrFrame = 0;
    mFrameSize = 0;
    mCurrAnimation = 0;
    mAnimationCounter = 0;
    mAnimationIndex = 0;
    mAnimationCountMax = 0;
    for (int i = 0; i < MAX_ANIMATIONS; ++i)
    {
        mAnimations[i] = nullptr;
    }
}

bool GameObject::SetTexture(const std::string& name)
{
    Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
    if (tex == nullptr)
    {
        return false;
    }

    mTexture = tex;
    
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
    // ���̴� �͸� ����
    if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
    {
        return false;
    }

    return true;
}
