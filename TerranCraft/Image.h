#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageNumber.h"
#include "../BWLib/IScriptAnimation.h"
#include "../BWLib/SpriteNumber.h"
#include "../BWLib/RLEType.h"
//#include "../BWLib/RLEType.h"

struct GRPHeader;
struct GRPFrame;
class Sprite;

class DDrawDevice;

class Image final
{
public:
	Image() = default;
	~Image() = default;

	bool Initialize(BW::ImageNumber imageID, Sprite* parent);

	void UpdateGraphicData();

	void DrawImage(DDrawDevice* ddrawDevice) const;

	const GRPFrame* GetCurrentFrame() const;
	const uint8* GetCompressedImage() const;
	IntVector2 GetPosition() const;
	IntVector2 GetMapPosition() const { return mMapPosition; }
	IntVector2 GetScreenPosition() const { return mScreenPosition; }

	BW::ImageNumber GetImageID() const { return mImageID; }
	BW::RLEType GetRLE() const { return mRLE; }
	uint8 GetRemapping() const { return mRemapping; }
	uint8 GetDirection() const { return mDirection; }
	void SetDirection(uint8 direction);
	bool IsFlipped() const { return mFlags & 0x0002; }
	bool IsHidden() const { return mFlags & 0x0040; }
	void SetHidden(bool isHidden) { isHidden ? mFlags |= 0x0040 : mFlags &= ~0x0040; }
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
	void UpdateFrameIndex();
	IntRect GetGRPBounds() const { return mGRPBounds; }
	const GRPHeader* GetGRPFile() const { return mGRPFile; }
	const Sprite* GetParent() const { return mParent; }
	Sprite* GetParent() { return mParent; }

private:
	BW::ImageNumber mImageID = BW::ImageNumber::None;
	BW::RLEType mRLE = BW::RLEType::Normal;
	uint8 mRemapping = 0;
	uint8 mDirection = 0; // 0 ~ 31
	/*  0x0001  - Redraw
		0x0002  - Flipped/Mirrored
		0x0008  - Has rotation frames
		0x0020  - Clickable
		0x0040  - Hidden/Invisible (don't draw)
	*/
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
	Sprite* mParent = nullptr;
};
