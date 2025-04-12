#pragma once

#include "../Common/typedef.h"

struct GRPFrame
{
	uint8 X;
	uint8 Y;
	uint8 Width;
	uint8 Height;
	uint32 DataOffset;
};

static_assert(sizeof(GRPFrame) == 8, "GRPFrame size is invalid");

#pragma pack(push, 1)
struct GRPHeader
{
	uint16 FrameCount;
	int16 Width;
	int16 Height;
	GRPFrame Frames[1];
};
#pragma pack(pop)

static_assert(sizeof(GRPHeader) == 14, "GRPHeader size is invalid");

struct Chunk
{
	uint8 Type[4];
	uint32 Length;
	uint8 Data[1];
};

struct PCXHeader
{
	uint8 Manufacturer;
	uint8 Version;
	uint8 Encoding;
	uint8 BitsPerPixel;
	uint16 XMin;
	uint16 YMin;
	uint16 XMax;
	uint16 YMax;
	uint16 HDpi;
	uint16 VDpi;
	uint8 Palette[48];
	uint8 Reserved;
	uint8 PlaneCount;
	uint16 BytesPerLine;
	uint16 PaletteType;
	uint16 HScreenSize;
	uint16 VScreenSize;
	uint8 Filler[54];
};

static_assert(sizeof(PCXHeader) == 128, "PCXHeader size is invalid");

struct PCXImage
{
	PALETTEENTRY PaletteData[256];
	int32 Width;
	int32 Height;
	uint8 Data[1];
};
