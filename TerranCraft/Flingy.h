#pragma once

#include "Thingy.h"
#include "../Common/typedef.h"
#include "../BWLib/FlingyType.h"
#include "../BWLib/MovementFlags.h"

class Flingy : public Thingy
{
public:
	Flingy() = default;
	virtual ~Flingy();
	bool Initialize(BW::FlingyType flingyType);
	void Cleanup();

	virtual void Update();

	IntVector2 GetMoveTarget() const { return mMoveTarget; }
	void SetMoveTarget(IntVector2 moveTarget) { mMoveTarget = moveTarget; }
	IntVector2 GetNextMovementWaypoint() const { return mNextMovementWaypoint; }
	void SetNextMovementWaypoint(IntVector2 nextMovementWaypoint) { mNextMovementWaypoint = nextMovementWaypoint; }
	int32 GetCurrentSpeed() const { return mCurrentSpeed; }
	uint8 GetFacingDirection() const { return mFacingDirection; }
	void SetFacingDirection(uint8 facingDirection) { mFacingDirection = facingDirection; }
	FloatVector2 GetCurrentVelocity() const { return mCurrentVelocity; }
	void SetCurrentVelocity(FloatVector2 currentVelocity) { mCurrentVelocity = currentVelocity; }
	FloatVector2 GetPosition() const { return mPosition; }
	void SetPosition(FloatVector2 position) { mPosition = position; }

	// 
	bool IsNobrkcode() const { return mbNobrkcode; }
	void SetNobrkcode(bool value) { mbNobrkcode = value; }
	bool IsRepeatAttackable() const { return mbRepeatAttackable; }
	void SetRepeatAttackable(bool value) { mbRepeatAttackable = value; }

protected:
	BW::FlingyType mFlingyType = BW::FlingyType::None;
	IntVector2 mMoveTarget = { 0, };
	IntVector2 mNextMovementWaypoint = { 0, };
	uint8 mMovementFlags = BW::MovementFlags::None;
	int32 mCurrentSpeed = 4;
	uint8 mFacingDirection = 0;
	FloatVector2 mCurrentVelocity = { 0, };
	FloatVector2 mPosition = { 0, };

	bool mbNobrkcode = false;
	bool mbRepeatAttackable = false;
};

