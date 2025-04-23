#include "pch.h"
#include "Bullet.h"
#include "ResourceManager.h"
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

bool Bullet::Initialize(eWeapon weaponType, Unit* sourceUnit)
{
	bool bResult = false;

	mWeaponType = weaponType;
	mSourceUnit = sourceUnit;

	ResourceManager* resourceManager = gGame->GetResourceManager();
	const WeaponData* weaponData = resourceManager->GetWeaponData();
	uint16 flingyID = (uint16)weaponData->Graphics[(uint32)weaponType];
	const FlingyData* flingyData = resourceManager->GetFlingyData();
	eSprite spriteNumber = (eSprite)flingyData->Sprites[flingyID];
	Flingy::Initialize(0, (eFlingyType)flingyID);

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
			eAnim anim = eAnim::Init;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			AnimationController* animationController = gGame->GetAnimationController();
			uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
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
			eAnim anim = eAnim::Death;
			image->SetAnim(anim);
			uint16 iscriptHeader = image->GetIScriptHeader();
			AnimationController* animationController = gGame->GetAnimationController();
			uint16 iscriptOffset = animationController->GetIScriptOffset(iscriptHeader, anim);
			image->SetIScriptOffset(iscriptOffset);
			image->SetSleep(0);
		}

		gGame->Bullets.erase(std::remove(gGame->Bullets.begin(), gGame->Bullets.end(), this), gGame->Bullets.end());

		mTimeRemaining = 0;
	}
}
