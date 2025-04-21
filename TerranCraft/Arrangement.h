#pragma once

#include "../Common/typedef.h"

struct UnitData;
struct WeaponData;
struct FlingyData;
struct SpriteData;
struct ImageData;
struct UpgradeData;
struct TechData;
struct SoundData;
struct OrderData;
struct ButtonsetData;

class Arrangement
{
public:
	Arrangement() = default;
	~Arrangement();

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
	const ButtonsetData* GetButtonSet() const { return mButtonsetData; }

private:
	bool loadDat(void** outData, uint32 dataSize, const char* filename);
	bool loadTBL(uint8** outData, const char* filename);

private:
	UnitData* mUnitData = nullptr;
	WeaponData* mWeaponData = nullptr;
	FlingyData* mFlingyData = nullptr;
	SpriteData* mSpriteData = nullptr;
	uint8* mImageTBLData = nullptr;
	ImageData* mImageData = nullptr;
	UpgradeData* mUpgradeData = nullptr;
	TechData* mTechData = nullptr;
	uint8* mSfxTBLData = nullptr;
	SoundData* mSoundData = nullptr;
	OrderData* mOrderData = nullptr;
	uint8* mBtnsetData = nullptr;
	ButtonsetData* mButtonsetData = nullptr;
};
