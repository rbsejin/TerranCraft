#include "pch.h"
#include "Unit.h"
#include "Sprite.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(BW::UnitType unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	mSprite = new Sprite();
	if (!mSprite->Initalize(BW::SpriteNumber::Marine))
	{
		goto LB_RETURN;
	}

	bResult = true;

LB_RETURN:
	return bResult;
}

void Unit::Cleanup()
{
	delete mSprite;
	mSprite = nullptr;
}
