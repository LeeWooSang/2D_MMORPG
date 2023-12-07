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

	std::pair<int, int> GetPosition()	const { return mPositions[mCurrentNum]; };

private:
	std::vector<Texture*> mTextures;
	std::vector<std::pair<int, int>> mPositions;

	int mCurrentNum;
	float mElapsedTime;
};