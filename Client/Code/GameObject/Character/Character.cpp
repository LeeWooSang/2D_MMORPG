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

AnimationCharacter::AnimationCharacter()
	: GameObject()
{
	mParent = nullptr;
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
	GameObject::Initialize(x, y);

	AnimationCharacter* head = nullptr;
	AnimationCharacter* body = nullptr;
	AnimationCharacter* arm = nullptr;
	AnimationCharacter* leftHand = nullptr;
	AnimationCharacter* rightHand = nullptr;
	AnimationCharacter* weapon = nullptr;

	{
		head = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("FrontHead0");
			head->AddAnimation(ANIMATION_MONTION_TYPE::IDLE, ani);
			head->AddAnimation(ANIMATION_MONTION_TYPE::WALK, ani);
			head->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		}
	}
	{
		body = new AnimationCharacter;
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
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpBody0");
			body->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		}
	}

	{
		arm = new AnimationCharacter;
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
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpArm0");
			arm->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		}
	}

	{
		leftHand = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpLeftHand0");
			leftHand->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		}
	}
	{
		rightHand = new AnimationCharacter;
		//{
		//	Animation* ani = new Animation;
		//	ani->Initialize();
		//	ani->SetTexture("JumpRightHand0");
		//	rightHand->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		//}
	}

	{
		weapon = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("IdleWeapon0");
			ani->SetTexture("IdleWeapon1");
			ani->SetTexture("IdleWeapon2");
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
		//{
		//	Animation* ani = new Animation;
		//	ani->Initialize();
		//	ani->SetTexture("JumpWeapon0");
		//	weapon->AddAnimation(ANIMATION_MONTION_TYPE::JUMP, ani);
		//}
	}

	
	AddChild("body", body);
	AddChild("leftHand", leftHand);
	AddChild("head", head);
	AddChild("weapon", weapon);
	AddChild("arm", arm);	
	AddChild("rightHand", rightHand);


	Visible();
	for (auto& obj : mChildObjects)
	{
		obj.second->Visible();
	}

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, 2);
	ANIMATION_MONTION_TYPE motion = static_cast<ANIMATION_MONTION_TYPE>(uid(dre));
	SetAnimationMotion(motion);
	//SetAnimationMotion(ANIMATION_MONTION_TYPE::WALK);
	//SetAnimationMotion(ANIMATION_MONTION_TYPE::JUMP);

	return true;
}

void AnimationCharacter::Update()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mAnimations.count(mMotion) == true)
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
		if (mAnimations.count(mMotion) == true)
		{
			std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
			Texture* tex = mAnimations[mMotion]->GetTexture();
			D2D1_RECT_F pos;
			//pos.left = mAnimations[mMotion]->GetPosition().first;
			//pos.top = mAnimations[mMotion]->GetPosition().second;
			//pos.right = pos.left + tex->GetSize().first;
			//pos.bottom = pos.top + tex->GetSize().second;

			pos.left = (mPos.first - cameraPos.first) * 65.0 + mAnimations[mMotion]->GetPosition().first+ 23.0;
			pos.top = (mPos.second - cameraPos.second) * 65.0 + mAnimations[mMotion]->GetPosition().second + 8.0;
			pos.right = pos.left + tex->GetSize().first;
			pos.bottom = pos.top + tex->GetSize().second;

			GET_INSTANCE(GraphicEngine)->RenderTexture(tex, pos);
		}
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
		mPos.first = x;
		mPos.second = y;
	}
	else
	{
		std::pair<int, int> pos = mParent->mPos;
		mPos.first = x;
		mPos.second = y;
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

Player::Player()
	: AnimationCharacter()
{
	mId = -1;
	mElapsedTime = 0.0;
	mFlag = false;
}

Player::~Player()
{
}

bool Player::Initialize(int x, int y)
{
	AnimationCharacter::Initialize(x, y);

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

	SetAnimationMotion(ANIMATION_MONTION_TYPE::WALK);

	return true;
}

void Player::Update()
{
	AnimationCharacter::Update();

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
	AnimationCharacter::Render();

	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	{
		// 이미지 위치
		D2D1_RECT_F pos;
		pos.left = (mPos.first - cameraPos.first) * 65.0 + 8.0;
		pos.top = (mPos.second - cameraPos.second) * 65.0 + 8.0;
		pos.right = pos.left + 65;
		pos.bottom = pos.top + 65;

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
				attackPos.left = (attackX - cameraPos.first) * 65.0 + 8.0;
				attackPos.top = (attackY - cameraPos.second) * 65.0 + 8.0;
				attackPos.right = attackPos.left + 65;
				attackPos.bottom = attackPos.top + 65;

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
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
	{
		Move(DIRECTION_TYPE::RIGHT);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::UP_KEY) == true)
	{
		Move(DIRECTION_TYPE::UP);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::DOWN_KEY) == true)
	{
		Move(DIRECTION_TYPE::DOWN);
	}
	//else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::CONTROL_KEY) == true)
	//{
	//	SetAnimationMotion(ANIMATION_MONTION_TYPE::JUMP);
	//}
	else
	{
		SetAnimationMotion(ANIMATION_MONTION_TYPE::IDLE);
	}

	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::ALT_KEY) == KEY_STATE::HOLD)
	{
		std::cout << "누르는 중" << std::endl;
		//SetAnimationMotion(ANIMATION_MONTION_TYPE::JUMP);
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
		SetPosition(x, y);
	}

	SetAnimationMotion(ANIMATION_MONTION_TYPE::WALK);
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
