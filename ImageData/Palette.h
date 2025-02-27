#pragma once

struct PaletteRGB;

class Palette final
{
public:
	Palette();
	~Palette();
	bool Load(const char* filename);

	uint32 GetColor(int index) const;

private:
	PaletteRGB* mData;
};
