#include "pch.h"
#include "Sprite.h"
#include "Image.h"

Sprite::~Sprite()
{
	Cleanup();
}

bool Sprite::Initalize(BW::SpriteNumber spriteID)
{
	bool bResult = false;

	mSpriteID = spriteID;

	mImagePrimary = new Image();
	if (!mImagePrimary->Initialize(BW::ImageNumber::Marine, this))
	{
		delete mImagePrimary;
		mImagePrimary = nullptr;
		goto LB_RETURN;
	}

	mImages.push_back(mImagePrimary);

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
