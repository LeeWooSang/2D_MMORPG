#include "ChattingMenuUI.h"
#include "../../../../Input/Input.h"
#include "../../../../GraphicEngine/GraphicEngine.h"
#include "../../../../Resource/Texture/Texture.h"

std::vector<ChattingType> ChattingMenuUI::mChattingTypes;
int ChattingMenuUI::mCurrentType;

ChattingMenuUI::ChattingMenuUI()
	: ButtonUI()
{
	mChattingTypes.reserve(2);
	mCurrentType = 0;
}

ChattingMenuUI::~ChattingMenuUI()
{
}

bool ChattingMenuUI::Initialize(int x, int y)
{
	ButtonUI::Initialize(x, y);
	mChattingTypes.emplace_back(ChattingType(CHATTING_TYPE::BROADCASTING, L"��ο���"));
	mChattingTypes.emplace_back(ChattingType(CHATTING_TYPE::BROADCASTING, L"��Ƽ����"));
	mChattingTypes.emplace_back(ChattingType(CHATTING_TYPE::WHISPERING, L"�ӼӸ�"));

	SetTexture("Slot");

	return true;
}

void ChattingMenuUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
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

void ChattingMenuUI::Render()
{
	// ���̴� �͸� ����
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	D2D1_RECT_F pos;
	pos.left = mPos.first;
	pos.top = mPos.second;
	pos.right = pos.left + mTexture->GetSize().first;
	pos.bottom = pos.top + mTexture->GetSize().second;

	if (mMouseLButtonDown)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos, "�Ķ���");
	}
	else if (mMouseOver)
	{
		GET_INSTANCE(GraphicEngine)->RenderRectangle(pos);
	}

	GET_INSTANCE(GraphicEngine)->RenderTexture(mTexture, pos);
	GET_INSTANCE(GraphicEngine)->RenderText(mChattingTypes[mCurrentType].name, mPos.first, mPos.second, "������", "������");

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Render();
		}
	}
}

void ChattingMenuUI::MouseOver()
{
	UI::MouseOver();
}

void ChattingMenuUI::MouseLButtonDown()
{
	UI::MouseLButtonDown();
}

void ChattingMenuUI::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void ChattingMenuUI::MouseLButtonClick()
{
	ButtonUI::MouseLButtonClick();
}

void ChattingMenuUI::ChangeType()
{
	++mCurrentType;
	if (mCurrentType >= mChattingTypes.size())
	{
		mCurrentType = 0;
	}
}
