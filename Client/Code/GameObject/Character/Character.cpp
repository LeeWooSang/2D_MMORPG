#include "Character.h"
#include "../../Network/Network.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"
#include "../UI/Inventory/Inventory.h"
#include "../../GameTimer/GameTimer.h"
#include <string>
#include <random>
#include "../UI/UIManager.h"
#include "../../Animation/Animation.h"

Character::Character()
    : GameObject()
{
	mId = -1;
	mState = CHARACTER_STATE_TYPE::DEFAULT;
}

Character::~Character()
{
}

bool Character::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	mState = CHARACTER_STATE_TYPE::LIVE;

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
}

void Character::SetAnimationInfo(int frameSize)
{
	mFrameSize = frameSize;
}

Player::Player()
	: Character()
{
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

	return true;
}

void Player::Update()
{
	GET_INSTANCE(Camera)->SetPosition(mPos.first, mPos.second);

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
			std::vector<std::pair<int, int>> dir{ {-1, 1}, {0, 1}, {1, 1}, {-1, 0}, {1, 0}, {-1, -1}, {0, -1}, {1, -1} };

			for (int i = 0; i < dir.size(); ++i)
			{
				int attackX = mPos.first + dir[i].first;
				int attackY = mPos.second + dir[i].second;
				D2D1_RECT_F attackPos;
				//attackPos.left = (attackX - cameraPos.first) * 65.0 + 8.0;
				//attackPos.top = (attackY - cameraPos.second) * 65.0 + 8.0;
				attackPos.left = (attackX - cameraPos.first) * 65.0 + 8.0;
				attackPos.top = (attackY - cameraPos.second) * 65.0 + 8.0;
				attackPos.right = attackPos.left + mTexture->GetSize().first;
				attackPos.bottom = attackPos.top + mTexture->GetSize().second;

				GET_INSTANCE(GraphicEngine)->RenderFillRectangle(attackPos, "주황색");
				GET_INSTANCE(GraphicEngine)->RenderRectangle(attackPos, "검은색");
			}
			GET_INSTANCE(GraphicEngine)->RenderText(L"ATTACK!!!",
				static_cast<int>(pos.left - 15), static_cast<int>(pos.top - 30), "메이플", "빨간색");
		}
	}
}

void Player::ProcessKeyboardMessage()
{
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::I_KEY) == true)
	{
		static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"))->OpenInventory();
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::Z_KEY) == true)
	{
		AddItem();
	}

	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::CONTROL_KEY) == true)
	{
		mFlag = true;
#ifdef SERVER_CONNECT
		GET_INSTANCE(Network)->SendAttackPacket();
#endif
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
	Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"));
	if (inventory != nullptr && inventory->IsVisible() == true)
	{
		inventory->ProcessMouseWheelEvent(wParam);
	}
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

AnimationCharacter::AnimationCharacter()
	: GameObject()
{
	mParent = nullptr;
	mOriginX = 0;
	mOriginY = 0;
	mChildObjects.clear();

	mMotion = ANIMATION_MONTION_TYPE::IDLE;
	mAnimations.clear();
}

AnimationCharacter::~AnimationCharacter()
{
	mRenderChildObjects.clear();
	for (auto& obj : mChildObjects)
	{
		if (obj.second != nullptr)
		{
			delete obj.second;
		}
	}
	mChildObjects.clear();

	for (auto& ani : mAnimations)
	{
		delete ani.second;
	}
	mAnimations.clear();
}

bool AnimationCharacter::Initialize(int x, int y)
{
	mOriginX = x;
	mOriginY = y;

	GameObject::Initialize(x, y);

	AnimationCharacter* head = nullptr;
	AnimationCharacter* body = nullptr;
	AnimationCharacter* arm = nullptr;
	AnimationCharacter* weapon = nullptr;

	{
		head = new AnimationCharacter;
		head->SetOriginPos(0, 0);
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("FrontHead0");
			head->AddAnimation(ANIMATION_MONTION_TYPE::IDLE, ani);
			head->AddAnimation(ANIMATION_MONTION_TYPE::WALK, ani);
		}
	}

	{
		body = new AnimationCharacter;
		body->SetOriginPos(5, 34);
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("IdleBody0");
			ani->SetTexture("IdleBody1");
			ani->SetTexture("IdleBody2");
			body->AddAnimation(ANIMATION_MONTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("WalkBody0");
			ani->SetTexture("WalkBody1");
			ani->SetTexture("WalkBody2");
			ani->SetTexture("WalkBody3");
			body->AddAnimation(ANIMATION_MONTION_TYPE::WALK, ani);
		}
	}

	{
		arm = new AnimationCharacter;
		arm->SetOriginPos(21, 35);
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("IdleArm0");
			ani->SetTexture("IdleArm1");
			ani->SetTexture("IdleArm2");
			arm->AddAnimation(ANIMATION_MONTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("WalkArm0");
			ani->SetTexture("WalkArm1");
			ani->SetTexture("WalkArm2");
			ani->SetTexture("WalkArm3");
			arm->AddAnimation(ANIMATION_MONTION_TYPE::WALK, ani);
		}
	}

	{
		weapon = new AnimationCharacter;
		weapon->SetOriginPos(-8, 48);
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("IdleWeapon0");
			weapon->AddAnimation(ANIMATION_MONTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("WalkWeapon0");
			ani->SetTexture("WalkWeapon1");
			ani->SetTexture("WalkWeapon2");
			ani->SetTexture("WalkWeapon3");
			weapon->AddAnimation(ANIMATION_MONTION_TYPE::WALK, ani);
		}

	}

	AddChild("body", body);
	AddChild("head", head);
	AddChild("weapon", weapon);
	AddChild("arm", arm);

	Visible();
	for (auto& obj : mChildObjects)
	{
		obj.second->Visible();
	}

	//mMotion = ANIMATION_MONTION_TYPE::IDLE;
	SetAnimationMotion(ANIMATION_MONTION_TYPE::WALK);

	return true;
}

void AnimationCharacter::Update()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mAnimations.size() > 0)
	{
		mAnimations[mMotion]->Update();
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->Update();
	}
}

void AnimationCharacter::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mParent != nullptr)
	{
		Texture* tex = mAnimations[mMotion]->GetTexture();
		D2D1_RECT_F pos;
		pos.left = mPos.first;
		pos.top = mPos.second;
		pos.right = pos.left + tex->GetSize().first;
		pos.bottom = pos.top + tex->GetSize().second;
		
		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, pos);
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->Render();
	}
}

void AnimationCharacter::SetPosition(int x, int y)
{	
	// 최상위 부모UI 라면,
	if (mParent == nullptr)
	{
		mPos.first = mOriginX + x;
		mPos.second = mOriginY + y;
	}
	else
	{
		std::pair<int, int> pos = mParent->mPos;
		mPos.first = mOriginX + pos.first;
		mPos.second = mOriginY + pos.second;
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->SetPosition(x, y);
	}
}

void AnimationCharacter::AddChild(const std::string& name, AnimationCharacter* obj)
{
	mChildObjects.emplace(name, obj);
	mRenderChildObjects.emplace_back(obj);

	obj->mParent = this;
}

void AnimationCharacter::SetAnimationMotion(ANIMATION_MONTION_TYPE motion)
{
	mMotion = motion;

	for (auto& obj : mRenderChildObjects)
	{
		obj->SetAnimationMotion(motion);
	}
}