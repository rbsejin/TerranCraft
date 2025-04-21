#include "pch.h"
#include "Flingy.h"
#include "Arrangement.h"
#include "BWFile.h"
#include "Sprite.h"
#include "Image.h"
#include "../BWLib/ImageNumber.h"

Flingy::~Flingy()
{
	Cleanup();
}

bool Flingy::Initialize(int32 hp, BW::FlingyType flingyType)
{
	bool bResult = false;
	mFlingyType = flingyType;
	const FlingyData* flingyData = Arrangement::Instance.GetFlingyData();
	uint32 flingyID = (uint32)flingyType;
	mFlingyTopSpeed = flingyData->Speeds[flingyID];
	mCurrentSpeed = (mFlingyTopSpeed == 1) ? 4.f : (mFlingyTopSpeed / 256.f);

	BW::SpriteNumber spriteNumber = (BW::SpriteNumber)flingyData->Sprites[flingyID];

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
	IntVector2 spritePosition = { (int32)mPosition.X, (int32)mPosition.Y };
	Sprite* sprite = GetSprite();
	sprite->SetPosition(spritePosition);

	const std::list<Image*>* images = sprite->GetImages();
	for (Image* image : *images)
	{
		BW::ImageNumber imageID = image->GetImageID();
		image->SetDirection(mFacingDirection);
		image->UpdateGraphicData();
	}

	Image* selectionCircleImage = sprite->GetSelectionCircleImage();
	if (selectionCircleImage != nullptr)
	{
		selectionCircleImage->UpdateGraphicData();
	}
}
