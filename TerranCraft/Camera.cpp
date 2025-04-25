#include "pch.h"
#include "Camera.h"
#include "PathFinder.h"

void Camera::MoveViewPort(int32 dx, int32 dy)
{
	mPosition.X += dx;
	if (mPosition.X < 0)
	{
		mPosition.X = 0;
	}
	if (mPosition.X > (int32)PathFinder::MAP_WIDTH * PathFinder::CELL_SIZE - (int32)mSize.X)
	{
		mPosition.X = (int32)PathFinder::MAP_WIDTH * PathFinder::CELL_SIZE - (int32)mSize.X;
	}
	mPosition.Y += dy;
	if (mPosition.Y < 0)
	{
		mPosition.Y = 0;
	}
	if (mPosition.Y > (int32)PathFinder::MAP_HEIGHT * PathFinder::CELL_SIZE - (int32)mSize.Y)
	{
		mPosition.Y = (int32)PathFinder::MAP_HEIGHT * PathFinder::CELL_SIZE - (int32)mSize.Y;
	}
}
