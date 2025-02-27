#include "pch.h"

#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Image.h"

Image::Image(BW::ImageNumber imageNumber)
	: mImageNumber(imageNumber)
	, mGRPFile(nullptr)
	, mFrameIndex(0)
{
	mGRPFile = ImageResource::Instance.GetGRPFile(imageNumber);

#ifdef _DEBUG
	if (mGRPFile == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	// TODO: Set Palette by ImageNumber
	mPalette = ImageResource::Instance.GetPalette(6);

#ifdef _DEBUG
	if (mPalette == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG
}

Image::~Image()
{
}

const GraphicFrame* Image::GetFrame() const
{
	return mGRPFile->Frames + mFrameIndex;
}

const uint8* Image::GetCompressedImage() const
{
	const GraphicFrame* frame = GetFrame();
	return (uint8*)mGRPFile + frame->DataOffset;
}

const Palette* Image::GetPalette() const
{
	return mPalette;
}
