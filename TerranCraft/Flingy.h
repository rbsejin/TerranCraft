#pragma once

#include "Thingy.h"
#include "../Common/typedef.h"
#include "../BWLib/FlingyType.h"
#include "../BWLib/MovementFlags.h"
#include "Target.h"

class Flingy : public Thingy
{
public:
	Flingy() = default;
	virtual ~Flingy();
	bool Initialize(int32 hp, eFlingy flingyType);
	void Cleanup();

	virtual void Update();

	Target GetMoveTarget() const { return mMoveTarget; }
	void SetMoveTarget(Target moveTarget) { mMoveTarget = moveTarget; }
	Int32Vector2 GetNextMovementWaypoint() const { return mNextMovementWaypoint; }
	void UpdateNextMovementWaypoint(Int32Vector2 nextMovementWaypoint);
	int32 GetFlingyTopSpeed() const { return mFlingyTopSpeed; }
	float GetCurrentSpeed() const { return mCurrentSpeed; }
	uint8 GetFacingDirection() const { return mFacingDirection; }
	void SetFacingDirection(uint8 facingDirection) { mFacingDirection = facingDirection; }
	FloatVector2 GetPosition() const { return mPosition; }
	void SetPosition(FloatVector2 position) { mPosition = position; }

	// 
	bool IsNobrkcode() const { return mbNobrkcode; }
	void SetNobrkcode(bool value) { mbNobrkcode = value; }
	bool IsRepeatAttackable() const { return mbRepeatAttackable; }
	void SetRepeatAttackable(bool value) { mbRepeatAttackable = value; }

protected:
	eFlingy mFlingyType = eFlingy::None;
	Target mMoveTarget = { 0, };
	Int32Vector2 mNextMovementWaypoint = { 0, };
	uint8 mMovementFlags = 0;
	int32 mFlingyTopSpeed = 0;
	float mCurrentSpeed = 0.f;
	uint8 mFacingDirection = 0;
	FloatVector2 mCurrentOrientation = { 0, };
	FloatVector2 mPosition = { 0, };

	bool mbNobrkcode = false;
	bool mbRepeatAttackable = false;
};

