#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageType.h"
#include "../BWLib/AnimType.h"
#include "../BWLib/SpriteType.h"
#include "../BWLib/RLEType.h"

struct GRPHeader;
struct GRPFrame;
class Sprite;

class DDrawDevice;

class Image final
{
public:
	Image() = default;
	~Image() = default;

	bool Initialize(eImage imageID, Sprite* parent);
	void UpdateGraphicData();
	void DrawImage(DDrawDevice* ddrawDevice) const;

	const GRPFrame* GetCurrentFrame() const;
	const uint8* GetCompressedImage() const;
	Int32Vector2 GetPosition() const;
	Int32Vector2 GetMapPosition() const { return mMapPosition; }
	Int32Vector2 GetScreenPosition() const { return mScreenPosition; }

	eImage GetImageID() const { return mImageID; }
	eRLET GetRLE() const { return mRLE; }
	uint8 GetRemapping() const { return mRemapping; }
	uint8 GetDirection() const { return mDirection; }
	void SetDirection(uint8 direction);
	bool IsFlipped() const { return mFlags & 0x0002; }
	bool IsHidden() const { return mFlags & 0x0040; }
	void SetHidden(bool isHidden) { isHidden ? mFlags |= 0x0040 : mFlags &= ~0x0040; }
	void SetOffsets(Int32Vector2 offset) { mOffset = offset; }
	Int32Vector2 GetOffsets() const { return mOffset; }
	uint16 GetIScriptHeader() const { return mIScriptHeader; }
	uint16 GetIScriptOffset() const { return mIScriptOffset; }
	void SetIScriptOffset(uint16 offset) { mIScriptOffset = offset; }
	uint16 GetIScriptOffsetCount() const { return mIScriptOffsetCount; }
	eAnim GetAnim() const { return mAnim; }
	uint8 GetSleep() const { return mSleep; }
	void SetSleep(uint32 sleep) { mSleep = sleep; }
	uint16 GetFrameSet() const { return mFrameSet; }
	void SetFrameSet(uint16 frameSet) { mFrameSet = frameSet; }
	uint32 GetFrameIndex() const { return mFrameIndex; }
	void UpdateFrameIndex();
	void UpdateFrameIndex(uint16 frameIndex);
	Int32Rect GetGRPBounds() const { return mGRPBounds; }
	const GRPHeader* GetGRPFile() const { return mGRPFile; }
	const Sprite* GetParent() const { return mParent; }
	Sprite* GetParent() { return mParent; }

	void UpdateAnim(eAnim anim);

	bool HasRotationFrames() const { return mFlags & 0x0008; }
	bool IsFullIScript() const { return mFlags & 0x0010; }
	bool IsClickable() const { return mFlags & 0x0020; }

private:
	eImage mImageID = eImage::None;
	eRLET mRLE = eRLET::Normal;
	uint8 mRemapping = 0;
	uint8 mDirection = 0; // 0 ~ 31
	uint8 mFlags = 0;
	/* flags
	 0x0001  - Redraw
	 0x0002  - Flipped/Mirrored
	 0x0004  - FreezeY
	 0x0008  - Has rotation frames
	 0x0010  - FullIScript
	 0x0020  - Clickable
	 0x0040  - Hidden/Invisible (don't draw)
	 0x0080  - UseParentLO
	*/
	Int32Vector2 mOffset = { 0, };
	uint16 mIScriptHeader = 0;
	uint16 mIScriptOffset = 0;
	uint16 mIScriptOffsetCount = 0;
	eAnim mAnim = eAnim::Init;
	uint8 mSleep = 0;
	uint16 mFrameSet = 0;
	uint16 mFrameIndex = 0;
	Int32Vector2 mMapPosition = { 0, };
	Int32Vector2 mScreenPosition = { 0, };
	Int32Rect mGRPBounds = { 0, };
	const GRPHeader* mGRPFile = nullptr;
	Sprite* mParent = nullptr;
};
