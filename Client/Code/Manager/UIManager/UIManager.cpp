#include "UIManager.h"
#include <Windows.h>
#include "../../Manager/SceneMangaer/SceneManager.h"
#include "../../Scene/Scene.h"

#include "../../GameObject/UI/UI.h"
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
}

void UIManager::Render()
{
	for (auto ui : GET_INSTANCE(SceneManager)->GetCurScene()->GetSceneUIs())
	{
		if (ui->IsVisible() == true)
		{
			ui->Render();
		}
	}
}

void UIManager::Update()
{
	HWND handle = GetFocus();
	if (handle == nullptr)
	{
		return;
	}

	Scene* scene = GET_INSTANCE(SceneManager)->GetCurScene();
	std::list<UI*> uis = GET_INSTANCE(SceneManager)->GetCurScene()->GetSceneUIs();
	for (auto& ui : uis)
	{
		if (ui->IsVisible() == true)
		{
			ui->Update();
		}
	}

	mFocusUI = getFocusUI();
	if (mFocusUI == nullptr)
	{
		return;
	}

	UI* targetUI = getTargetUI(mFocusUI);
	if (targetUI != nullptr)
	{
		targetUI->MouseOver();

		// 키를 누른 순간
		if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::TAP)
		{
			targetUI->MouseLButtonDown();
			targetUI->SetMouseLButtonDown(true);
		}
		// 키를 뗀 순간
		else if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::AWAY)
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

	std::list<UI*>& sceneUIs = GET_INSTANCE(SceneManager)->GetCurScene()->GetSceneUIs();
	std::list<UI*>::iterator targetIter = sceneUIs.end();
	for (auto iter = sceneUIs.begin(); iter != sceneUIs.end(); ++iter)
	{
		if (mFocusUI == (*iter))
		{
			targetIter = iter;
			break;
		}
	}

	if (targetIter == sceneUIs.end())
	{
		return;
	}

	sceneUIs.erase(targetIter);
	sceneUIs.emplace_back(mFocusUI);
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

	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) == KEY_STATE::AWAY)
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
	if (GET_INSTANCE(Input)->GetKeyState(KEY_TYPE::MOUSE_LBUTTON) != KEY_STATE::TAP)
	{
		return focusUI;
	}

	std::list<UI*>& sceneUIs = GET_INSTANCE(SceneManager)->GetCurScene()->GetSceneUIs();
	std::list<UI*>::iterator targetIter = sceneUIs.end();
	for (auto iter = sceneUIs.begin(); iter != sceneUIs.end(); ++iter)
	{
		if ((*iter)->IsVisible() == false)
		{
			continue;
		}

		if ((*iter)->GetMouseOver() == true)
		{
			targetIter = iter;
		}
	}

	if (targetIter == sceneUIs.end())
	{
		return nullptr;
	}

	focusUI = (*targetIter);
	sceneUIs.erase(targetIter);
	sceneUIs.emplace_back(focusUI);

	return focusUI;
}
