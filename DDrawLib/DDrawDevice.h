#pragma once

#include <ddraw.h>
#include <list>
#include "../Common/typedef.h"

struct GRPFrame;
class Palette;
struct Chunk;
union RGBColor;

class DDrawDevice final
{
public:
	DDrawDevice();
	~DDrawDevice();

	bool Initialize(HWND hWnd);
	void Cleanup();

	bool BeginDraw();
	void EndDraw();
	void OnDraw();
	bool CheckFPS();

	bool BeginGDI(HDC* outDC);
	void EndGDI(HDC hdc);
	void DrawInfo(HDC hdc) const;

	void DrawMap(int32 gridSize, int32 rowCount, int32 colCount, const void* map, const uint32* mapImage, int32 mapImageWidth);
	void DrawPath(const std::list<IntVector2>& path, int32 cellSize, uint32 color);
	void DrawPath(const std::list<IntVector2>& path, int32 cellSize, IntRect countourBounds, uint32 color);
	void DrawGrid(int32 gridSize, int32 rowCount, int32 colCount, uint32 color);
	void DrawRect(int32 screenX, int32 screenY, int32 width, int32 height, uint32 color);
	void DrawBound(IntRect bound, uint32 color);
	bool DrawBitmap(int32 screenX, int32 screenY, int32 width, int32 height, const uint8* buffer);
	bool DrawPCX(int32 screenX, int32 screenY, const Chunk* chunk, int32 width, int32 height, const RGBColor* palette);
	bool DrawGRP(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const Palette* palette);
	bool DrawGRPWithBlending(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const Palette* palette);
	bool DrawGRP2(int32 screenX, int32 screenY, const GRPFrame* frame, IntRect grpRect, const uint8* compressedImage, const Palette* palette);
	bool DrawGRP2Flipped(int32 screenX, int32 screenY, const GRPFrame* frame, IntRect grpRect, const uint8* compressedImage, const Palette* palette);
	bool CalculateClipArea(IntVector2* outSrcStart, IntVector2* outDestStart, IntVector2* outDestSize, IntVector2 pos, IntVector2 imageSize) const;

	void Clear();

	void OnUpdateWindowSize();
	void OnUpdateWindowPos();

	uint32 GetWidth() const;
	uint32 GetHeight() const;

private:
	bool createBackSurface(uint32 width, uint32 height);
	void cleanupBackSurface();

	void updateInfoText();
	void writeText(HDC hdc, int32 x, int32 y, uint32 color, const wchar_t* text, int32 length) const;

private:
	IDirectDraw* mDirectDraw;
	IDirectDraw7* mDirectDraw7;
	IDirectDrawSurface7* mPrimarySurface;
	IDirectDrawSurface7* mBackSurface;
	IDirectDrawClipper* mClipper;

	uint8* mLockedBackBuffer;
	uint32 mLockedBackBufferPitch;
	uint32 mWidth;
	uint32 mHeight;
	RECT mWindowRect;
	HWND mhWnd;

	uint32 mFrameCount;
	uint32 mFPS;
	ULONGLONG mLastDrawTick;
	enum { MAX_FPS_TEXT_LENGTH = 256 };
	wchar_t mInfoText[MAX_FPS_TEXT_LENGTH];
	uint32 mInfoTextLength;

public:
#ifdef _DEBUG
	bool IsVF4On;
	bool IsGridOn;
	bool IsPathOn;
	bool IsBoundOn;
#endif
};
