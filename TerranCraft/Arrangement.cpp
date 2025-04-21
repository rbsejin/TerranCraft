#include "pch.h"
#include "Arrangement.h"
#include "BWFile.h"

Arrangement::~Arrangement()
{
	Destroy();
}

bool Arrangement::Load()
{
	bool bResult = false;

	loadDat((void**)&mUnitData, sizeof(UnitData), "../data/STARDAT/arr/units.dat");
	loadDat((void**)&mWeaponData, sizeof(WeaponData), "../data/STARDAT/arr/weapons.dat");
	loadDat((void**)&mFlingyData, sizeof(FlingyData), "../data/STARDAT/arr/flingy.dat");
	loadDat((void**)&mSpriteData, sizeof(SpriteData), "../data/STARDAT/arr/sprites.dat");
	loadDat((void**)&mImageData, sizeof(ImageData), "../data/STARDAT/arr/images.dat");
	loadDat((void**)&mUpgradeData, sizeof(UpgradeData), "../data/STARDAT/arr/upgrades.dat");
	loadDat((void**)&mTechData, sizeof(TechData), "../data/STARDAT/arr/techdata.dat");
	loadDat((void**)&mSoundData, sizeof(SoundData), "../data/STARDAT/arr/sounddata.dat");
	loadDat((void**)&mOrderData, sizeof(OrderData), "../data/STARDAT/arr/orders.dat");

	loadTBL(&mImageTBLData, "../data/STARDAT/arr/images.tbl");
	loadTBL(&mSfxTBLData, "../data/STARDAT/arr/sfxdata.tbl");

#pragma region btnset.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/btnset.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);

		mBtnsetData = (uint8*)malloc(fileSize);
		uint8* p = mBtnsetData;

		fread(p, fileSize, 1, fp);

		mButtonsetData = (ButtonsetData*)malloc(fileSize);

		for (int32 i = 0; i < ButtonsetData::BUTTON_SET_COUNT; i++)
		{
			mButtonsetData->Buttonsets[i] = (ButtonsetData::Buttonset*)p;
			uint32 buttonCount = mButtonsetData->Buttonsets[i]->ButtonCount;
			if (buttonCount == 0)
			{
				p += 8;
				continue;
			}

			p += sizeof(ButtonsetData::Buttonset) + (buttonCount - 1) * sizeof(ButtonsetData::Buttonset::ButtonInfo);
		}

		fclose(fp);
	}
#pragma endregion

	bResult = true;

LB_RETURN:
	return bResult;
}

void Arrangement::Destroy()
{
	free(mButtonsetData);
	mButtonsetData = nullptr;

	free(mBtnsetData);
	mBtnsetData = nullptr;

	free(mOrderData);
	mOrderData = nullptr;

	free(mSoundData);
	mSoundData = nullptr;

	free(mSfxTBLData);
	mSfxTBLData = nullptr;

	free(mTechData);
	mTechData = nullptr;

	free(mUpgradeData);
	mUpgradeData = nullptr;

	free(mImageData);
	mImageData = nullptr;

	free(mImageTBLData);
	mImageTBLData = nullptr;

	free(mSpriteData);
	mSpriteData = nullptr;

	free(mFlingyData);
	mFlingyData = nullptr;

	free(mWeaponData);
	mWeaponData = nullptr;

	free(mUnitData);
	mUnitData = nullptr;
}

const char* Arrangement::GetImageName(uint16 index) const
{
	uint16 offset = *(uint16*)(mImageTBLData + (index + 1) * 2);
	return (const char*)(mImageTBLData + offset);
}

bool Arrangement::loadDat(void** outData, uint32 dataSize, const char* filename)
{
	bool bResult = false;

	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);

	if (fileSize != dataSize)
	{
		goto LB_FILE_CLOSE;
	}

	free(*outData);
	*outData = malloc(fileSize);

	rewind(fp);
	fread(*outData, fileSize, 1, fp);

	bResult = true;

LB_FILE_CLOSE:
	fclose(fp);

LB_RETURN:
	return bResult;
}

bool Arrangement::loadTBL(uint8** outData, const char* filename)
{
	bool bResult = false;

	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);

	rewind(fp);

	free(*outData);
	*outData = (uint8*)malloc(fileSize);

	fread(*outData, fileSize, 1, fp);

	uint16 count = *(uint16*)*outData;
	uint16* offsets = (uint16*)(*outData + 2);

#if 0
	for (int32 i = 0; i < count; i++)
	{
		uint16 offset = offsets[i];
		char* filepath = (char*)(*outData + offset);

		char buf[256];
		sprintf_s(buf, "%d: %s\n", i, filepath);
		OutputDebugStringA(buf);
	}
#endif

	fclose(fp);

	bResult = true;
LB_RETURN:

	return bResult;
}
