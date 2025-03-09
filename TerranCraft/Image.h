#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageNumber.h"
#include "../BWLib/IScriptAnimation.h"
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

	bool Initialize(BW::ImageNumber imageID, const Sprite* parent);

	void Update();

	const GRPFrame* GetCurrentFrame() const;
	const uint8* GetCompressedImage() const;
	IntVector2 GetPosition() const;
	IntVector2 GetMapPosition() const { return mMapPosition; }
	IntVector2 GetScreenPosition() const { return mScreenPosition; }

	BW::ImageNumber GetImageID() const { return mImageID; }
	void SetOffsets(IntVector2 offset) { mOffset = offset; }
	IntVector2 GetOffsets() const { return mOffset; }
	uint32 GetFrameIndex() const { return mFrameIndex; }
	const GRPHeader* GetGRPFile() const { return mGRPFile; }
	const Palette* GetPaletteIndex() const { return mPaletteIndex; }

private:
	BW::ImageNumber mImageID = BW::ImageNumber::None;
	//BW::RLEType mRLE = BW::RLEType::Normal;
	//uint8 direction = 0; // 0 ~ 31
	//ImageFlags mFlags = 0;
	IntVector2 mOffset = { 0, };
	//uint32 mIScriptHeader = 0;
	//uint32 mIScriptOffset = 0;
	//BW::IScriptAnimation mAnim = BW::IScriptAnimation::Init;
	//uint32 mSleep = 0;
	//uint32 mFrameSet = 0;
	uint32 mFrameIndex = 0;
	IntVector2 mMapPosition = { 0, };
	IntVector2 mScreenPosition = { 0, };
	//IntRect mGRPBounds = { 0, };
	const GRPHeader* mGRPFile = nullptr;
	const Palette* mPaletteIndex = nullptr;
	const Sprite* mParent = nullptr;
};
