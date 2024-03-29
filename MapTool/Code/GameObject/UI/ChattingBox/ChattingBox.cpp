#include "ChattingBox.h"
#include "../InputUI/ChattingInputUI/ChattingInputUI.h"
#include "../ButtonUI/ChattingMenuUI/ChattingMenuUI.h"

#include "../../../Network/Network.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"

void ChangeChattingMenu(const std::string& name);
void ChangeChattingMenu(const std::string& name)
{
	ChattingMenuUI* ui = static_cast<ChattingMenuUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("ChattingBox"));
	if (ui == nullptr)
	{
		return;
	}
	ui->ChangeType();
}
ChattingBox::ChattingBox()
	: UI()
{
	mChattingLog.clear();
	mChattingLog.reserve(100);

	mChattings.clear();
}

ChattingBox::~ChattingBox()
{
}

bool ChattingBox::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	SetRect(400, 235);
	Visible();

	{
		ChattingMenuUI* ui = new ChattingMenuUI;
		if (ui->Initialize(0, 205) == false)
		{
			return false;
		}
		ui->SetLButtonClickCallback(ChangeChattingMenu, "모두에게");

		AddChildUI("ChattingMenuUI", ui);
	}

	ChattingInputUI* ui = new ChattingInputUI;
	if (ui->Initialize(105, 205) == false)
	{
		return false;
	}
	AddChildUI("ChattingInputUI", ui);

	SetPosition(0, 420);
	ui->SetCarrotPos();

	return true;
}

void ChattingBox::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	{
		ChattingInputUI* ui = static_cast<ChattingInputUI*>(FindChildUIs("ChattingInputUI").front());
		if (ui->GetIsOpen() == true)
		{
			FindChildUIs("ChattingMenuUI").front()->Visible();
		}
		else
		{
			FindChildUIs("ChattingMenuUI").front()->NotVisible();
		}
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void ChattingBox::Render()
{
	// 보이는 것만 렌더
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mRect.first;
	pos.bottom = pos.top + mRect.second;

	GET_INSTANCE(GraphicEngine)->RenderFillRectangle(pos, "밝은검은색");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "파란색");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}
	
	//mChattingLogMtx.lock();
	int chattingLogSize = mChattingLog.size();
	//mChattingLogMtx.unlock();
	for (int i = 0; i < chattingLogSize; ++i)
	{
		int x = mPos.first;
		//int y = mPos.second + 200 - (GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize * (mChattingLog.size() - i));
		int y = mPos.second + 200 - ((GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize + 1) * (chattingLogSize - i));

		int left = x;
		int top = y;
		int right = left + 400;
		int bottom = top + (GET_INSTANCE(GraphicEngine)->GetFont("메이플").fontSize) + 1;
		if (CheckContain(left, top, right, bottom) == false)
		{
			continue;
		}

		//mChattingLogMtx.lock();
		std::wstring chatting = std::to_wstring(mChattingLog[i].id) + L" : " + mChattingLog[i].chatting;
		//mChattingLogMtx.unlock();
		GET_INSTANCE(GraphicEngine)->RenderText(chatting, x, y, "메이플", "흰색");
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ChattingBox::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + 400;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + 235;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void ChattingBox::MouseOver()
{
}

void ChattingBox::MouseLButtonDown()
{
}

void ChattingBox::MouseLButtonUp()
{
}

void ChattingBox::MouseLButtonClick()
{
	std::cout << "채팅창 클릭" << std::endl;
}

bool ChattingBox::CheckContain(int left, int top, int right, int bottom)
{
	D2D1_RECT_F rect;
	rect.left = mPos.first;
	rect.top = mPos.second;
	rect.right = rect.left + 400;
	rect.bottom = rect.top + 200;

	if (left < rect.left || right > rect.right || top < rect.top || bottom > rect.bottom)
	{
		return false;
	}

	return true;
}

void ChattingBox::OpenChattingBox()
{
	ChattingInputUI* ui = static_cast<ChattingInputUI*>(FindChildUIs("ChattingInputUI").front());
	ui->OpenChattingBox();
}

void ChattingBox::AddChattingLog(int id, const wchar_t* chatting)
{
	mChattingLog.emplace_back(ChattingLog(id, chatting, 0, 0));
}
