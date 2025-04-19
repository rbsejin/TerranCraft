#include "pch.h"
#include "Bullet.h"
#include "Arrangement.h"
#include "BWFile.h"
#include <list>
#include "Image.h"
#include "Sprite.h"
#include "Unit.h"
#include "AnimationController.h"
#include "Game.h"

Bullet::~Bullet()
{
	Cleanup();
}

bool Bullet::Initialize(BW::WeaponType weaponType, Unit* sourceUnit)
{
	bool bResult = false;

	mWeaponType = weaponType;
	mSourceUnit = sourceUnit;

	const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
	uint16 flingyID = (uint16)weaponData->Graphics[(uint32)weaponType];
	const FlingyData* flingyData = Arrangement::Instance.GetFlingyData();
	BW::SpriteNumber spriteNumber = (BW::SpriteNumber)flingyData->Sprites[flingyID];
	Thingy::Initialize(1, spriteNumber);
	Flingy::Initialize((BW::FlingyType)flingyID);

	mTimeRemaining = weaponData->WeaponCooldowns[(uint32)weaponType];

	mMoveTarget = mSourceUnit->GetOrderTarget();
	FloatVector2 targetPosition = { (float)mMoveTarget.Position.X, (float)mMoveTarget.Position.Y };
	if (mMoveTarget.Unit != nullptr)
	{
		targetPosition = mMoveTarget.Unit->GetPosition();
	}

	FloatVector2 position = mSourceUnit->GetPosition();
	float distanceX = targetPosition.X - position.X;
	float distanceY = targetPosition.Y - position.Y;
	float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);

	mCurrentVelocity.X = distanceX / distance * mCurrentSpeed;
	mCurrentVelocity.Y = distanceY / distance * mCurrentSpeed;




	{
		Sprite* sprite = GetSprite();
		std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			BW::IScriptAnimation anim = BW::IScriptAnimation::Init;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}
	}

	bResult = true;
	return bResult;
}

void Bullet::Cleanup()
{
	Thingy::Cleanup();
}

void Bullet::Update()
{
	Flingy::Update();

	FloatVector2 targetPosition = { (float)mMoveTarget.Position.X, (float)mMoveTarget.Position.Y };
	if (mMoveTarget.Unit != nullptr)
	{
		targetPosition = mMoveTarget.Unit->GetPosition();
	}

	float distanceSquare =
		(targetPosition.X - mPosition.X) * (targetPosition.X - mPosition.X) +
		(targetPosition.Y - mPosition.Y) * (targetPosition.Y - mPosition.Y);

	if (mTimeRemaining > 0 && distanceSquare >= mCurrentVelocity.X * mCurrentVelocity.X + mCurrentVelocity.Y * mCurrentVelocity.Y)
	{
		uint8 direction = mSourceUnit->GetFacingDirection();
		float radian = (float)(direction - 8) * (float)M_PI / 16.0f;
		mPosition.X += mCurrentVelocity.X;
		mPosition.Y += mCurrentVelocity.Y;

		mTimeRemaining--;
	}
	else
	{
		mPosition.X = targetPosition.X;
		mPosition.Y = targetPosition.Y;

		// Die
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

		Game::sBullets.erase(std::remove(Game::sBullets.begin(), Game::sBullets.end(), this), Game::sBullets.end());

		mTimeRemaining = 0;
	}
}
