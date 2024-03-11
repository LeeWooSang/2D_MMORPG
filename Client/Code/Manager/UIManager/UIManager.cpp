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
	// SceneManager에서 현재 Scene에서 렌더링 되는 순서의 UI들을 갖고온다.
	std::list<UI*>& sceneUIs = GET_INSTANCE(SceneManager)->GetCurScene()->GetSceneUIs();
	std::list<UI*>::iterator targetIter = sceneUIs.end();

	for (auto iter = sceneUIs.begin(); iter != sceneUIs.end(); ++iter)
	{
		if ((*iter)->IsVisible() == false)	continue;

		// UI들을 루프돌면서,  현재 보이면서, 마우스와 충돌중인 UI를 찾는다.
		if ((*iter)->GetMouseOver() == true)
		{
			targetIter = iter;
		}
	}

	if (targetIter == sceneUIs.end())
	{
		return nullptr;
	}
	// 가장 마지막의 targetIter가 맨 위에 UI로 올라와야 함으로, 그것을 focusUI로 정함
	// SceneManager의 UI집합중 targetIter를 맨 뒤로 넣는다. (맨 마지막에 그려져야 맨위에 올라오기 때문)
	focusUI = (*targetIter);
	sceneUIs.erase(targetIter);
	sceneUIs.emplace_back(focusUI);

	return focusUI;
}
