#pragma once

#include <list>
#include "../BWLib/SpriteNumber.h"
#include "../BWLib/UnitType.h"

class Image;

class Sprite
{
public:
	static BW::SpriteNumber GetSpriteNumber(BW::UnitType unitType);

	Sprite() = default;
	~Sprite();

	bool Initalize(BW::SpriteNumber spriteID);
	void Cleanup();

	IntVector2 GetPosition() const { return mPosition; }
	void SetPosition(IntVector2 position) { mPosition = position; }

	BW::SpriteNumber GetSpriteID() const { return mSpriteID; }
	int32 GetSelectionIndex() const { return mSelectionIndex; }
	void SetSelectionIndex(int32 selectionIndex) { mSelectionIndex = selectionIndex; }
	const Image* GetPrimaryImage() const { return mImagePrimary; }
	Image* GetPrimaryImage() { return mImagePrimary; }
	const std::list<Image*>* GetImages() const { return &mImages; }
	void AddBefore(Image* image) { mImages.push_front(image); }
	void AddAffter(Image* image) { mImages.push_back(image); }
	void RemoveImage(Image* image) { mImages.remove(image); }

private:
	BW::SpriteNumber mSpriteID = BW::SpriteNumber::None;
	//uint8 mPlayerID = 0;
	int32 mSelectionIndex = -1;
	//uint8 mVisibilityFlags = 0;
	//uint8 mElevationLevel = 0;
	//uint8 mFlags = 0;
	/*	0x01  Draw selection circle.
		0x02
		0x04
		0x08  Selected.
		0x10
		0x20  Hidden
		0x40  Burrowed
		0x80  Iscript unbreakable code section.
	*/
	//uint8 mSelectionTimer = 0;
	//uint16 mIndex = 0;
	IntVector2 mPosition = { 0, };
	Image* mImagePrimary = nullptr;
	//Image* mImageHead = nullptr;
	//Image* mImageTail = nullptr;

	std::list<Image*> mImages;
};

