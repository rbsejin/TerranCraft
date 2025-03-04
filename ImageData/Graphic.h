#pragma once

#include "../Common/typedef.h"

struct GRPFrame
{
	int8 X;
	int8 Y;
	int8 Width;
	int8 Height;
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

union RGBColor
{
	struct
	{
		uint8 B;
		uint8 G;
		uint8 R;
		uint8 Reserved;
	};
	struct
	{
		// 0xXXRRGGBB
		uint32 Value;
	};
};
