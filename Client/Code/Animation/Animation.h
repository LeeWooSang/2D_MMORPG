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

	void Reset();
	//void AddTexture(const std::string& name);
	void SetName(const std::string& objName) { mName = objName; }
	void SetTexture(const std::string& objName, const std::string& texName);
	void SetTexture(const std::string& objName, int count);
	void SetTexture(const std::string& texName);

	Texture* GetTexture() { return mTextures[mCurrentNum]; }

	std::pair<int, int> GetPosition()	const { return mPositions[mCurrentNum]; };
	void IsRepeat() { mRepeat = true; }
	bool IsVisible();

	void ResetCurrentNum();
	int GetCurrentNum()		const { return mCurrentNum; }

private:
	std::string mName;
	std::vector<Texture*> mTextures;
	std::vector<std::pair<int, int>> mPositions;

	int mCurrentNum;
	float mElapsedTime;

	bool mRepeat;
	bool mFlag;
};