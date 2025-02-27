#pragma once

#include "../BWLib/ImageNumber.h"
#include "Palette.h"

struct GraphicHeader;

class ImageResource
{
public:
	bool Load(const char* grpListFilename, const char* paletteListFilename);
	void Destroy();

	Palette* GetPalette(int index) const;
	const GraphicHeader* GetGRPFile(BW::ImageNumber imageNumber) const;

private:
	ImageResource();
	~ImageResource() = default;

public:
	static ImageResource Instance;
private:
	enum { IMAGE_COUNT = BW::ImageNumber::None };
	GraphicHeader* mGRPFiles[IMAGE_COUNT];

	enum { PALETTE_COUNT = 7 };
	Palette* mPalettes[PALETTE_COUNT];
};
