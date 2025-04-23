#pragma once

#include "../Common/typedef.h"

class Camera final
{
public:
	void MoveViewPort(int32 dx, int32 dy);

	Int32Vector2 GetPosition() const { return mPosition; }
	void SetPosition(Int32Vector2 position) { mPosition = position; }
	Int32Vector2 GetSize() const { return mSize; }
	void SetSize(Int32Vector2 size) { mSize = size; }

private:
	Int32Vector2 mPosition = { 0, 0 };
	Int32Vector2 mSize = { 0, };
};
