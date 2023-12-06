#include "Animation.h"
#include "../Resource/ResourceManager.h"
#include "../GameTimer/GameTimer.h"

Animation::Animation()
{
	mTextures.clear();
	mCurrentNum = 0;
	mElapsedTime = 0.0;
}

Animation::~Animation()
{
	mTextures.clear();
}

bool Animation::Initialize()
{
	return true;
}

void Animation::Update()
{
	mElapsedTime += GET_INSTANCE(GameTimer)->GetElapsedTime();
	if (mElapsedTime > 0.5)
	{
		mElapsedTime = 0.0;
		++mCurrentNum;
		if (mCurrentNum >= mTextures.size())
		{
			mCurrentNum = 0;
		}
	}

}

void Animation::SetTexture(const std::string& name)
{
	mTextures.emplace_back(GET_INSTANCE(ResourceManager)->FindTexture(name));
}
