#include "Animation.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Texture/Texture.h"
#include "../GameTimer/GameTimer.h"
#include "../Input/Input.h"

Animation::Animation()
{
	mName.clear();
	mTextures.clear();
	mPositions.clear();

	mPlaySpeed = 1.0;
	mCurrentNum = 0;
	mElapsedTime = 0.0;

	mRepeat = false;
	mFlag = false;
	mFinish = false;
}

Animation::~Animation()
{
	mName.clear();
	mTextures.clear();
	mPositions.clear();
}

bool Animation::Initialize()
{
	//mElapsedTime = 0.2;
	return true;
}

void Animation::Update()
{
	mElapsedTime += GET_INSTANCE(GameTimer)->GetElapsedTime() * mPlaySpeed;
	if (mRepeat == false)
	{
		if (mElapsedTime > 0.2)
		{
			mElapsedTime -= 0.2;
			++mCurrentNum;
			if (mCurrentNum >= mTextures.size())
			{
				mCurrentNum = 0;
				mFinish = true;
			}
		}
	}
	// 전진 애니메이션
	else
	{
		if (mElapsedTime > 0.3)
		{
			mElapsedTime -= 0.3;
			if (mFlag == false)
			{
				++mCurrentNum;
				if (mCurrentNum >= mTextures.size())
				{
					int currentNum = mTextures.size() - 1;
					mCurrentNum = currentNum <= 0 ? 0 : currentNum;
					mFlag = true;
				}
			}
			else
			{
				--mCurrentNum;
				if (mCurrentNum < 0)
				{
					mCurrentNum = 0;
					mFlag = false;
				}
			}
		}
	}
}

void Animation::Reset()
{
	mTextures.clear();
	mPositions.clear();
	mName.clear();

}

void Animation::SetTexture(const std::string& objName, const std::string& texName)
{
	// 다른 오브젝트가 올경우
	if (mName != objName)
	{
		mTextures.clear();
		mPositions.clear();
		//mCurrentNum = 0;
		mName = objName;
	}

	// 기존 다른 텍스쳐가 있으면 정보를 날림
	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(texName);
	mTextures.emplace_back(tex);
	mPositions.emplace_back(tex->GetOrigin());
}

void Animation::SetTexture(const std::string& objName, int count)
{
	mTextures.clear();
	mPositions.clear();
	mName = objName;

	for (int i = 0; i < count; ++i)
	{
		Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(objName + std::to_string(i));
		if (tex == nullptr)
		{
			continue;
		}
		mTextures.emplace_back(tex);
		mPositions.emplace_back(tex->GetOrigin());
	}
}

void Animation::SetTexture(const std::string& texName)
{
	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(texName);
	if (tex == nullptr)
	{
		return;
	}
	mTextures.emplace_back(tex);
	mPositions.emplace_back(tex->GetOrigin());
}

bool Animation::IsVisible()
{
	if (mTextures.size() > 0)
	{
		return true;
	}

	return false;
}

void Animation::ResetCurrentNum()
{
	mElapsedTime = 0.0;
	mCurrentNum = 0;
	mFlag = false;
}
