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
	return (rect.Left <= point.X && point.X <= rect.Right && rect.Top <= point.Y && point.Y <= rect.Bottom);
}
