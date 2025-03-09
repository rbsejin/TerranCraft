#pragma once

#include "../Common/typedef.h"
#include "../BWLib/UnitType.h"
#include "CommandIcon.h"
#include <list>

class Sprite;
class Image;

class Unit final
{
public:
	Unit() = default;
	~Unit();

	bool Initialize(BW::UnitType unitType);
	void Cleanup();

	int32 GetHP() const { return mHP; }
	Sprite* GetSprite() const { return mSprite; }

	IntVector2 GetMoveTarget() const { return mMoveTarget; }
	void SetMoveTarget(IntVector2 moveTarget) { mMoveTarget = moveTarget; }
	IntVector2 GetNextMovementWaypoint() const { return mNextMovementWaypoint; }
	void SetNextMovementWaypoint(IntVector2 nextMovementWaypoint) { mNextMovementWaypoint = nextMovementWaypoint; }
	int32 GetCurrentSpeed() const { return mCurrentSpeed; }
	uint8 GetDirection() const { return mDirection; }
	FloatVector2 GetCurrentVelocity() const { return mCurrentVelocity; }
	void SetCurrentVelocity(FloatVector2 currentVelocity) { mCurrentVelocity = currentVelocity; }

	BW::UnitType GetUnitType() const { return mUnitType; }
	FloatVector2 GetPosition() const { return mPosition; }
	void SetPosition(FloatVector2 position) { mPosition = position; }
	IntRect GetCountourBounds() const { return mCountourBounds; }
	eButtonset GetCurrentButtonset() const { return mCurrentButtonset; }

	std::list<IntVector2>* GetPath() { return &mPath; }

private:
	// Tingy
	int32 mHP = 40;
	Sprite* mSprite = nullptr;

	// Flingy
	IntVector2 mMoveTarget = { 0, };
	IntVector2 mNextMovementWaypoint = { 0, };
	int32 mCurrentSpeed = 4;
	uint8 mDirection = 0;
	FloatVector2 mCurrentVelocity = { 0, };

	// Unit
	BW::UnitType mUnitType = BW::UnitType::None;
	FloatVector2 mPosition = { 16, 16 };
	IntRect mCountourBounds = { 8, 8, 9, 10 };
	eButtonset mCurrentButtonset = eButtonset::Marine;

	std::list<IntVector2> mPath;
};

