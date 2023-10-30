#include "Character.h"
#include "../../Network/Network.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"
#include <string>

Character::Character()
    : GameObject()
{
	mId = -1;
	mState = CHARACTER_STATE_TYPE::DEFAULT;
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

	memset(mMessage, 0, sizeof(mMessage));
	mMessageTime = 0;
}

Character::~Character()
{
	for (int i = 0; i < MAX_ANIMATIONS; ++i)
	{
		if (mAnimations[i] != nullptr)
		{
			delete mAnimations[i];
			mAnimations[i] = nullptr;
		}
	}
}

bool Character::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	mState = CHARACTER_STATE_TYPE::LIVE;

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

	memset(mMessage, 0, sizeof(mMessage));
	mMessageTime = 0;

    return true;
}

void Character::Update()
{	
}

void Character::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	RECT src;
	src.left = mTexture->GetPos(mCurrFrame).first;
	src.top = mTexture->GetPos(mCurrFrame).second;
	src.right = mTexture->GetPos(mCurrFrame).first + mTexture->GetSize().first;
	src.bottom = mTexture->GetPos(mCurrFrame).second + mTexture->GetSize().second;
	
	//D3DXVECTOR3 pos = D3DXVECTOR3((mPos.first - g_left_x) * 65.0f + 8, (mPos.second - g_top_y) * 65.0f + 8, 0.0);
	//D3DXVECTOR3 pos = D3DXVECTOR3(mPos.first * 65.0f + 8, mPos.second * 65.0f + 8, 0.0);
	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	D3DXVECTOR3 pos = D3DXVECTOR3((mPos.first - cameraPos.first) * 65.0f + 8, (mPos.second - cameraPos.second) * 65.0f + 8, 0.0);
	GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	if (mMessageTime > GetTickCount() - 2000)
	{
		GET_INSTANCE(GraphicEngine)->RenderText(mMessage, static_cast<int>(pos.x), static_cast<int>(pos.y), D3DCOLOR_ARGB(255, 200, 200, 255));
	}
}

void Character::Reset()
{
	mId = -1;
	mState = CHARACTER_STATE_TYPE::DEFAULT;
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

	memset(mMessage, 0, sizeof(mMessage));
	mMessageTime = 0;
}

void Character::SetAnimationInfo(int frameSize)
{
	mFrameSize = frameSize;
}

Player::Player()
	: Character()
{
}

Player::~Player()
{
}

bool Player::Initialize(int x, int y)
{
	Character::Initialize(x, y);

	return true;
}

void Player::Update()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::KEYBOARD_LEFT) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::LEFT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::KEYBOARD_RIGHT) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::RIGHT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::KEYBOARD_UP) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::UP);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::KEYBOARD_DOWN) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::DOWN);
	}
}

void Player::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	RECT src;
	src.left = mTexture->GetPos(mCurrFrame).first;
	src.top = mTexture->GetPos(mCurrFrame).second;
	src.right = mTexture->GetPos(mCurrFrame).first + mTexture->GetSize().first;
	src.bottom = mTexture->GetPos(mCurrFrame).second + mTexture->GetSize().second;

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();

	D3DXVECTOR3 pos = D3DXVECTOR3((mPos.first - cameraPos.first) * 65.0f + 8, (mPos.second - cameraPos.second) * 65.0f + 8, 0.0);
	GET_INSTANCE(GraphicEngine)->GetSprite()->Draw(mTexture->GetBuffer(), &src, NULL, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));

	if (mMessageTime > GetTickCount() - 2000)
	{
		GET_INSTANCE(GraphicEngine)->RenderText(mMessage, static_cast<int>(pos.x), static_cast<int>(pos.y), D3DCOLOR_ARGB(255, 200, 200, 255));
	}

	int windowHeight = 800;
	std::wstring text = L"MY POSITION (" + std::to_wstring(mPos.first) + L", " + std::to_wstring(mPos.second) + L")";
	GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), 10, windowHeight - 64, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void Player::Move(char dir)
{
	int x = mPos.first;
	int y = mPos.second;
	switch (dir)
	{
	case KEY_TYPE::KEYBOARD_UP:
	{
		++y;
		break;
	}
	case KEY_TYPE::KEYBOARD_DOWN:
	{
		--y;
		break;
	}

	case KEY_TYPE::KEYBOARD_LEFT:
	{
		--x;
		break;
	}
	case KEY_TYPE::KEYBOARD_RIGHT:
	{
		++x;
		break;
	}

	default:
		break;
	}

	if (x <= 999 && x >= 0 && y <= 999 && y >= 0)
	{
		mPos.first = x;
		mPos.second = y;
	}
	std::cout << mPos.first << ", " << mPos.second << std::endl;
}