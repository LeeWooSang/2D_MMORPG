#include "SkillUI.h"
#include "../../../Common/Utility.h"
#include "../../../Resource/ResourceManager.h"
#include "../../../Resource/Texture/Texture.h"
#include "../../../Input/Input.h"
#include "../ButtonUI/ButtonUI.h"
#include "../../../GraphicEngine/GraphicEngine.h"
#include "../../Skill/Skill.h"

#include "../../../Manager/SceneMangaer/SceneManager.h"
#include "../../../Scene/InGameScene/InGameScene.h"

void SkillLevelUP(const std::string& skillName);

SkillUI::SkillUI()
	: UI()
{
	mOpen = false;
	mSkillPoint = 0;
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

	AddSkillSlot("레이징 블로우", 123123);
	AddSkillSlot("인사이징", 123124);
	AddSkillSlot("매직 크래쉬", 123125);
	AddSkillSlot("인레이지", 123126);
	AddSkillSlot("메이플 용사", 123127);
	AddSkillSlot("용사의 의지", 123128);
	AddSkillSlot("어드밴스드 콤보", 123129);
	AddSkillSlot("컴뱃 마스터리", 123130);
	AddSkillSlot("스탠스", 123131);
	AddSkillSlot("어드밴스드 파이널 어택", 123132);


	SetPosition(400, 150);

	mSkillPoint = 100;

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
	GET_INSTANCE(GraphicEngine)->RenderText(std::to_wstring(mSkillPoint), mPos.first + 5, mPos.second + 27, "검은색");
	GET_INSTANCE(GraphicEngine)->RenderText(L"궁극의 히어로", mPos.first - 100, mPos.second + 58, "흰색");
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
		std::string name = mSkill->GetName();
		if (name.length() > 18)
		{
			name.clear();
			//for (int i = 0; i < 9;)
			//{
			//	name[i] = mSkill->GetName()[i];
			//}
			name = "어드밴스드 파이..";
		}
		GET_INSTANCE(GraphicEngine)->RenderText(StringToWString(name), mPos.first + 40, mPos.second);
		GET_INSTANCE(GraphicEngine)->RenderText(std::to_wstring(mSkill->GetSkillLevel()), mPos.first + 40, mPos.second + 18);
	}

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

		mSkill = new Skill;
		mSkill->Initialize(0, 0);
		mSkill->SetName(name);
		if (v.size() > 1)
		{
			mSkill->AddEffect(texId);
		}
	}
	// 스킬 레벨업 버튼 추가
	{
		TextureData& data = GET_INSTANCE(ResourceManager)->GetTextureData("SkillUIButton50");
		ButtonUI* ui = new ButtonUI;
		
		int x = mOriginX + 125;
		int y = mOriginY + 20;
		if (ui->Initialize(x, y) == false)
		{
			return;
		}
		ui->SetTexture(data.name);
		ui->Visible();
		mParentUI->AddChildUI("Button", ui);
		ui->SetLButtonClickCallback(SkillLevelUP, name);
	}
}

void SkillLevelUP(const std::string& skillName)
{
	SkillUI* ui = static_cast<SkillUI*>(GET_INSTANCE(SceneManager)->FindScene(SCENE_TYPE::INGAME_SCENE)->FindUI("SkillUI"));
	int skillPoint = ui->GetSkillPoint();
	if (skillPoint > 0)
	{
		ui->SetSkillPoint(--skillPoint);
		Skill* skill = ui->FindSkill(skillName);
		if (skill == nullptr)
		{
			return;
		}

		int skillLevel = skill->GetSkillLevel();
		skill->SetSkillLevel(++skillLevel);
	}
}