#include "pch.h"
#include "ResourceManager.h"
#include "BWFile.h"
#include "../ImageData/Graphic.h"
#include "Map.h"
#include "PathFinder.h"

ResourceManager::~ResourceManager()
{
	Destroy();
}

bool ResourceManager::Load()
{
	bool bResult = false;

	if (!loadDat((void**)&mUnitData, sizeof(UnitData), "../data/STARDAT/arr/units.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mWeaponData, sizeof(WeaponData), "../data/STARDAT/arr/weapons.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mFlingyData, sizeof(FlingyData), "../data/STARDAT/arr/flingy.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mSpriteData, sizeof(SpriteData), "../data/STARDAT/arr/sprites.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mImageData, sizeof(ImageData), "../data/STARDAT/arr/images.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mUpgradeData, sizeof(UpgradeData), "../data/STARDAT/arr/upgrades.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mTechData, sizeof(TechData), "../data/STARDAT/arr/techdata.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mSoundData, sizeof(SoundData), "../data/STARDAT/arr/sfxdata.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadDat((void**)&mOrderData, sizeof(OrderData), "../data/STARDAT/arr/orders.dat"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadTBL(&mImageTBLData, "../data/STARDAT/arr/images.tbl"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

	if (!loadTBL(&mSfxTBLData, "../data/STARDAT/arr/sfxdata.tbl"))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

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
		memset(mBtnsetData, 0, fileSize);
		uint8* p = mBtnsetData;

		fread(p, fileSize, 1, fp);

		mButtonsetData = (ButtonsetData*)malloc(fileSize);
		memset(mButtonsetData, 0, fileSize);

		for (int32 i = 0; i < ButtonsetData::BUTTON_SET_COUNT; i++)
		{
			mButtonsetData->Buttonsets[i] = (Buttonset*)p;
			uint32 buttonCount = mButtonsetData->Buttonsets[i]->ButtonCount;
			if (buttonCount == 0)
			{
				p += 8;
				continue;
			}

			p += sizeof(Buttonset) + (buttonCount - 1) * sizeof(ButtonInfo);
		}

		fclose(fp);
	}
#pragma endregion

	loadUnitImages();

	loadGRP(&mButtonsGRP, "../data/STARDAT/unit/cmdbtns/cmdicons.grp");
	loadGRP(&mTCmdBtnsGRP, "../data/STARDAT/unit/cmdbtns/tcmdbtns.grp");
#pragma region wirefram
	{
		char paths[WIRE_FRAME_COUNT][MAX_PATH] = {
			"../data/STARDAT/unit/wirefram/grpwire.grp", // multi selection wireframe
			"../data/STARDAT/unit/wirefram/tranwire.grp", // transport wireframe
			"../data/STARDAT/unit/wirefram/wirefram.grp" // single selection wireframe
		};

		for (int32 i = 0; i < WIRE_FRAME_COUNT; i++)
		{
			loadGRP(mWireframeGRPs + i, paths[i]);
		}
	}
#pragma endregion

#pragma region Cursor
	FILE* cursorListFile = nullptr;

	fopen_s(&cursorListFile, "../data/cursorlist.txt", "rb");

	if (cursorListFile == nullptr)
	{
		goto LB_RETURN;
	}

	char buffer[1024] = { 0, };
	int32 index = 0;
	char cursorFilename[MAX_PATH] = { 0, };

	while (fgets(buffer, 1024, cursorListFile) != nullptr)
	{
		int count = sscanf(buffer, "%d %s", &index, cursorFilename);
		if (count != 2)
		{
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG
			continue;
		}
		if (strlen(cursorFilename) == 0)
		{
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG
			continue;
		}

		loadGRP(mCursorGRPs + index, cursorFilename);
	}

	fclose(cursorListFile);
#pragma endregion

	loadPCX(&mTConsolePCX, "../data/STARDAT/game/tconsole.pcx");
	loadPCX(&mTUnitPCX, "../data/STARDAT/game/tunit.pcx");
	loadPCX(&mTSelectPCX, "../data/STARDAT/game/tselect.pcx");
	loadPCX(&mTWirePCX, "../data/STARDAT/game/twire.pcx");

	loadMap();

	bResult = true;

LB_RETURN:
	return bResult;
}

void ResourceManager::Destroy()
{
	destroyMap();

	destroyPCX(&mTSelectPCX);
	destroyPCX(&mTUnitPCX);
	destroyPCX(&mTConsolePCX);
	destroyPCX(&mTWirePCX);

	destroyGRP(&mTCmdBtnsGRP);

	for (int32 i = 0; i < WIRE_FRAME_COUNT; i++)
	{
		destroyGRP(mWireframeGRPs + i);
	}

	for (int i = 0; i < CURSOR_IMAGE_COUNT; i++)
	{
		if (mCursorGRPs[i] != nullptr)
		{
			free(mCursorGRPs[i]);
			mCursorGRPs[i] = nullptr;
		}
	}

	destroyGRP(&mButtonsGRP);

	uint32 grpCount = GetImageCount();

	destroyUnitImages();

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

const char* ResourceManager::GetImageName(uint16 index) const
{
	uint16 offset = *(uint16*)(mImageTBLData + (index + 1) * 2);
	return (const char*)(mImageTBLData + offset);
}

bool ResourceManager::loadDat(void** outData, uint32 dataSize, const char* filename)
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

bool ResourceManager::loadTBL(uint8** outData, const char* filename)
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

bool ResourceManager::loadGRP(GRPHeader** outGRPHeader, const char* filepath)
{
	bool bResult = false;

	FILE* fp = nullptr;
	fopen_s(&fp, filepath, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);

	rewind(fp);

	*outGRPHeader = (GRPHeader*)malloc(fileSize);
	fread(*outGRPHeader, fileSize, 1, fp);

	fclose(fp);

	bResult = true;

LB_RETURN:
	return false;
}

void ResourceManager::destroyGRP(GRPHeader** grpHeader)
{
	free(*grpHeader);
	*grpHeader = nullptr;
}

bool ResourceManager::loadUnitImages()
{
	bool bResult = false;

#pragma region Load GRP
	{
		constexpr size_t BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE] = { 0, };
		int index = 0;

		uint32 grpCount = GetImageCount();
		for (uint32 i = 0; i < grpCount; i++)
		{
			const char* grpFilename = GetImageName(i);
			char grpListFilename[MAX_PATH] = { 0, };
			sprintf(grpListFilename, "%s/%s", "../data/STARDAT/unit/", grpFilename);

			// Load GRP files
			loadGRP(mGRPFiles + i, grpListFilename);
		}
	}
#pragma endregion

	bResult = true;

	return bResult;
}

void ResourceManager::destroyUnitImages()
{
	for (uint32 i = 0; i < GetImageCount(); i++)
	{
		destroyGRP(mGRPFiles + i);
	}
}


bool ResourceManager::loadPCX(PCXImage** outPCXImage, const char* filepath)
{
	bool bResult = false;

#pragma region Read File
	FILE* fp = nullptr;
	fopen_s(&fp, filepath, "rb");
	if (fp == nullptr)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);
	rewind(fp);

	Chunk* chunk = (Chunk*)malloc(fileSize + sizeof(Chunk) - 1);
	memset(chunk, 0, fileSize + sizeof(Chunk) - 1);

	chunk->Length = fileSize;
	fread(chunk->Data, 1, fileSize, fp);

	fclose(fp);
#pragma endregion

	PCXHeader header = { 0, };
	int32 width = 0;
	int32 height = 0;

	if (chunk->Length <= sizeof(PCXHeader) + 0x300)
	{
		goto LB_RETURN;
	}

	memcpy(&header, chunk->Data, sizeof(PCXHeader));
	width = header.XMax - header.XMin + 1;
	height = header.YMax - header.YMin + 1;

	if (header.Manufacturer != 0x0a ||
		header.Version != 5 ||
		header.Encoding != 1 ||
		header.BitsPerPixel != 8 ||
		header.PlaneCount != 1 ||
		width < 0 ||
		height < 0 ||
		header.BytesPerLine & 0x01 ||
		header.BytesPerLine < width)
	{
		goto LB_RETURN;
	}

	size_t pcxSize = sizeof(PCXImage) + chunk->Length - sizeof(PCXHeader) - 0x300 - 1;
	*outPCXImage = (PCXImage*)malloc(pcxSize);
	memset(*outPCXImage, 0, pcxSize);

	(*outPCXImage)->Width = (int32)header.BytesPerLine;
	(*outPCXImage)->Height = (int32)height;

	size_t dataSize = chunk->Length - sizeof(PCXHeader) - 0x300;
	memcpy((*outPCXImage)->Data, chunk->Data + sizeof(PCXHeader), dataSize);

	const uint8* paletteCurrent = chunk->Data + chunk->Length - 0x300;

	if (*(paletteCurrent - 1) != 0x0c)
	{
		goto LB_RETURN;
	}

	for (int32 i = 0; i < 256; i++)
	{
		(*outPCXImage)->PaletteData[i].peRed = paletteCurrent[3 * i];
		(*outPCXImage)->PaletteData[i].peGreen = paletteCurrent[3 * i + 1];
		(*outPCXImage)->PaletteData[i].peBlue = paletteCurrent[3 * i + 2];
		(*outPCXImage)->PaletteData[i].peFlags = 0;
	}

	free(chunk);
	chunk = nullptr;

	bResult = true;

LB_RETURN:
	return bResult;
}

void ResourceManager::destroyPCX(PCXImage** pcxImage)
{
	free(*pcxImage);
	*pcxImage = nullptr;
}

void ResourceManager::loadMap()
{
#pragma region Read Files
	FILE* fp = nullptr;

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.CV5", "rb");
	if (fp == nullptr)
	{
		return;
	}
	CV5* cv5 = new CV5;
	memset(cv5, 0, sizeof(CV5));
	fread(cv5, sizeof(CV5), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VF4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VF4* vf4 = new VF4;
	memset(vf4, 0, sizeof(VF4));
	fread(vf4, sizeof(VF4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VX4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VX4* vx4 = new VX4;
	memset(vx4, 0, sizeof(VX4));
	fread(vx4, sizeof(VX4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VR4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VR4* vr4 = new VR4;
	memset(vr4, 0, sizeof(VR4));
	fread(vr4, sizeof(VR4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.WPE", "rb");
	if (fp == nullptr)
	{
		return;
	}
	WPE* wpe = new WPE;
	memset(wpe, 0, sizeof(WPE));
	fread(wpe, sizeof(WPE), 1, fp);
	fclose(fp);

	const int32 width = 128;
	const int32 height = 128;
	uint16* mtmxData = new uint16[width * height];
	memset(mtmxData, 0, sizeof(uint16) * width * height);
	fopen_s(&fp, "../data/MTXM", "rb");

	if (fp == nullptr)
	{
		return;
	}

	fread(mtmxData, sizeof(uint16), (size_t)(width * height), fp);

	fclose(fp);
#pragma endregion

	mMapImageWidth = width * 32;
	mMapImageHeight = height * 32;
	mMapImage = new uint32[mMapImageWidth * mMapImageHeight];

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			int32 group = mtmxData[y * width + x] >> 4;
			int32 index = mtmxData[y * width + x] & 0x0f;
			int32 megatile = cv5->Group[group].MegatileIndex[index];

			for (int32 subY = 0; subY < 4; subY++)
			{
				for (int32 subX = 0; subX < 4; subX++)
				{
					int minitileIndex = vx4->Data[megatile].VR4Index[subY * 4 + subX] >> 1; // 상위 15비트는 이미지
					bool flipped = vx4->Data[megatile].VR4Index[subY * 4 + subX] & 0x01; // 하위 1비트는 좌우 반전 여부

					gMiniTiles[y * 4 + subY][x * 4 + subX] = (uint8)vf4->Data[megatile].Flags[subY * 4 + subX];

					const VR4Data* vr4Data = vr4->Image + minitileIndex;

					const int32 offsetX = x * 32 + subX * 8;
					const int32 offsetY = y * 32 + subY * 8;

					for (int32 j = 0; j < 8; j++)
					{
						for (int32 i = 0; i < 8; i++)
						{
							int32 drawX = offsetX + (flipped ? 7 - i : i);
							int32 drawY = offsetY + j;
							const WPEData* wpeData = wpe->Data + vr4Data->Color[j * 8 + i];
							uint32 color = 0xff000000 | wpeData->R << 16 | wpeData->G << 8 | wpeData->B;
							mMapImage[drawY * mMapImageWidth + drawX] = color;
						}
					}

				}
			}
		}
	}

	delete[] mtmxData;
	delete wpe;
	delete vr4;
	delete vx4;
	delete vf4;
	delete cv5;
}

void ResourceManager::destroyMap()
{
	delete[] mMapImage;
	mMapImage = nullptr;

	mMapImageWidth = 0;
	mMapImageHeight = 0;
}
