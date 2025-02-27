#pragma once

#include <ddraw.h>
#include "../Common/typedef.h"

class GraphicFrame;
class Palette;

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

	void DrawGrid(int32 gridSize, int32 rowCount, int32 colCount, uint32 color);
	void DrawRect(int32 screenX, int32 screenY, int32 width, int32 height, uint32 color);
	void DrawBound(int left, int top, int right, int bottom, uint32 color);
	bool DrawGRP(int32 screenX, int32 screenY, const GraphicFrame* frame, const uint8* compressedImage, const Palette* palette, bool bFlipped, bool bDrawingBound);
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
};
