#include "pch.h"
#include "Flingy.h"
#include "ResourceManager.h"
#include "BWFile.h"
#include "Sprite.h"
#include "Image.h"
#include "../BWLib/ImageType.h"
#include "Game.h"

Flingy::~Flingy()
{
	Cleanup();
}

bool Flingy::Initialize(int32 hp, eFlingy flingyType)
{
	bool bResult = false;
	mFlingyType = flingyType;
	ResourceManager* resourceManager = gGame->GetResourceManager();
	const FlingyData* flingyData = resourceManager->GetFlingyData();
	uint32 flingyID = (uint32)flingyType;
	mFlingyTopSpeed = flingyData->Speeds[flingyID];
	mCurrentSpeed = (mFlingyTopSpeed == 1) ? 4.f : (mFlingyTopSpeed / 256.f);

	eSprite spriteNumber = (eSprite)flingyData->Sprites[flingyID];

	Thingy::Initialize(hp, spriteNumber);

	// TODO: Implement the following
	//Accelerations[flingyID];
	//HaltDistances[flingyID];
	//TurnRadiuses[flingyID];
	//MovementControls[flingyID];

	return bResult;
}

void Flingy::Cleanup()
{
	Thingy::Cleanup();
}

void Flingy::Update()
{
	Int32Vector2 spritePosition = { (int32)mPosition.X, (int32)mPosition.Y };
	Sprite* sprite = GetSprite();
	sprite->SetPosition(spritePosition);

	const std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		eImage imageID = image->GetImageID();
		image->SetDirection(mFacingDirection);
		image->UpdateGraphicData();
	}

	Image* selectionCircleImage = sprite->GetSelectionCircleImage();
	if (selectionCircleImage != nullptr)
	{
		selectionCircleImage->UpdateGraphicData();
	}
}
