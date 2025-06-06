#include "Character.h"
#include <string>
#include <random>
#include <unordered_set>

#include "../../Network/Network.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Resource/Texture/Texture.h"
#include "../../Input/Input.h"
#include "../UI/Inventory/Inventory.h"
#include "../../GameTimer/GameTimer.h"

#include "../../Resource/ResourceManager.h"
#include "../../Manager/SceneMangaer/SceneManager.h"
#include "../../Scene/Scene.h"
#include "../../Animation/Animation.h"

#include "../../Common/Utility.h"

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
	GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top + 60), "메이플", "검은색");
}

void Character::Reset()
{
	mId = -1;
	mState = CHARACTER_STATE_TYPE::DEFAULT;
}

AnimationCharacter::AnimationCharacter()
	: GameObject()
{
	mId = -1;
	mParent = nullptr;
	mChildObjects.clear();

	mMotion = ANIMATION_MOTION_TYPE::IDLE;
	mAnimations.clear();
	mAvatarIds.clear();
	mAvatarIds.resize(MAX_AVATAR_SLOT_SIZE);
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
		if (ani.second != nullptr)
		{
			delete ani.second;
			ani.second = nullptr;
		}
	}
	mAnimations.clear();
	mAvatarIds.clear();
}

bool AnimationCharacter::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);

	AnimationCharacter* hair = nullptr;
	AnimationCharacter* head = nullptr;
	AnimationCharacter* face = nullptr;
	AnimationCharacter* body = nullptr;
	AnimationCharacter* arm = nullptr;
	AnimationCharacter* leftHand = nullptr;
	AnimationCharacter* rightHand = nullptr;
	AnimationCharacter* weapon = nullptr;
	AnimationCharacter* equipBody = nullptr;
	AnimationCharacter* equipArm = nullptr;

	{
		hair = new AnimationCharacter;
		{
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Hair", 1);
				hair->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Hair", 1);
				hair->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Hair", 1);
				hair->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
			}
		}
	}
	{
		head = new AnimationCharacter;
		{
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("FrontHead", 1);
				head->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("FrontHead", 1);
				head->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("FrontHead", 1);
				head->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
			}
		}
	}
	{
		face = new AnimationCharacter;
		{
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Face", 1);
				face->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Face", 1);
				face->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
			}
			{
				Animation* ani = new Animation;
				ani->Initialize();
				ani->SetTexture("Face", 1);
				face->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
			}
		}
	}
	{
		body = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			ani->SetTexture("IdleBody", 3);
			body->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("WalkBody", 4);
			body->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpBody", 1);
			body->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}

	{
		arm = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			ani->SetTexture("IdleArm", 3);
			arm->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("WalkArm", 4);
			arm->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpArm", 1);
			arm->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}

	{
		leftHand = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->SetTexture("JumpLeftHand", 1);
			leftHand->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}
	{
		rightHand = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			//ani->SetTexture("JumpRightHand", 1);
			rightHand->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}

	{
		// 1. 플레이어의 각 부위를 생성
		// 2. 플레이어의 애니메이션(IDLE, 걷기, 점프)을 추가
		// ex) 플레이어가 무기를 장착했을 때, 무기의 애니메이션 동작들을 추가한다.
		weapon = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			weapon->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			weapon->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);

		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			weapon->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}

	{
		equipBody = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			equipBody->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			equipBody->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			equipBody->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}
	{
		equipArm = new AnimationCharacter;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			equipArm->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			equipArm->AddAnimation(ANIMATION_MOTION_TYPE::WALK, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			equipArm->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}

	}

	AddChild("Body", body);
	AddChild("Longcoat", equipBody);
	AddChild("LeftHand", leftHand);
	AddChild("Head", head);
	AddChild("Face", face);
	AddChild("Hair", hair);
	AddChild("Weapon", weapon);
	AddChild("Arm", arm);
	AddChild("EquipArm", equipArm);
	AddChild("RightHand", rightHand);

	Visible();

	std::random_device rd;
	std::default_random_engine dre(rd());
	std::uniform_int_distribution<int> uid(0, 2);
	ANIMATION_MOTION_TYPE motion = static_cast<ANIMATION_MOTION_TYPE>(uid(dre));
	SetAnimationMotion(motion);

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
		if (mAnimations[mMotion]->IsVisible() == true)
		{
			mAnimations[mMotion]->Update();
		}
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
			if (mAnimations[mMotion]->IsVisible() == true)
			{
				std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
				Texture* tex = mAnimations[mMotion]->GetTexture();

				D2D1_RECT_F rect;
				rect.left = 0;
				rect.top = 0;
				rect.right = rect.left + tex->GetSize().first;
				rect.bottom = rect.top + tex->GetSize().second;

				D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();
				mat._11 *= mDir;
				mat._32 = (mPos.second - cameraPos.second) * 65.0 + 8.0 + mAnimations[mMotion]->GetPosition().second;

				if (mDir == 1)
				{
					mat._31 = (mPos.first - cameraPos.first) * 65.0 + mAnimations[mMotion]->GetPosition().first * mDir + 25;
				}
				else
				{
					mat._31 = (mPos.first - cameraPos.first) * 65.0 + mAnimations[mMotion]->GetPosition().first * mDir + 60;
				}

				GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

				GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);

				mat = D2D1::Matrix3x2F::Identity();
				GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
			}
		}
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->Render();
	}
}

void AnimationCharacter::AvatarRender(int x, int y)
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
			if (mAnimations[mMotion]->IsVisible() == true)
			{
				Texture* tex = mAnimations[mMotion]->GetTexture();

				D2D1_RECT_F rect;
				rect.left = 0;
				rect.top = 0;
				rect.right = rect.left + tex->GetSize().first;
				rect.bottom = rect.top + tex->GetSize().second;

				D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();
				mat._11 *= mDir;
				mat._31 = x + mAnimations[mMotion]->GetPosition().first * mDir;
				mat._32 = y + mAnimations[mMotion]->GetPosition().second;
				GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

				GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
				mat = D2D1::Matrix3x2F::Identity();
				GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
			}
		}
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->AvatarRender(x, y);
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

void AnimationCharacter::SetDirection(char dir)
{
	mDir = dir;
	for (auto& obj : mRenderChildObjects)
	{
		if (obj != nullptr)
		{
			obj->SetDirection(dir);
		}
	}
}

void AnimationCharacter::Visible()
{
	mAttr |= ATTR_STATE_TYPE::VISIBLE;

	for (auto& obj : mRenderChildObjects)
	{
		if (obj != nullptr)
		{
			obj->Visible();
		}
	}
}

void AnimationCharacter::NotVisible()
{
	mAttr &= ~ATTR_STATE_TYPE::VISIBLE;

	for (auto& obj : mRenderChildObjects)
	{
		if (obj != nullptr)
		{
			obj->Visible();
		}
	}
}

void AnimationCharacter::AddChild(const std::string& name, AnimationCharacter* obj)
{
	mChildObjects.emplace(name, obj);
	mRenderChildObjects.emplace_back(obj);

	obj->mParent = this;
}

AnimationCharacter* AnimationCharacter::FindChildObject(const std::string& name)
{
	if (mChildObjects.count(name) == true)
	{
		return mChildObjects[name];
	}

	return nullptr;
}

void AnimationCharacter::SetAvatar(const std::string& parts, ANIMATION_MOTION_TYPE motion, const std::string& partsName, int count)
{
	if (mChildObjects.count(parts) == false)
	{
		return;
	}

	for (int i = 0; i < count; ++i)
	{
		mChildObjects[parts]->mAnimations[motion]->SetTexture(partsName, partsName + std::to_string(i));
	}
}

void AnimationCharacter::SetAvatarId(int slotType, int texId)
{
	mAvatarIds[slotType] = texId;
	SetAvatar(texId);
}

int AnimationCharacter::RemoveAvatarId(int slotType)
{
	int oldAvatarId = mAvatarIds[slotType];
	// 텍스id 삭제
	mAvatarIds[slotType] = 0;

	return oldAvatarId;
}

void AnimationCharacter::SetAvatar(int texId)
{
	std::vector<TextureData>& v = GET_INSTANCE(ResourceManager)->GetTextureDatas(texId);
	std::unordered_set<std::string> partsList;
	for (int i = 0; i < v.size(); ++i)
	{
		partsList.emplace(v[i].parts);
	}

	for (auto parts : partsList)
	{
		// 해당 파트의 모든 모션을 리셋
		for (auto& motion : mChildObjects[parts]->mAnimations)
		{
			motion.second->Reset();
		}
	}

	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i].icon == true)
		{
			continue;
		}

		std::string partsName = v[i].parts;
		ANIMATION_MOTION_TYPE motion = static_cast<ANIMATION_MOTION_TYPE>(v[i].motion);
		mChildObjects[partsName]->mAnimations[motion]->SetTexture(v[i].name);
	}

	// 모든 부위의 애니메이션 번호를 초기화
	for (auto& obj : mRenderChildObjects)
	{
		for (auto& ani : obj->mAnimations)
		{
			ani.second->ResetCurrentNum();
		}
	}
}

void AnimationCharacter::TakeOffAvatar(int slotType)
{
	// 저장된 아바타 아이디 삭제
	int oldTexId = RemoveAvatarId(slotType);
	
	std::vector<TextureData>& v = GET_INSTANCE(ResourceManager)->GetTextureDatas(oldTexId);
	std::unordered_set<std::string> partsList;
	for (int i = 0; i < v.size(); ++i)
	{
		partsList.emplace(v[i].parts);
	}

	for (auto parts : partsList)
	{
		// 해당 파트의 모든 모션을 리셋
		for (auto& motion : mChildObjects[parts]->mAnimations)
		{
			motion.second->Reset();
		}
	}

}

void AnimationCharacter::SetAnimationMotion(ANIMATION_MOTION_TYPE motion)
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
	//for (auto& ani : FindChildObject("Face")->GetAnimations())
	//{
	//	ani.second->SetTexture("Face2");
	//}

	SetAnimationMotion(ANIMATION_MOTION_TYPE::IDLE);

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
		GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top + 60), "메이플", "검은색");

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
		SetDirection(1);
		GET_INSTANCE(Network)->SendMovePacket(DIRECTION_TYPE::LEFT);
	}
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
	{
		SetDirection(-1);
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

#else
	if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
	{
		Move(DIRECTION_TYPE::LEFT);
		SetDirection(1);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
	{
		Move(DIRECTION_TYPE::RIGHT);
		SetDirection(-1);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::UP_KEY) == true)
	{
		Move(DIRECTION_TYPE::UP);
	}
	else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::DOWN_KEY) == true)
	{
		Move(DIRECTION_TYPE::DOWN);
	}
#endif
	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::LEFT_KEY) == KEY_STATE::HOLD
		|| GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::RIGHT_KEY) == KEY_STATE::HOLD)
	{
		SetAnimationMotion(ANIMATION_MOTION_TYPE::WALK);
	}
	else if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::ALT_KEY) == KEY_STATE::HOLD)
	{
		SetAnimationMotion(ANIMATION_MOTION_TYPE::JUMP);
	}
	else
	{
		SetAnimationMotion(ANIMATION_MOTION_TYPE::IDLE);
	}

}

void Player::ProcessMouseMessage(unsigned int msg, unsigned long long wParam, long long lParam)
{
	//Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"));
	////Inventory* inventory = static_cast<Inventory*>(GET_INSTANCE(UIManager)->FindUI("Inventory"));
	//if (inventory != nullptr && inventory->IsVisible() == true)
	//{
	//	inventory->ProcessMouseWheelEvent(wParam);
	//}
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
}

void Player::AddItem()
{
	std::string itemName = "";
	int texId = 10000;
	switch (GetRandomNumber(0, 4))
	{
		case 0:
		{
			itemName = "Ax";
			texId = 10002;
			break;
		}

		case 1:
		{
			itemName = "Sword";
			texId = 10000;
			break;
		}

		case 2:
		{
			itemName = "Dagger";
			texId = 10003;
			break;
		}

		case 3:
		{
			itemName = "Club";
			texId = 10001;
			break;
		}

		default:
		{
			itemName = "DreamOfDoll";
			texId = 9999;
			break;
		}
	}

	//static_cast<Inventory*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("Inventory"))->AddItem(texId);
}

OtherPlayer::OtherPlayer()
	: AnimationCharacter()
{
}

OtherPlayer::~OtherPlayer()
{
}

bool OtherPlayer::Initialize(int x, int y)
{
	AnimationCharacter::Initialize(x, y);

	return true;
}

void OtherPlayer::Update()
{
	AnimationCharacter::Update();
}

void OtherPlayer::Render()
{
	AnimationCharacter::Render();

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	// 이미지 위치
	D2D1_RECT_F pos;
	pos.left = (mPos.first - cameraPos.first) * 65.0 + 8.0;
	pos.top = (mPos.second - cameraPos.second) * 65.0 + 8.0;

	std::wstring text = L"Id (" + std::to_wstring(mId) + L")";
	GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top + 60), "메이플", "검은색");
}

Monster::Monster()
	: AnimationCharacter()
{
	mId = -1;
	mElapsedTime = 0.0;
}

Monster::~Monster()
{
}

bool Monster::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);

	Monster* monster = nullptr;
	
	{
		monster = new Monster;
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			ani->SetName("IdleOrangeMushroom");
			ani->SetTexture("IdleOrangeMushroom", "IdleOrangeMushroom0");
			ani->SetTexture("IdleOrangeMushroom", "IdleOrangeMushroom1");
			monster->AddAnimation(ANIMATION_MOTION_TYPE::IDLE, ani);
		}
		{
			Animation* ani = new Animation;
			ani->Initialize();
			ani->IsRepeat();
			ani->SetName("JumpOrangeMushroom");
			ani->SetTexture("JumpOrangeMushroom", "JumpOrangeMushroom0");
			ani->SetTexture("JumpOrangeMushroom", "JumpOrangeMushroom1");
			monster->AddAnimation(ANIMATION_MOTION_TYPE::JUMP, ani);
		}
	}
	monster->mId = mId;
	AddChild("monster", monster);

	//Visible();

	ANIMATION_MOTION_TYPE motion = ANIMATION_MOTION_TYPE::IDLE;
	SetAnimationMotion(motion);

	SetDirection(1);

	return true;
}

void Monster::Update()
{
	AnimationCharacter::Update();
}

void Monster::Render()
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

			D2D1_RECT_F rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = rect.left + tex->GetSize().first;
			rect.bottom = rect.top + tex->GetSize().second;

			D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();
			mat._11 *= mDir;
			mat._32 = (mPos.second - cameraPos.second) * 65.0 + 8.0 + mAnimations[mMotion]->GetPosition().second;
			if (mDir == 1)
			{
				mat._31 = (mPos.first - cameraPos.first) * 65.0 + 10.0 + mAnimations[mMotion]->GetPosition().first * mDir;
			}
			else
			{
				mat._31 = (mPos.first - cameraPos.first) * 65.0 + 70.0 + mAnimations[mMotion]->GetPosition().first * mDir;
			}

			GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

			GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);

			//std::wstring text = L"Id (" + std::to_wstring(mParent->GetId()) + L")";
			//GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), rect.left, rect.right, "메이플", "검은색");

			mat = D2D1::Matrix3x2F::Identity();
			GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
		}
	}
	else
	{
		std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
		// 이미지 위치
		D2D1_RECT_F pos;
		pos.left = (mPos.first - cameraPos.first) * 65.0 + 8.0;
		pos.top = (mPos.second - cameraPos.second) * 65.0 + 8.0;

		std::wstring text = L"Id (" + std::to_wstring(mId) + L")";
		GET_INSTANCE(GraphicEngine)->RenderText(text.c_str(), static_cast<int>(pos.left), static_cast<int>(pos.top + 60), "메이플", "검은색");
	}

	for (auto& obj : mRenderChildObjects)
	{
		obj->Render();
	}
}