#pragma once

#include "../Common/typedef.h"
#include "../BWLib/IScriptEntryNumber.h"
#include "../BWLib/IScriptAnimation.h"

class Thingy;
class Image;

struct IndexEntry
{
	uint16 ID;
	uint16 Offset;
};

struct IScriptEntry
{
	uint16 Header;
	uint32 Type;
	uint16 Offsets[28]; // Index: IScriptAnimation, Value: Offset
};

class AnimationController final
{
public:
	AnimationController() = default;
	~AnimationController();

	bool Load(const char* filename);
	void Destroy();
	void UpdateImageFrame(Thingy* thingy, Image* image) const;

	uint16 GetIScriptHeader(uint32 entryNumber) const;
	uint16 GetIScriptOffset(uint16 headerOffset, BW::IScriptAnimation animation) const;
	void GetData(void* dest, uint16 offset, uint32 size) const;

private:
	constexpr static uint32 sHeaderOffsetCounts[] =
	{
		2, 2, 4, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 14, 14, 16, 16, 0, 0, 0, 0,
		22, 22, 0, 24, 26, 0, 28, 28, 28, 28
	};
	static_assert(sizeof(sHeaderOffsetCounts) / sizeof(sHeaderOffsetCounts[0]) == 30, "sHeaderOffsetCounts size is not 30");

	enum { ISCRIPT_ENTRY_COUNT = 412 };
	IScriptEntry mIScriptEntries[ISCRIPT_ENTRY_COUNT] = { 0, };

	uint8* mData = nullptr;
};
