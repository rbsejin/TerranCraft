#pragma once

#include <list>
#include "../Common/typedef.h"

enum { MAP_WIDTH = 128 * 4 };
enum { MAP_HEIGHT = 128 * 4 };
enum { CELL_SIZE = 8 };

extern uint8 gMiniTiles[MAP_HEIGHT][MAP_WIDTH];

int32 FindPath(std::list<IntVector2>* outPath, const uint8* map, IntVector2 start, IntVector2 end);

int32 FindPathWithUnitSize(std::list<IntVector2>* outPath, const uint8* map, IntVector2 start, IntVector2 end, IntRect countourBounds);

bool CanMoveTo(const uint8* map, IntVector2 pos, IntRect unitSize);
