#include "pch.h"
#include "Sprite.h"
#include "Image.h"
#include "BWFile.h"
#include "ResourceManager.h"
#include "../DDrawLib/DDrawDevice.h"
#include "Game.h"

Sprite::~Sprite()
{
	Cleanup();
}

bool Sprite::Initalize(eSprite spriteID)
{
	bool bResult = false;

	mSpriteID = spriteID;

	mImagePrimary = new Image();

	ResourceManager* resourceManager = gGame->GetResourceManager();
	const SpriteData* spriteData = resourceManager->GetSpriteData();
	eImage imageNumber = (eImage)spriteData->ImageFiles[(int32)spriteID];

	if (!mImagePrimary->Initialize(imageNumber, this))
	{
		delete mImagePrimary;
		mImagePrimary = nullptr;
		goto LB_RETURN;
	}

	mImages.push_back(mImagePrimary);

#if 1
	uint8 selectionCircleIndex = spriteData->SelectionCircleImages[(int32)spriteID - 130];
	eImage selectionCircle = (eImage)((uint16)eImage::IMGSELECT022 + selectionCircleIndex);

	mSelectionCircleImage = new Image();
	if (!mSelectionCircleImage->Initialize(selectionCircle, this))
	{
		delete mSelectionCircleImage;
		goto LB_RETURN;
	}
	int32 offsetY = spriteData->SelectionCircleOffsets[selectionCircleIndex];
	mSelectionCircleImage->SetOffsets({ 0, offsetY });
#endif
	bResult = true;

LB_RETURN:
	return bResult;
}

void Sprite::Cleanup()
{
	delete mSelectionCircleImage;
	mSelectionCircleImage = nullptr;

	for (Image* image : mImages)
	{
		delete image;
	}
	mImages.clear();

	mImagePrimary = nullptr;
}

void Sprite::Draw(DDrawDevice* ddrawDevice) const
{
	if (IsDrawSelectionCircle())
	{
		mSelectionCircleImage->DrawImage(ddrawDevice);
	}

	for (Image* image : mImages)
	{
		image->DrawImage(ddrawDevice);
	}
}

void Sprite::UpdatePrimaryAnim(eAnim anim)
{
	mImagePrimary->UpdateAnim(anim);
}

void Sprite::OnSelected(int32 selectedIndex)
{
#ifdef _DEBUG
	if (selectedIndex < 0 || selectedIndex >= 12)
	{
		__debugbreak();
	}
#endif // _DEBUG

	mFlags |= 0x09; // 0x01: Draw selection circle, 0x08: Selected
	mSelectionIndex = selectedIndex;
}

void Sprite::OnDeselected()
{
	mFlags &= ~0x09; // 0x01: Draw selection circle, 0x08: Selected
	mSelectionIndex = -1;
}
