#include "pch.h"
#include "Thingy.h"
#include "Sprite.h"
#include "Game.h"

Thingy::~Thingy()
{
	Cleanup();
}

bool Thingy::Initialize(int32 hp, BW::SpriteNumber spriteNumber)
{
	bool bResult = false;

	mHP = hp;
	mSprite = new Sprite();
	mSprite->Initalize(spriteNumber);

	return bResult;
}

void Thingy::Cleanup()
{
	delete mSprite;
	mSprite = nullptr;
}
