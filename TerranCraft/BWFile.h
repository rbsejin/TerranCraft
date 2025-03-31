#pragma once

#include "../Common/typedef.h"

#pragma pack(push, 1)
struct ImageData
{
	enum { IMAGE_COUNT = 999 };
	uint32 GRPFiles[IMAGE_COUNT];
	uint8 GfxTurns[IMAGE_COUNT];
	uint8 Clickables[IMAGE_COUNT];
	uint8 UseFullIscripts[IMAGE_COUNT];
	uint8 DrawIfCloakeds[IMAGE_COUNT];
	uint8 DrawFunctions[IMAGE_COUNT];
	uint8 Remappings[IMAGE_COUNT];
	uint32 IscriptIDs[IMAGE_COUNT];
	uint32 ShieldOverlays[IMAGE_COUNT];
	uint32 AttackOverlays[IMAGE_COUNT];
	uint32 DamageOverlays[IMAGE_COUNT];
	uint32 SpecialOverlays[IMAGE_COUNT];
	uint32 LandingDustOverlays[IMAGE_COUNT];
	uint32 LiftOffDustOverlays[IMAGE_COUNT];
};
#pragma pack(pop)

static_assert(sizeof(ImageData) == 37962, "ImageData size is invalid");

#pragma pack(push, 1)
struct SpriteData
{
	enum { SPRITE_COUNT = 517 };
	enum { HEALTH_BAR_COUNT = 387 };
	uint16 ImageFiles[SPRITE_COUNT];
	uint8 HealthBars[HEALTH_BAR_COUNT];
	uint8 Unknown[SPRITE_COUNT];
	uint8 IsVisibles[SPRITE_COUNT];
	uint8 SelectionCircleImages[HEALTH_BAR_COUNT];
	uint8 SelectionCircleOffsets[HEALTH_BAR_COUNT];
};
#pragma pack(pop)

static_assert(sizeof(SpriteData) == 3229, "SpriteData size is invalid");

#pragma pack(push, 1)
struct FlingyData
{
	enum { FLINGY_COUNT = 209 };
	uint16 Sprites[FLINGY_COUNT];
	uint32 Speeds[FLINGY_COUNT];
	uint16 Accelerations[FLINGY_COUNT];
	uint32 HaltDistances[FLINGY_COUNT];
	uint8 TurnRadiuses[FLINGY_COUNT];
	uint8 Unuseds[FLINGY_COUNT];
	uint8 MovementControls[FLINGY_COUNT];
};
#pragma pack(pop)

//static_assert(sizeof(FlingyData) == 16, "FlingyData size is invalid");

#pragma pack(push, 1)
struct UnitData
{
	enum { UNIT_COUNT = 228 };
	enum { INFESTATION_COUNT = 96 };
	uint8 Graphics[UNIT_COUNT];
	// 0xe4
	uint16 Subunit1[UNIT_COUNT];
	// 0x2ac
	uint16 Subunit2[UNIT_COUNT];
	// 0x474
	uint16 Infestation[INFESTATION_COUNT];
	// 0x53f
	uint32 ConstructionAnimation[UNIT_COUNT];
	// 0x8c4
	uint8 UnitDirection[UNIT_COUNT];
	uint8 ShieldEnable[UNIT_COUNT];
	uint16 ShieldAmount[UNIT_COUNT];
	// 0xc53
	uint32 HitPoints[UNIT_COUNT]; // Hit Points of unit, note that the displayed value in broodwar is ceil(hitPoints/256)
								  // Set to 1 to indicate existence for non-units. Official.
	// 0xfe4
	uint8 ElevationLevel[UNIT_COUNT];
	uint8 Unknown[UNIT_COUNT];
	uint8 Sublabel[UNIT_COUNT];
	// 0x1290
	uint8 CompAIIdle[UNIT_COUNT];
	uint8 HumanAIIdle[UNIT_COUNT];
	uint8 ReturntoIdle[UNIT_COUNT];
	uint8 AttackUnit[UNIT_COUNT];
	uint8 AttackMove[UNIT_COUNT];
	// 0x1704
	uint8 GroundWeapon[UNIT_COUNT];
	uint8 MaxGroundHits[UNIT_COUNT];
	uint8 AirWeapon[UNIT_COUNT];
	uint8 MaxAirHits[UNIT_COUNT];
	// 0x1a94
	uint8 AIInternal[UNIT_COUNT]; // Ingore Strategic Suicide missions/Don't become a guard => Eanble
	// 0x1b78
	uint32 SpecialAbilityFlags[UNIT_COUNT];
	uint8 TargetAcquisitionRange[UNIT_COUNT];
	uint8 SightRange[UNIT_COUNT];
	uint8 ArmorUpgrade[UNIT_COUNT];
	uint8 UnitSize[UNIT_COUNT];
	uint8 Armor[UNIT_COUNT];
	uint8 RightClickAction[UNIT_COUNT];
	// 0x2460
	uint16 ReadySound[106];
	// 0x2534
	uint16 WhatSoundStart[UNIT_COUNT];
	uint16 WhatSoundEnd[UNIT_COUNT];
	// 0x28c4
	uint16 PissSoundStart[106];
	uint16 PissSoundEnd[106];
	uint16 YesSoundStart[106];
	uint16 YesSoundEnd[106];
	// 0x2c14
	Int16Vector2 StarEditPlacementBox[UNIT_COUNT];
	// 0x2fa4
	Int16Vector2 AddonPosition[96]; // 106~201: 96 * 4
	// 0x3124
	Int16Rect UnitDimensions[UNIT_COUNT];
	uint16 Portrait[UNIT_COUNT];
	uint16 MineralCost[UNIT_COUNT];
	uint16 VespeneCost[UNIT_COUNT];
	// 0x3d9c
	uint16 BuildTime[UNIT_COUNT];
	// 0x3f64
	uint16 Unknown1[UNIT_COUNT];
	// 0x412c
	uint8 StarEditGroupFlags[UNIT_COUNT];
	// 0x4210
	uint8 SupplyProvided[UNIT_COUNT];
	uint8 SupplyRequired[UNIT_COUNT];
	uint8 SpaceRequired[UNIT_COUNT];
	uint8 SpaceProvided[UNIT_COUNT];
	// 0x45a0
	uint16 BuildScore[UNIT_COUNT];
	uint16 DestroyScore[UNIT_COUNT];
	// 0x4930
	uint16 UnitMapString[UNIT_COUNT];
	uint8 BroodwarUnitFlag[UNIT_COUNT];
	uint16 StarEditAvailabilityFlags[UNIT_COUNT];
};
#pragma pack(pop)

//static_assert(sizeof(UnitData) == 19876, "UnitData size is invalid");