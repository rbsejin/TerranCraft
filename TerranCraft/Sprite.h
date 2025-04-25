#pragma once

#include <list>
#include "../BWLib/SpriteType.h"
#include "../BWLib/UnitType.h"
#include "../BWLib/AnimType.h"
#include "Image.h"

class Image;
class DDrawDevice;

class Sprite
{
public:
	Sprite() = default;
	~Sprite();

	bool Initalize(eSprite spriteID);
	void Cleanup();

	void Draw(DDrawDevice* ddrawDevice) const;
	void UpdatePrimaryAnim(eAnim anim);
	eAnim GetPrimaryImageAnim() const { return mImagePrimary->GetAnim(); }

	Int32Vector2 GetPosition() const { return mPosition; }
	void SetPosition(Int32Vector2 position) { mPosition = position; }

	eSprite GetSpriteID() const { return mSpriteID; }
	int32 GetSelectionIndex() const { return mSelectionIndex; }
	uint8 GetElevationLevel() const { return mElevationLevel; }
	void SetElevationLevel(uint8 elevationLevel) { mElevationLevel = elevationLevel; }
	const Image* GetPrimaryImage() const { return mImagePrimary; }
	Image* GetPrimaryImage() { return mImagePrimary; }
	void SetPrimaryImage(Image* image) { mImagePrimary = image; }
	const std::list<Image*>* GetImages() const { return &mImages; }
	std::list<Image*>* GetImages() { return &mImages; }
	void AddBefore(Image* image) { mImages.push_front(image); }
	void AddAffter(Image* image) { mImages.push_back(image); }
	void RemoveImage(Image* image) { mImages.remove(image); }
	uint32 GetImageCount() const { return (uint32)mImages.size(); }

	bool IsDrawSelectionCircle() const { return (mFlags & 0x01) != 0; }
	bool IsSelected() const { return (mFlags & 0x08) != 0; }
	bool IsHidden() const { return (mFlags & 0x20) != 0; }
	bool IsBurrowed() const { return (mFlags & 0x40) != 0; }
	bool IsUnbreakable() const { return (mFlags & 0x80) != 0; }
	uint8 GetFlags() const { return mFlags; }
	void SetFlags(uint8 flags) { mFlags = flags; }

	void OnSelected(int32 selectedIndex);
	void OnDeselected();
	Image* GetSelectionCircleImage() const { return mSelectionCircleImage; }

private:
	eSprite mSpriteID = eSprite::None;
	//uint8 mPlayerID = 0;
	int32 mSelectionIndex = -1;
	//uint8 mVisibilityFlags = 0;
	uint8 mElevationLevel = 0;
	uint8 mFlags = 0;
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
	Int32Vector2 mPosition = { 0, };
	Image* mImagePrimary = nullptr;
	//Image* mImageHead = nullptr;
	//Image* mImageTail = nullptr;

	std::list<Image*> mImages;
	Image* mSelectionCircleImage = nullptr;
};

