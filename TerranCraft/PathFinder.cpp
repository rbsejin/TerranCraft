#include "pch.h"

#include <queue>
#include <unordered_map>
#include "PathFinder.h"

uint8 gMap[MAP_HEIGHT][MAP_WIDTH] = { 0, };

int32 FindPath(std::list<IntVector2>* outPath, const uint8* map, IntVector2 start, IntVector2 end)
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

	if (map == nullptr)
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

	std::queue<IntVector2> queue;
	std::unordered_map<int32, int32> prevs;

	queue.push(start);
	prevs[start.Y * MAP_WIDTH + start.X] = -1;

	while (!queue.empty())
	{
		IntVector2 next = queue.front();
		queue.pop();

		if (next.X == end.X && next.Y == end.Y)
		{
			int32 distance = 0;
			int32 prev = next.Y * MAP_WIDTH + next.X;
			int32 n = prev;

			while (prevs[n] != -1)
			{
				IntVector2 temp = { n % MAP_WIDTH, n / MAP_WIDTH };
				outPath->push_front(temp);
				distance++;
				n = prevs[n];
			}

			return distance;
		}

		IntVector2 arr[8] =
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

			IntVector2 neighbor = { next.X + i, next.Y + j };

			if (neighbor.X < 0 || neighbor.X >= MAP_WIDTH || neighbor.Y < 0 || neighbor.Y >= MAP_HEIGHT)
			{
				continue;
			}

			if (map[neighbor.Y * MAP_WIDTH + neighbor.X] == 1)
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

int32 FindPathWithUnitSize(std::list<IntVector2>* outPath, const uint8* map, IntVector2 start, IntVector2 end, IntRect unitSize)
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

	if (map == nullptr)
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

	std::queue<IntVector2> queue;
	std::unordered_map<int32, int32> prevs;

	queue.push(start);
	prevs[start.Y * MAP_WIDTH + start.X] = -1;

	while (!queue.empty())
	{
		IntVector2 next = queue.front();
		queue.pop();

		if (next.X == end.X && next.Y == end.Y)
		{
			int32 distance = 0;
			int32 prev = next.Y * MAP_WIDTH + next.X;
			int32 n = prev;

			while (prevs[n] != -1)
			{
				IntVector2 temp = { n % MAP_WIDTH, n / MAP_WIDTH };
				outPath->push_front(temp);
				distance++;
				n = prevs[n];
			}

			return distance;
		}

		IntVector2 arr[8] =
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

			IntVector2 neighbor = { next.X + i, next.Y + j };

			if (neighbor.X < 0 || neighbor.X >= MAP_WIDTH || neighbor.Y < 0 || neighbor.Y >= MAP_HEIGHT)
			{
				continue;
			}

			if (map[neighbor.Y * MAP_WIDTH + neighbor.X] == 1)
			{
				continue;
			}

			if (prevs.find(neighbor.Y * MAP_WIDTH + neighbor.X) != prevs.end())
			{
				continue;
			}

			if (CanMoveTo(map, neighbor, unitSize))
			{
				queue.push(neighbor);
				prevs[neighbor.Y * MAP_WIDTH + neighbor.X] = next.Y * MAP_WIDTH + next.X;
			}
		}
	}

	return -1;
}

bool CanMoveTo(const uint8* map, IntVector2 pos, IntRect unitSize)
{
	IntVector2 unitPos = { pos.X * CELL_SIZE + CELL_SIZE / 2, pos.Y * CELL_SIZE + CELL_SIZE / 2 };
	IntRect unitBound = { unitPos.X - unitSize.Left, unitPos.Y - unitSize.Top, unitPos.X + unitSize.Right, unitPos.Y + unitSize.Bottom };
	IntRect uintCellBound = { unitBound.Left / CELL_SIZE, unitBound.Top / CELL_SIZE, unitBound.Right / CELL_SIZE, unitBound.Bottom / CELL_SIZE };

	for (int y = uintCellBound.Top; y <= uintCellBound.Bottom; y++)
	{
		for (int x = uintCellBound.Left; x <= uintCellBound.Right; x++)
		{
			if (map[y * MAP_WIDTH + x] != 0)
			{
				return false;
			}
		}
	}

	return true;
}
