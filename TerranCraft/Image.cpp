#include "pch.h"

#include "../ImageData/Graphic.h"
#include "Sprite.h"
#include "Image.h"
#include "Camera.h"
#include "../BWLib/IScriptEntryNumber.h"
#include "AnimationController.h"
#include "BWFile.h"
#include "Arrangement.h"

#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/Palette.h"
#include "Game.h"

bool Image::Initialize(BW::ImageNumber imageID, Sprite* parent)
{
	bool bResult = false;

	mImageID = imageID;
	mParent = parent;

	Arrangement* arrangement = gGame->GetArrangement();
	const ImageData* imageData = arrangement->GetImageData();
	int32 index = imageData->GRPFiles[(int32)imageID] - 1;
	mGRPFile = gGame->GRPFiles[index];

	uint32 IscriptID = imageData->IscriptIDs[(int32)imageID];

	mRemapping = imageData->Remappings[(int32)imageID];

	AnimationController* animationController = gGame->GetAnimationController();
	mIScriptHeader = animationController->GetIScriptHeader(IscriptID);
	mIScriptOffset = animationController->GetIScriptOffset(mIScriptHeader, mAnim);

	if (imageData->GfxTurns[(int32)imageID] != 0)
	{
		mFlags |= 0x08;
	}

	if (imageData->UseFullIscripts[(int32)imageID] != 0)
	{
		mFlags |= 0x10;
	}

	if (imageData->Clickables[(int32)imageID] != 0)
	{
		mFlags |= 0x20;
	}

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

	Camera* camera = gGame->GetCamera();
	IntVector2 cameraPosition = camera->GetPosition();
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

	IntVector2 cameraSize = camera->GetSize();
	bounds.Right = std::min<int>(bounds.Right, cameraSize.X - screen.X);
	bounds.Bottom = std::min<int>(bounds.Bottom, cameraSize.Y - screen.Y);

	mScreenPosition = screen;
	mGRPBounds = bounds;
}

void Image::DrawImage(DDrawDevice* ddrawDevice) const
{
	if (IsHidden())
	{
		return;
	}

	const GRPFrame* frame = GetCurrentFrame();
	const uint8* compressedImage = GetCompressedImage();

	PALETTEENTRY* palette = nullptr;
	
	switch (mRemapping)
	{
	case 0:
		palette = Palette::sData;
		break;
	case 1:
		palette = Palette::sOfireData;
		break;
	case 2:
		palette = Palette::sGfireData;
		break;
	case 3:
		palette = Palette::sBfireData;
		break;
	case 4:
		palette = Palette::sBexplData;
	default:
		break;
	}

	if (!IsFlipped())
	{
		ddrawDevice->DrawGRP2(mScreenPosition.X, mScreenPosition.Y, frame, mGRPBounds, compressedImage, palette);
	}
	else
	{
		ddrawDevice->DrawGRP2Flipped(mScreenPosition.X, mScreenPosition.Y, frame, mGRPBounds, compressedImage, palette);
	}

#if 0
	mDDrawDevice->DrawBound({ screenPosition.X, screenPosition.Y, screenPosition.X + grpBounds.Right, screenPosition.Y + grpBounds.Bottom }, 0xffff0000);
#endif
}

const GRPFrame* Image::GetCurrentFrame() const
{
#ifdef _DEBUG
	if (mFrameIndex >= mGRPFile->FrameCount)
	{
		__debugbreak();
	}
#endif // _DEBUG

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
