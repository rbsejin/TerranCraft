#pragma once

#include "../Common/typedef.h"
#include "../BWLib/UnitType.h"
#include "../BWLib/ButtonsetType.h"
#include "../BWLib/AnimType.h"
#include "../BWLib/OrderType.h"
#include "Flingy.h"
#include <list>
#include "../BWLib/FlingyType.h"
#include "Target.h"

class Sprite;
class Image;
class Weapon;
class Order;

class Unit final : public Flingy
{
public:
	Unit() = default;
	virtual ~Unit();

	bool Initialize(eUnit unitType);
	void Cleanup();

	virtual void Update() override;
	void PerformOrder();

	eUnit GetUnitType() const { return mUnitType; }
	uint8 GetGroundWeaponCooldown() const { return mGroundWeaponCooldown; }
	void SetGroundWeaponCooldown(uint8 groundWeaponCooldown) { mGroundWeaponCooldown = groundWeaponCooldown; }
	Int32Rect GetContourBounds() const { return mContourBounds; }
	eButtonset GetCurrentButtonset() const { return mCurrentButtonset; }

	std::list<Int32Vector2>* GetPath() { return &mPath; }

	eAnim GetAnimation() const;
	bool IsMoving() const;
	bool IsAttacking() const;

	void AddOrder(Order* order) { mOrderQueue.push_back(order); }
	void RemoveOrder() { mOrderQueue.pop_front(); }
	Order* GetFrontOrder() const { return (mOrderQueue.empty()) ? nullptr : mOrderQueue.front(); }
	void ClearOrders();
	bool IsOrderQueueEmpty() const { return mOrderQueue.empty(); }
	void SetStandby() { mOrderType = eOrder::None; }

	int32 GetMaxHP() const;
	uint8 GetFlingyID() const;
	Target GetOrderTarget() const { return mOrderTarget; }

	bool IsBuilding() const { return mSpecialAbilityFlags & 0x0001; }

private:
	void startMove();
	void move();
	void startAttackUnit();
	void attackUnit();
	void startBuilding();
	void build();
	void lookAt(FloatVector2 targetPosition);

private:
	uint8 mGroundWeaponCooldown = 0;
	int32 mCoolTime = 0;

	// Unit
	eUnit mUnitType = eUnit::None;
	Int32Rect mContourBounds = { 0, };
	eButtonset mCurrentButtonset = eButtonset::TerranMarine;
	uint16 mHPGain = 0;
	uint16 mBuildTime = 0;
	uint16 mRemainingBuildTime = 0;
	Image* mConstructionImage = nullptr;

	std::list<Int32Vector2> mPath;
	std::list<Order*> mOrderQueue;
	eOrder mOrderType = eOrder::None;
	Target mOrderTarget = { 0, };

	uint32 mSpecialAbilityFlags = 0;
	uint8 mMovementFlags = 0;
};
