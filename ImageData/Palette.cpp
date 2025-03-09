#include "pch.h"

#include <memory>

#include "../Common/typedef.h"
#include "Palette.h"

#pragma pack(push, 1)
struct PaletteRGB
{
	uint8 Red;
	uint8 Green;
	uint8 Blue;
};
#pragma pack(pop)

static_assert(sizeof(PaletteRGB) == 3, "PaletteRGB size is invalid");

Palette::Palette()
	: mData(nullptr)
{
}

Palette::~Palette()
{
	free(mData);
}

bool Palette::Load(const char* filename)
{
	bool bResult = false;

	FILE* fp = nullptr;
	fopen_s(&fp, filename, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	mData = (PaletteRGB*)malloc(fileSize);

#ifdef _DEBUG
	if (fileSize % sizeof(Palette) != 0)
	{
		__debugbreak();
	}
#endif
	fread((void*)mData, 1, fileSize, fp);
	fclose(fp);

	bResult = true;

LB_RETURN:
	return bResult;
}

uint32 Palette::GetColor(int index) const
{
	PaletteRGB rgb = mData[index];
	return (uint32)(((rgb.Blue) | ((uint32)((rgb.Green)) << 8)) | (((uint32)(rgb.Red)) << 16));
}
