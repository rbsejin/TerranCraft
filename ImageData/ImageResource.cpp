#include "pch.h"
#include "ImageResource.h"
#include "../TerranCraft/Arrangement.h"
#include "../TerranCraft/BWFile.h"

ImageResource ImageResource::Instance;

bool ImageResource::Load(const char* paletteListFilename)
{
	bool bResult = false;

	Destroy();

#pragma region Load GRP
	{
		constexpr size_t BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE] = { 0, };
		int index = 0;

		// format: [index] [path]
		//while (fgets(buffer, BUFFER_SIZE, fp) != nullptr)

		uint16 imageCount = Arrangement::Instance.GetImageCount();

		for (int i = 0; i < imageCount; i++)
		{
			const ImageData* imageData = Arrangement::Instance.GetImageData();
			int32 index = imageData->GRPFiles[i] - 1;
			const char* grpFilename = Arrangement::Instance.GetImageName((uint16)index);
			char grpListFilename[MAX_PATH] = { 0, };
			sprintf(grpListFilename, "%s/%s", "../data/STARDAT/unit/", grpFilename);

			// Load GRP files
			{
				FILE* fp = nullptr;
				fopen_s(&fp, grpListFilename, "rb");

				if (fp == nullptr)
				{
					continue;
				}

				fseek(fp, 0, SEEK_END);
				int32 fileSize = ftell(fp);

				fseek(fp, 0, SEEK_SET);

#ifdef _DEBUG
				if (mGRPFiles[i] != nullptr)
				{
					__debugbreak();
				}
#endif // _DEBUG

				GRPHeader* grpFile = (GRPHeader*)malloc(fileSize);
				fread(grpFile, fileSize, 1, fp);
				mGRPFiles[i] = grpFile;

				fclose(fp);
			}
		}
	}
#pragma endregion

#pragma region Load Palette
	{
		FILE* fp = nullptr;
		fopen_s(&fp, paletteListFilename, "rt");

		if (fp == nullptr)
		{
			goto LB_RETURN;
		}

		constexpr size_t BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE] = { 0, };
		int index = 0;

		// format: [index] [path]
		while (fgets(buffer, BUFFER_SIZE, fp) != nullptr)
		{
			char paletteListFilename[MAX_PATH] = { 0, };
			int count = sscanf(buffer, "%d %s", &index, paletteListFilename);

#ifdef _DEBUG
			if (count != 2)
			{
				__debugbreak();
			}
#endif // _DEBUG

			if (strlen(paletteListFilename) == 0)
			{
				continue;
			}

			// Load Palette files
			{
				Palette* palette = new Palette();
				if (!palette->Load(paletteListFilename))
				{
					delete palette;
					continue;
				}
				mPalettes[index] = palette;
			}
		}

		fclose(fp);
	}
#pragma endregion

	bResult = true;

LB_RETURN:
	return bResult;
}

void ImageResource::Destroy()
{
	for (uint32 i = 0; i < IMAGE_COUNT; i++)
	{
		free(mGRPFiles[i]);
		mGRPFiles[i] = nullptr;
	}

	for (uint32 i = 0; i < PALETTE_COUNT; i++)
	{
		delete mPalettes[i];
		mPalettes[i] = nullptr;
	}
}

const Palette* ImageResource::GetPalette(int index) const
{
	return mPalettes[index];
}

const GRPHeader* ImageResource::GetGRPFile(BW::ImageNumber imageNumber) const
{
	return mGRPFiles[(int)imageNumber];
}
