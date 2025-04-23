#include "pch.h"

#include <memory>
#include <wingdi.h>

#include "../Common/typedef.h"
#include "PaletteManager.h"

uint32 PaletteManager::GetColor(const PALETTEENTRY* palette, uint8 index)
{
	return (palette[index].peRed << 16) | (palette[index].peGreen << 8) | palette[index].peBlue;
}

void PaletteManager::SetEntries(PALETTEENTRY * palette, int32 startIndex, int32 count, const PALETTEENTRY * colors)
{
	memcpy(palette + startIndex, colors, count * sizeof(PALETTEENTRY));
}

void PaletteManager::LoadPal(PALETTEENTRY* data, const char* filename)
{
	FILE* fp = nullptr;
	uint8* buffer = (uint8*)malloc(sizeof(PALETTEENTRY) * 256);
	fopen_s(&fp, filename, "rb");
	if (fp == nullptr)
	{
		return;
	}
	fread(buffer, sizeof(PALETTEENTRY), 256, fp);
	fclose(fp);

	for (int32 i = 0; i < 256; i++)
	{
		data[i].peRed = buffer[i * 3 + 0];
		data[i].peGreen = buffer[i * 3 + 1];
		data[i].peBlue = buffer[i * 3 + 2];
	}

	free(buffer);
}
