#pragma once
#include "../Scene.h"

class ServerScene : public Scene
{
public:
	ServerScene();
	virtual ~ServerScene();

	virtual bool Initialize();
	virtual void Update();
	virtual void Render();

	virtual void processKeyboardMessage();

	void ProcessServerSelect(short* channelUserSize);
	void ProcessChannelLogin(int channel);

	int GetServer(const std::string& key) { return mServerList[key]; }

private:
	std::unordered_map<std::string, int> mServerList;
};

void ServerSelectClick(const std::string& name);