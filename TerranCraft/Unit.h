#pragma once

#include "../Common/typedef.h"
#include "../BWLib/UnitType.h"
#include "CommandIcon.h"
#include "../BWLib/IScriptAnimation.h"
#include "../BWLib/OrderType.h"
#include <list>

class Sprite;
class Image;
class Weapon;
class Order;

class Unit final
{
public:
	Unit() = default;
	~Unit();

	bool Initialize(BW::UnitType unitType);
	void Cleanup();

	void Update();

	int32 GetHP() const { return mHP; }
	void SetHP(int32 hp) { mHP = hp; }
	Sprite* GetSprite() const { return mSprite; }
	void SetSprite(Sprite* sprite) { mSprite = sprite; }

	IntVector2 GetMoveTarget() const { return mMoveTarget; }
	void SetMoveTarget(IntVector2 moveTarget) { mMoveTarget = moveTarget; }
	IntVector2 GetNextMovementWaypoint() const { return mNextMovementWaypoint; }
	void SetNextMovementWaypoint(IntVector2 nextMovementWaypoint) { mNextMovementWaypoint = nextMovementWaypoint; }
	int32 GetCurrentSpeed() const { return mCurrentSpeed; }
	uint8 GetFacingDirection() const { return mFacingDirection; }
	void SetFacingDirection(uint8 facingDirection) { mFacingDirection = facingDirection; }
	FloatVector2 GetCurrentVelocity() const { return mCurrentVelocity; }
	void SetCurrentVelocity(FloatVector2 currentVelocity) { mCurrentVelocity = currentVelocity; }

	BW::UnitType GetUnitType() const { return mUnitType; }
	FloatVector2 GetPosition() const { return mPosition; }
	void SetPosition(FloatVector2 position) { mPosition = position; }
	uint8 GetGroundWeaponCooldown() const { return mGroundWeaponCooldown; }
	void SetGroundWeaponCooldown(uint8 groundWeaponCooldown) { mGroundWeaponCooldown = groundWeaponCooldown; }
	IntRect GetContourBounds() const { return mContourBounds; }
	eButtonset GetCurrentButtonset() const { return mCurrentButtonset; }

	std::list<IntVector2>* GetPath() { return &mPath; }

	BW::IScriptAnimation GetAnimation() const;
	bool IsMoving() const;
	bool IsAttacking() const;

	// 
	bool IsNobrkcode() const { return mbNobrkcode; }
	void SetNobrkcode(bool value) { mbNobrkcode = value; }
	bool IsAttackable() const { return mbAttackable; }
	void SetAttackable(bool value) { mbAttackable = value; }
	void AddOrder(Order* order) { mOrders.push_front(order); }
	Order* GetFrontOrder() const { return mOrders.empty() ? nullptr : mOrders.front(); }
	void RemoveFrontOrder() { if (!mOrders.empty()) mOrders.pop_front(); }
	void ClearOrders()
	{
		auto iter = mOrders.begin();
		while (iter != mOrders.end())
		{
			Order* order = *iter;
			delete order;
			iter = mOrders.erase(iter);
		}
	}

	int32 GetMaxHP() const;
	uint8 GetScriptID() const;

private:
	// Tingy
	int32 mHP = 0;
	Sprite* mSprite = nullptr;

	// Flingy
	IntVector2 mMoveTarget = { 0, };
	IntVector2 mNextMovementWaypoint = { 0, };
	int32 mCurrentSpeed = 4;
	uint8 mFacingDirection = 0;
	FloatVector2 mCurrentVelocity = { 0, };
	FloatVector2 mPosition = { 0, };

	uint8 mGroundWeaponCooldown = 0;
	int32 mCoolTime = 0;

	// Unit
	BW::UnitType mUnitType = BW::UnitType::None;
	IntRect mContourBounds = { 0, };
	eButtonset mCurrentButtonset = eButtonset::Terran_Marine;

	std::list<IntVector2> mPath;
	std::list<Order*> mOrders;

	bool mbNobrkcode = false;
	bool mbAttackable = true;
};
