#pragma once

#include "../Common/typedef.h"
#include "../BWLib/ImageNumber.h"

class GraphicHeader;
class GraphicFrame;
class palette;

class Image final
{
public:
	Image(BW::ImageNumber mImageNumber);
	~Image();

	const GraphicFrame* GetFrame() const;
	const uint8* GetCompressedImage() const;
	const Palette* GetPalette() const;

private:
	BW::ImageNumber mImageNumber;
	const GraphicHeader* mGRPFile;
	uint32 mFrameIndex;
	Palette* mPalette;
};
