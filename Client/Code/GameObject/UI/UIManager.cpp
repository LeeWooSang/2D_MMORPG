#include "UIManager.h"
#include "../../Core/Core.h"
#include "../Character/Character.h"
#include "UI.h"
#include  "Inventory/Inventory.h"
#include "../../Input/Input.h"
#include <queue>
#include <list>

INIT_INSTACNE(UIManager)
UIManager::UIManager()
{
	mFocusUI = nullptr;
}

UIManager::~UIManager()
{
	for (auto& ui : mUIs)
	{
		delete ui;
	}
	mUIs.clear();
}

void UIManager::Render()
{
	for (auto ui : mUIs)
	{
		ui->Render();
	}
}

void UIManager::Update()
{
	for (auto ui : mUIs)
	{
		ui->Visible();
		ui->Update();
	}

	mFocusUI = getFocusUI();
	if (mFocusUI == nullptr)
	{
		return;
	}

	//UI* targetUI = getTargetUI(mUIs.front());
	UI* targetUI = getTargetUI(mFocusUI);
	if (targetUI != nullptr)
	{
		targetUI->MouseOver();

		// 키를 누른 순간
		if (GET_INSTANCE(Input)->GetIsPushed(KEY_TYPE::MOUSE_LBUTTON) == true)
		{
			targetUI->MouseLButtonDown();
			targetUI->SetMouseLButtonDown(true);
		}
		// 키를 뗀 순간
		else if (GET_INSTANCE(Input)->GetIsPop(KEY_TYPE::MOUSE_LBUTTON) == true)
		{
			targetUI->MouseLButtonUp();
			// 유아이 안에서 키를 누르고 뗐을 때만 클릭
			if (targetUI->GetMouseLButtonDown() == true)
			{
				targetUI->MouseLButtonClick();
			}
			targetUI->SetMouseLButtonDown(false);
		}
	}
}

void UIManager::SetFocusUI(UI* ui)
{
	if (mFocusUI == ui || ui == nullptr)
	{
		mFocusUI = ui;
		return;
	}
	mFocusUI = ui;

	std::list<UI*>::iterator targetIter = mUIs.end();
	for (auto iter = mUIs.begin(); iter != mUIs.end(); ++iter)
	{
		if (mFocusUI == (*iter))
		{
			break;
		}
	}

	mUIs.erase(targetIter);
	mUIs.emplace_back(mFocusUI);
}

UI* UIManager::getTargetUI(UI* parentUI)
{
	UI* targetUI = nullptr;

	std::list<UI*> noneTargetUIs;

	std::queue<UI*> qu;
	qu.push(parentUI);

	while (qu.empty() == false)
	{
		UI* ui = qu.front();
		qu.pop();

		// 마우스 오버상태
		if (ui->GetMouseOver() == true)
		{
			if (targetUI != nullptr)
			{
				noneTargetUIs.emplace_back(targetUI);
			}

			targetUI = ui;
		}
		else
		{
			noneTargetUIs.emplace_back(ui);
		}

		for (auto& child : ui->GetChildUIs())
		{
			for (int i = 0; i < child.second.size(); ++i)
			{
				qu.push(child.second[i]);
			}
		}
	}

	if (GET_INSTANCE(Input)->GetIsPop(KEY_TYPE::MOUSE_LBUTTON) == true)
	{
		for (auto& ui : noneTargetUIs)
		{
			ui->SetMouseLButtonDown(false);
		}
	}

	return targetUI;
}

UI* UIManager::getFocusUI()
{
	// 기존 포커스된 UI와 새로운 포커스된 UI 비교용도
	UI* focusUI = mFocusUI;
	// 키를 누른 순간
	if (GET_INSTANCE(Input)->GetIsPushed(KEY_TYPE::MOUSE_LBUTTON) == false)
	{
		return focusUI;
	}

	std::list<UI*>::iterator targetIter = mUIs.end();
	for (auto iter = mUIs.begin(); iter != mUIs.end(); ++iter)
	{
		if ((*iter)->GetMouseOver() == true)
		{
			targetIter = iter;
		}
	}

	if (targetIter == mUIs.end())
	{
		return nullptr;
	}

	focusUI = (*targetIter);
	mUIs.erase(targetIter);
	mUIs.emplace_back(focusUI);

	return focusUI;
}
