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
	mCurrentNum = 0;
	mElapsedTime = 0.0;

	mRepeat = false;
	mFlag = false;
}

Animation::~Animation()
{
	mName.clear();
	mTextures.clear();
	mPositions.clear();
}

bool Animation::Initialize()
{
	return true;
}

void Animation::Update()
{
	mElapsedTime += GET_INSTANCE(GameTimer)->GetElapsedTime();
	if (mRepeat == false)
	{
		if (mElapsedTime > 0.2)
		{
			mElapsedTime = 0.0;
			++mCurrentNum;
			if (mCurrentNum >= mTextures.size())
			{
				mCurrentNum = 0;
			}
		}
	}
	// 전진 애니메이션
	else
	{
		if (mElapsedTime > 0.3)
		{
			mElapsedTime = 0.0;
			if (mFlag == false)
			{
				++mCurrentNum;
				if (mCurrentNum >= mTextures.size())
				{
					mCurrentNum = mTextures.size() - 1;
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
