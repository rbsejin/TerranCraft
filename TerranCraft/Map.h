#pragma once

#include "../Common/typedef.h"

struct CV5Data
{
	uint8 Somthing[20];
	uint16 MegatileIndex[16];
};

struct CV5
{
	CV5Data Group[4096];
};

struct VF4Data
{
	uint16 Flags[16];
};

struct VF4
{
	VF4Data Data[65536];
};

struct VX4Data
{
	uint16 VR4Index[16];
};

struct VX4
{
	VX4Data Data[65536];
};

struct VR4Data
{
	uint8 Color[64];
};

struct VR4
{
	VR4Data Image[32768];
};

struct WPEData
{
	uint8 R;
	uint8 G;
	uint8 B;
	uint8 Padding;
};

struct WPE
{
	WPEData Data[256];
};
