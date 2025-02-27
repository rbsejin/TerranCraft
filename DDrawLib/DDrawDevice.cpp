#include "pch.h"

#include <stdio.h>
#include "../Common/typedef.h"
#include "DDrawDevice.h"

DDrawDevice::DDrawDevice()
{
	memset(this, 0, sizeof(DDrawDevice));
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

void DDrawDevice::DrawGrid(int32 gridSize, int32 rowCount, int32 colCount, Color color)
{
#ifdef _DEBUG
	if (mLockedBackBuffer == nullptr)
	{
		__debugbreak();
	}
#endif

	if (rowCount * gridSize >= (int32)mHeight)
	{
		return;
	}

	if (colCount * gridSize >= (int32)mWidth)
	{
		return;
	}

	uint8* pDest = mLockedBackBuffer;

	for (int32 y = 0; y <= rowCount; y++)
	{
		for (int32 x = 0; x < colCount * gridSize; x++)
		{
			*(Color*)(pDest + x * 4) = color;
		}

		pDest += mLockedBackBufferPitch * gridSize;
	}

	pDest = mLockedBackBuffer;

	for (int32 x = 0; x <= colCount; x++)
	{
		for (int32 y = 0; y < rowCount * gridSize; y++)
		{
			*(Color*)(pDest + y * mLockedBackBufferPitch) = color;
		}

		pDest += gridSize * 4;
	}
}

void DDrawDevice::DrawRect(int32 screenX, int32 screenY, int32 width, int32 height, Color color)
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
			Color* pPixel = (Color*)(mLockedBackBuffer + y * mLockedBackBufferPitch + x * 4);
			*pPixel = color;
		}
	}
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

void DDrawDevice::writeText(HDC hdc, int32 x, int32 y, Color color, const wchar_t* text, int32 length) const
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
