#include "pch.h"

#include <memory>
#include <wingdi.h>

#include "../Common/typedef.h"
#include "Palette.h"

#define RGBRESERVE {0x23, 0x23, 0xff}
PALETTEENTRY Palette::sData[256] =
{
  {0,0,0}      , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  {24,36,44}   , {72,36,20}   , {92,44,20}   , {112,48,20}  , {104,60,36}  , {124,64,24}  , {120,76,44}  , {168,8,8}    ,
  {140,84,48}  , {132,96,68}  , {160,84,28}  , {196,76,24}  , {188,104,36} , {180,112,60} , {208,100,32} , {220,148,52} ,
  {224,148,84} , {236,196,84} , {52,68,40}   , {64,108,60}  , {72,108,80}  , {76,128,80}  , {80,140,92}  , {92,160,120} ,
  {0,0,24}     , {0,16,52}    , {0,8,80}     , {36,52,72}   , {48,64,84}   , {20,52,124}  , {52,76,108}  , {64,88,116}  ,
  {72,104,140} , {0,112,156}  , {88,128,164} , {64,104,212} , {24,172,184} , {36,36,252}  , {100,148,188}, {112,168,204},
  {140,192,216}, {148,220,244}, {172,220,232}, {172,252,252}, {204,248,248}, {252,252,0}  , {244,228,144}, {252,252,192},
  {12,12,12}   , {24,20,16}   , {28,28,32}   , {40,40,48}   , {56,48,36}   , {56,60,68}   , {76,64,48}   , {76,76,76}   ,
  {92,80,64}   , {88,88,88}   , {104,104,104}, {120,132,108}, {104,148,108}, {116,164,124}, {152,148,140}, {144,184,148},
  {152,196,168}, {176,176,176}, {172,204,176}, {196,192,188}, {204,224,208}, {240,240,240}, {28,16,8}    , {40,24,12}   ,
  {52,16,8}    , {52,32,12}   , {56,16,32}   , {52,40,32}   , {68,52,8}    , {72,48,24}   , {96,0,0}     , {84,40,32}   ,
  {80,64,20}   , {92,84,20}   , {132,4,4}    , {104,76,52}  , {124,56,48}  , {112,100,32} , {124,80,80}  , {164,52,28}  ,
  {148,108,0}  , {152,92,64}  , {140,128,52} , {152,116,84} , {184,84,68}  , {176,144,24} , {176,116,92} , {244,4,4}    ,
  {200,120,84} , {252,104,84} , {224,164,132}, {252,148,104}, {252,204,44} , {16,252,24}  , {12,0,32}    , {28,28,44}   ,
  {36,36,76}   , {40,44,104}  , {44,48,132}  , {32,24,184}  , {52,60,172}  , {104,104,148}, {100,144,252}, {124,172,252},
  {0,228,252}  , {156,144,64} , {168,148,84} , {188,164,92} , {204,184,96} , {232,216,128}, {236,196,176}, {252,252,56} ,
  {252,252,124}, {252,252,164}, {8,8,8}      , {16,16,16}   , {24,24,24}   , {40,40,40}   , {52,52,52}   , {76,60,56}   ,
  {68,68,68}   , {72,72,88}   , {88,88,104}  , {116,104,56} , {120,100,92} , {96,96,124}  , {132,116,116}, {132,132,156},
  {172,140,124}, {172,152,148}, {144,144,184}, {184,184,232}, {248,140,20} , {16,84,60}   , {32,144,112} , {44,180,148} ,
  {4,32,100}   , {72,28,80}   , {8,52,152}   , {104,48,120} , {136,64,156} , {12,72,204}  , {188,184,52} , {220,220,60} ,
  {16,0,0}     , {36,0,0}     , {52,0,0}     , {72,0,0}     , {96,24,4}    , {140,40,8}   , {200,24,24}  , {224,44,44}  ,
  {232,32,32}  , {232,80,20}  , {252,32,32}  , {232,120,36} , {248,172,60} , {0,20,0}     , {0,40,0}     , {0,68,0}     ,
  {0,100,0}    , {8,128,8}    , {36,152,36}  , {60,156,60}  , {88,176,88}  , {104,184,104}, {128,196,128}, {148,212,148},
  {12,20,36}   , {36,60,100}  , {48,80,132}  , {56,92,148}  , {72,116,180} , {84,132,196} , {96,148,212} , {120,180,236},
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   ,
  RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , RGBRESERVE   , {255,255,255}
};
#undef RGBRESERVE

PALETTEENTRY Palette::sOfireData[256] = { 0, };
PALETTEENTRY Palette::sGfireData[256] = { 0, };
PALETTEENTRY Palette::sBfireData[256] = { 0, };
PALETTEENTRY Palette::sBexplData[256] = { 0, };

PALETTEENTRY Palette::sIconData[16] =
{
	{ 0xFC, 0xFC, 0x38 }, { 0xE7, 0xE9, 0x2F }, { 0xFD, 0xD1, 0x39 }, { 0xFC, 0xCC, 0x2C },
	{ 0xFC, 0xCC, 0x2C }, { 0xBC, 0xB8, 0x34 }, { 0xB0, 0x90, 0x18 }, { 0x8C, 0x80, 0x34 },
	{ 0x70, 0x64, 0x20 }, { 0x5C, 0x54, 0x14 }, { 0x50, 0x40, 0x14 }, { 0x34, 0x20, 0x00 },
	{ 0x28, 0x18, 0x00 }, { 0x1C, 0x10, 0x08 }, { 0x19, 0x00, 0x19 }, { 0x2C, 0x24, 0x18 }
};

PALETTEENTRY Palette::sIconWireData[16] =
{
	{ 0xFF, 0xFF, 0xFF }, { 0xF0, 0xF0, 0xF0 }, { 0xCC, 0xE0, 0xD0 }, { 0xC4, 0xC0, 0xBC },
	{ 0xB0, 0xB0, 0xB0 }, { 0xAC, 0x98, 0x94 },	{ 0x98, 0x94, 0x8C }, { 0x84, 0x74, 0x74 },
	{ 0x68, 0x68, 0x68 }, { 0x58, 0x58, 0x58 },	{ 0x44, 0x44, 0x44 }, { 0x34, 0x34, 0x34 },
	{ 0x28, 0x28, 0x28 }, { 0x10, 0x10, 0x10 },	{ 0x08, 0x08, 0x08 }, { 0x2C, 0x24, 0x18 }
};

PALETTEENTRY Palette::sTUnitPaletteEntries[128] = { 0, };

PALETTEENTRY Palette::sTSelectPaletteEntries[24] = { 0, };

PALETTEENTRY Palette::sTWirePaletteEntries[24] = { 0, };

uint32 Palette::GetColor(const PALETTEENTRY* palette, uint8 index)
{
	return (palette[index].peRed << 16) | (palette[index].peGreen << 8) | palette[index].peBlue;
}

void Palette::SetEntries(PALETTEENTRY* palette, int32 startIndex, int32 count, const PALETTEENTRY* colors)
{
	memcpy(palette + startIndex, colors, count * sizeof(PALETTEENTRY));
}

void Palette::LoadPal(PALETTEENTRY* data, const char* filename)
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
