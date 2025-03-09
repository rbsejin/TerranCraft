#pragma once

#include "../Common/typedef.h"
#include "../BWLib/UnitType.h"

enum class CommandIcon : uint16
{
	Marine = 0,
	Ghost = 1,
	Vulture = 2,
	Goliath = 3,
	BLANK_4 = 4,
	Tank = 5,
	BLANK_6 = 6,
	SCV = 7,

	CommandCenter = 106,
	ComsatStation = 107,
	NuclearSilo = 108,
	SupplyDepot = 109,
	Refinery = 110,
	Barracks = 111,
	Academy = 112,
	EngineeringBay = 122,
	MissileTower = 124,
	Bunker = 125,

	Move = 228,
	Stop = 229,
	Attack = 230,
	Gather = 231,
	Repair = 232,
	ReturnResource = 233,
	TerranBasicBuilding = 234,
	TerranAdvancedBuilding = 235,
	Cancel = 236,
	UseStimpack = 237,
	U_238Shells = 238,

	LiftOff = 282,
	Land = 283,
	SetRallyPoint = 286,

	Patrol = 0xFE,
	HoldPosition = 0xFF,
};

struct ButtonInfo
{
	// Basic Info
	uint16 Location;
	CommandIcon Icon;
	uint16 Enable;
	uint16 Disable;

	// Button Work
	uint8 Condition;
	uint8 ConditionValue;
	uint8 Action;
	uint8 ActionValue;
};
static_assert(sizeof(ButtonInfo) == 12, "Button size is invalid");

struct Buttonset
{
	// Button List
	enum { MAX_BUTTON_COUNT = 12 };
	ButtonInfo ButtonList[MAX_BUTTON_COUNT];
	int32 ButtonCount;
};
static_assert(sizeof(Buttonset) == 148, "Buttonset size is invalid");

enum eButtonset : uint16
{
	Marine = 0,
	SCV = 7,
	TerranCommandCenter = 106,
	TerranStructureConstruction = 239,
	TerranAdvancedStructureConstruction = 242
};