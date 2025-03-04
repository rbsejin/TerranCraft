#include "pch.h"

#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Sprite.h"
#include "Image.h"

bool Image::Initialize(BW::ImageNumber imageID, const Sprite* parent)
{
	bool bResult = false;

	mImageID = imageID;
	mParent = parent;
	mGRPFile = ImageResource::Instance.GetGRPFile(imageID);

	switch (imageID)
	{
	case BW::ImageNumber::Marine:
		mPaletteIndex = ImageResource::Instance.GetPalette(6);
		break;
	case BW::ImageNumber::Marine_Shadow:
		mPaletteIndex = ImageResource::Instance.GetPalette(6);
		break;
	default:
		break;
	}

	bResult = true;

LB_RETURN:
	return bResult;
}

void Image::Update()
{
	IntVector2 position = GetPosition();
	const GRPFrame* frame = GetCurrentFrame();
	mMapPosition.X = position.X - mGRPFile->Width / 2 + frame->X;
	mMapPosition.Y = position.Y - mGRPFile->Height / 2 + frame->Y;

	// TODO: Implement screen position calculation
	mScreenPosition = mMapPosition;
}

const GRPFrame* Image::GetCurrentFrame() const
{
	return mGRPFile->Frames + mFrameIndex;
}

const uint8* Image::GetCompressedImage() const
{
	const GRPFrame* frame = GetCurrentFrame();
	return (uint8*)mGRPFile + frame->DataOffset;
}

IntVector2 Image::GetPosition() const
{
	IntVector2 position = mParent->GetPosition();
	position.X += mOffset.X;
	position.Y += mOffset.Y;
	return position;
}
