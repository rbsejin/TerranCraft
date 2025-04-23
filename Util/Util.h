#pragma once

#include "../Common/typedef.h"

bool IsCollisonRectVsRect(const Int32Rect& rect1, const Int32Rect& rect2);

bool IsCollisonRectVsPoint(const Int32Rect& rect, const Int32Vector2& point);
