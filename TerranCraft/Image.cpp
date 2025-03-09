#include "pch.h"

#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Sprite.h"
#include "Image.h"
#include "Camera.h"

BW::ImageNumber Image::GetImagePrimaryID(BW::SpriteNumber spriteID)
{
	BW::ImageNumber imageID = BW::ImageNumber::None;
	switch (spriteID)
	{
	case BW::SpriteNumber::Marine:
		imageID = BW::ImageNumber::Marine;
		break;
	case BW::SpriteNumber::SCV:
		imageID = BW::ImageNumber::SCV;
		break;
	case BW::SpriteNumber::Command_Center:
		imageID = BW::ImageNumber::Command_Center;
		break;
	default:
		break;
	}
	return imageID;
}

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
	case BW::ImageNumber::SCV:
		mPaletteIndex = ImageResource::Instance.GetPalette(6);
		break;
	case BW::ImageNumber::Command_Center:
		mPaletteIndex = ImageResource::Instance.GetPalette(6);
		break;
	case BW::ImageNumber::IMG_SELECT_022:
		mPaletteIndex = ImageResource::Instance.GetPalette(6);
		break;
	default:
#ifdef _DEBUG
		__debugbreak();
#endif // _DEBUG

		break;
	}

	bResult = true;

	return bResult;
}

void Image::Update()
{
	IntVector2 position = GetPosition();
	const GRPFrame* frame = GetCurrentFrame();
	mMapPosition.X = position.X - mGRPFile->Width / 2 + frame->X;
	mMapPosition.Y = position.Y - mGRPFile->Height / 2 + frame->Y;

	// TODO: Implement screen position calculation
	IntVector2 cameraPosition = Camera::Instance.GetPosition();
	mScreenPosition.X = mMapPosition.X - cameraPosition.X;
	mScreenPosition.Y = mMapPosition.Y - cameraPosition.Y;
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
	IntVector2 position = { 0, };
	if (mParent != nullptr)
	{
		position = mParent->GetPosition();
	}
	position.X += mOffset.X;
	position.Y += mOffset.Y;
	return position;
}
