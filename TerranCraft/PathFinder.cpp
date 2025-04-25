#include "pch.h"

#include <queue>
#include <unordered_map>

#include "PathFinder.h"

int32 PathFinder::FindPath(std::list<Int32Vector2>* outPath, Int32Vector2 start, Int32Vector2 end) const
{
#ifdef _DEBUG
	if (outPath == nullptr)
	{
		__debugbreak();
	}

	if (outPath->size() != 0)
	{
		__debugbreak();
	}

	if (mMiniTiles == nullptr)
	{
		__debugbreak();
	}
#endif

	if (start.X < 0 || start.X >= MAP_WIDTH || start.Y < 0 || start.Y >= MAP_HEIGHT)
	{
		return -1;
	}

	if (end.X < 0 || end.X >= MAP_WIDTH || end.Y < 0 || end.Y >= MAP_HEIGHT)
	{
		return -1;
	}

	std::queue<Int32Vector2> queue;
	std::unordered_map<int32, int32> prevs;

	queue.push(start);
	prevs[start.Y * MAP_WIDTH + start.X] = -1;

	while (!queue.empty())
	{
		Int32Vector2 next = queue.front();
		queue.pop();

		if (next.X == end.X && next.Y == end.Y)
		{
			int32 length = 0;
			int32 prev = next.Y * MAP_WIDTH + next.X;
			int32 n = prev;

			while (prevs[n] != -1)
			{
				Int32Vector2 temp = { n % MAP_WIDTH, n / MAP_WIDTH };
				outPath->push_front(temp);
				length++;
				n = prevs[n];
			}

			return length;
		}

		Int32Vector2 arr[8] =
		{
			{ -1, 0 },
			{ 1, 0 },
			{ 0, -1 },
			{ 0, 1 },
			{ -1, -1 },
			{ 1, -1 },
			{ -1, 1 },
			{ 1, 1 }
		};

		for (int32 k = 0; k < 8; k++)
		{
			int32 i = arr[k].X;
			int32 j = arr[k].Y;

			Int32Vector2 neighbor = { next.X + i, next.Y + j };

			if (neighbor.X < 0 || neighbor.X >= MAP_WIDTH || neighbor.Y < 0 || neighbor.Y >= MAP_HEIGHT)
			{
				continue;
			}

			if (mMiniTiles[neighbor.Y][neighbor.X] == 1)
			{
				continue;
			}

			if (prevs.find(neighbor.Y * MAP_WIDTH + neighbor.X) != prevs.end())
			{
				continue;
			}

			queue.push(neighbor);
			prevs[neighbor.Y * MAP_WIDTH + neighbor.X] = next.Y * MAP_WIDTH + next.X;
		}
	}

	return -1;
}

int32 PathFinder::FindPathWithUnitSize(std::list<Int32Vector2>* outPath, Int32Vector2 start, Int32Vector2 end, Int32Rect countourBounds) const
{
#ifdef _DEBUG
	if (outPath == nullptr)
	{
		__debugbreak();
	}

	if (outPath->size() != 0)
	{
		__debugbreak();
	}

	if (mMiniTiles == nullptr)
	{
		__debugbreak();
	}
#endif

	if (start.X < 0 || start.X >= MAP_WIDTH || start.Y < 0 || start.Y >= MAP_HEIGHT)
	{
		return -1;
	}

	if (end.X < 0 || end.X >= MAP_WIDTH || end.Y < 0 || end.Y >= MAP_HEIGHT)
	{
		return -1;
	}

	std::queue<Int32Vector2> queue;
	std::unordered_map<int32, int32> prevs;

	queue.push(start);
	prevs[start.Y * MAP_WIDTH + start.X] = -1;

	while (!queue.empty())
	{
		Int32Vector2 next = queue.front();
		queue.pop();

		if (next.X == end.X && next.Y == end.Y)
		{
			int32 length = 0;
			int32 prev = next.Y * MAP_WIDTH + next.X;
			int32 n = prev;

			while (prevs[n] != -1)
			{
				Int32Vector2 temp = { n % MAP_WIDTH, n / MAP_WIDTH };
				outPath->push_front(temp);
				length++;
				n = prevs[n];
			}

			return length;
		}

		Int32Vector2 arr[8] =
		{
			{ -1, 0 },
			{ 1, 0 },
			{ 0, -1 },
			{ 0, 1 },
			{ -1, -1 },
			{ 1, -1 },
			{ -1, 1 },
			{ 1, 1 }
		};

		for (int32 k = 0; k < 8; k++)
		{
			int32 i = arr[k].X;
			int32 j = arr[k].Y;

			Int32Vector2 neighbor = { next.X + i, next.Y + j };

			if (neighbor.X < 0 || neighbor.X >= MAP_WIDTH || neighbor.Y < 0 || neighbor.Y >= MAP_HEIGHT)
			{
				continue;
			}

			if ((mMiniTiles[neighbor.Y][neighbor.X] & 0x01) == 0)
			{
				continue;
			}

			if (prevs.find(neighbor.Y * MAP_WIDTH + neighbor.X) != prevs.end())
			{
				continue;
			}

			if (canMoveTo(neighbor, countourBounds))
			{
				queue.push(neighbor);
				prevs[neighbor.Y * MAP_WIDTH + neighbor.X] = next.Y * MAP_WIDTH + next.X;
			}
		}
	}

	return -1;
}

bool PathFinder::canMoveTo(Int32Vector2 pos, Int32Rect unitSize) const
{
	Int32Vector2 unitPos = { pos.X * CELL_SIZE + CELL_SIZE / 2, pos.Y * CELL_SIZE + CELL_SIZE / 2 };
	Int32Rect unitBound = { unitPos.X - unitSize.Left, unitPos.Y - unitSize.Top, unitPos.X + unitSize.Right, unitPos.Y + unitSize.Bottom };
	Int32Rect unitCellBound = { unitBound.Left / CELL_SIZE, unitBound.Top / CELL_SIZE, unitBound.Right / CELL_SIZE, unitBound.Bottom / CELL_SIZE };

	for (int y = unitCellBound.Top; y <= unitCellBound.Bottom; y++)
	{
		for (int x = unitCellBound.Left; x <= unitCellBound.Right; x++)
		{
			if (mMiniTiles[y * MAP_WIDTH + x] == 0)
			{
				return false;
			}
		}
	}

	return true;
}
