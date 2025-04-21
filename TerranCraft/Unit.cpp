#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "Image.h"
#include "../BWLib/ButtonsetType.h"
#include "Arrangement.h"
#include "BWFile.h"
#include "AnimationController.h"
#include "Order.h"
#include "Bullet.h"
#include "Game.h"
#include "PathFinder.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(eUnit unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	mCurrentButtonset = (eButtonset)unitType;

	Arrangement* arrangement = gGame->GetArrangement();
	const UnitData* unitData = arrangement->GetUnitData();
	Int16Rect contourBounds = unitData->UnitDimensions[(uint32)unitType];
	mContourBounds.Left = (int32)contourBounds.Left;
	mContourBounds.Top = (int32)contourBounds.Top;
	mContourBounds.Right = (int32)contourBounds.Right;
	mContourBounds.Bottom = (int32)contourBounds.Bottom;

	mSpecialAbilityFlags = unitData->SpecialAbilityFlags[(uint32)unitType];
	mMovementFlags = unitData->Unknowns[(uint32)unitType];

	// Weapon
	const WeaponData* weaponData = arrangement->GetWeaponData();
	mGroundWeaponCooldown = weaponData->WeaponCooldowns[(uint32)unitType];

	int32 hp = GetMaxHP();
	uint8 flingyID = GetFlingyID();

	Flingy::Initialize(hp, (eFlingyType)flingyID);

	mBuildTime = unitData->BuildTimes[(uint32)unitType];
	mRemainingBuildTime = mBuildTime;

	bResult = true;

	return bResult;
}

void Unit::Cleanup()
{
	for (Order* order : mOrderQueue)
	{
		delete order;
	}
	mOrderQueue.clear();

	Flingy::Cleanup();
}

void Unit::Update()
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
		attackUnit();
		break;
	}
	case eOrder::AttackMove:
	{
		// TODO: 추후 attackMove() 함수 구현해서 대체할 것...
		move();
		break;
	}
	case eOrder::ConstructingBuilding:
		build();
		break;
	default:
		break;
	}

	Sprite* sprite = GetSprite();
	IntVector2 spritePosition = { (int32)mPosition.X, (int32)mPosition.Y };
	sprite->SetPosition(spritePosition);

	const std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		eImage imageID = image->GetImageID();
		if (image->HasRotationFrames())
		{
			image->SetDirection(mFacingDirection);
		}
		image->UpdateGraphicData();
	}

	Image* selectionCircleImage = sprite->GetSelectionCircleImage();
	if (selectionCircleImage != nullptr)
	{
		selectionCircleImage->UpdateGraphicData();
	}
}

void Unit::PerformOrder()
{
	if (mbNobrkcode)
	{
		return;
	}

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
			eAnim anim = eAnim::Death;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			AnimationController* animationController = gGame->GetAnimationController();
			uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		gGame->Units.erase(std::remove(gGame->Units.begin(), gGame->Units.end(), this));
		gGame->SelectedUnits.erase(std::remove(gGame->SelectedUnits.begin(), gGame->SelectedUnits.end(), this), gGame->SelectedUnits.end());
		break;
	}
	case eOrder::Stop:
	{
		Sprite* sprite = GetSprite();

		const std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			eAnim anim = image->GetAnim();

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

			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			AnimationController* animationController = gGame->GetAnimationController();
			uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		mCoolTime = 0;
		SetRepeatAttackable(true);
		mOrderType = eOrder::None;
		break;
	}
	case eOrder::Move:
	{
		startMove();
		break;
	}
	case eOrder::Attack1:
		break;
	case eOrder::AttackUnit:
	{
		startAttackUnit();
		break;
	}
	case eOrder::AttackMove:
		// TODO: 추후 startAttackMove() 함수 구현해서 대체할 것...
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

eAnim Unit::GetAnimation() const
{
	Sprite* sprite = GetSprite();
	Image* primaryImage = sprite->GetPrimaryImage();
	eAnim anim = primaryImage->GetAnim();
	return anim;
}

bool Unit::IsMoving() const
{
	eAnim anim = GetAnimation();
	return anim == eAnim::Walking || anim == eAnim::WalkingToIdle;
}

bool Unit::IsAttacking() const
{
	eAnim anim = GetAnimation();
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
	Arrangement* arrangement = gGame->GetArrangement();
	const UnitData* unitData = arrangement->GetUnitData();
	return unitData->HitPoints[(uint32)mUnitType] >> 8;
}

uint8 Unit::GetFlingyID() const
{
	Arrangement* arrangement = gGame->GetArrangement();
	const UnitData* unitData = arrangement->GetUnitData();
	return unitData->Graphics[(uint32)mUnitType];
}

void Unit::startMove()
{
	mMoveTarget = mOrderTarget;
	int32 x = mMoveTarget.Position.X;
	int32 y = mMoveTarget.Position.Y;

	FloatVector2 position = GetPosition();

	// find path
	IntVector2 currentCell = { (int32)(position.X / CELL_SIZE), (int32)(position.Y / CELL_SIZE) };
	IntVector2 targetCell = { x / CELL_SIZE, y / CELL_SIZE };

	gGame->CellPath.clear();
	IntRect countourBounds = GetContourBounds();
	int32 length = FindPathWithUnitSize(&gGame->CellPath, (const uint8*)gMiniTiles, currentCell, targetCell, countourBounds);

	if (length <= 0)
	{
		return;
	}

	std::list<IntVector2>* path = GetPath();
	path->clear();

	for (IntVector2& cell : gGame->CellPath)
	{
		IntVector2 pos = { cell.X * CELL_SIZE + CELL_SIZE / 2, cell.Y * CELL_SIZE + CELL_SIZE / 2 };
		path->push_back(pos);
	}

	IntVector2 nextMovementWaypoint = path->front();
	path->pop_front();
	SetNextMovementWaypoint(nextMovementWaypoint);

	float distanceX = nextMovementWaypoint.X - position.X;
	float distanceY = nextMovementWaypoint.Y - position.Y;

	float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
	FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
	SetCurrentVelocity(currentVelocity);

	// Walk Animation
	Sprite* sprite = GetSprite();
	std::list<Image*>* images = sprite->GetImages();

	for (Image* image : *images)
	{
		eAnim anim = eAnim::Walking;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		AnimationController* animationController = gGame->GetAnimationController();
		uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
		image->SetIScriptOffset(iscriptOffset);
		image->SetSleep(0);
	}
}

void Unit::move()
{
	float distanceX = mNextMovementWaypoint.X - mPosition.X;
	float distanceY = mNextMovementWaypoint.Y - mPosition.Y;
	float distanceSquare = distanceX * distanceX + distanceY * distanceY;

	float speed = mCurrentSpeed;

	if (distanceSquare > speed * speed)
	{
		FloatVector2 currentVelocity = mCurrentVelocity;
		mPosition.X += currentVelocity.X * speed;
		mPosition.Y += currentVelocity.Y * speed;
	}
	else
	{
		mPosition.X = (float)mNextMovementWaypoint.X;
		mPosition.Y = (float)mNextMovementWaypoint.Y;

		if (!mPath.empty())
		{
			mNextMovementWaypoint = mPath.front();
			mPath.pop_front();

			distanceX = mNextMovementWaypoint.X - mPosition.X;
			distanceY = mNextMovementWaypoint.Y - mPosition.Y;

			float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
			FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
			mCurrentVelocity = currentVelocity;

			float angle = atan2f(distanceY, distanceX);
			angle += (float)M_PI;
			uint8 direction = (uint8)(angle * 128.0f / M_PI - 64);
			direction /= 8;

			mFacingDirection = direction;
		}
		else
		{
			// WalkingToIdle Animation
			eAnim anim = eAnim::WalkingToIdle;
			Sprite* sprite = GetSprite();

			const std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{
				image->SetAnim(anim);
				uint16 iscriptHeader = image->GetIScriptHeader();
				AnimationController* animationController = gGame->GetAnimationController();
				uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			mOrderType = eOrder::None;
		}
	}
}

void Unit::startAttackUnit()
{
	Unit* targetUnit = mOrderTarget.Unit;
	FloatVector2 targetPosition = targetUnit->GetPosition();
	lookAt(targetPosition);

	Sprite* sprite = GetSprite();
	std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		eAnim anim = eAnim::GndAttkInit;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		AnimationController* animationController = gGame->GetAnimationController();
		uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
		image->SetIScriptOffset(iscriptOffset);
		image->SetSleep(0);
		mbRepeatAttackable = true;
	}
}

void Unit::attackUnit()
{
	if ((mCoolTime <= 0) && mbRepeatAttackable)
	{
		mbRepeatAttackable = false;

		Unit* targetUnit = mOrderTarget.Unit;
		FloatVector2 targetPosition = targetUnit->GetPosition();
		lookAt(targetPosition);

		Unit* dealer = this;
		Arrangement* arrangement = gGame->GetArrangement();
		const UnitData* dealerUnitData = arrangement->GetUnitData();
		eUnit dealerUnitType = dealer->GetUnitType();
		uint8 weaponID = dealerUnitData->GroundWeapons[(uint32)dealerUnitType];
		const WeaponData* weaponData = arrangement->GetWeaponData();
		uint16 amount = weaponData->DamageAmounts[weaponID];

		const UnitData* targetUnitData = arrangement->GetUnitData();

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
			std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{
				eAnim anim = eAnim::GndAttkToIdle;
				image->SetAnim(anim);
				uint16 iscriptHeader = image->GetIScriptHeader();
				AnimationController* animationController = gGame->GetAnimationController();
				uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			ClearOrders();

			mCoolTime = 0;

			mOrderType = eOrder::None;
		}
		else
		{
			Sprite* sprite = GetSprite();
			std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{

				uint16 iscriptHeader = image->GetIScriptHeader();
				AnimationController* animationController = gGame->GetAnimationController();
				uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, eAnim::GndAttkRpt);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			mCoolTime = mGroundWeaponCooldown;
			mbRepeatAttackable = false;
		}

		targetUnit->SetHP(hp);

		// Bullet
		Bullet* bullet = new Bullet();
		const UnitData* unitData = arrangement->GetUnitData();
		eWeapon weaponType = (eWeapon)unitData->GroundWeapons[(uint32)mUnitType];
		bullet->Initialize(weaponType, this);
		//FloatVector2 position = targetUnit->GetPosition();
		FloatVector2 position = targetPosition;
		if (mUnitType == eUnit::TerranVulture)
		{
			position = mPosition;
		}

		bullet->SetPosition(position);
		gGame->Bullets.push_back(bullet);
		gGame->Thingies.push_back(bullet);
	}

	mCoolTime--;

	char buf[256];
	sprintf_s(buf, "Cooldown: %d\n", mCoolTime);
	//OutputDebugStringA(buf);
}

void Unit::startBuilding()
{
	Arrangement* arrangement = gGame->GetArrangement();
	const UnitData* unitData = arrangement->GetUnitData();
	int32 constructionAnimation = unitData->ConstructionAnimations[(uint32)mUnitType];
	if (constructionAnimation != 0)
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			image->SetHidden(true);
		}

		mConstructionImage = new Image();
		mConstructionImage->Initialize((eImage)constructionAnimation, sprite);
		mConstructionImage->SetAnim(eAnim::Init);
		sprite->AddAffter(mConstructionImage);
	}
}

void Unit::build()
{
	// 걸린 시간
	float elapsedTime = (float)(mBuildTime - mRemainingBuildTime);
	float percent = elapsedTime / mBuildTime;

	if (percent >= 1.f)
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			eAnim anim = eAnim::Built;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			AnimationController* animationController = gGame->GetAnimationController();
			uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		mOrderType = eOrder::None;
	}
	else if (percent >= 0.57f)
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		auto iter = images->begin();
		while (iter != images->end())
		{
			Image* image = *iter;
			if (image == mConstructionImage)
			{
				iter = images->erase(iter);
				delete mConstructionImage;
				mConstructionImage = nullptr;
			}
			else
			{
				eAnim anim = eAnim::AlmostBuilt;
				image->SetAnim(anim);
				image->SetHidden(false);
				uint16 iscriptHeader = image->GetIScriptHeader();
				AnimationController* animationController = gGame->GetAnimationController();
				uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);

				iter++;
			}
		}
	}
	else if (percent >= 0.34f)
	{
		Sprite* sprite = GetSprite();
		Image* image = mConstructionImage;
		eAnim anim = eAnim::SpecialState2;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		AnimationController* animationController = gGame->GetAnimationController();
		uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
		image->SetIScriptOffset(iscriptOffset);
		image->SetSleep(0);
	}
	else if (percent >= 0.13f)
	{
		Sprite* sprite = GetSprite();
		Image* image = mConstructionImage;
		eAnim anim = eAnim::SpecialState1;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		AnimationController* animationController = gGame->GetAnimationController();
		uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
		image->SetIScriptOffset(iscriptOffset);
		image->SetSleep(0);
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
