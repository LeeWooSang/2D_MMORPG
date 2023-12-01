#include "Character.h"
#include "../../Network/Network.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"
#include "../UI/Inventory/Inventory.h"
#include "../UI/ChattingBox/ChattingBox.h"

#include "../../GameTimer/GameTimer.h"
#include <string>
#include <random>
#include "../UI/UIManager.h"


Character::Character()
    : GameObject()
{
	mId = -1;
	mState = CHARACTER_STATE_TYPE::DEFAULT;

	memset(mMessage, 0, sizeof(mMessage));
	mMessageTime = 0;
}

Character::~Character()
{
}

bool Character::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	mState = CHARACTER_STATE_TYPE::LIVE;
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

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();

	// 이미지 위치
	D2D1_RECT_F pos;
	pos.left = (mPos.first - cameraPos.first) * 65.0 + 8.0;
	pos.top = (mPos.second - cameraPos.second) * 65.0 + 8.0;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;
	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

	std::wstring text = L"Id (" + std::to_wstring(mId) + L")";
	GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top), "메이플", "검은색");
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
	mChattingBox = nullptr;
	mElapsedTime = 0.0;
	mFlag = false;
}

Player::~Player()
{
}

bool Player::Initialize(int x, int y)
{
	Character::Initialize(x, y);

	if (SetTexture("Player") == false)
	{
		return false;
	}

	// 부모 ui
	{
		UI* inventory = new Inventory;
		if (inventory->Initialize(0, 0) == false)
		{
			return false;
		}
		if (inventory->SetTexture("Inventory") == false)
		{
			return false;
		}

		GET_INSTANCE(UIManager)->AddUI("Inventory", inventory);
	}

	// 부모 ui
	{
		mChattingBox = new ChattingBox;
		if (mChattingBox->Initialize(0, 0) == false)
		{
			return false;
		}
		GET_INSTANCE(UIManager)->AddUI("ChattingBox", mChattingBox);
	}

	return true;
}

void Player::Update()
{
	ProcessKeyboardMessage();
	GET_INSTANCE(Camera)->SetPosition(mPos.first, mPos.second);

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::CONTROL_KEY) == true)
	{
		mFlag = true;
#ifdef SERVER_CONNECT
		GET_INSTANCE(Network)->SendAttackPacket();
#endif // SERVER_CONNECT
	}

	if (mFlag == true)
	{
		mElapsedTime += GET_INSTANCE(GameTimer)->GetElapsedTime();
	}

	if (mElapsedTime > 2.0)
	{
		mElapsedTime = 0.0;
		mFlag = false;
	}
}

void Player::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();

	//if (mMessageTime > GetTickCount() - 2000)
	//{
	//	GET_INSTANCE(GraphicEngine)->RenderText(mMessage, static_cast<int>(pos.x), static_cast<int>(pos.y), D3DCOLOR_ARGB(255, 200, 200, 255));
	//}
	//{
	//	int windowHeight = 800;
	//	std::wstring text = L"MY POSITION (" + std::to_wstring(mPos.first) + L", " + std::to_wstring(mPos.second) + L")";
	//	GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), 10, windowHeight - 64, D3DCOLOR_ARGB(255, 255, 255, 255));
	//}

	{
		// 이미지 위치
		D2D1_RECT_F pos;
		pos.left = (mPos.first - cameraPos.first) * 65.0 + 8.0;
		pos.top = (mPos.second - cameraPos.second) * 65.0 + 8.0;
		pos.right = pos.left + mTexture->GetSize().first;
		pos.bottom = pos.top + mTexture->GetSize().second;

		//GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "흰색");
		GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);

		std::wstring text = L"MyId (" + std::to_wstring(mId) + L")";
		GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top), "메이플", "검은색");

		text = L"MY POSITION (" + std::to_wstring(mPos.first) + L", " + std::to_wstring(mPos.second) + L")";
		GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), 0, 0, "메이플", "파란색");

		if (mFlag == true)
		{
			GET_INSTANCE(GraphicEngine)->RenderText(L"ATTACK!!!", 
				static_cast<int>(pos.left), static_cast<int>(pos.top - 30), "메이플", "빨간색");
		}
	}
}

void Player::ProcessKeyboardMessage()
{

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::I_KEY) == true
		&& mChattingBox->GetIsOpen() == false)
	{
		static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"))->OpenInventory();
	}

	if (mChattingBox->GetIsOpen() == true)
	{
		return;
	}

#ifdef SERVER_CONNECT
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::LEFT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::RIGHT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::UP_KEY) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::UP);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::DOWN_KEY) == true)
	{
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::DOWN);
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::F1_KEY) == true)
	{
		GET_INSTANCE(Network)->SendChangeChannelPacket(0);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::F2_KEY) == true)
	{
		GET_INSTANCE(Network)->SendChangeChannelPacket(1);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::F3_KEY) == true)
	{
		GET_INSTANCE(Network)->SendChangeChannelPacket(2);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::F4_KEY) == true)
	{
		GET_INSTANCE(Network)->SendChangeChannelPacket(3);
	}
#else
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
	{
		Move(DIRECTION_TYPE::LEFT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
	{
		Move(DIRECTION_TYPE::RIGHT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::UP_KEY) == true)
	{
		Move(DIRECTION_TYPE::UP);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::DOWN_KEY) == true)
	{
		Move(DIRECTION_TYPE::DOWN);
	}
#endif
}

void Player::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"))->ProcessMouseWheelEvent(wParam);
}

void Player::Move(char dir)
{
	int x = mPos.first;
	int y = mPos.second;
	switch (dir + 1)
	{
	case KEY_TYPE::UP_KEY:
	{
		--y;
		break;
	}
	case KEY_TYPE::DOWN_KEY:
	{
		++y;
		break;
	}

	case KEY_TYPE::LEFT_KEY:
	{
		--x;
		break;
	}
	case KEY_TYPE::RIGHT_KEY:
	{
		++x;
		break;
	}

	default:
		break;
	}

	if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT)
	{		
		mPos.first = x;
		mPos.second = y;
	}
}

void Player::AddItem()
{
	std::string itemName = "";

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, 3);
	switch (uid(dre))
	{
	case 0:
		itemName = "Ax";
		break;

	case 1:
		itemName = "Sword";
		break;

	case 2:
		itemName = "Dagger";
		break;

	default:
		itemName = "Club";
		break;
	}
	
	static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"))->AddItem(itemName);
}
