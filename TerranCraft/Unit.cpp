#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "Image.h"
#include "ResourceManager.h"
#include "BWFile.h"
#include "AnimationController.h"
#include "Order.h"
#include "Bullet.h"
#include "Game.h"
#include "PathFinder.h"
#include "../BWLib/ButtonsetType.h"
#include "../BWLib/SpecialAbilityFlags.h"
#include "../Util/Util.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(eUnit unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	ResourceManager* resourceManager = gGame->GetResourceManager();
	const UnitData* unitData = resourceManager->GetUnitData();

	int32 hp = unitData->HitPoints[(uint32)mUnitType] >> 8;
	uint8 flingyID = unitData->Graphics[(uint32)mUnitType];
	Flingy::Initialize(hp, (eFlingy)flingyID);

	Sprite* sprite = GetSprite();
	sprite->SetElevationLevel(unitData->ElevationLevels[(uint32)unitType]);

	mCurrentButtonset = (eButtonset)unitType;

	Int16Rect contourBounds = unitData->UnitDimensions[(uint32)unitType];
	mContourBounds.Left = (int32)contourBounds.Left;
	mContourBounds.Top = (int32)contourBounds.Top;
	mContourBounds.Right = (int32)contourBounds.Right;
	mContourBounds.Bottom = (int32)contourBounds.Bottom;

	mSpecialAbilityFlags = unitData->SpecialAbilityFlags[(uint32)unitType];
	mMovementFlags = unitData->Unknowns[(uint32)unitType];

	// Weapon
	const WeaponData* weaponData = resourceManager->GetWeaponData();
	mGroundWeaponCooldown = weaponData->WeaponCooldowns[(uint32)unitType];

	mBuildTime = unitData->BuildTimes[(uint32)unitType];
	mRemainingBuildTime = mBuildTime;

	mTargetAcquisitionRange = unitData->TargetAcquisitionRanges[(uint32)unitType];
	mSightRange = unitData->SightRanges[(uint32)unitType];

	bResult = true;

	return bResult;
}

void Unit::Cleanup()
{
	for (Image* tempImage : mTempImages)
	{
		delete tempImage;
	}
	mTempImages.clear();

	for (Order* order : mOrderQueue)
	{
		delete order;
	}
	mOrderQueue.clear();

	Flingy::Cleanup();
}

void Unit::Update()
{
	if (!mbNobrkcode)
	{
		switch (mOrderType)
		{
		case eOrder::None:
		{
			PerformOrder();
			break;
		}
		case eOrder::Move:
		{
			move();
			break;
		}
		case eOrder::AttackUnit:
		{
			if (mMoveTarget.Unit != nullptr)
			{
				const FloatVector2 targetUnitPosition = mMoveTarget.Unit->GetPosition();
				mMoveTarget.Position.X = (int32)targetUnitPosition.X;
				mMoveTarget.Position.Y = (int32)targetUnitPosition.Y;
			}

			if (canAttackTarget() || mCoolTime > 0)
			{
				if (!IsAttacking())
				{
					startAttackUnit();
				}

				attackUnit();
			}
			else
			{
				moveToAttack();
			}
			break;
		}
		case eOrder::AttackMove:
		{
			// TODO: 추후 attackMove() 함수 구현해서 대체할 것...
			move();
			break;
		}
		case eOrder::ConstructingBuilding:
		{
			build();
			break;
		}
		default:
			break;
		}
	}

	Sprite* sprite = GetSprite();
	Int32Vector2 spritePosition = { (int32)mPosition.X, (int32)mPosition.Y };
	sprite->SetPosition(spritePosition);

	const std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		eImage imageID = image->GetImageID();
		if (image->HasRotationFrames())
		{
			image->SetDirection(mFacingDirection);
		}
	}
}

void Unit::PerformOrder()
{
	if (mOrderQueue.empty())
	{
		return;
	}

	Order* order = mOrderQueue.front();

#ifdef _DEBUG
	if (order == nullptr)
	{
		__debugbreak();
	}
#endif

	// 명령의 타깃이 유닛 리스트에 없을 경우 처리... (예: 유닛이 죽었을 경우)
	if (order->Target.Unit != nullptr)
	{
		auto iter = std::find(gGame->Thingies.begin(), gGame->Thingies.end(), order->Target.Unit);
		if (iter == gGame->Thingies.end())
		{
			goto LB_RETURN;
		}
	}

#pragma region Order
	eOrder orderType = order->OrderType;
	mOrderTarget = order->Target;
	mOrderType = orderType;

	switch (orderType)
	{
	case eOrder::Die:
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			image->UpdateAnim(eAnim::Death);
		}

		gGame->Units.erase(std::remove(gGame->Units.begin(), gGame->Units.end(), this));
		gGame->SelectedUnits.erase(std::remove(gGame->SelectedUnits.begin(), gGame->SelectedUnits.end(), this), gGame->SelectedUnits.end());
		break;
	}
	case eOrder::Stop:
	{
		Sprite* sprite = GetSprite();
		Image* primaryImage = sprite->GetPrimaryImage();
		eAnim anim = primaryImage->GetAnim();

		switch (anim)
		{
		case eAnim::Init:
			break;
		case eAnim::Death:
			break;
		case eAnim::GndAttkInit:
			anim = eAnim::GndAttkToIdle;
			break;
		case eAnim::AirAttkInit:
			anim = eAnim::AirAttkToIdle;
			break;
		case eAnim::Unused1:
			break;
		case eAnim::GndAttkRpt:
			anim = eAnim::GndAttkToIdle;
			break;
		case eAnim::AirAttkRpt:
			anim = eAnim::AirAttkToIdle;
			break;
		case eAnim::CastSpell:
			break;
		case eAnim::GndAttkToIdle:
			break;
		case eAnim::AirAttkToIdle:
			break;
		case eAnim::Unused2:
			break;
		case eAnim::Walking:
			anim = eAnim::WalkingToIdle;
			break;
		case eAnim::WalkingToIdle:
			break;
		case eAnim::SpecialState1:
			break;
		case eAnim::SpecialState2:
			break;
		case eAnim::AlmostBuilt:
			break;
		case eAnim::Built:
			break;
		case eAnim::Landing:
			break;
		case eAnim::LiftOff:
			break;
		case eAnim::IsWorking:
			break;
		case eAnim::WorkingToIdle:
			break;
		case eAnim::WarpIn:
			break;
		case eAnim::Unused3:
			break;
		case eAnim::StarEditInit:
			break;
		case eAnim::Disable:
			break;
		case eAnim::Burrow:
			break;
		case eAnim::UnBurrow:
			break;
		case eAnim::Enable:
			break;
		default:
			break;
		}

		if (anim != eAnim::WalkingToIdle)
		{
			primaryImage->UpdateAnim(anim);
		}
		else
		{
			std::list<Image*>* images = sprite->GetImages();

			for (Image* image : *images)
			{
				uint16 offsetCount = image->GetIScriptOffsetCount();
				if (offsetCount >= 12)
				{
					image->UpdateAnim(eAnim::WalkingToIdle);
				}
			}
		}

		mCoolTime = 0;
		SetRepeatAttackable(true);
		mOrderType = eOrder::None;
		break;
	}
	case eOrder::Move:
	{
		mMoveTarget = mOrderTarget;
		startMove();
		break;
	}
	case eOrder::Attack1:
		break;
	case eOrder::AttackUnit:
	{
		mMoveTarget = mOrderTarget;

		if (canAttackTarget() || mCoolTime > 0)
		{
			startAttackUnit();
		}
		else
		{
			startMoveToAttack();
		}
		break;
	}
	case eOrder::AttackMove:
		// TODO: 추후 startAttackMove() 함수 구현해서 대체할 것...
		mMoveTarget = mOrderTarget;
		startMove();
		break;
	case eOrder::ConstructingBuilding:
		startBuilding();
		break;
	case eOrder::Nothing:
		break;
	default:
		break;
	}
#pragma endregion

	LB_RETURN:
	mOrderQueue.pop_front();
	delete order;
}

bool Unit::IsMoving() const
{
	Sprite* sprite = GetSprite();
	eAnim anim = sprite->GetPrimaryImageAnim();
	return anim == eAnim::Walking || anim == eAnim::WalkingToIdle;
}

bool Unit::IsAttacking() const
{
	Sprite* sprite = GetSprite();
	eAnim anim = sprite->GetPrimaryImageAnim();
	return anim == eAnim::GndAttkInit || anim == eAnim::GndAttkRpt
		|| anim == eAnim::AirAttkInit || anim == eAnim::AirAttkRpt;
}

void Unit::ClearOrders()
{
	for (Order* order : mOrderQueue)
	{
		delete order;
	}
	mOrderQueue.clear();
}

int32 Unit::GetMaxHP() const
{
	ResourceManager* resourceManager = gGame->GetResourceManager();
	const UnitData* unitData = resourceManager->GetUnitData();
	return unitData->HitPoints[(uint32)mUnitType] >> 8;
}

uint8 Unit::GetFlingyID() const
{
	ResourceManager* resourceManager = gGame->GetResourceManager();
	const UnitData* unitData = resourceManager->GetUnitData();
	return unitData->Graphics[(uint32)mUnitType];
}

void Unit::startMove()
{
	int32 x = mMoveTarget.Position.X;
	int32 y = mMoveTarget.Position.Y;

	Int32Vector2 nextMovementWaypoint = { x, y };

	if ((mSpecialAbilityFlags & ((uint32)eSpecialAbilityFlags::FlyingUnit | (uint32)eSpecialAbilityFlags::FlyingBuilding)) == 0)
	{
		FloatVector2 position = GetPosition();

		// find path
		Int32Vector2 currentCell = { (int32)(position.X / PathFinder::CELL_SIZE), (int32)(position.Y / PathFinder::CELL_SIZE) };
		Int32Vector2 targetCell = { x / PathFinder::CELL_SIZE, y / PathFinder::CELL_SIZE };

		gGame->CellPath.clear();
		Int32Rect countourBounds = GetContourBounds();
		PathFinder* pathFinder = gGame->GetPathFinder();
		int32 length = pathFinder->FindPathWithUnitSize(&gGame->CellPath, currentCell, targetCell, countourBounds);

		if (length <= 0)
		{
			return;
		}

		mPath.clear();

		for (Int32Vector2& cell : gGame->CellPath)
		{
			Int32Vector2 pos = { cell.X * PathFinder::CELL_SIZE + PathFinder::CELL_SIZE / 2, cell.Y * PathFinder::CELL_SIZE + PathFinder::CELL_SIZE / 2 };
			mPath.push_back(pos);
		}

		nextMovementWaypoint = mPath.front();
		mPath.pop_front();
	}

	UpdateNextMovementWaypoint(nextMovementWaypoint);

	// Walk Animation
	Sprite* sprite = GetSprite();
	eAnim currentAnim = sprite->GetPrimaryImageAnim();
	if (currentAnim != eAnim::Walking)
	{
		sprite->UpdatePrimaryAnim(eAnim::Walking);
	}
}

void Unit::move()
{
	float range = mTargetAcquisitionRange * PathFinder::GRID_SIZE;

	if (mMoveTarget.Unit != nullptr)
	{
		FloatVector2 moveTargetUnitPos = mMoveTarget.Unit->mPosition;
		float distanceSquare = ::GetDistanceSquare(mMoveTarget.Position, moveTargetUnitPos);
		if (distanceSquare > range * range)
		{
			mMoveTarget.Position.X = (int32)moveTargetUnitPos.X;
			mMoveTarget.Position.Y = (int32)moveTargetUnitPos.Y;

			startMove();
		}
	}

	float nextMovementWaypointDistanceSquare = ::GetDistanceSquare(mNextMovementWaypoint, mPosition);

	if (nextMovementWaypointDistanceSquare < mCurrentSpeed * mCurrentSpeed)
	{
		if (!mPath.empty())
		{
			Int32Vector2 nextMovementWaypoint = mPath.front();
			mPath.pop_front();

			UpdateNextMovementWaypoint(nextMovementWaypoint);
		}
		else
		{
			Sprite* sprite = GetSprite();
			std::list<Image*>* images = sprite->GetImages();

			for (Image* image : *images)
			{
				uint16 offsetCount = image->GetIScriptOffsetCount();
				if (offsetCount >= 12)
				{
					image->UpdateAnim(eAnim::WalkingToIdle);
				}
			}

			if (mOrderTarget.Unit == nullptr)
			{
				mOrderType = eOrder::None;
			}
		}
	}

	if (mMoveTarget.Unit == nullptr)
	{
		mPosition.X += mCurrentOrientation.X * mCurrentSpeed;
		mPosition.Y += mCurrentOrientation.Y * mCurrentSpeed;
	}
	else
	{
		float moveTargetDistanceSquare = ::GetDistanceSquare(mMoveTarget.Position, mPosition);

		if (moveTargetDistanceSquare >= range * range)
		{
			mPosition.X += mCurrentOrientation.X * mCurrentSpeed;
			mPosition.Y += mCurrentOrientation.Y * mCurrentSpeed;
		}
		else
		{
			Sprite* sprite = GetSprite();
			std::list<Image*>* images = sprite->GetImages();

			for (Image* image : *images)
			{
				uint16 offsetCount = image->GetIScriptOffsetCount();
				if (offsetCount >= 12)
				{
					image->UpdateAnim(eAnim::WalkingToIdle);
				}
			}
		}
	}

	mCoolTime = 0;
}

void Unit::startMoveToAttack()
{
	int32 x = mMoveTarget.Position.X;
	int32 y = mMoveTarget.Position.Y;

	Int32Vector2 nextMovementWaypoint = { x, y };

	if ((mSpecialAbilityFlags & ((uint32)eSpecialAbilityFlags::FlyingUnit | (uint32)eSpecialAbilityFlags::FlyingBuilding)) == 0)
	{
		FloatVector2 position = GetPosition();

		// find path
		Int32Vector2 currentCell = { (int32)(position.X / PathFinder::CELL_SIZE), (int32)(position.Y / PathFinder::CELL_SIZE) };
		Int32Vector2 targetCell = { x / PathFinder::CELL_SIZE, y / PathFinder::CELL_SIZE };

		gGame->CellPath.clear();
		Int32Rect countourBounds = GetContourBounds();
		PathFinder* pathFinder = gGame->GetPathFinder();
		int32 length = pathFinder->FindPathWithUnitSize(&gGame->CellPath, currentCell, targetCell, countourBounds);

		if (length <= 0)
		{
			return;
		}

		mPath.clear();

		for (Int32Vector2& cell : gGame->CellPath)
		{
			Int32Vector2 pos = { cell.X * PathFinder::CELL_SIZE + PathFinder::CELL_SIZE / 2, cell.Y * PathFinder::CELL_SIZE + PathFinder::CELL_SIZE / 2 };
			mPath.push_back(pos);
		}

		nextMovementWaypoint = mPath.front();
		mPath.pop_front();
	}

	UpdateNextMovementWaypoint(nextMovementWaypoint);

	// Walk Animation
	Sprite* sprite = GetSprite();
	eAnim currentAnim = sprite->GetPrimaryImageAnim();
	if (currentAnim != eAnim::Walking)
	{
		sprite->UpdatePrimaryAnim(eAnim::Walking);
	}
}

void Unit::moveToAttack()
{
	float range = 2 * PathFinder::GRID_SIZE;

	if (mMoveTarget.Unit != nullptr)
	{
		FloatVector2 moveTargetUnitPos = mMoveTarget.Unit->mPosition;
		float distanceSquare = ::GetDistanceSquare(mMoveTarget.Position, moveTargetUnitPos);
		if (distanceSquare > range * range)
		{
			mMoveTarget.Position.X = (int32)moveTargetUnitPos.X;
			mMoveTarget.Position.Y = (int32)moveTargetUnitPos.Y;
		}
	}

	startMoveToAttack();

	float nextMovementWaypointDistanceSquare = ::GetDistanceSquare(mNextMovementWaypoint, mPosition);

	if (nextMovementWaypointDistanceSquare < mCurrentSpeed * mCurrentSpeed)
	{
		if (!mPath.empty())
		{
			Int32Vector2 nextMovementWaypoint = mPath.front();
			mPath.pop_front();

			UpdateNextMovementWaypoint(nextMovementWaypoint);
		}
	}

	mPosition.X += mCurrentOrientation.X * mCurrentSpeed;
	mPosition.Y += mCurrentOrientation.Y * mCurrentSpeed;

	mCoolTime = 0;
}

void Unit::startAttackUnit()
{
	Sprite* sprite = GetSprite();
	sprite->UpdatePrimaryAnim(eAnim::GndAttkInit);

	mbRepeatAttackable = true;
}

void Unit::attackUnit()
{
	if (mbRepeatAttackable)
	{
		Unit* targetUnit = mOrderTarget.Unit;
		FloatVector2 targetPosition = targetUnit->GetPosition();
		lookAt(targetPosition);

		if (mCoolTime <= 0)
		{
			mbRepeatAttackable = false;

			Unit* dealer = this;
			ResourceManager* resourceManager = gGame->GetResourceManager();
			const UnitData* dealerUnitData = resourceManager->GetUnitData();
			eUnit dealerUnitType = dealer->GetUnitType();
			uint8 weaponID = dealerUnitData->GroundWeapons[(uint32)dealerUnitType];
			const WeaponData* weaponData = resourceManager->GetWeaponData();
			uint16 amount = weaponData->DamageAmounts[weaponID];

			const UnitData* targetUnitData = resourceManager->GetUnitData();

#ifdef _DEBUG
			if (targetUnit == nullptr)
			{
				__debugbreak();
			}
#endif
			eUnit targetUnitType = targetUnit->GetUnitType();
			uint8 armor = targetUnitData->Armors[(uint32)targetUnitType];
			int32 damage = amount - armor;

			int32 hp = targetUnit->GetHP();
			hp -= damage;

			char buf[256];
			sprintf_s(buf, "Dealer: %d, Target: %d, Damage: %d, HP: %d\n", (int)dealerUnitType, (int)targetUnitType, damage, hp);
			OutputDebugStringA(buf);

			if (hp <= 0)
			{
				hp = 0;

				Sprite* sprite = GetSprite();
				sprite->UpdatePrimaryAnim(eAnim::GndAttkToIdle);

				ClearOrders();

				mCoolTime = 0;

				mOrderType = eOrder::None;
			}
			else
			{
				Sprite* sprite = GetSprite();
				sprite->UpdatePrimaryAnim(eAnim::GndAttkRpt);

				mCoolTime = mGroundWeaponCooldown;
				mbRepeatAttackable = false;
			}

			targetUnit->SetHP(hp);

			// Bullet
			Bullet* bullet = new Bullet();
			const UnitData* unitData = resourceManager->GetUnitData();
			eWeapon weaponType = (eWeapon)unitData->GroundWeapons[(uint32)mUnitType];
			bullet->Initialize(weaponType, this);
			FloatVector2 position = targetPosition;
			if (mUnitType == eUnit::TerranVulture)
			{
				position = mPosition;
			}

			bullet->SetPosition(position);
			gGame->Bullets.push_back(bullet);
			gGame->Thingies.push_back(bullet);
		}
	}

	mCoolTime--;

	char buf[256];
	sprintf_s(buf, "Cooldown: %d\n", mCoolTime);
	//OutputDebugStringA(buf);
}

void Unit::startBuilding()
{
	ResourceManager* resourceManager = gGame->GetResourceManager();
	const UnitData* unitData = resourceManager->GetUnitData();
	int32 constructionAnimation = unitData->ConstructionAnimations[(uint32)mUnitType];
	if (constructionAnimation != 0)
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			mTempImages.push_back(image);
		}
		images->clear();

		Image* constructionImage = new Image();
		constructionImage->Initialize((eImage)constructionAnimation, sprite);
		images->push_back(constructionImage);
		sprite->SetPrimaryImage(constructionImage);
	}
}

void Unit::build()
{
	// 걸린 시간
	float elapsedTime = (float)(mBuildTime - mRemainingBuildTime);
	float percent = elapsedTime / mBuildTime;
	Sprite* sprite = GetSprite();

	if (percent >= 1.f)
	{
		sprite->UpdatePrimaryAnim(eAnim::Init);

		mOrderType = eOrder::None;
	}
	else if (percent >= 0.57f)
	{
		if (!mTempImages.empty())
		{
			std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{
				delete image;
			}
			images->clear();

			for (Image* tempImage : mTempImages)
			{
				images->push_back(tempImage);
			}
			mTempImages.clear();
			sprite->SetPrimaryImage(images->front());
			sprite->UpdatePrimaryAnim(eAnim::AlmostBuilt);
		}
	}
	else if (percent >= 0.34f)
	{
		sprite->UpdatePrimaryAnim(eAnim::SpecialState2);
	}
	else if (percent >= 0.13f)
	{
		sprite->UpdatePrimaryAnim(eAnim::SpecialState1);
	}

	mRemainingBuildTime -= 10;
	if ((int32)mRemainingBuildTime < 0)
	{
		mRemainingBuildTime = 0;
	}
}

void Unit::lookAt(FloatVector2 targetPosition)
{
	FloatVector2 position = mPosition;

	float distanceX = targetPosition.X - position.X;
	float distanceY = targetPosition.Y - position.Y;

	float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
	float directionX = distanceX / distance;
	float directionY = distanceY / distance;

	float angle = atan2f(distanceY, distanceX);
	angle += (float)M_PI;
	uint8 direction = (uint8)(angle * 128.0f / M_PI - 64);
	direction /= 8;

	SetFacingDirection(direction);
}

bool Unit::canAttackTarget() const
{
	float range = mTargetAcquisitionRange * PathFinder::GRID_SIZE;
	float distanceSquare = ::GetDistanceSquare(mMoveTarget.Position, mPosition);
	return distanceSquare <= range * range;
}
