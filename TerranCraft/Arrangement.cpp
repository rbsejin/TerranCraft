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

#pragma region images.tbl
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/images.tbl", "rb");

		if (fp == nullptr)
		{
			goto LB_RETURN;
		}

		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);

		rewind(fp);

		mImageTBLData = (uint8*)malloc(fileSize);

		fread(mImageTBLData, fileSize, 1, fp);

		uint16 count = *(uint16*)mImageTBLData;
		uint16* offsets = (uint16*)(mImageTBLData + 2);

		//for (int32 i = 0; i < count; i++)
		//{
		//	uint16 offset = offsets[i];
		//	char* filepath = (char*)(mImageTBLData + offset);

		//	char buf[256];
		//	sprintf_s(buf, "%d:%s\n", i, filepath);
		//	OutputDebugStringA(buf);
		//}

		fclose(fp);
	}
#pragma endregion

#pragma region images.dat
	{
		FILE* fp = nullptr;

		fopen_s(&fp, "../data/STARDAT/arr/images.dat", "rb");

		if (fp == nullptr)
		{
			goto LB_RETURN;
		}

		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);

		rewind(fp);

		mImageData = new ImageData();

		fread(mImageData, fileSize, 1, fp);

		fclose(fp);
	}
#pragma endregion

#pragma region sprites.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/sprites.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);
		mSpriteData = new SpriteData();
		fread(mSpriteData, fileSize, 1, fp);
		fclose(fp);
	}
#pragma endregion

#pragma region flingy.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/flingy.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);
		mFlingyData = new FlingyData();
		fread(mFlingyData, fileSize, 1, fp);
		fclose(fp);
	}
#pragma endregion

#pragma region units.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/units.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);
		mUnitData = new UnitData();
		fread(mUnitData, fileSize, 1, fp);
		fclose(fp);
	}
#pragma endregion

#pragma region weapons.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/weapons.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);
		mWeaponData = new WeaponData();
		fread(mWeaponData, fileSize, 1, fp);
		fclose(fp);
	}
#pragma endregion

#pragma region upgrades.dat
	{
		FILE* fp = nullptr;
		fopen_s(&fp, "../data/STARDAT/arr/upgrades.dat", "rb");
		if (fp == nullptr)
		{
			goto LB_RETURN;
		}
		fseek(fp, 0, SEEK_END);
		int32 fileSize = ftell(fp);
		rewind(fp);
		mUpgradeData = new UpgradeData();
		fread(mUpgradeData, fileSize, 1, fp);
		fclose(fp);
	}
#pragma endregion


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

		mButtonSetData = (uint8*)malloc(fileSize);
		uint8* p = mButtonSetData;

		fread(p, fileSize, 1, fp);

		mButtonSet = (ButtonSet*)malloc(fileSize);

		for (int32 i = 0; i < ButtonSet::BUTTON_SET_COUNT; i++)
		{
			mButtonSet->ButtonLists[i] = (ButtonSet::ButtonList*)p;
			uint32 buttonCount = mButtonSet->ButtonLists[i]->ButtonCount;
			if (buttonCount == 0)
			{
				p += 8;
				continue;
			}

			p += sizeof(ButtonSet::ButtonList) + (buttonCount - 1) * sizeof(ButtonSet::ButtonList::ButtonInfo);
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
	delete mUpgradeData;
	mUpgradeData = nullptr;
	free(mButtonSetData);
	mButtonSetData = nullptr;
	free(mButtonSet);
	mButtonSet = nullptr;
	delete mWeaponData;
	mWeaponData = nullptr;
	delete mUnitData;
	mUnitData = nullptr;
	delete mFlingyData;
	mFlingyData = nullptr;
	free(mImageTBLData);
	mImageTBLData = nullptr;
	delete mImageData;
	mImageData = nullptr;
	delete mSpriteData;
	mSpriteData = nullptr;
}

const char* Arrangement::GetImageName(uint16 index) const
{
	uint16 offset = *(uint16*)(mImageTBLData + (index + 1) * 2);
	return (const char*)(mImageTBLData + offset);
}
