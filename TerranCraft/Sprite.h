#pragma once

#include <list>
#include "../BWLib/SpriteNumber.h"

class Image;

class Sprite
{
private:
	BW::SpriteNumber mSpriteID = BW::SpriteNumber::None;
	//uint8 mPlayerID = 0;
	//uint8 mSelectionIndex = 0;
	//uint8 mVisibilityFlags = 0;
	//uint8 mElevationLevel = 0;
	//uint8 mFlags = 0;
	//uint8 mSelectionTimer = 0;
	//uint16 mIndex = 0;
	IntVector2 mPosition = { 0, };
	Image* mImagePrimary = nullptr;
	//Image* mImageHead = nullptr;
	//Image* mImageTail = nullptr;

	std::list<Image*> mImages;
public:
	Sprite() = default;
	~Sprite();

	bool Initalize(BW::SpriteNumber spriteID);
	void Cleanup();

	IntVector2 GetPosition() const { return mPosition; }
	void SetPosition(IntVector2 position) { mPosition = position; }

	BW::SpriteNumber GetSpriteID() const { return mSpriteID; }
	const Image* GetImage() const { return mImagePrimary; }
	const std::list<Image*>* GetImages() const { return &mImages; }
};

