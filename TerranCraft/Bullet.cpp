#include "pch.h"
#include "Bullet.h"
#include "Arrangement.h"
#include "BWFile.h"
#include <list>
#include "Image.h"
#include "Sprite.h"
#include "AnimationController.h"

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

	mWeaponType = (BW::WeaponType)flingyData->Sprites[flingyID];

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
