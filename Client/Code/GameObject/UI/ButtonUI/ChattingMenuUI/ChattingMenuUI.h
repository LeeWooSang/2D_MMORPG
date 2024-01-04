#pragma once
#include "../ButtonUI.h"
#include <vector>
#include <string>

enum class CHATTING_TYPE
{
	BROADCASTING,
	PARTY,
	WHISPERING
};

struct ChattingType
{
	ChattingType(CHATTING_TYPE _type, const std::wstring& _name)
		: type(_type), name(_name) {}

	CHATTING_TYPE type;
	std::wstring name;
};

class ChattingMenuUI : public ButtonUI
{
public:
	ChattingMenuUI();
	virtual ~ChattingMenuUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void ChangeType();
	static int GetChattingType()	{ return mCurrentType; }

private:
	static std::vector<ChattingType> mChattingTypes;
	static int mCurrentType;
};