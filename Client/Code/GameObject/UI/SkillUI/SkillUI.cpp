#include "SkillUI.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../Skill/Skill.h"

SkillUI::SkillUI()
	: UI()
{
	mOpen = false;
}

SkillUI::~SkillUI()
{
}

bool SkillUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("SkillUIBackground0");
		SetTexture(data.name);
	}
	for(int i = 1; i < 5; ++i)
	{
		std::string name = "SkillUIBackground" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Background", ui);
	}

	for(int i = 0; i < 7; ++i)
	{
		std::string name = "SkillUITab";
		if (i == 4)
		{
			name += "40";
		}
		else
		{
			name += std::to_string(i) + "2";
		}

		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Tab", ui);
	}

	for (int i = 0; i < 5; ++i)
	{
		std::string name = "SkillUIButton" + std::to_string(i);
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData(name);
		ButtonUI* ui = new ButtonUI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return false;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		AddChildUI("Button", ui);
	}

	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("SkillUISlot00");
		int gap = 3;
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				int originX = data.origin.first + (j * (data.size.first + gap));
				int originY = data.origin.second + (i * (data.size.second + gap));

				SkillSlotUI* ui = new SkillSlotUI;
				if (ui->Initialize(originX, originY) == false)
				{
					return false;
				}
				ui->SetTexture(data.name);
				ui->Visible();
				AddChildUI("Slot", ui);
			}
		}
	}
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("SkillUISlot1");
		int gap = 3;

		for (int i = 0; i < 2; ++i)
		{
			int originX = data.origin.first + (i * (data.size.first + gap));
			int originY = data.origin.second;

			UI* ui = new UI;
			if (ui->Initialize(originX, originY) == false)
			{
				return false;
			}
			ui->SetTexture(data.name);
			ui->Visible();
			AddChildUI("EmptySlot", ui);
		}
	}

	SetPosition(400, 150);

	return true;
}

void SkillUI::Update()
{
	if (!(mAttr & ATTR_STATE_TYPE::VISIBLE))
	{
		return;
	}

	std::pair<int, int> mousePos = GET_INSTANCE(Input)->GetMousePos();
	MouseOverCollision(mousePos.first, mousePos.second);
	if (mMouseLButtonDown == true)
	{
		Move(mousePos.first, mousePos.second);
	}
	
	if (mMouseLButtonClick == true)
	{
		//UI* ui = mChildUIs["Icon"].front();
		//ui->Click(mousePos.first, mousePos.second);
		//ui->Move(mousePos.first, mousePos.second);
	}

	for (auto& child : mChildUIs)
	{
		for (int i = 0; i < child.second.size(); ++i)
		{
			child.second[i]->Update();
		}
	}
}

void SkillUI::Render()
{
	UI::Render();
	GET_INSTANCE(GraphicEngine)->RenderText(L"999", mPos.first + 205, mPos.second + 27);
	GET_INSTANCE(GraphicEngine)->RenderText(L"궁극의 히어로", mPos.first + 125, mPos.second + 58, "흰색");
}

void SkillUI::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + mTexture->GetSize().first;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + mTexture->GetSize().second;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void SkillUI::MouseOver()
{
	UI::MouseOver();
}

void SkillUI::MouseLButtonDown()
{
	UI::MouseLButtonDown();
}

void SkillUI::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void SkillUI::MouseLButtonClick()
{
	UI::MouseLButtonClick();
}

void SkillUI::OpenSkillUI()
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

void SkillUI::AddSkillSlot(const std::string& name, int texId)
{
	for (auto& skillSlot : mChildUIs["Slot"])
	{
		SkillSlotUI* slot = static_cast<SkillSlotUI*>(skillSlot);
		// 비어있는 슬롯에 추가
		if (slot->GetEmpty() == true)
		{
			slot->AddSkill(name, texId);
			break;
		}
	}
}

Skill* SkillUI::FindSkill(const std::string& name)
{
	for (auto& skillSlot : mChildUIs["Slot"])
	{
		SkillSlotUI* slot = static_cast<SkillSlotUI*>(skillSlot);
		// 비어있는 슬롯에 추가
		if (slot->GetEmpty() == true)
		{
			continue;
		}

		Skill* skill = slot->GetSkill();
		if (skill == nullptr)
		{
			continue;
		}

		if (skill->GetName() == name)
		{
			return skill;
		}
	}

	return nullptr;
}

SkillSlotUI::SkillSlotUI()
	: UI()
{
	mEmpty = true;
	mSkill = nullptr;
}

SkillSlotUI::~SkillSlotUI()
{
	if (mSkill != nullptr)
	{
		delete mSkill;
		mSkill = nullptr;
	}
}

bool SkillSlotUI::Initialize(int x, int y)
{
	UI::Initialize(x, y);

	return true;
}

void SkillSlotUI::Update()
{
	UI::Update();

	if (mSkill != nullptr)
	{
		mSkill->Update();
	}
}

void SkillSlotUI::Render()
{
	UI::Render();

	if (mSkill != nullptr)
	{
		mSkill->Render();
	}
}

void SkillSlotUI::MouseOverCollision(int x, int y)
{
	// ui의 rect와 닿았는가?
	RECT collisionBox;
	collisionBox.left = mPos.first;
	collisionBox.right = mPos.first + mTexture->GetSize().first;
	collisionBox.top = mPos.second;
	collisionBox.bottom = mPos.second + mTexture->GetSize().second;

	if (x >= collisionBox.left && x <= collisionBox.right && y >= collisionBox.top && y <= collisionBox.bottom)
	{
		mMouseOver = true;
	}
	else
	{
		mMouseOver = false;
	}
}

void SkillSlotUI::MouseOver()
{
	UI::MouseOver();
}

void SkillSlotUI::MouseLButtonDown()
{
	UI::MouseLButtonDown();
}

void SkillSlotUI::MouseLButtonUp()
{
	UI::MouseLButtonUp();
}

void SkillSlotUI::MouseLButtonClick()
{
	UI::MouseLButtonClick();
}

void SkillSlotUI::AddSkill(const std::string& name, int texId)
{
	mEmpty = false;

	// 스킬 아이콘 추가
	{
		int index = -1;
		std::vector<TextureData>& v = GET_INSTANCE(ResourceManager)->GetTextureDatas(texId);
		for (int i = 0; i < v.size(); ++i)
		{
			if (v[i].icon == true)
			{
				index = i;
				break;
			}
		}
		if (index == -1)
		{
			return;
		}

		UI* ui = new UI;
		if (ui->Initialize(v[index].origin.first, v[index].origin.second) == false)
		{
			return;
		}
		ui->SetTexture(v[index].name);
		ui->Visible();
		mParentUI->AddChildUI("Icon", ui);
		ui->SetPosition(mPos.first, mPos.second);

		if (v.size() > 1)
		{
			mSkill = new Skill;
			mSkill->Initialize(0, 0);
			mSkill->SetName(name);
			mSkill->AddEffect(texId);
		}
	}
	// 스킬 레벨업 버튼 추가
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("SkillUIButton50");
		UI* ui = new UI;
		if (ui->Initialize(data.origin.first, data.origin.second) == false)
		{
			return;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		mParentUI->AddChildUI("Button", ui);
		ui->SetPosition(mPos.first, mPos.second);
	}
}
