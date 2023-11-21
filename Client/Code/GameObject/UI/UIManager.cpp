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
}

UIManager::~UIManager()
{
}

void UIManager::Update()
{
	if (GET_INSTANCE(Core)->GetPlayer() == nullptr)
	{
		return;
	}

	Inventory* inventory = GET_INSTANCE(Core)->GetPlayer()->GetInventory();

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	if (inventory->Collision(mousePos.first, mousePos.second) == true)
	{
		inventory->SetMouseOver(true);
	}
	else
	{
		inventory->SetMouseOver(false);
	}

	UI* targetUI = getTargetUI(inventory);
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
