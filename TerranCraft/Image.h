#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageNumber.h"
#include "../BWLib/IScriptAnimation.h"
#include "../BWLib/SpriteNumber.h"
//#include "../BWLib/RLEType.h"

struct GRPHeader;
struct GRPFrame;
class Palette;
class Sprite;

class Image final
{
public:
	static BW::ImageNumber GetImagePrimaryID(BW::SpriteNumber spriteID);

	Image() = default;
	~Image() = default;

	bool Initialize(BW::ImageNumber imageID, Sprite* parent);

	void UpdateGraphicData();

	const GRPFrame* GetCurrentFrame() const;
	const uint8* GetCompressedImage() const;
	IntVector2 GetPosition() const;
	IntVector2 GetMapPosition() const { return mMapPosition; }
	IntVector2 GetScreenPosition() const { return mScreenPosition; }

	BW::ImageNumber GetImageID() const { return mImageID; }
	//BW::RLEType GetRLE() const { return mRLE; }
	uint8 GetDirection() const { return mDirection; }
	void SetDirection(uint8 direction);
	bool IsFlipped() const { return mFlags & 0x0002; }
	bool IsHidden() const { return mFlags & 0x0040; }
	void Hide(bool isHidden) { isHidden ? mFlags |= 0x0040 : mFlags &= ~0x0040; }
	void SetOffsets(IntVector2 offset) { mOffset = offset; }
	IntVector2 GetOffsets() const { return mOffset; }
	uint16 GetIScriptHeader() const { return mIScriptHeader; }
	uint16 GetIScriptOffset() const { return mIScriptOffset; }
	void SetIScriptOffset(uint16 offset) { mIScriptOffset = offset; }
	void SetAnim(BW::IScriptAnimation anim) { mAnim = anim; }
	BW::IScriptAnimation GetAnim() const { return mAnim; }
	uint8 GetSleep() const { return mSleep; }
	void SetSleep(uint32 sleep) { mSleep = sleep; }
	uint16 GetFrameSet() const { return mFrameSet; }
	void SetFrameSet(uint16 frameSet) { mFrameSet = frameSet; }
	uint32 GetFrameIndex() const { return mFrameIndex; }
	//void SetFrameIndex(uint32 frameIndex) { mFrameIndex = frameIndex; }
	void UpdateFrameIndex();
	IntRect GetGRPBounds() const { return mGRPBounds; }
	const GRPHeader* GetGRPFile() const { return mGRPFile; }
	const Palette* GetPalette() const { return mPalette; }
	const Sprite* GetParent() const { return mParent; }
	Sprite* GetParent() { return mParent; }

private:
	BW::ImageNumber mImageID = BW::ImageNumber::None;
	//BW::RLEType mRLE = BW::RLEType::Normal;
	uint8 mDirection = 0; // 0 ~ 31
	uint8 mFlags = 0;
	IntVector2 mOffset = { 0, };
	uint16 mIScriptHeader = 0;
	uint16 mIScriptOffset = 0;
	BW::IScriptAnimation mAnim = BW::IScriptAnimation::Init;
	uint8 mSleep = 0;
	uint16 mFrameSet = 0;
	uint16 mFrameIndex = 0;
	IntVector2 mMapPosition = { 0, };
	IntVector2 mScreenPosition = { 0, };
	IntRect mGRPBounds = { 0, };
	const GRPHeader* mGRPFile = nullptr;
	const Palette* mPalette = nullptr;
	Sprite* mParent = nullptr;
};
