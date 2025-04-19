#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "Image.h"
#include "CommandIcon.h"
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

bool Unit::Initialize(BW::UnitType unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	uint8 flingyID = GetFlingyID();
	const FlingyData* flingyData = Arrangement::Instance.GetFlingyData();
	BW::SpriteNumber spriteNumber = (BW::SpriteNumber)flingyData->Sprites[flingyID];

	int32 hp = GetMaxHP();
	Thingy::Initialize(hp, spriteNumber);

	//mCurrentSpeed = flingyData->Speeds[flingyID];

	mCurrentButtonset = (eButtonset)unitType;

	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	Int16Rect contourBounds = unitData->UnitDimensions[(uint32)unitType];
	mContourBounds.Left = (int32)contourBounds.Left;
	mContourBounds.Top = (int32)contourBounds.Top;
	mContourBounds.Right = (int32)contourBounds.Right;
	mContourBounds.Bottom = (int32)contourBounds.Bottom;

	// Weapon
	const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
	mGroundWeaponCooldown = weaponData->WeaponCooldowns[(uint32)unitType];

	Flingy::Initialize((BW::FlingyType)flingyID);

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
	case BW::OrderType::None:
	{
		PerformOrder();
		break;
	}
	case BW::OrderType::Move:
	{
		move();
		break;
	}
	case BW::OrderType::AttackUnit:
	{
		attackUnit();
		break;
	}
	case BW::OrderType::AttackMove:
	{
		// TODO: 추후 attackMove() 함수 구현해서 대체할 것...
		move();
		break;
	}
	case BW::OrderType::Die:
		break;
	default:
		break;
	}

	IntVector2 spritePosition = { (int32)mPosition.X, (int32)mPosition.Y };
	Sprite* sprite = GetSprite();
	sprite->SetPosition(spritePosition);

	const std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		BW::ImageNumber imageID = image->GetImageID();
		image->SetDirection(mFacingDirection);
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
		auto iter = std::find(Game::sThingies.begin(), Game::sThingies.end(), order->Target.Unit);
		if (iter == Game::sThingies.end())
		{
			goto LB_RETURN;
		}
	}

#pragma region Order
	BW::OrderType orderType = order->OrderType;
	mOrderTarget = order->Target;
	mOrderType = orderType;

	switch (orderType)
	{
	case BW::OrderType::Die:
	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			BW::IScriptAnimation anim = BW::IScriptAnimation::Death;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		Game::sUnits.erase(std::remove(Game::sUnits.begin(), Game::sUnits.end(), this));
		Game::sSelectedUnits.erase(std::remove(Game::sSelectedUnits.begin(), Game::sSelectedUnits.end(), this), Game::sSelectedUnits.end());
		break;
	}
	case BW::OrderType::Stop:
	{
		Sprite* sprite = GetSprite();

		const std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			BW::IScriptAnimation anim = image->GetAnim();

			switch (anim)
			{
			case BW::IScriptAnimation::Init:
				break;
			case BW::IScriptAnimation::Death:
				break;
			case BW::IScriptAnimation::GndAttkInit:
				anim = BW::IScriptAnimation::GndAttkToIdle;
				break;
			case BW::IScriptAnimation::AirAttkInit:
				anim = BW::IScriptAnimation::AirAttkToIdle;
				break;
			case BW::IScriptAnimation::Unused1:
				break;
			case BW::IScriptAnimation::GndAttkRpt:
				anim = BW::IScriptAnimation::GndAttkToIdle;
				break;
			case BW::IScriptAnimation::AirAttkRpt:
				anim = BW::IScriptAnimation::AirAttkToIdle;
				break;
			case BW::IScriptAnimation::CastSpell:
				break;
			case BW::IScriptAnimation::GndAttkToIdle:
				break;
			case BW::IScriptAnimation::AirAttkToIdle:
				break;
			case BW::IScriptAnimation::Unused2:
				break;
			case BW::IScriptAnimation::Walking:
				anim = BW::IScriptAnimation::WalkingToIdle;
				break;
			case BW::IScriptAnimation::WalkingToIdle:
				break;
			case BW::IScriptAnimation::SpecialState1:
				break;
			case BW::IScriptAnimation::SpecialState2:
				break;
			case BW::IScriptAnimation::AlmostBuilt:
				break;
			case BW::IScriptAnimation::Built:
				break;
			case BW::IScriptAnimation::Landing:
				break;
			case BW::IScriptAnimation::LiftOff:
				break;
			case BW::IScriptAnimation::IsWorking:
				break;
			case BW::IScriptAnimation::WorkingToIdle:
				break;
			case BW::IScriptAnimation::WarpIn:
				break;
			case BW::IScriptAnimation::Unused3:
				break;
			case BW::IScriptAnimation::StarEditInit:
				break;
			case BW::IScriptAnimation::Disable:
				break;
			case BW::IScriptAnimation::Burrow:
				break;
			case BW::IScriptAnimation::UnBurrow:
				break;
			case BW::IScriptAnimation::Enable:
				break;
			default:
				break;
			}

			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		mCoolTime = 0;
		SetRepeatAttackable(true);
		mOrderType = BW::OrderType::None;
		break;
	}
	case BW::OrderType::Move:
	{
		startMove();
		break;
	}
	case BW::OrderType::Attack1:
		break;
	case BW::OrderType::AttackUnit:
	{
		startAttackUnit();
		break;
	}
	case BW::OrderType::AttackMove:
		// TODO: 추후 startAttackMove() 함수 구현해서 대체할 것...
		startMove();
		break;
	case BW::OrderType::Nothing:
		break;
	default:
		break;
	}
#pragma endregion

	LB_RETURN:
	mOrderQueue.pop_front();
	delete order;
}

BW::IScriptAnimation Unit::GetAnimation() const
{
	Sprite* sprite = GetSprite();
	Image* primaryImage = sprite->GetPrimaryImage();
	BW::IScriptAnimation anim = primaryImage->GetAnim();
	return anim;
}

bool Unit::IsMoving() const
{
	BW::IScriptAnimation anim = GetAnimation();
	return anim == BW::IScriptAnimation::Walking || anim == BW::IScriptAnimation::WalkingToIdle;
}

bool Unit::IsAttacking() const
{
	BW::IScriptAnimation anim = GetAnimation();
	return anim == BW::IScriptAnimation::GndAttkInit || anim == BW::IScriptAnimation::GndAttkRpt
		|| anim == BW::IScriptAnimation::AirAttkInit || anim == BW::IScriptAnimation::AirAttkRpt;
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
	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	return unitData->HitPoints[(uint32)mUnitType] >> 8;
}

uint8 Unit::GetFlingyID() const
{
	const UnitData* unitData = Arrangement::Instance.GetUnitData();
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

	Game::sCellPath.clear();
	IntRect countourBounds = GetContourBounds();
	int32 length = FindPathWithUnitSize(&Game::sCellPath, (const uint8*)gMiniTiles, currentCell, targetCell, countourBounds);

	if (length <= 0)
	{
		return;
	}

	std::list<IntVector2>* path = GetPath();
	path->clear();

	for (IntVector2& cell : Game::sCellPath)
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
		BW::IScriptAnimation anim = BW::IScriptAnimation::Walking;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
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
			BW::IScriptAnimation anim = BW::IScriptAnimation::WalkingToIdle;
			Sprite* sprite = GetSprite();

			const std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{
				image->SetAnim(anim);
				uint16 iscriptHeader = image->GetIScriptHeader();
				uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			mOrderType = BW::OrderType::None;
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
		BW::IScriptAnimation anim = BW::IScriptAnimation::GndAttkInit;
		image->SetAnim(anim);
		uint16 iscriptHeader = image->GetIScriptHeader();
		uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
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
		const UnitData* dealerUnitData = Arrangement::Instance.GetUnitData();
		BW::UnitType dealerUnitType = dealer->GetUnitType();
		uint8 weaponID = dealerUnitData->GroundWeapons[(uint32)dealerUnitType];
		const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
		uint16 amount = weaponData->DamageAmounts[weaponID];

		const UnitData* targetUnitData = Arrangement::Instance.GetUnitData();

#ifdef _DEBUG
		if (targetUnit == nullptr)
		{
			__debugbreak();
		}
#endif
		BW::UnitType targetUnitType = targetUnit->GetUnitType();
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
				BW::IScriptAnimation anim = BW::IScriptAnimation::GndAttkToIdle;
				image->SetAnim(anim);
				uint16 iscriptHeader = image->GetIScriptHeader();
				uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			ClearOrders();

			mCoolTime = 0;

			mOrderType = BW::OrderType::None;
		}
		else
		{
			Sprite* sprite = GetSprite();
			std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{

				uint16 iscriptHeader = image->GetIScriptHeader();
				uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, BW::IScriptAnimation::GndAttkRpt);
				image->SetIScriptOffset(iscriptOffset);
				image->SetSleep(0);
			}

			mCoolTime = mGroundWeaponCooldown;
			mbRepeatAttackable = false;
		}

		targetUnit->SetHP(hp);

		// Bullet
		Bullet* bullet = new Bullet();
		const UnitData* unitData = Arrangement::Instance.GetUnitData();
		BW::WeaponType weaponType = (BW::WeaponType)unitData->GroundWeapons[(uint32)mUnitType];
		bullet->Initialize(weaponType, this);
		//FloatVector2 position = targetUnit->GetPosition();
		FloatVector2 position = targetPosition;
		if (mUnitType == BW::UnitType::Terran_Vulture)
		{
			position = mPosition;
		}

		bullet->SetPosition(position);
		Game::sBullets.push_back(bullet);
		Game::sThingies.push_back(bullet);
	}

	mCoolTime--;

	char buf[256];
	sprintf_s(buf, "Cooldown: %d\n", mCoolTime);
	//OutputDebugStringA(buf);
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
