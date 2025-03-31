#pragma once

#include "../Common/typedef.h"

struct ImageData;
struct UnitData;
struct SpriteData;
struct FlingyData;

class Arrangement
{
public:
	bool Load();
	void Destroy();

	uint16 GetImageCount() const { return *(uint16*)mImageTBLData; }
	const char* GetImageName(uint16 index) const;

	const ImageData* GetImageData() const { return mImageData; }
	const SpriteData* GetSpriteData() const { return mSpriteData; }
	const FlingyData* GetFlingyData() const { return mFlingyData; }
	const UnitData* GetUnitData() const { return mUnitData; }
private:
	Arrangement() = default;
	~Arrangement();
public:
	static Arrangement Instance;
private:
	uint8* mImageTBLData = nullptr;
	ImageData* mImageData = nullptr;
	SpriteData* mSpriteData = nullptr;
	FlingyData* mFlingyData = nullptr;
	UnitData* mUnitData = nullptr;
};
