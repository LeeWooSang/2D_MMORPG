#pragma once
#include "../UI.h"

enum class EQUIP_SLOT_TYPE
{
	RING,
	POCKET,
	PENDANT,
	WEAPON,
	BELT,
	CAP,
	FACE_ACC,
	EYE_ACC,
	TOP_BODY,
	BOTTOM_BODY,
	SHOES,
	EAR_ACC,
	SHOULDER,
	GLOVES,
	ANDROID,
	EMBLEM,
	BADGE,
	MEDAL,
	SUB_WEAPON,
	CAPE,
	HEART
};

class InventoryItem;
class EquipUI : public UI
{
public:
	EquipUI();
	virtual ~EquipUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	void OpenEquipUI();
	void AddEquipItem(int slot, int texId);
	InventoryItem* AddEquipItem(InventoryItem* item);

private:
	bool mOpen;
};

class EquipSlotUI : public UI
{
public:
	EquipSlotUI();
	virtual ~EquipSlotUI();
	virtual bool Initialize(int x, int y);
	virtual void Update();
	virtual void Render();

	virtual void MouseOver();
	virtual void MouseLButtonDown();
	virtual void MouseLButtonUp();
	virtual void MouseLButtonClick();

	virtual void SetPosition(int x, int y);

	InventoryItem* AddItem(InventoryItem* newItem);
	void SetSlotType(EQUIP_SLOT_TYPE type) { mType = type; }

private:
	EQUIP_SLOT_TYPE mType;
	InventoryItem* mItem;
};