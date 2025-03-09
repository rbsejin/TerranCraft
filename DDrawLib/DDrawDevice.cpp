#include "pch.h"

#include <stdio.h>
#include "../Common/typedef.h"
#include "DDrawDevice.h"
#include "../ImageData/Graphic.h"
#include "../ImageData/Palette.h"
#include "../TerranCraft/PathFinder.h"
#include "../TerranCraft/Camera.h"
#include "../TerranCraft/Map.h"

DDrawDevice::DDrawDevice()
{
	memset(this, 0, sizeof(DDrawDevice));

#ifdef _DEBUG
	IsVF4On = false;
	IsGridOn = false;
	IsPathOn = false;
	IsBoundOn = false;
#endif // _DEBUG

}

DDrawDevice::~DDrawDevice()
{
	Cleanup();
}

bool DDrawDevice::Initialize(HWND hWnd)
{
	bool bResult = false;

	mhWnd = hWnd;

	DDSURFACEDESC2 ddsd = { 0, };
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (FAILED(DirectDrawCreate(nullptr, &mDirectDraw, nullptr)))
	{
		MessageBox(nullptr, L"DirectDrawCreate() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mDirectDraw->QueryInterface(IID_IDirectDraw7, (void**)&mDirectDraw7)))
	{
		MessageBox(nullptr, L"QueryInterface() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mDirectDraw7->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
	{
		MessageBox(nullptr, L"SetCooperativeLevel() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mDirectDraw7->CreateSurface(&ddsd, &mPrimarySurface, nullptr)))
	{
		MessageBox(nullptr, L"CreateSurface() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mDirectDraw->CreateClipper(0, &mClipper, nullptr)))
	{
		MessageBox(nullptr, L"CreateClipper() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mClipper->SetHWnd(0, hWnd)))
	{
		MessageBox(nullptr, L"SetHWnd() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	if (FAILED(mPrimarySurface->SetClipper(mClipper)))
	{
		MessageBox(nullptr, L"SetClipper() Failed", L"Error", MB_OK);
		goto LB_RESULT;
	}

	OnUpdateWindowPos();

	uint32 width = mWindowRect.right - mWindowRect.left;
	uint32 height = mWindowRect.bottom - mWindowRect.top;

	Camera::Instance.SetSize({ (int32)width, (int32)height });

	if (!createBackSurface(width, height))
	{
		goto LB_RESULT;
	}

	bResult = true;

LB_RESULT:
	return bResult;
}

void DDrawDevice::Cleanup()
{
	cleanupBackSurface();

	if (mClipper != nullptr)
	{
		mClipper->Release();
		mClipper = nullptr;
	}

	if (mPrimarySurface != nullptr)
	{
		mPrimarySurface->Release();
		mPrimarySurface = nullptr;
	}

	if (mDirectDraw7 != nullptr)
	{
		mDirectDraw7->Release();
		mDirectDraw7 = nullptr;
	}

	if (mDirectDraw != nullptr)
	{
		mDirectDraw->Release();
		mDirectDraw = nullptr;
	}
}

bool DDrawDevice::BeginDraw()
{
	bool bResult = false;

	DDSURFACEDESC2 ddsd = { 0, };
	ddsd.dwSize = sizeof(ddsd);

	if (mBackSurface == nullptr)
	{
		goto LB_RETURN;
	}

	if (FAILED(mBackSurface->Lock(nullptr, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, nullptr)))
	{
#ifdef _DEBUG
		__debugbreak();
#endif
		goto LB_RETURN;
	}

#ifdef _DEBUG
	if (ddsd.dwWidth != mWidth)
	{
		__debugbreak();
	}

	if (ddsd.dwHeight != mHeight)
	{
		__debugbreak();
	}
#endif

	mLockedBackBuffer = (uint8*)ddsd.lpSurface;
	mLockedBackBufferPitch = ddsd.lPitch;

	bResult = true;

LB_RETURN:
	return bResult;
}

void DDrawDevice::EndDraw()
{
	if (mBackSurface != nullptr)
	{
		mBackSurface->Unlock(nullptr);
	}

	mLockedBackBuffer = nullptr;
	mLockedBackBufferPitch = 0;
}

void DDrawDevice::OnDraw()
{
#ifdef _DEBUG
	if (mPrimarySurface == nullptr)
	{
		__debugbreak();
	}

	if (mBackSurface == nullptr)
	{
		__debugbreak();
	}
#endif

	mPrimarySurface->Blt(&mWindowRect, mBackSurface, nullptr, DDBLT_WAIT, nullptr);
}

bool DDrawDevice::CheckFPS()
{
	bool bUpdated = false;

	ULONGLONG currentTick = GetTickCount64();
	if (currentTick - mLastDrawTick >= 1000)
	{
		uint32 oldFPS = mFPS;
		mFPS = mFrameCount;
		mFrameCount = 0;
		mLastDrawTick = currentTick;

		if (oldFPS != mFPS)
		{
			updateInfoText();
			bUpdated = true;
		}
	}

	mFrameCount++;

	return bUpdated;
}

bool DDrawDevice::BeginGDI(HDC* outDC)
{
#ifdef _DEBUG
	if (outDC == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	bool bResult = false;
	HDC hdc = nullptr;

	if (mBackSurface == nullptr)
	{
		goto LB_RETURN;
	}

	if (FAILED(mBackSurface->GetDC(&hdc)))
	{
#ifdef _DEBUG
		__debugbreak();
#endif // _DEBUG

		goto LB_RETURN;
	}

	bResult = true;
	*outDC = hdc;

LB_RETURN:
	return bResult;
}

void DDrawDevice::EndGDI(HDC hdc)
{
#ifdef _DEBUG
	if (hdc == nullptr)
	{
		__debugbreak();
	}

	if (mBackSurface == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	mBackSurface->ReleaseDC(hdc);
}

void DDrawDevice::DrawInfo(HDC hdc) const
{
	writeText(hdc, 0, 0, 0xffff0000, mInfoText, mInfoTextLength);
}

void DDrawDevice::DrawMap(int32 gridSize, int32 rowCount, int32 colCount, const void* map, const uint32* mapImage, int32 mapImageWidth)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	IntVector2 cameraPosition = Camera::Instance.GetPosition();
	IntVector2 cameraSize = Camera::Instance.GetSize();

	int32 cameraX = cameraPosition.X;
	int32 cameraY = cameraPosition.Y;
	int32 cameraWidth = cameraSize.X;
	int32 cameraHeight = cameraSize.Y;

	int32 minX = (int32)ceilf((float)cameraX / gridSize);
	int32 minY = (int32)ceilf((float)cameraY / gridSize);
	int32 maxX = (int32)((cameraX + cameraWidth) / gridSize);
	int32 maxY = (int32)((cameraY + cameraHeight) / gridSize);

	uint8* pDest = mLockedBackBuffer;

	for (int32 y = 0; y < cameraHeight; y++)
	{
		for (int32 x = 0; x < cameraWidth; x++)
		{
			*(uint32*)(pDest + y * mLockedBackBufferPitch + x * 4)
				= mapImage[(cameraPosition.Y + y) * mapImageWidth + (cameraPosition.X + x)];
		}
	}

#ifdef _DEBUG
	if (IsVF4On)
	{
		for (int32 y = minY; y < maxY; y++)
		{
			for (int32 x = minX; x < maxX; x++)
			{
				uint32 color = 0xff000000;

				if (((uint8*)map)[y * colCount + x] & 0x01)
				{
					color = 0xff220000;

					if (((uint8*)map)[y * colCount + x] & 0x02)
					{
						color = 0xff880000;


					}

					if (((uint8*)map)[y * colCount + x] & 0x04)
					{
						color = 0xffbb0000;

					}

					if (((uint8*)map)[y * colCount + x] & 0x08)
					{
						color = 0xffffff00;
					}

					if (((uint8*)map)[y * colCount + x] & 0x10)
					{
						color = 0xff550000;

					}
				}
				else
				{
					color = 0xff000022;

					if (((uint8*)map)[y * colCount + x] & 0x02)
					{
						color = 0xff000088;

					}

					if (((uint8*)map)[y * colCount + x] & 0x04)
					{
						color = 0xff0000bb;

					}

					if (((uint8*)map)[y * colCount + x] & 0x08)
					{

						color = 0xff00ffff;


					}

					if (((uint8*)map)[y * colCount + x] & 0x10)
					{
						color = 0xff000055;

					}
				}

				int32 left = x * gridSize - cameraX;
				int32 top = y * gridSize - cameraY;

				for (int32 i = 0; i < gridSize; i++)
				{
					for (int32 j = 0; j < gridSize; j++)
					{
						*(uint32*)(pDest + (top + i) * mLockedBackBufferPitch + (left + j) * 4) = color;
					}
				}
			}
		}
	}
#endif // _DEBUG
}

void DDrawDevice::DrawPath(const std::list<IntVector2>& path, int32 cellSize, uint32 color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}

	if (cellSize <= 0)
	{
		__debugbreak();
	}
#endif

	for (const IntVector2& pos : path)
	{
		IntVector2 RectPos = { pos.X * cellSize + 1, pos.Y * cellSize + 1 };
		IntVector2 RectSize = { cellSize - 1, cellSize - 1 };

		DrawRect(RectPos.X, RectPos.Y, RectSize.X, RectSize.Y, color);
	}
}

void DDrawDevice::DrawPath(const std::list<IntVector2>& path, int32 cellSize, IntRect countourBounds, uint32 color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}

	if (cellSize <= 0)
	{
		__debugbreak();
	}
#endif

	for (IntVector2 pos : path)
	{
		IntVector2 unitPos = { pos.X * cellSize + cellSize / 2, pos.Y * cellSize + cellSize / 2 };
		unitPos.X -= Camera::Instance.GetPosition().X;
		unitPos.Y -= Camera::Instance.GetPosition().Y;
		IntRect unitBound = { unitPos.X - countourBounds.Left, unitPos.Y - countourBounds.Top, unitPos.X + countourBounds.Right, unitPos.Y + countourBounds.Bottom };
		DrawBound(unitBound, color);
	}
}

void DDrawDevice::DrawGrid(int32 gridSize, int32 rowCount, int32 colCount, uint32 color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	IntVector2 cameraPosition = Camera::Instance.GetPosition();
	IntVector2 cameraSize = Camera::Instance.GetSize();

	int32 cameraX = cameraPosition.X;
	int32 cameraY = cameraPosition.Y;
	int32 cameraWidth = cameraSize.X;
	int32 cameraHeight = cameraSize.Y;

	int32 minGridX = (int32)ceilf((float)cameraX / gridSize);
	int32 minGridY = (int32)ceilf((float)cameraY / gridSize);

	IntVector2 startGridPos = { minGridX * gridSize - cameraX, minGridY * gridSize - cameraY };

	uint8* pDest = mLockedBackBuffer;

	for (int32 y = startGridPos.Y; y < cameraHeight; y += gridSize)
	{
		for (int32 x = startGridPos.X; x < cameraWidth; x++)
		{
			*(uint32*)(pDest + x * 4) = color;
		}

		pDest += mLockedBackBufferPitch * gridSize;
	}

	pDest = mLockedBackBuffer;

	for (int32 x = startGridPos.X; x < cameraWidth; x += gridSize)
	{
		for (int32 y = startGridPos.Y; y < cameraHeight; y++)
		{
			*(uint32*)(pDest + y * mLockedBackBufferPitch) = color;
		}

		pDest += gridSize * 4;
	}
}

void DDrawDevice::DrawRect(int32 screenX, int32 screenY, int32 width, int32 height, uint32 color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	int32 left = max(0, screenX);
	int32 top = max(0, screenY);
	int32 right = min(screenX + width, (int32)mWidth);
	int32 bottom = min(screenY + height, (int32)mHeight);

	for (int32 y = top; y < bottom; y++)
	{
		for (int32 x = left; x < right; x++)
		{
			uint32* pPixel = (uint32*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
			*pPixel = color;
		}
	}
}

void DDrawDevice::DrawBound(IntRect bound, uint32 color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	bool bResult = false;

	if (bound.Left > bound.Right)
	{
		int32 temp = bound.Left;
		bound.Left = bound.Right;
		bound.Right = temp;
	}

	if (bound.Top > bound.Bottom)
	{
		int32 temp = bound.Top;
		bound.Top = bound.Bottom;
		bound.Bottom = temp;
	}

	int left = max(bound.Left, 0);
	int top = max(bound.Top, 0);
	int right = min(bound.Right, (int32)mWidth - 1);
	int bottom = min(bound.Bottom, (int32)mHeight - 1);

	int width = right - left;
	int height = bottom - top;

	if (width <= 0 || height <= 0)
	{
		return;
	}

	int y = top;

	for (int x = left; x <= right; x++)
	{
		uint32* pPixel = (uint32*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
		*pPixel = color;
	}

	y = bottom;

	for (int x = left; x <= right; x++)
	{
		uint32* pPixel = (uint32*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
		*pPixel = color;
	}

	int x = left;

	for (int y = top; y <= bottom; y++)
	{
		uint32* pPixel = (uint32*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
		*pPixel = color;
	}

	x = right;

	for (int y = top; y <= bottom; y++)
	{
		uint32* pPixel = (uint32*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
		*pPixel = color;
	}
}

bool DDrawDevice::DrawBitmap(int32 screenX, int32 screenY, int32 width, int32 height, const uint8* buffer)
{
	bool bResult = false;

#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	screenX -= width / 2;
	screenY -= height / 2;

	int32 left = max(0, screenX);
	int32 top = max(0, screenY);
	int32 right = min(screenX + width, (int32)mWidth);
	int32 bottom = min(screenY + height, (int32)mHeight);

	uint8* pSrc = (uint8*)buffer;
	uint8* pDst = (uint8*)(mLockedBackBuffer + top * mLockedBackBufferPitch);

	for (int32 y = top; y < bottom; y++)
	{
		for (int32 x = left; x < right; x++)
		{
			*(uint32*)(pDst + x * 4) = *(uint32*)pSrc;
			pSrc += 4;
		}

		pDst += mLockedBackBufferPitch;
	}

	bResult = true;

	return bResult;
}

bool DDrawDevice::DrawPCX(int32 screenX, int32 screenY, const Chunk* chunk, int32 width, int32 height, const RGBColor* palette)
{
	bool bResult = false;

	const uint8* dataCurrent = chunk->Data + 128;
	const uint8* dataEnd = chunk->Data + chunk->Length - 767;

	uint8* pDest = mLockedBackBuffer + screenY * mLockedBackBufferPitch + screenX * 4;

	for (int32 y = 0; y < height; y++)
	{
		int32 x = 0;

		while (x < width)
		{
			uint8 byte = *dataCurrent++;
			if ((byte & 0xc0) == 0xc0)
			{
				int32 count = byte & 0x3f;
				uint8 color = *dataCurrent++;

				if (palette[color].Value != 0x00000000)
				{
					for (int32 i = 0; i < count; i++)
					{
						*(uint32*)(pDest + (x + i) * 4) = palette[color].Value;
					}
				}

				x += count;
			}
			else
			{
				*(uint32*)(pDest + x * 4) = palette[byte].Value;
				x++;
			}
		}

		pDest += mLockedBackBufferPitch;
	}

	bResult = true;

LB_RETURN:
	return bResult;
}

bool DDrawDevice::DrawGRP(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const Palette* palette)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}

	if (compressedImage == nullptr)
	{
		__debugbreak();
	}

	if (frame == nullptr)
	{
		__debugbreak();
	}

	if (palette == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	bool bResult = false;

	IntVector2 srcStart = { 0, };
	IntVector2 destStart = { 0, };
	IntVector2 destSize = { 0, };

	IntVector2 imageSize = { frame->Width, frame->Height };
	IntVector2 screenPos = { screenX, screenY };

	if (!CalculateClipArea(&srcStart, &destStart, &destSize, screenPos, imageSize))
	{
		goto LB_RETURN;
	}

	uint8* pDestPerLine = mLockedBackBuffer + destStart.Y * mLockedBackBufferPitch;

	const uint16* offsets = (const uint16*)compressedImage;
	const uint16* pOffset = offsets + srcStart.Y;

	if (false)
	{
		for (int32 y = 0; y < destSize.Y; y++)
		{
			const uint8* pStream = compressedImage + (uint64)*pOffset++;

			const uint8* pDest = pDestPerLine;
			int32 destX = screenPos.X + imageSize.X - 1;

			while (destX >= destStart.X)
			{
				uint8 opcode = *pStream++;

				if (opcode >= 0x80)
				{
					// 1. byte >= 0x80: 0을 출력
					int32 count = opcode - 0x80;
					destX -= count;
				}
				else if (opcode >= 0x40)
				{
					// 2. 0x40 <= byte < 0x80 : 다음 바이트를 (byte - 0x40)만큼 반복해서 출력
					int32 count = opcode - 0x40;
					uint32 pixel = palette->GetColor(*pStream++);
					int32 pixelCount = count;

					if (destX >= (int32)mWidth)
					{
						pixelCount += (mWidth - destX - 1);
						destX -= count - pixelCount;
					}
					if (destX < pixelCount - 1)
					{
						pixelCount = destX + 1;
					}

					pDest = pDestPerLine + (uint64)(destX * 4);

					for (int i = 0; i < pixelCount; i++)
					{
						*(uint32*)pDest = pixel;
						pDest -= 4;
					}

					destX -= pixelCount;
				}
				else
				{
					// 3. byte < 0x40 : 그대로 출력
					int32 count = opcode;
					int32 pixelCount = count;

					if (destX >= (int32)mWidth)
					{
						pixelCount += (mWidth - destX - 1);
						destX -= count - pixelCount;
						pStream += count - max(pixelCount, 0);
					}
					if (destX < pixelCount - 1)
					{
						pixelCount = destX + 1;
					}

					pDest = pDestPerLine + (uint64)(destX * 4);

					for (int i = 0; i < pixelCount; i++)
					{
						uint32 pixel = palette->GetColor(*pStream++);
						*(uint32*)pDest = pixel;
						pDest -= 4;
					}

					destX -= pixelCount;
				}
			}

			pDestPerLine += mLockedBackBufferPitch;
		}
	}
	else
	{
		for (int32 y = 0; y < destSize.Y; y++)
		{
			const uint8* pStream = (uint8*)compressedImage + (uint64)*pOffset++;

			const uint8* pDest = pDestPerLine;
			int32 destX = screenPos.X;

			while (destX < destStart.X + destSize.X)
			{
				uint8 opcode = *pStream++;

				if (opcode >= 0x80)
				{
					// 1. byte >= 0x80: 0을 출력
					int32 count = opcode - 0x80;
					destX += count;
				}
				else if (opcode >= 0x40)
				{
					// 2. 0x40 <= byte < 0x80 : 다음 바이트를 (byte - 0x40)만큼 반복해서 출력
					int32 count = opcode - 0x40;
					uint32 pixel = palette->GetColor(*pStream++);
					int32 pixelCount = count;

					if (destX < 0)
					{
						pixelCount += destX;
						destX += count - pixelCount;
					}
					if (destX + pixelCount > (int32)mWidth)
					{
						pixelCount = (int32)mWidth - destX;
					}

					pDest = pDestPerLine + (uint64)(destX * 4);

					for (int i = 0; i < pixelCount; i++)
					{
						*(uint32*)pDest = pixel;
						pDest += 4;
					}

					destX += pixelCount;
				}
				else
				{
					// 3. byte < 0x40 : 그대로 출력
					int32 count = opcode;
					int32 pixelCount = count;

					if (destX < 0)
					{
						pixelCount += destX;
						destX += count - pixelCount;
						pStream += count - max(pixelCount, 0);
					}
					if (destX + pixelCount > (int32)mWidth)
					{
						pixelCount = (int32)mWidth - destX;
					}

					pDest = pDestPerLine + (uint64)(destX * 4);

					for (int i = 0; i < pixelCount; i++)
					{
						uint32 pixel = palette->GetColor(*pStream++);
						*(uint32*)pDest = pixel;
						pDest += 4;
					}

					destX += pixelCount;
				}
			}

			pDestPerLine += mLockedBackBufferPitch;
		}
	}

	bResult = true;
LB_RETURN:
	return bResult;
}

bool DDrawDevice::DrawGRPWithBlending(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const Palette* palette)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}

	if (compressedImage == nullptr)
	{
		__debugbreak();
	}

	if (frame == nullptr)
	{
		__debugbreak();
	}

	if (palette == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	bool bResult = false;

	float alpha = 0.2f;

	IntVector2 srcStart = { 0, };
	IntVector2 destStart = { 0, };
	IntVector2 destSize = { 0, };

	IntVector2 imageSize = { frame->Width, frame->Height };
	IntVector2 screenPos = { screenX, screenY };

	if (!CalculateClipArea(&srcStart, &destStart, &destSize, screenPos, imageSize))
	{
		goto LB_RETURN;
	}

	uint8* pDestPerLine = mLockedBackBuffer + destStart.Y * mLockedBackBufferPitch;

	const uint16* offsets = (const uint16*)compressedImage;
	const uint16* pOffset = offsets + srcStart.Y;

	for (int32 y = 0; y < destSize.Y; y++)
	{
		const uint8* pStream = (uint8*)compressedImage + (uint64)*pOffset++;

		const uint8* pDest = pDestPerLine;
		int32 destX = screenPos.X;

		while (destX < destStart.X + destSize.X)
		{
			uint8 opcode = *pStream++;

			if (opcode >= 0x80)
			{
				// 1. byte >= 0x80: 0을 출력
				int32 count = opcode - 0x80;

				int32 pixelCount = count;

				if (destX < 0)
				{
					pixelCount += destX;
					destX += count - pixelCount;
					pStream += count - max(pixelCount, 0);
				}
				if (destX + pixelCount > (int32)mWidth)
				{
					pixelCount = (int32)mWidth - destX;
				}

				pDest = pDestPerLine + (uint64)(destX * 4);

				for (int i = 0; i < pixelCount; i++)
				{
					uint32 pixel = *(uint32*)pDest;
					{
						uint32 destR = (pixel & 0x00ff0000) >> 16;
						uint32 destG = (pixel & 0x0000ff00) >> 8;
						uint32 destB = (pixel & 0x000000ff);

						uint32 blendedR = (uint32)(destR * (1 - alpha));
						uint32 blendedG = (uint32)(destG * (1 - alpha) + alpha * 255);
						uint32 blendedB = (uint32)(destB * (1 - alpha));

						pixel = (blendedR << 16) | (blendedG << 8) | blendedB;
					}
					*(uint32*)pDest = pixel;
					pDest += 4;
				}

				destX += count;
			}
			else if (opcode >= 0x40)
			{
				// 2. 0x40 <= byte < 0x80 : 다음 바이트를 (byte - 0x40)만큼 반복해서 출력
				int32 count = opcode - 0x40;
				uint32 pixel = palette->GetColor(*pStream++);
				int32 pixelCount = count;

				if (destX < 0)
				{
					pixelCount += destX;
					destX += count - pixelCount;
				}
				if (destX + pixelCount > (int32)mWidth)
				{
					pixelCount = (int32)mWidth - destX;
				}

				pDest = pDestPerLine + (uint64)(destX * 4);

				for (int i = 0; i < pixelCount; i++)
				{
					{
						uint32 srcR = (*(uint32*)pDest & 0x00ff0000 >> 16);
						uint32 srcG = (*(uint32*)pDest & 0x0000ff00 >> 8);
						uint32 srcB = (*(uint32*)pDest & 0x000000ff);

						uint32 destR = (pixel & 0x00ff0000) >> 16;
						uint32 destG = (pixel & 0x0000ff00) >> 8;
						uint32 destB = (pixel & 0x000000ff);

						uint32 blendedR = (uint32)(srcR * 0.5f + destR * 0.5f);
						uint32 blendedG = (uint32)(srcG * 0.5f * (1 - alpha) + destG * 0.5f * (1 - alpha) + 255 * alpha);
						uint32 blendedB = (uint32)(srcB * 0.5f + destB * 0.5f);

						pixel = (blendedR << 16) | (blendedG << 8) | blendedB;
					}
					*(uint32*)pDest = pixel;
					pDest += 4;
				}

				destX += pixelCount;
			}
			else
			{
				// 3. byte < 0x40 : 그대로 출력
				int32 count = opcode;
				int32 pixelCount = count;

				if (destX < 0)
				{
					pixelCount += destX;
					destX += count - pixelCount;
					pStream += count - max(pixelCount, 0);
				}
				if (destX + pixelCount > (int32)mWidth)
				{
					pixelCount = (int32)mWidth - destX;
				}

				pDest = pDestPerLine + (uint64)(destX * 4);

				for (int i = 0; i < pixelCount; i++)
				{
					uint32 pixel = palette->GetColor(*pStream++);
					{
						uint32 srcR = (*(uint32*)pDest & 0x00ff0000 >> 16);
						uint32 srcG = (*(uint32*)pDest & 0x0000ff00 >> 8);
						uint32 srcB = (*(uint32*)pDest & 0x000000ff);

						uint32 destR = (pixel & 0x00ff0000) >> 16;
						uint32 destG = (pixel & 0x0000ff00) >> 8;
						uint32 destB = (pixel & 0x000000ff);

						uint32 blendedR = (uint32)(srcR * 0.5f + destR * 0.5f);
						uint32 blendedG = (uint32)(srcG * 0.5f * (1 - alpha) + destG * 0.5f * (1 - alpha) + 255 * alpha);
						uint32 blendedB = (uint32)(srcB * 0.5f + destB * 0.5f);

						pixel = (blendedR << 16) | (blendedG << 8) | blendedB;
					}
					*(uint32*)pDest = pixel;
					pDest += 4;
				}

				destX += pixelCount;
			}
		}

		pDestPerLine += mLockedBackBufferPitch;
	}

	bResult = true;
LB_RETURN:
	return bResult;
}

bool DDrawDevice::CalculateClipArea(IntVector2* outSrcStart, IntVector2* outDestStart, IntVector2* outDestSize, IntVector2 pos, IntVector2 size)  const
{
#ifdef _DEBUG
	if (outSrcStart == nullptr)
	{
		__debugbreak();
	}

	if (outDestStart == nullptr)
	{
		__debugbreak();
	}

	if (outDestSize == nullptr)
	{
		__debugbreak();
	}
#endif // _DEBUG

	bool bResult = false;

	const IntVector2 bufferSize = { (int32)mWidth, (int32)mHeight };

	if (bufferSize.X <= 0)
	{
		goto LB_RETURN;
	}

	if (bufferSize.Y <= 0)
	{
		goto LB_RETURN;
	}

	int32 destStartX = max(pos.X, 0);
	int32 destStartY = max(pos.Y, 0);
	destStartX = min(destStartX, bufferSize.X);
	destStartY = min(destStartY, bufferSize.Y);

	int32 destEndX = max(pos.X + size.X, 0);
	int32 destEndY = max(pos.Y + size.Y, 0);
	destEndX = min(destEndX, bufferSize.X);
	destEndY = min(destEndY, bufferSize.Y);

	int32 destWidth = destEndX - destStartX;
	int32 destHeight = destEndY - destStartY;

	if (destWidth <= 0 || destHeight <= 0)
	{
		goto LB_RETURN;
	}

	outSrcStart->X = destStartX - pos.X;
	outSrcStart->Y = destStartY - pos.Y;
	outDestStart->X = destStartX;
	outDestStart->Y = destStartY;
	outDestSize->X = destWidth;
	outDestSize->Y = destHeight;

	bResult = true;

LB_RETURN:
	return bResult;
}

void DDrawDevice::Clear()
{
	if (mLockedBackBuffer == nullptr)
	{
		return;
	}

	memset(mLockedBackBuffer, 128, (size_t)mLockedBackBufferPitch * mHeight);
}

void DDrawDevice::OnUpdateWindowSize()
{
	cleanupBackSurface();

	OnUpdateWindowPos();

	uint32 width = mWindowRect.right - mWindowRect.left;
	uint32 height = mWindowRect.bottom - mWindowRect.top;

	createBackSurface(width, height);

	if (BeginDraw())
	{
		Clear();
		EndDraw();
	}
}

void DDrawDevice::OnUpdateWindowPos()
{
	GetClientRect(mhWnd, &mWindowRect);
	::ClientToScreen(mhWnd, (LPPOINT)&mWindowRect.left);
	::ClientToScreen(mhWnd, (LPPOINT)&mWindowRect.right);
}

uint32 DDrawDevice::GetWidth() const
{
	return mWidth;
}

uint32 DDrawDevice::GetHeight() const
{
	return mHeight;
}

bool DDrawDevice::createBackSurface(uint32 width, uint32 height)
{
	bool bResult = false;

	DDSURFACEDESC2 ddsd = { 0, };
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	if (FAILED(mDirectDraw7->CreateSurface(&ddsd, &mBackSurface, nullptr)))
	{
		MessageBox(nullptr, L"CreateSurface() Failed", L"Error", MB_OK);
		goto LB_RETURN;
	}

	mBackSurface->GetSurfaceDesc(&ddsd);

	mWidth = ddsd.dwWidth;
	mHeight = ddsd.dwHeight;

	bResult = true;

LB_RETURN:
	return bResult;
}

void DDrawDevice::cleanupBackSurface()
{
	if (mBackSurface != nullptr)
	{
		mBackSurface->Release();
		mBackSurface = nullptr;
	}
}

void DDrawDevice::updateInfoText()
{
	swprintf(mInfoText, sizeof(mInfoText) / sizeof(mInfoText[0]), L"FPS: %d", mFPS);
	mInfoTextLength = (uint32)wcslen(mInfoText);
}

void DDrawDevice::writeText(HDC hdc, int32 x, int32 y, uint32 color, const wchar_t* text, int32 length) const
{
	SetBkMode(hdc, TRANSPARENT);

	RECT textRect = { 0, };
	SIZE size = { 0, };

	GetTextExtentPoint32W(hdc, text, length, &size);

	textRect.left = x;
	textRect.top = y;
	textRect.right = textRect.left + size.cx;
	textRect.bottom = textRect.top + size.cy;

	RGBQUAD rgb = *(RGBQUAD*)&color;
	COLORREF colorref = RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
	SetTextColor(hdc, colorref);
	DrawText(hdc, text, length, &textRect, DT_LEFT | DT_TOP);
}
