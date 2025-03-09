#include "pch.h"
#include "Util.h"

bool IsCollisonRectVsRect(const IntRect& rect1, const IntRect& rect2)
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

bool IsCollisonRectVsPoint(const IntRect& rect, const IntVector2& point)
{
	IntRect tempRect = rect;

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
