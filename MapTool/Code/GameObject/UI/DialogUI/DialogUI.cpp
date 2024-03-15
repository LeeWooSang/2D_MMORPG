#include "DialogUI.h"

DialogUI::DialogUI()
	: UI()
{
	mOpen = false;
}

DialogUI::~DialogUI()
{
}

bool DialogUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void DialogUI::Update()
{
}

void DialogUI::Render()
{
}

void DialogUI::MouseOver()
{
}

void DialogUI::MouseLButtonDown()
{
}

void DialogUI::MouseLButtonUp()
{
}

void DialogUI::MouseLButtonClick()
{
}

void DialogUI::OpenDialogUI()
{
	// 닫혀있다면
	if (mOpen == false)
	{
		mOpen = true;
		Visible();
	}
	// 열려있다면
	else
	{
		mOpen = false;
		NotVisible();
	}
}

void DialogUI::Visible()
{
	mAttr |= ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Visible();
		}
	}
}

void DialogUI::NotVisible()
{
	mAttr &= ~ATTR_STATE_TYPE::VISIBLE;

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->NotVisible();
		}
	}
}
