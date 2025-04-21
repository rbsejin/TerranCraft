#pragma once

#include "../Common/typedef.h"

class Camera final
{
public:
	void MoveViewPort(int32 dx, int32 dy);

	IntVector2 GetPosition() const { return mPosition; }
	void SetPosition(IntVector2 position) { mPosition = position; }
	IntVector2 GetSize() const { return mSize; }
	void SetSize(IntVector2 size) { mSize = size; }

private:
	IntVector2 mPosition = { 0, 0 };
	IntVector2 mSize = { 0, };
};
