#include "ChattingBox.h"
#include "../../../Input/Input.h"
#include "../../../GraphicEngine/GraphicEngine.h"

ChattingBox::ChattingBox()
	: UI()
{
	mOpen = false;
}

ChattingBox::~ChattingBox()
{
}

bool ChattingBox::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	SetPosition(0, 500);
	return true;
}

void ChattingBox::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);

	if (mOpen == true)
	{
		processInput();
	}

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
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + 400;
	pos.bottom = pos.top + 200;

	GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "���");

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	std::wstring chatting = L"";
	for(auto& chat : mChattings)
	{
		chatting += chat;
	}
	GET_INSTANCE(GraphicEngine)->RenderText(chatting, mPos.first, mPos.second, "������", "���");
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
	// ui�� rect�� ��Ҵ°�?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + 400;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + 200;

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
	std::cout << "ä��â Ŭ��" << std::endl;
}

void ChattingBox::OpenChattingBox()
{
	// �����ִٸ�
	if (mOpen == false)
	{
		mOpen = true;
		Visible();
	}
	// �����ִٸ�
	else
	{
		mOpen = false;
		NotVisible();
	}
}

void ChattingBox::processInput()
{
	for (int i = KEY_TYPE::A_KEY; i <= KEY_TYPE::Z_KEY; ++i)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(static_cast<KEY_TYPE>(i)) == true)
		{
			mChattings.emplace_back(i + 76);
		}
	}

	if (mChattings.size() > 0)
	{
		if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::LEFT_KEY) == true)
		{
			mChattings.pop_back();
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::RIGHT_KEY) == true)
		{
			mChattings.pop_back();
		}
		else if (GET_INSTANCE(Input)->KeyOnceCheck(KEY_TYPE::BACK_SPACE) == true)
		{
			mChattings.pop_back();
		}


	}
}
