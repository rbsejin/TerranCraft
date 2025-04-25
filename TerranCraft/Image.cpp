#include "pch.h"

#include "../ImageData/Graphic.h"
#include "Sprite.h"
#include "Image.h"
#include "Camera.h"
#include "../BWLib/ScriptType.h"
#include "AnimationController.h"
#include "BWFile.h"
#include "ResourceManager.h"

#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/PaletteManager.h"
#include "Game.h"

bool Image::Initialize(eImage imageID, Sprite* parent)
{
	bool bResult = false;

	mImageID = imageID;
	mParent = parent;

	ResourceManager* resourceManager = gGame->GetResourceManager();
	const ImageData* imageData = resourceManager->GetImageData();
	int32 index = imageData->GRPFiles[(int32)imageID] - 1;
	mGRPFile = resourceManager->GetGRPFile(index);

	uint32 IscriptID = imageData->IscriptIDs[(int32)imageID];

	mRemapping = imageData->Remappings[(int32)imageID];

	AnimationController* animationController = gGame->GetAnimationController();
	mIScriptHeader = animationController->GetIScriptHeader(IscriptID);
	mIScriptOffset = animationController->GetIScriptOffset(mIScriptHeader, mAnim);
	mIScriptOffsetCount = animationController->GetIScriptOffsetCount(IscriptID);

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
	Int32Vector2 position = GetPosition();
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
	Int32Vector2 cameraPosition = camera->GetPosition();
	Int32Vector2 screen = mMapPosition;
	screen.X -= cameraPosition.X;
	screen.Y -= cameraPosition.Y;

	Int32Rect bounds = { 0, 0, frame->Width, frame->Height };

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

	Int32Vector2 cameraSize = camera->GetSize();
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

	PaletteManager* paletteManager = gGame->GetPaletteManager();
	PALETTEENTRY* palette = nullptr;
	
	switch (mRemapping)
	{
	case 0:
		palette = paletteManager->Data;
		break;
	case 1:
		palette = paletteManager->OfireData;
		break;
	case 2:
		palette = paletteManager->GfireData;
		break;
	case 3:
		palette = paletteManager->BfireData;
		break;
	//case 4:
	//	palette = paletteManager->BexplData;
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

Int32Vector2 Image::GetPosition() const
{
	Int32Vector2 position = { 0, };
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

void Image::UpdateFrameIndex(uint16 frameIndex)
{
	mFrameIndex = frameIndex;
	mFrameSet = frameIndex / 0x11;
}

void Image::UpdateAnim(eAnim anim)
{
	mAnim = anim;
	AnimationController* animationController = gGame->GetAnimationController();
	mIScriptOffset = animationController->GetIScriptOffset(mIScriptHeader, mAnim);
	mSleep = 0;
}
