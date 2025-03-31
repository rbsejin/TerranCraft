#pragma once

#include "../BWLib/ImageNumber.h"
#include "Palette.h"

struct GRPHeader;

class ImageResource
{
public:
	bool Load(const char* paletteListFilename);
	void Destroy();

	const Palette* GetPalette(int index) const;
	const GRPHeader* GetGRPFile(BW::ImageNumber imageNumber) const;

private:
	ImageResource() = default;
	~ImageResource() = default;

public:
	static ImageResource Instance;
private:
	enum { IMAGE_COUNT = BW::ImageNumber::None };
	GRPHeader* mGRPFiles[IMAGE_COUNT] = { 0, };

	enum { PALETTE_COUNT = 7 };
	Palette* mPalettes[PALETTE_COUNT] = { 0, };
};
