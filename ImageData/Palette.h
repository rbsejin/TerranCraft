#pragma once

class Palette final
{
public:
	static uint32 GetColor(const PALETTEENTRY* palette, uint8 index);
	static void SetEntries(PALETTEENTRY* palette, int32 startIndex, int32 count, const PALETTEENTRY* colors);

public:
	static PALETTEENTRY sData[256];
	static PALETTEENTRY sIconData[16];
	static PALETTEENTRY sIconWireData[16];
	static PALETTEENTRY sTUnitPaletteEntries[128];
	static PALETTEENTRY sTSelectPaletteEntries[24];
	static PALETTEENTRY sTWirePaletteEntries[24];
};
