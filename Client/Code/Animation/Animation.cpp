#include "Animation.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Texture/Texture.h"
#include "../GameTimer/GameTimer.h"
#include "../Input/Input.h"

Animation::Animation()
{
	mTextures.clear();
	mPositions.clear();
	mCurrentNum = 0;
	mElapsedTime = 0.0;
}

Animation::~Animation()
{
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
	if (mElapsedTime > 0.3)
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
	Texture* tex = GET_INSTANCE(ResourceManager)->FindTexture(name);
	mTextures.emplace_back(tex);
	mPositions.emplace_back(tex->GetOrigin());
}
