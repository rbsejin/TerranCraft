#pragma once

#include "../Common/typedef.h"

struct ImageData;
struct UnitData;
struct SpriteData;
struct FlingyData;
struct ButtonSet;
struct WeaponData;
struct UpgradeData;

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
	const WeaponData* GetWeaponData() const { return mWeaponData; }
	const UpgradeData* GetUpgradeData() const { return mUpgradeData; }
	const ButtonSet* GetButtonSet() const { return mButtonSet; }
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
	WeaponData* mWeaponData = nullptr;
	UpgradeData* mUpgradeData = nullptr;
	ButtonSet* mButtonSet = nullptr;
	uint8* mButtonSetData = nullptr;
};
