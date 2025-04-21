#pragma once

#include "../Common/typedef.h"
#include "../BWLib/UnitType.h"
#include "CommandIcon.h"
#include "../BWLib/IScriptAnimation.h"
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

	bool Initialize(BW::UnitType unitType);
	void Cleanup();

	virtual void Update() override;
	void PerformOrder();

	BW::UnitType GetUnitType() const { return mUnitType; }
	uint8 GetGroundWeaponCooldown() const { return mGroundWeaponCooldown; }
	void SetGroundWeaponCooldown(uint8 groundWeaponCooldown) { mGroundWeaponCooldown = groundWeaponCooldown; }
	IntRect GetContourBounds() const { return mContourBounds; }
	eButtonset GetCurrentButtonset() const { return mCurrentButtonset; }

	std::list<IntVector2>* GetPath() { return &mPath; }

	BW::IScriptAnimation GetAnimation() const;
	bool IsMoving() const;
	bool IsAttacking() const;

	void AddOrder(Order* order) { mOrderQueue.push_back(order); }
	void RemoveOrder() { mOrderQueue.pop_front(); }
	Order* GetFrontOrder() const { return (mOrderQueue.empty()) ? nullptr : mOrderQueue.front(); }
	void ClearOrders();
	bool IsOrderQueueEmpty() const { return mOrderQueue.empty(); }
	void SetStandby() { mOrderType = BW::OrderType::None; }

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
	BW::UnitType mUnitType = BW::UnitType::None;
	IntRect mContourBounds = { 0, };
	eButtonset mCurrentButtonset = eButtonset::Terran_Marine;
	uint16 mHPGain = 0;
	uint16 mBuildTime = 0;
	uint16 mRemainingBuildTime = 0;
	Image* mConstructionImage = nullptr;

	std::list<IntVector2> mPath;
	std::list<Order*> mOrderQueue;
	BW::OrderType mOrderType = BW::OrderType::None;
	Target mOrderTarget = { 0, };

	uint32 mSpecialAbilityFlags = 0;
	uint8 mMovementFlags = 0;
};
