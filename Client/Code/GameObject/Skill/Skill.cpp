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
	mCurrentEffect = 0;
	mEffects.clear();
}

Skill::~Skill()
{
	for (auto& effect : mEffects)
	{
		if (effect != nullptr)
		{
			delete effect;
		}
	}
	mEffects.clear();
}

bool Skill::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	mCurrentEffect = 0;

	return true;
}

void Skill::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mEffects.size() <= 0)
	{
		return;
	}

	if (checkFinish() == true)
	{
		NotVisible();

		if (mCurrentEffect == 0)
		{
			mCurrentEffect = 1;
		}
		else
		{
			mCurrentEffect = 0;
		}
		return;
	}

	mEffects[mCurrentEffect]->Update();
}

void Skill::Render()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	if (mEffects.size() <= 0)
	{
		return;
	}

	mEffects[mCurrentEffect]->Render();
}

void Skill::AddEffect(int texId)
{
	std::vector<TextureData>& v = GET_INSTANCE(ResourceManager)->GetTextureDatas(texId);

	// 모션번호, 인덱스
	std::unordered_map<int, std::vector<int>> um;
	for (int i = 0; i < v.size(); ++i)
	{
		if (v[i].icon == true)
		{
			continue;
		}

		int motion = v[i].motion;
		if (um.count(motion) == false)
		{
			std::vector<int> vv;
			vv.emplace_back(i);
			um.emplace(motion, vv);
		}
		else
		{
			um[motion].emplace_back(i);
		}
	}

	for (auto& motion : um)
	{
		SkillEffect* effect = new SkillEffect;
		effect->Initialize(0, 0);

		for (int i = 0; i < motion.second.size(); ++i)
		{
			int index = motion.second[i];
			std::string name = v[index].name;
			effect->AddAnimation(name);
		}

		mEffects.emplace_back(effect);
	}
}

void Skill::UseSkill()
{
	mEffects[mCurrentEffect]->GetAnimation()->SetFinish(false);
	Visible();
}

bool Skill::checkFinish()
{
	if (mEffects[mCurrentEffect]->GetAnimation()->GetIsFinish() == true)
	{
		return true;
	}

	return false;

}

SkillEffect::SkillEffect()
	: GameObject()
{
	mAnimation = nullptr;
}

SkillEffect::~SkillEffect()
{
	if (mAnimation != nullptr)
	{
		delete mAnimation;
		mAnimation = nullptr;
	}
}

bool SkillEffect::Initialize(int x, int y)
{
	GameObject::Initialize(x, y);
	mAnimation = new Animation;
	mAnimation->SetPlaySpeed(3.0);

	mDir = -1;

	//SetPosition(0, 0);

	return true;
}

void SkillEffect::Update()
{
	if (mAnimation == nullptr || mAnimation->IsVisible() == false)
	{
		return;
	}

	if (mAnimation->GetIsFinish() == false)
	{
		mAnimation->Update();
	}
	else
	{
		mAnimation->SetFinish(false);
	}
}

void SkillEffect::Render()
{
	if (mAnimation == nullptr || mAnimation->GetIsFinish() == true)
	{
		return;
	}

	std::pair<int, int> cameraPos = GET_INSTANCE(Camera)->GetPosition();
	Texture* tex = mAnimation->GetTexture();
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
	mat._32 = (pos.second - cameraPos.second) * 65.0 + (-130) + mAnimation->GetPosition().second;
	if (dir == 1)
	{
		mat._31 = (pos.first - cameraPos.first) * 65.0 + (-60) + mAnimation->GetPosition().first * dir;
	}
	else
	{
		mat._31 = (pos.first - cameraPos.first) * 65.0 + 145.0 + mAnimation->GetPosition().first * dir;
	}

	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);

	GET_INSTANCE(GraphicEngine)->RenderTexture(tex, rect);
	//GET_INSTANCE(GraphicEngine)->RenderRectangle(rect);

	mat = D2D1::Matrix3x2F::Identity();
	GET_INSTANCE(GraphicEngine)->GetRenderTarget()->SetTransform(mat);
}

void SkillEffect::AddAnimation(const std::string& texName)
{
	mAnimation->SetTexture(texName);
}
