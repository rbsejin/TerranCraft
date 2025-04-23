#pragma once

#include <list>
#include "../Common/typedef.h"

enum { MAP_WIDTH = 128 * 4 };
enum { MAP_HEIGHT = 128 * 4 };
enum { CELL_SIZE = 8 };

extern uint8 gMiniTiles[MAP_HEIGHT][MAP_WIDTH];

int32 FindPath(std::list<Int32Vector2>* outPath, const uint8* map, Int32Vector2 start, Int32Vector2 end);

int32 FindPathWithUnitSize(std::list<Int32Vector2>* outPath, const uint8* map, Int32Vector2 start, Int32Vector2 end, Int32Rect countourBounds);

bool CanMoveTo(const uint8* map, Int32Vector2 pos, Int32Rect unitSize);
