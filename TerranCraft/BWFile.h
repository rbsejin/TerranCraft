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
	uint8 SelectionCircleOffsets[HEALTH_BAR_COUNT]; // Vert. Position
};
#pragma pack(pop)

static_assert(sizeof(SpriteData) == 3229, "SpriteData size is invalid");

#pragma pack(push, 1)
struct FlingyData
{
	enum { FLINGY_COUNT = 209 };
	uint16 Sprites[FLINGY_COUNT];
	int32 Speeds[FLINGY_COUNT];
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
	uint8 Graphics[UNIT_COUNT]; // Flingy
	// 0xe4
	uint16 Subunit1s[UNIT_COUNT];
	// 0x2ac
	uint16 Subunit2s[UNIT_COUNT];
	// 0x474
	uint16 Infestations[INFESTATION_COUNT];
	// 0x53f
	uint32 ConstructionAnimations[UNIT_COUNT];
	// 0x8c4
	uint8 UnitDirections[UNIT_COUNT];
	uint8 ShieldEnables[UNIT_COUNT];
	uint16 ShieldAmounts[UNIT_COUNT];
	// 0xc53
	uint32 HitPoints[UNIT_COUNT]; // Hit Points of unit, note that the displayed value in broodwar is ceil(hitPoints/256)
	// Set to 1 to indicate existence for non-units. Official.
	// 0xfe4
	uint8 ElevationLevels[UNIT_COUNT];
	uint8 Unknowns[UNIT_COUNT];
	uint8 Sublabels[UNIT_COUNT];
	// 0x1290
	uint8 CompAIIdles[UNIT_COUNT];
	uint8 HumanAIIdles[UNIT_COUNT];
	uint8 ReturntoIdles[UNIT_COUNT];
	uint8 AttackUnits[UNIT_COUNT];
	uint8 AttackMoves[UNIT_COUNT];
	// 0x1704
	uint8 GroundWeapons[UNIT_COUNT];
	uint8 MaxGroundHits[UNIT_COUNT];
	uint8 AirWeapons[UNIT_COUNT];
	uint8 MaxAirHits[UNIT_COUNT];
	// 0x1a94
	uint8 AIInternals[UNIT_COUNT]; // Ingore Strategic Suicide missions/Don't become a guard => Eanble
	// 0x1b78
	uint32 SpecialAbilityFlags[UNIT_COUNT];
	uint8 TargetAcquisitionRanges[UNIT_COUNT];
	uint8 SightRanges[UNIT_COUNT];
	uint8 ArmorUpgrades[UNIT_COUNT];
	uint8 UnitSizes[UNIT_COUNT];
	uint8 Armors[UNIT_COUNT];
	uint8 RightClickActions[UNIT_COUNT];
	// 0x2460
	uint16 ReadySounds[106];
	// 0x2534
	uint16 WhatSoundStarts[UNIT_COUNT];
	uint16 WhatSoundEnds[UNIT_COUNT];
	// 0x28c4
	uint16 PissSoundStarts[106];
	uint16 PissSoundEnds[106];
	uint16 YesSoundStarts[106];
	uint16 YesSoundEnds[106];
	// 0x2c14
	Int16Vector2 StarEditPlacementBoxes[UNIT_COUNT];
	// 0x2fa4
	Int16Vector2 AddonPositions[96]; // 106~201: 96 * 4
	// 0x3124
	Int16Rect UnitDimensions[UNIT_COUNT];
	uint16 Portraits[UNIT_COUNT];
	uint16 MineralCosts[UNIT_COUNT];
	uint16 VespeneCosts[UNIT_COUNT];
	// 0x3d9c
	uint16 BuildTimes[UNIT_COUNT];
	// 0x3f64
	uint16 Unknown1s[UNIT_COUNT];
	// 0x412c
	/*
	0x01: Zerg
	0x02: Terran
	0x04: Protoss
	0x08: Men
	0x10: Building
	0x20: Produces Units
	0x40: Independent Unit
	0x80: neutral
	*/
	uint8 StarEditGroupFlags[UNIT_COUNT]; 	
	// 0x4210
	uint8 SupplyProvideds[UNIT_COUNT];
	uint8 SupplyRequireds[UNIT_COUNT];
	uint8 SpaceRequireds[UNIT_COUNT];
	uint8 SpaceProvideds[UNIT_COUNT];
	// 0x45a0
	uint16 BuildScores[UNIT_COUNT];
	uint16 DestroyScores[UNIT_COUNT];
	// 0x4930
	uint16 UnitMapStrings[UNIT_COUNT];
	uint8 BroodwarUnitFlags[UNIT_COUNT];
	uint16 StarEditAvailabilityFlags[UNIT_COUNT];
};
#pragma pack(pop)

//static_assert(sizeof(UnitData) == 19876, "UnitData size is invalid");

#pragma pack(push, 1)
struct WeaponData
{
	enum { WEAPON_COUNT = 130 };
	uint16 Labels[WEAPON_COUNT]; // Name
	uint32 Graphics[WEAPON_COUNT]; // Flingy
	uint8 Unuseds[WEAPON_COUNT];
	uint16 TargetFlags[WEAPON_COUNT];
	uint32 MinimumRanges[WEAPON_COUNT];
	uint32 MaximumRanges[WEAPON_COUNT];
	uint8 DamageUpgrades[WEAPON_COUNT];
	uint8 WeaponTypes[WEAPON_COUNT];
	uint8 WeaponBehaviors[WEAPON_COUNT];
	uint8 RemoveAfters[WEAPON_COUNT];
	uint8 ExplosionTypes[WEAPON_COUNT];
	uint16 InnerSplashRanges[WEAPON_COUNT]; // Inside
	uint16 MediumSplashRanges[WEAPON_COUNT]; // Center
	uint16 OuterSplashRanges[WEAPON_COUNT]; // Outside
	uint16 DamageAmounts[WEAPON_COUNT];
	uint16 DamageBonuss[WEAPON_COUNT];
	uint8 WeaponCooldowns[WEAPON_COUNT];
	uint8 DamageFactors[WEAPON_COUNT];
	uint8 AttackAngles[WEAPON_COUNT]; // degree
	uint8 LaunchSpins[WEAPON_COUNT]; // degree
	uint8 ForwardOffsets[WEAPON_COUNT]; // x
	uint8 UpwardOffsets[WEAPON_COUNT]; // y
	uint16 TargetErrorMessages[WEAPON_COUNT];
	uint16 Icons[WEAPON_COUNT];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct UpgradeData
{
	enum { UPGRADE_COUNT = 61 };
	uint16 MineralCostBases[UPGRADE_COUNT];
	uint16 MineralCostFactors[UPGRADE_COUNT];
	uint16 VespeneCostBases[UPGRADE_COUNT];
	uint16 BespeneCostFactors[UPGRADE_COUNT];
	uint16 ResearchTimeBases[UPGRADE_COUNT];
	uint16 ResearchTimeFactors[UPGRADE_COUNT];
	uint16 Unknowns[UPGRADE_COUNT];
	uint16 Icons[UPGRADE_COUNT];
	uint16 Labels[UPGRADE_COUNT]; // Name
	uint8 Races[UPGRADE_COUNT];
	uint8 MaxRepeats[UPGRADE_COUNT];
	uint8 BroodwarOnlys[UPGRADE_COUNT];
};
#pragma pack(pop)

struct ButtonSet
{
	enum { BUTTON_SET_COUNT = 250 };
	struct ButtonList
	{
		uint32 ButtonCount;
		uint32 Unknown1;
		struct ButtonInfo
		{
			uint16 Location;
			uint16 Icon;
			uint32 Condition;
			uint32 Action;
			uint16 ConditionValue;
			uint16 ActionValue;
			uint16 Enable;
			uint16 Disable;
		};
		ButtonInfo ButtonInfos[1];
	};
	ButtonList* ButtonLists[BUTTON_SET_COUNT];
};
