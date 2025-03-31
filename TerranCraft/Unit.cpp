#include "pch.h"
#include "Unit.h"
#include "Sprite.h"
#include "CommandIcon.h"
#include "Arrangement.h"
#include "BWFile.h"

Unit::~Unit()
{
	Cleanup();
}

bool Unit::Initialize(BW::UnitType unitType)
{
	bool bResult = false;

	mUnitType = unitType;

	mSprite = new Sprite();

	const UnitData* unitData = Arrangement::Instance.GetUnitData();
	uint8 scripteID = unitData->Graphics[(uint32)unitType];
	const FlingyData* flingyData = Arrangement::Instance.GetFlingyData();
	BW::SpriteNumber spriteNumber = (BW::SpriteNumber)flingyData->Sprites[scripteID];

	if (!mSprite->Initalize(spriteNumber))
	{
		goto LB_RETURN;
	}

	mCurrentButtonset = (eButtonset)unitType;

	uint32 hp = unitData->HitPoints[(uint32)unitType];
	if (hp != 1)
	{
		mHP = hp >> 8;
	}
	else
	{
		mHP = 0;
	}

	Int16Rect contourBounds = unitData->UnitDimensions[(uint32)unitType];
	mContourBounds.Left = contourBounds.Left;
	mContourBounds.Top = contourBounds.Top;
	mContourBounds.Right = contourBounds.Right;
	mContourBounds.Bottom = contourBounds.Bottom;

	bResult = true;

LB_RETURN:
	return bResult;
}

void Unit::Cleanup()
{
	delete mSprite;
	mSprite = nullptr;
}
