#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "CommandIcon.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(BW::UnitType unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	mSprite = new Sprite();

	BW::SpriteNumber spriteNumber = Sprite::GetSpriteNumber(unitType);
	if (!mSprite->Initalize(spriteNumber))
	{
		goto LB_RETURN;
	}

	mCurrentButtonset = (eButtonset)unitType;

	bResult = true;

LB_RETURN:
	return bResult;
}

void Unit::Cleanup()
{
	delete mSprite;
	mSprite = nullptr;
}
