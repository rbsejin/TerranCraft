#pragma once

#include <list>
#include "../Common/typedef.h"

class PathFinder
{
public:
	int32 FindPath(std::list<Int32Vector2>* outPath, Int32Vector2 start, Int32Vector2 end) const;
	int32 FindPathWithUnitSize(std::list<Int32Vector2>* outPath, Int32Vector2 start, Int32Vector2 end, Int32Rect countourBounds) const;
	const uint8* GetMiniTiles() const { return (const uint8*)mMiniTiles; }
	uint8 GetMiniTile(int32 x, int32 y) const { return mMiniTiles[y][x]; }
	void SetMiniTile(int32 x, int32 y, uint8 value) { mMiniTiles[y][x] = value; }

private:
	bool canMoveTo(Int32Vector2 pos, Int32Rect unitSize) const;

public:
	enum { MAP_WIDTH = 128 * 4 };
	enum { MAP_HEIGHT = 128 * 4 };
	enum { CELL_SIZE = 8 };
	enum { GRID_SIZE = 32 };

private:
	uint8 mMiniTiles[MAP_HEIGHT][MAP_WIDTH] = { 0, };
};