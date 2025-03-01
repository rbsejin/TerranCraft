#pragma once

#include "../Common/typedef.h"

bool IsCollisonRectVsRect(const IntRect& rect1, const IntRect& rect2);

bool IsCollisonRectVsPoint(const IntRect& rect, const IntVector2& point);
