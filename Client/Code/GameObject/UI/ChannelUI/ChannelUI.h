#pragma once
#include "../UI.h"

enum class CHANNEL_TYPE
{
	NONE,
	CURRENT_CHANNEL,
	SELECT_CHANNEL
};

class ChannelUI : public UI
{
public:
	ChannelUI();
	virtual ~ChannelUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	void OpenChannelUI();

	int GetCurrentChannel()	const { return mCurrentChannel; }
	void SetCurrentChannel(int channel);
	int GetSelectChannel()	const { return mSelectChannel; }
	void SetSelectChannel(int channel);
	void ResetSelectChannel();

protected:
	bool mOpen;
	int mCurrentChannel;
	int mSelectChannel;
};

class LoginChannelUI : public ChannelUI
{
public:
	LoginChannelUI();
	virtual ~LoginChannelUI();
	virtual bool Initialize(int x, int y);
	void ChangeLogo(int type);

private:
	std::string mLogoName;
};

class ChannelUISlot : public UI
{
public:
	ChannelUISlot();
	virtual ~ChannelUISlot();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseLButtonClick();
	void SetChannel(int channel) { mChannel = channel; }
	void SetSlotName(const std::string& name) { mSlotName = name; }
	void SetChannelType(CHANNEL_TYPE type) { mChannelType = type; }

protected:
	int mChannel;
	std::string mSlotName;
	CHANNEL_TYPE mChannelType;
};

class LoginChannelUISlot : public ChannelUISlot
{
public:
	LoginChannelUISlot();
	virtual ~LoginChannelUISlot();
	virtual bool Initialize(int x, int y);
	virtual void Render();
	virtual void MouseLButtonClick();

	void SetGagueRate(float rate) { mGaugeRate = rate; }

private:
	float mGaugeRate;
};

void ChannelClick(const std::string& channel);
void ChannelChangeClick(const std::string& name);
void ChannelCancelClick(const std::string& name);
void ChannelLoginClick(const std::string& name);