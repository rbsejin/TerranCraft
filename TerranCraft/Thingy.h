#pragma once

#include "../Common/typedef.h"
#include "../BWLib/SpriteType.h"

class Sprite;

class Thingy
{
public:
	Thingy() = default;
	virtual ~Thingy();
	bool Initialize(int32 hp, eSprite spriteNumber);
	void Cleanup();
	int32 GetHP() const { return mHP; }
	void SetHP(int32 hp) { mHP = hp; }
	Sprite* GetSprite() const { return mSprite; }
	void SetSprite(Sprite* sprite) { mSprite = sprite; }
private:
	int32 mHP = 0;
	Sprite* mSprite = nullptr;
};
