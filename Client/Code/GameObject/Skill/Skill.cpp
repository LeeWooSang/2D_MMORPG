#include "Skill.h"
#include "../../Resource/ResourceManager.h"
#include "../../Resource/Texture/Texture.h"
#include "../../GraphicEngine/GraphicEngine.h"
#include "../../Animation/Animation.h"

#include "../../Manager/SceneMangaer/SceneManager.h"
#include "../../Scene/InGameScene/InGameScene.h"
#include "../Character/Character.h"

Skill::Skill()
	: GameObject()
{
	mMotion = SKILL_TYPE::RAGING_BLOW;
}

Skill::~Skill()
{
	for (auto& ani : mAnimations)
	{
		if (ani.second != nullptr)
		{
			delete ani.second;
			ani.second = nullptr;
		}
	}
	mAnimations.clear();
}

bool Skill::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);

	Animation* ani = new Animation;
	ani->SetTexture("Effect", 15);
	ani->SetPlaySpeed(2.0);
	mAnimations.emplace(SKILL_TYPE::RAGING_BLOW, ani);
	
	//Visible();
	mMotion = SKILL_TYPE::RAGING_BLOW;
	mDir = -1;

	//SetPosition(0, 0);
	return true;
}

void Skill::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mAnimations.count(mMotion) == true)
	{
		if (mAnimations[mMotion]->IsVisible() == true)
		{
			if (mAnimations[mMotion]->GetIsFinish() == false)
			{
				mAnimations[mMotion]->Update();
			}
			else
			{
				NotVisible();
				mAnimations[mMotion]->SetFinish(false);
			}
		}
	}

}

void Skill::Render()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mAnimations[mMotion]->GetIsFinish() == false && mAnimations.size() > 0)
	{
		std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
		Texture* tex = mAnimations[mMotion]->GetTexture();
		D2D1_RECT_F rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = rect.left + tex->GetSize().first;
		rect.bottom = rect.top + tex->GetSize().second;

		InGameScene* scene = static_cast<InGameScene*>(GET_INSTANCE(SceneManager)->GetCurScene());
		std::pair<int, int> pos = scene->GetPlayer()->GetPosition();
		char dir = scene->GetPlayer()->GetDirection();

		D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F::Identity();
		mat._11 *= dir;
		mat._32 = (pos.second - cameraPos.second) * 65.0 + (-130) + mAnimations[mMotion]->GetPosition().second;
		if (dir == 1)
		{
			mat._31 = (pos.first - cameraPos.first) * 65.0 + (-60) + mAnimations[mMotion]->GetPosition().first * dir;
		}
		else
		{
			mat._31 = (pos.first - cameraPos.first) * 65.0 + 145.0 + mAnimations[mMotion]->GetPosition().first * dir;
		}

		GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

		GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
		//GET_INSTANCE(GraphicEngine)->RenderRectangle(rect);

		mat = D2D1::Matrix3x2F::Identity();
		GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
	}
}
