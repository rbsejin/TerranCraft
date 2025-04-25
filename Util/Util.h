#pragma once

#include "../Common/typedef.h"

bool IsCollisonRectVsRect(const Int32Rect& rect1, const Int32Rect& rect2);

bool IsCollisonRectVsPoint(const Int32Rect& rect, const Int32Vector2& point);

int32 GetDistanceSquare(const Int32Vector2& point1, const Int32Vector2& point2);

float GetDistanceSquare(const FloatVector2& point1, const FloatVector2& point2);

float GetDistanceSquare(const Int32Vector2& point1, const FloatVector2& point2);

float GetDistance(const Int32Vector2& point1, const Int32Vector2& point2);

float GetDistance(const FloatVector2& point1, const FloatVector2& point2);

float GetDistance(const Int32Vector2& point1, const FloatVector2& point2);
