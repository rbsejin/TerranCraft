#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "Image.h"
#include "CommandIcon.h"
#include "Arrangement.h"
#include "BWFile.h"
#include "AnimationController.h"
#include "Order.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(BW::UnitType unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	mSprite = new Sprite();

	uint8 scripteID = GetScriptID();
	const FlingyData* flingyData = Arrangement::Instance.GetFlingyData();
	BW::SpriteNumber spriteNumber = (BW::SpriteNumber)flingyData->Sprites[scripteID];

	if (!mSprite->Initalize(spriteNumber))
	{
		goto LB_RETURN;
	}

	mCurrentButtonset = (eButtonset)unitType;
	mHP = GetMaxHP();

	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	Int16Rect contourBounds = unitData->UnitDimensions[(uint32)unitType];
	mContourBounds.Left = (int32)contourBounds.Left;
	mContourBounds.Top = (int32)contourBounds.Top;
	mContourBounds.Right = (int32)contourBounds.Right;
	mContourBounds.Bottom = (int32)contourBounds.Bottom;

	// Weapon
	const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
	mGroundWeaponCooldown = weaponData->WeaponCooldowns[(uint32)unitType];

	bResult = true;

LB_RETURN:
	return bResult;
}

void Unit::Cleanup()
{
	while (!mOrders.empty())
	{
		Order* order = mOrders.front();
		delete order;
		mOrders.pop_front();
	}

	delete mSprite;
	mSprite = nullptr;
}

void Unit::Update()
{
	FloatVector2 newPosition = mPosition;

	if (mOrders.empty())
	{
		goto LB_RETURN;
	}

	Order* order = mOrders.front();

#ifdef _DEBUG
	if (order == nullptr)
	{
		__debugbreak();
	}
#endif

#pragma region Order
	BW::OrderType orderType = order->OrderType;
	switch (orderType)
	{
	case BW::OrderType::Die:
	{
		// TODO: 현재 죽는 애니메이션이 한 프레임 밀리고 있음
		// TODO: 죽은 유닛을 제거해야함

		Sprite* sprite = mSprite;
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

		ClearOrders();
	}
	break;
	case BW::OrderType::Stop:
		break;
	case BW::OrderType::Guard:
		break;
	case BW::OrderType::PlayerGuard:
		break;
	case BW::OrderType::TurretGuard:
		break;
	case BW::OrderType::BunkerGuard:
		break;
	case BW::OrderType::Move:
	{
		float distanceX = mNextMovementWaypoint.X - mPosition.X;
		float distanceY = mNextMovementWaypoint.Y - mPosition.Y;
		float distanceSquare = distanceX * distanceX + distanceY * distanceY;

		int32 speed = mCurrentSpeed;

		if (distanceSquare > speed * speed)
		{
			FloatVector2 currentVelocity = mCurrentVelocity;
			newPosition.X += currentVelocity.X * speed;
			newPosition.Y += currentVelocity.Y * speed;
		}
		else
		{
			newPosition.X = (float)mNextMovementWaypoint.X;
			newPosition.Y = (float)mNextMovementWaypoint.Y;

			if (!mPath.empty())
			{
				mNextMovementWaypoint = mPath.front();
				mPath.pop_front();

				distanceX = mNextMovementWaypoint.X - newPosition.X;
				distanceY = mNextMovementWaypoint.Y - newPosition.Y;

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
				// Walk Animation
				Sprite* sprite = mSprite;
				Image* primaryImage = sprite->GetPrimaryImage();
				BW::IScriptAnimation currentAnim = primaryImage->GetAnim();
				if (currentAnim != BW::IScriptAnimation::Init && currentAnim != BW::IScriptAnimation::WalkingToIdle)
				{
					BW::IScriptAnimation anim = BW::IScriptAnimation::WalkingToIdle;
					primaryImage->SetAnim(anim);
					uint16 iscriptHeader = primaryImage->GetIScriptHeader();
					uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
					primaryImage->SetIScriptOffset(iscriptOffset);
					primaryImage->SetSleep(0);
				}
			}
		}

		mPosition = newPosition;
	}
	break;
	case BW::OrderType::ReaverStop:
		break;
	case BW::OrderType::Attack1:
		break;
	case BW::OrderType::Attack2:
		break;
	case BW::OrderType::AttackUnit:
	{
		if ((mCoolTime == 0) && mbAttackable)
		{
			mCoolTime = mGroundWeaponCooldown;

			mbAttackable = false;

			Unit* target = order->Target;
			Order* targetOrder = target->GetFrontOrder();

			if (targetOrder == nullptr || targetOrder->OrderType != BW::OrderType::Die)
			{
				Unit* dealer = this;
				const UnitData* dealerUnitData = Arrangement::Instance.GetUnitData();
				BW::UnitType dealerUnitType = dealer->GetUnitType();
				uint8 weaponID = dealerUnitData->GroundWeapons[(uint32)dealerUnitType];
				const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
				uint16 amount = weaponData->DamageAmounts[weaponID];

				const UnitData* targetUnitData = Arrangement::Instance.GetUnitData();
				BW::UnitType targetUnitType = target->GetUnitType();
				uint8 armor = targetUnitData->Armors[(uint32)targetUnitType];
				int32 damage = amount - armor;

				int32 hp = target->GetHP();
				hp -= damage;

				char buf[256];
				sprintf_s(buf, "Dealer: %d, Target: %d, Damage: %d, HP: %d\n", (int)dealerUnitType, (int)targetUnitType, damage, hp);
				OutputDebugStringA(buf);

				BW::IScriptAnimation anim = BW::IScriptAnimation::GndAttkRpt;

				if (hp <= 0)
				{
					hp = 0;

					ClearOrders();

					target->ClearOrders();
					targetOrder = new Order();
					targetOrder->OrderType = BW::OrderType::Die;
					targetOrder->Target = nullptr;
					target->mOrders.push_front(targetOrder);

					anim = BW::IScriptAnimation::GndAttkToIdle;
				}
				target->SetHP(hp);

				Sprite* sprite = mSprite;
				Image* primaryImage = sprite->GetPrimaryImage();
				primaryImage->SetAnim(anim);
				uint16 iscriptHeader = primaryImage->GetIScriptHeader();
				uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
				primaryImage->SetIScriptOffset(iscriptOffset);
				primaryImage->SetSleep(0);
			}
		}

		mCoolTime--;

		char buf[256];
		sprintf_s(buf, "Cooldown: %d\n", mCoolTime);
		//OutputDebugStringA(buf);
	}
	break;
	case BW::OrderType::AttackFixedRange:
		break;
	case BW::OrderType::AttackTile:
		break;
	case BW::OrderType::Hover:
		break;
	case BW::OrderType::AttackMove:
		break;
	case BW::OrderType::InfestedCommandCenter:
		break;
	case BW::OrderType::UnusedNothing:
		break;
	case BW::OrderType::UnusedPowerup:
		break;
	case BW::OrderType::TowerGuard:
		break;
	case BW::OrderType::TowerAttack:
		break;
	case BW::OrderType::VultureMine:
		break;
	case BW::OrderType::StayInRange:
		break;
	case BW::OrderType::TurretAttack:
		break;
	case BW::OrderType::Nothing:
		break;
	case BW::OrderType::Unused_24:
		break;
	case BW::OrderType::DroneStartBuild:
		break;
	case BW::OrderType::DroneBuild:
		break;
	case BW::OrderType::CastInfestation:
		break;
	case BW::OrderType::MoveToInfest:
		break;
	case BW::OrderType::InfestingCommandCenter:
		break;
	case BW::OrderType::PlaceBuilding:
		break;
	case BW::OrderType::PlaceProtossBuilding:
		break;
	case BW::OrderType::CreateProtossBuilding:
		break;
	case BW::OrderType::ConstructingBuilding:
		break;
	case BW::OrderType::Repair:
		break;
	case BW::OrderType::MoveToRepair:
		break;
	case BW::OrderType::PlaceAddon:
		break;
	case BW::OrderType::BuildAddon:
		break;
	case BW::OrderType::Train:
		break;
	case BW::OrderType::RallyPointUnit:
		break;
	case BW::OrderType::RallyPointTile:
		break;
	case BW::OrderType::ZergBirth:
		break;
	case BW::OrderType::ZergUnitMorph:
		break;
	case BW::OrderType::ZergBuildingMorph:
		break;
	case BW::OrderType::IncompleteBuilding:
		break;
	case BW::OrderType::IncompleteMorphing:
		break;
	case BW::OrderType::BuildNydusExit:
		break;
	case BW::OrderType::EnterNydusCanal:
		break;
	case BW::OrderType::IncompleteWarping:
		break;
	case BW::OrderType::Follow:
		break;
	case BW::OrderType::Carrier:
		break;
	case BW::OrderType::ReaverCarrierMove:
		break;
	case BW::OrderType::CarrierStop:
		break;
	case BW::OrderType::CarrierAttack:
		break;
	case BW::OrderType::CarrierMoveToAttack:
		break;
	case BW::OrderType::CarrierIgnore2:
		break;
	case BW::OrderType::CarrierFight:
		break;
	case BW::OrderType::CarrierHoldPosition:
		break;
	case BW::OrderType::Reaver:
		break;
	case BW::OrderType::ReaverAttack:
		break;
	case BW::OrderType::ReaverMoveToAttack:
		break;
	case BW::OrderType::ReaverFight:
		break;
	case BW::OrderType::ReaverHoldPosition:
		break;
	case BW::OrderType::TrainFighter:
		break;
	case BW::OrderType::InterceptorAttack:
		break;
	case BW::OrderType::ScarabAttack:
		break;
	case BW::OrderType::RechargeShieldsUnit:
		break;
	case BW::OrderType::RechargeShieldsBattery:
		break;
	case BW::OrderType::ShieldBattery:
		break;
	case BW::OrderType::InterceptorReturn:
		break;
	case BW::OrderType::DroneLand:
		break;
	case BW::OrderType::BuildingLand:
		break;
	case BW::OrderType::BuildingLiftOff:
		break;
	case BW::OrderType::DroneLiftOff:
		break;
	case BW::OrderType::LiftingOff:
		break;
	case BW::OrderType::ResearchTech:
		break;
	case BW::OrderType::Upgrade:
		break;
	case BW::OrderType::Larva:
		break;
	case BW::OrderType::SpawningLarva:
		break;
	case BW::OrderType::Harvest1:
		break;
	case BW::OrderType::Harvest2:
		break;
	case BW::OrderType::MoveToGas:
		break;
	case BW::OrderType::WaitForGas:
		break;
	case BW::OrderType::HarvestGas:
		break;
	case BW::OrderType::ReturnGas:
		break;
	case BW::OrderType::MoveToMinerals:
		break;
	case BW::OrderType::WaitForMinerals:
		break;
	case BW::OrderType::MiningMinerals:
		break;
	case BW::OrderType::Harvest3:
		break;
	case BW::OrderType::Harvest4:
		break;
	case BW::OrderType::ReturnMinerals:
		break;
	case BW::OrderType::Interrupted:
		break;
	case BW::OrderType::EnterTransport:
		break;
	case BW::OrderType::PickupIdle:
		break;
	case BW::OrderType::PickupTransport:
		break;
	case BW::OrderType::PickupBunker:
		break;
	case BW::OrderType::Pickup4:
		break;
	case BW::OrderType::PowerupIdle:
		break;
	case BW::OrderType::Sieging:
		break;
	case BW::OrderType::Unsieging:
		break;
	case BW::OrderType::WatchTarget:
		break;
	case BW::OrderType::InitCreepGrowth:
		break;
	case BW::OrderType::SpreadCreep:
		break;
	case BW::OrderType::StoppingCreepGrowth:
		break;
	case BW::OrderType::GuardianAspect:
		break;
	case BW::OrderType::ArchonWarp:
		break;
	case BW::OrderType::CompletingArchonSummon:
		break;
	case BW::OrderType::HoldPosition:
		break;
	case BW::OrderType::QueenHoldPosition:
		break;
	case BW::OrderType::Cloak:
		break;
	case BW::OrderType::Decloak:
		break;
	case BW::OrderType::Unload:
		break;
	case BW::OrderType::MoveUnload:
		break;
	case BW::OrderType::FireYamatoGun:
		break;
	case BW::OrderType::MoveToFireYamatoGun:
		break;
	case BW::OrderType::CastLockdown:
		break;
	case BW::OrderType::Burrowing:
		break;
	case BW::OrderType::Burrowed:
		break;
	case BW::OrderType::Unburrowing:
		break;
	case BW::OrderType::CastDarkSwarm:
		break;
	case BW::OrderType::CastParasite:
		break;
	case BW::OrderType::CastSpawnBroodlings:
		break;
	case BW::OrderType::CastEMPShockwave:
		break;
	case BW::OrderType::NukeWait:
		break;
	case BW::OrderType::NukeTrain:
		break;
	case BW::OrderType::NukeLaunch:
		break;
	case BW::OrderType::NukePaint:
		break;
	case BW::OrderType::NukeUnit:
		break;
	case BW::OrderType::CastNuclearStrike:
		break;
	case BW::OrderType::NukeTrack:
		break;
	case BW::OrderType::InitializeArbiter:
		break;
	case BW::OrderType::CloakNearbyUnits:
		break;
	case BW::OrderType::PlaceMine:
		break;
	case BW::OrderType::RightClickAction:
		break;
	case BW::OrderType::SuicideUnit:
		break;
	case BW::OrderType::SuicideLocation:
		break;
	case BW::OrderType::SuicideHoldPosition:
		break;
	case BW::OrderType::CastRecall:
		break;
	case BW::OrderType::Teleport:
		break;
	case BW::OrderType::CastScannerSweep:
		break;
	case BW::OrderType::Scanner:
		break;
	case BW::OrderType::CastDefensiveMatrix:
		break;
	case BW::OrderType::CastPsionicStorm:
		break;
	case BW::OrderType::CastIrradiate:
		break;
	case BW::OrderType::CastPlague:
		break;
	case BW::OrderType::CastConsume:
		break;
	case BW::OrderType::CastEnsnare:
		break;
	case BW::OrderType::CastStasisField:
		break;
	case BW::OrderType::CastHallucination:
		break;
	case BW::OrderType::Hallucination2:
		break;
	case BW::OrderType::ResetCollision:
		break;
	case BW::OrderType::ResetHarvestCollision:
		break;
	case BW::OrderType::Patrol:
		break;
	case BW::OrderType::CTFCOPInit:
		break;
	case BW::OrderType::CTFCOPStarted:
		break;
	case BW::OrderType::CTFCOP2:
		break;
	case BW::OrderType::ComputerAI:
		break;
	case BW::OrderType::AtkMoveEP:
		break;
	case BW::OrderType::HarassMove:
		break;
	case BW::OrderType::AIPatrol:
		break;
	case BW::OrderType::GuardPost:
		break;
	case BW::OrderType::RescuePassive:
		break;
	case BW::OrderType::Neutral:
		break;
	case BW::OrderType::ComputerReturn:
		break;
	case BW::OrderType::InitializePsiProvider:
		break;
	case BW::OrderType::SelfDestructing:
		break;
	case BW::OrderType::Critter:
		break;
	case BW::OrderType::HiddenGun:
		break;
	case BW::OrderType::OpenDoor:
		break;
	case BW::OrderType::CloseDoor:
		break;
	case BW::OrderType::HideTrap:
		break;
	case BW::OrderType::RevealTrap:
		break;
	case BW::OrderType::EnableDoodad:
		break;
	case BW::OrderType::DisableDoodad:
		break;
	case BW::OrderType::WarpIn:
		break;
	case BW::OrderType::Medic:
		break;
	case BW::OrderType::MedicHeal:
		break;
	case BW::OrderType::HealMove:
		break;
	case BW::OrderType::MedicHoldPosition:
		break;
	case BW::OrderType::MedicHealToIdle:
		break;
	case BW::OrderType::CastRestoration:
		break;
	case BW::OrderType::CastDisruptionWeb:
		break;
	case BW::OrderType::CastMindControl:
		break;
	case BW::OrderType::DarkArchonMeld:
		break;
	case BW::OrderType::CastFeedback:
		break;
	case BW::OrderType::CastOpticalFlare:
		break;
	case BW::OrderType::CastMaelstrom:
		break;
	case BW::OrderType::JunkYardDog:
		break;
	case BW::OrderType::Fatal:
		break;
	case BW::OrderType::None:
		break;
	default:
		break;
	}
#pragma endregion

LB_RETURN:

	IntVector2 spritePosition = { (int32)newPosition.X, (int32)newPosition.Y };
	mSprite->SetPosition(spritePosition);

	const std::list<Image*>* images = mSprite->GetImages();
	for (Image* image : *images)
	{
		BW::ImageNumber imageID = image->GetImageID();
		if (imageID < BW::ImageNumber::IMG_SELECT_022 || imageID > BW::ImageNumber::IMG_SELECT_224)
		{
			image->SetDirection(mFacingDirection);
		}
		image->UpdateGraphicData();
	}

	Image* selectionCircleImage = mSprite->GetSelectionCircleImage();
	if (selectionCircleImage != nullptr)
	{
		selectionCircleImage->UpdateGraphicData();
	}
}


BW::IScriptAnimation Unit::GetAnimation() const
{
	Sprite* sprite = mSprite;
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

int32 Unit::GetMaxHP() const
{
	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	return unitData->HitPoints[(uint32)mUnitType] >> 8;
}

uint8 Unit::GetScriptID() const
{
	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	return unitData->Graphics[(uint32)mUnitType];
}
