#include "pch.h"

#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Sprite.h"
#include "Image.h"
#include "Camera.h"
#include "../BWLib/IScriptEntryNumber.h"
#include "AnimationController.h"
#include "BWFile.h"
#include "Arrangement.h"

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

bool Image::Initialize(BW::ImageNumber imageID, Sprite* parent)
{
	bool bResult = false;

	mImageID = imageID;
	mParent = parent;
	mGRPFile = ImageResource::Instance.GetGRPFile(imageID);

	const ImageData* imageData = Arrangement::Instance.GetImageData();
	

	uint32 IscriptID = imageData->IscriptIDs[(int32)imageID];

#if 0
	switch (imageID)
	{
	case BW::ImageNumber::Marine:
		//mPalette = ImageResource::Instance.GetPalette(6);
		iscriptEntryNumber = BW::IScriptEntryNumber::Marine;
		break;
	case BW::ImageNumber::Marine_Shadow:
		//mPalette = ImageResource::Instance.GetPalette(4);
		iscriptEntryNumber = BW::IScriptEntryNumber::Shadow_Header;
		break;
	case BW::ImageNumber::SCV:
		//mPalette = ImageResource::Instance.GetPalette(6);
		iscriptEntryNumber = BW::IScriptEntryNumber::SCV;
		break;
	case BW::ImageNumber::SCV_Shadow:
		//mPalette = ImageResource::Instance.GetPalette(1);
		iscriptEntryNumber = BW::IScriptEntryNumber::Shadow_Header;
		break;
	case BW::ImageNumber::Command_Center:
		//mPalette = ImageResource::Instance.GetPalette(6);
		iscriptEntryNumber = BW::IScriptEntryNumber::Command_Center;
		break;
	case BW::ImageNumber::Command_Center_Shadow:
		//mPalette = ImageResource::Instance.GetPalette(1);
		iscriptEntryNumber = BW::IScriptEntryNumber::Shadow_Header;
		break;
	case BW::ImageNumber::Command_Center_Overlay:
		//mPalette = ImageResource::Instance.GetPalette(6);
		iscriptEntryNumber = BW::IScriptEntryNumber::Command_Center_Overlay;
		break;
	case BW::ImageNumber::IMG_SELECT_022:
		//mPalette = ImageResource::Instance.GetPalette(6);
		iscriptEntryNumber = BW::IScriptEntryNumber::Circle_Marker;
		break;
	default:
#ifdef _DEBUG
		__debugbreak();
#endif // _DEBUG
		break;
	}
#endif

	mPalette = ImageResource::Instance.GetPalette(6);

	mIScriptHeader = AnimationController::Instance.GetIScriptHeader(IscriptID);
	mIScriptOffset = AnimationController::Instance.GetIScriptOffset(mIScriptHeader, mAnim);

	bResult = true;

	return bResult;
}

void Image::UpdateGraphicData()
{
	IntVector2 position = GetPosition();
	const GRPFrame* frame = GetCurrentFrame();

	if (!(mFlags & 0x02))
	{
		mMapPosition.X = position.X - mGRPFile->Width / 2 + frame->X;
	}
	else
	{
		mMapPosition.X = position.X + mGRPFile->Width / 2 - (frame->Width + frame->X);
	}

	mMapPosition.Y = position.Y - mGRPFile->Height / 2 + frame->Y;

	IntVector2 cameraPosition = Camera::Instance.GetPosition();
	IntVector2 screen = mMapPosition;
	screen.X -= cameraPosition.X;
	screen.Y -= cameraPosition.Y;

	IntRect bounds = { 0, 0, frame->Width, frame->Height };

	if (screen.X < 0)
	{
		bounds.Right += screen.X;
		bounds.Left -= screen.X;
		screen.X = 0;
	}

	if (screen.Y < 0)
	{
		bounds.Bottom += screen.Y;
		bounds.Top -= screen.Y;
		screen.Y = 0;
	}

	IntVector2 cameraSize = Camera::Instance.GetSize();
	bounds.Right = std::min<int>(bounds.Right, cameraSize.X - screen.X);
	bounds.Bottom = std::min<int>(bounds.Bottom, cameraSize.Y - screen.Y);

	mScreenPosition = screen;
	mGRPBounds = bounds;
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

void Image::SetDirection(uint8 direction)
{
#ifdef _DEBUG
	if (direction >= 0x20)
	{
		__debugbreak();
	}
#endif // _DEBUG

	mDirection = direction;

	UpdateFrameIndex();
}

void Image::UpdateFrameIndex()
{
	int32 d = mDirection;
	if (d > 0x10)
	{
		d = 0x20 - d;
		mFlags |= 0x0002;
	}
	else
	{
		mFlags &= ~0x0002;
	}

	mFrameIndex = mFrameSet * 0x11 + d;
}
