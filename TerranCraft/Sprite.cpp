#include "pch.h"
#include "Sprite.h"
#include "Image.h"
#include "BWFile.h"
#include "Arrangement.h"

BW::SpriteNumber Sprite::GetSpriteNumber(BW::UnitType unitType)
{
	BW::SpriteNumber spriteNumber = BW::SpriteNumber::None;
	switch (unitType)
	{
	case BW::UnitType::Terran_Marine:
		spriteNumber = BW::SpriteNumber::Marine;
		break;
	case BW::UnitType::Terran_SCV:
		spriteNumber = BW::SpriteNumber::SCV;
		break;
	case BW::UnitType::Terran_Command_Center:
		spriteNumber = BW::SpriteNumber::Command_Center;
		break;
	default:
		break;
	}
	return spriteNumber;
}

Sprite::~Sprite()
{
	Cleanup();
}

bool Sprite::Initalize(BW::SpriteNumber spriteID)
{
	bool bResult = false;

	mSpriteID = spriteID;

	mImagePrimary = new Image();
	//BW::ImageNumber imageNumber = Image::GetImagePrimaryID(spriteID);

	const SpriteData* spriteData = Arrangement::Instance.GetSpriteData();
	BW::ImageNumber imageNumber = (BW::ImageNumber)spriteData->ImageFiles[(int32)spriteID];

	if (!mImagePrimary->Initialize(imageNumber, this))
	{
		delete mImagePrimary;
		mImagePrimary = nullptr;
		goto LB_RETURN;
	}

	mImages.push_back(mImagePrimary);

#if 1

	uint8 selectionCircleIndex = spriteData->SelectionCircleImages[(int32)spriteID - 130];
	BW::ImageNumber selectionCircle = (BW::ImageNumber)((uint16)BW::ImageNumber::IMG_SELECT_022 + selectionCircleIndex);
	//BW::ImageNumber selectionCircle = BW::ImageNumber::IMG_SELECT_022;
	Image* selectionCircleImage = new Image();
	if (!selectionCircleImage->Initialize(selectionCircle, this))
	{
		delete selectionCircleImage;
		goto LB_RETURN;
	}

	//selectionCircleImage->SetOffsets({ 0, 11 });
	mImages.push_back(selectionCircleImage);
#endif
	bResult = true;

LB_RETURN:
	return bResult;
}

void Sprite::Cleanup()
{
	for (Image* image : mImages)
	{
		delete image;
	}
	mImages.clear();

	mImagePrimary = nullptr;
}
