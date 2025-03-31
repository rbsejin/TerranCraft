#pragma once

#include <cstdint>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct IntRect
{
	int32 Left;
	int32 Top;
	int32 Right;
	int32 Bottom;
};

static_assert(sizeof(IntRect) == 16, "Rect size is invalid");

struct Int16Rect
{
	int16 Left;
	int16 Top;
	int16 Right;
	int16 Bottom;
};

struct IntVector2
{
	int32 X;
	int32 Y;
};

struct Int16Vector2
{
	int16 X;
	int16 Y;
};;

struct FloatVector2
{
	float X;
	float Y;
};

static_assert(sizeof(IntVector2) == 8, "IntVector2 size is invalid");
