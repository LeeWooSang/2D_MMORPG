#pragma once
#include <vector>
#include <string>

class Texture;
class Animation
{
public:
	Animation();
	~Animation();

	bool Initialize();
	void Update();

	void SetTexture(const std::string& name);
	Texture* GetTexture() { return mTextures[mCurrentNum]; }

private:
	std::vector<Texture*> mTextures;
	int mCurrentNum;
	float mElapsedTime;
};