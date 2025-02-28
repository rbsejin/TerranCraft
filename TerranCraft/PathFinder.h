#pragma once

#include <list>
#include "../Common/typedef.h"

enum { MAP_WIDTH = 64 };
enum { MAP_HEIGHT = 64 };
enum { CELL_SIZE = 8 };

extern uint8 gMap[MAP_HEIGHT][MAP_WIDTH];

int32 FindPath(std::list<IntVector2>* outPath, const uint8* map, IntVector2 start, IntVector2 end);
