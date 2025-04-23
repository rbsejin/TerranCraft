#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageType.h"

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

struct GRPHeader;
struct PCXImage;

class ResourceManager
{
public:
	ResourceManager() = default;
	~ResourceManager();

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

	const GRPHeader* GetGRPFile(uint32 index) const { return mGRPFiles[index]; }
	const GRPHeader* GetButtonsGRP() const { return mButtonsGRP; }
	const GRPHeader* GetTCmdBtnsGRP() const { return mTCmdBtnsGRP; }
	const GRPHeader* GetWireframeGRP(uint32 index) const { return mWireframeGRPs[index]; }
	const GRPHeader* GetCursorGRP(uint32 index) const { return mCursorGRPs[index]; }

	const PCXImage* GetTConsolePCX() const { return mTConsolePCX; }
	const PCXImage* GetTUnitPCX() const { return mTUnitPCX; }
	const PCXImage* GetTSelectPCX() const { return mTSelectPCX; }
	const PCXImage* GetTWirePCX() const { return mTWirePCX; }

	const uint32* GetMapImage() const { return mMapImage; }
	uint32 GetMapImageWidth() const { return mMapImageWidth; }

private:
	bool loadDat(void** outData, uint32 dataSize, const char* filename);
	bool loadTBL(uint8** outData, const char* filename);

	bool loadGRP(GRPHeader** outGRPHeader, const char* filepath);
	void destroyGRP(GRPHeader** grpHeader);
	bool loadUnitImages();
	void destroyUnitImages();

	bool loadPCX(PCXImage** outPCXImage, const char* filepath);
	void destroyPCX(PCXImage** pcxImage);

	void loadMap();
	void destroyMap();

private:
#pragma region arr
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
#pragma endregion

	// GRP Unit Images
	enum { IMAGE_COUNT = eImage::None };
	GRPHeader* mGRPFiles[IMAGE_COUNT];

	// GRP
	GRPHeader* mButtonsGRP = nullptr;
	GRPHeader* mTCmdBtnsGRP = nullptr;
	enum { WIRE_FRAME_COUNT = 3 };
	GRPHeader* mWireframeGRPs[WIRE_FRAME_COUNT] = { 0, };
	enum { CURSOR_IMAGE_COUNT = 19 };
	GRPHeader* mCursorGRPs[CURSOR_IMAGE_COUNT] = { 0, };

	// PCX
	PCXImage* mTConsolePCX = nullptr;
	PCXImage* mTUnitPCX = nullptr;
	PCXImage* mTSelectPCX = nullptr;
	PCXImage* mTWirePCX = nullptr;

	// Map
	uint32* mMapImage = nullptr;
	uint32 mMapImageWidth = 0;
	uint32 mMapImageHeight = 0;
};
