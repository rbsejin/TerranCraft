#include "pch.h"
#include "Util.h"

bool IsCollisonRectVsRect(const Int32Rect& rect1, const Int32Rect& rect2)
{
	if (rect1.Left > rect2.Right || rect1.Right < rect2.Left)
	{
		return false;
	}

	if (rect1.Top > rect2.Bottom || rect1.Bottom < rect2.Top)
	{
		return false;
	}

	return true;
}

bool IsCollisonRectVsPoint(const Int32Rect& rect, const Int32Vector2& point)
{
	Int32Rect tempRect = rect;

	if (rect.Left > rect.Right)
	{
		int32 temp = rect.Left;
		tempRect.Left = rect.Right;
		tempRect.Right = temp;
	}

	if (rect.Top > rect.Bottom)
	{
		int32 temp = rect.Top;
		tempRect.Top = rect.Bottom;
		tempRect.Bottom = temp;
	}

	return point.X >= tempRect.Left && point.X <= tempRect.Right && point.Y >= tempRect.Top && point.Y <= tempRect.Bottom;
}

int32 GetDistanceSquare(const Int32Vector2& point1, const Int32Vector2& point2)
{
	int32 distanceX = point1.X - point2.X;
	int32 distanceY = point1.Y - point2.Y;
	return distanceX * distanceX + distanceY * distanceY;
}

float GetDistanceSquare(const FloatVector2& point1, const FloatVector2& point2)
{
	float distanceX = point1.X - point2.X;
	float distanceY = point1.Y - point2.Y;
	return distanceX * distanceX + distanceY * distanceY;
}

float GetDistanceSquare(const Int32Vector2& point1, const FloatVector2& point2)
{
	float distanceX = (float)point1.X - point2.X;
	float distanceY = (float)point1.Y - point2.Y;
	return distanceX * distanceX + distanceY * distanceY;
}

float GetDistance(const Int32Vector2& point1, const Int32Vector2& point2)
{
	return sqrtf(GetDistanceSquare(point1, point2));
}

float GetDistance(const FloatVector2& point1, const FloatVector2& point2)
{
	return sqrtf(GetDistanceSquare(point1, point2));
}

float GetDistance(const Int32Vector2& point1, const FloatVector2& point2)
{
	return sqrtf(GetDistanceSquare(point1, point2));
}
