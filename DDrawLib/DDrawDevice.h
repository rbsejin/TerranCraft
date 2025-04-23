#pragma once

#include <ddraw.h>
#include <list>
#include "../Common/typedef.h"

struct GRPFrame;

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
	void DrawPath(const std::list<Int32Vector2>& path, int32 cellSize, uint32 color);
	void DrawPath(const std::list<Int32Vector2>& path, int32 cellSize, Int32Rect countourBounds, uint32 color);
	void DrawGrid(int32 gridSize, int32 rowCount, int32 colCount, uint32 color);
	void DrawRect(int32 screenX, int32 screenY, int32 width, int32 height, uint32 color);
	void DrawBound(Int32Rect bound, uint32 color);
	bool DrawBitmap(int32 screenX, int32 screenY, int32 width, int32 height, const uint8* buffer);
	bool DrawPCX(int32 screenX, int32 screenY, const uint8* buffer, int32 width, int32 height, const PALETTEENTRY* palette);
	bool DrawGRP(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const PALETTEENTRY* palette);
	bool DrawGRPWithBlending(int32 screenX, int32 screenY, const GRPFrame* frame, const uint8* compressedImage, const PALETTEENTRY* palette);
	bool DrawGRP2(int32 screenX, int32 screenY, const GRPFrame* frame, Int32Rect grpRect, const uint8* compressedImage, const PALETTEENTRY* palette);
	bool DrawGRP2Flipped(int32 screenX, int32 screenY, const GRPFrame* frame, Int32Rect grpRect, const uint8* compressedImage, const PALETTEENTRY* palette);
	bool CalculateClipArea(Int32Vector2* outSrcStart, Int32Vector2* outDestStart, Int32Vector2* outDestSize, Int32Vector2 pos, Int32Vector2 imageSize) const;

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
	IDirectDraw7* mDirectDraw;
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
