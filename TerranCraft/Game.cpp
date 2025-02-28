#include "pch.h"

#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Game.h"
#include "Image.h"
#include "Map.h"
#include "PathFinder.h"

Game* gGame = nullptr;

Game::Game()
	: mDDrawDevice(nullptr)
	, mhWnd(nullptr)
	, mGameFPS(DEFAULT_GAME_FPS)
	, mTicksPerFrame(1000.0f / mGameFPS)
	, mImage(nullptr)
{
	QueryPerformanceFrequency(&mFrequency);
	QueryPerformanceCounter(&mPrevCounter);
}

Game::~Game()
{
	Cleanup();
}

bool Game::Initalize(HWND hWnd)
{
	bool bResult = false;

	mhWnd = hWnd;

	mDDrawDevice = new DDrawDevice();
	if (!mDDrawDevice->Initialize(hWnd))
	{
		goto LB_RETURN;
	}

	ImageResource::Instance.Load("../data/grplist.txt", "../data/palettelist.txt");

	mImage = new Image(BW::ImageNumber::Marine);

	srand((unsigned int)time(nullptr));

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::Cleanup()
{
	delete mImage;
	mImage = nullptr;

	ImageResource::Instance.Destroy();

	delete mDDrawDevice;
	mDDrawDevice = nullptr;

	mhWnd = nullptr;
}

void Game::Process()
{
#ifdef _DEBUG
	if (mFrequency.QuadPart == 0)
	{
		__debugbreak();
	}
#endif

	LARGE_INTEGER currentCounter = { 0, };
	QueryPerformanceCounter(&currentCounter);
	float elapsedTick = (currentCounter.QuadPart - mPrevCounter.QuadPart) / (float)mFrequency.QuadPart * 1000.0f;

	if (elapsedTick > mTicksPerFrame)
	{
		ULONGLONG currentTick = GetTickCount64();
		onGameFrame(currentTick);
		mPrevCounter = currentCounter;
	}

	drawScene();
}

uint8 cell = 1;

void Game::OnKeyDown(unsigned int vkCode, unsigned int scanCode)
{
	switch (vkCode)
	{
	case 'C':
		memset(gMap, 0, MAP_WIDTH * MAP_HEIGHT * sizeof(uint8));
		break;
	case 'W':
		cell = 1;
		break;
	case 'E':
		cell = 0;
		break;
	default:
		break;
	}
}

void Game::OnKeyUp(unsigned int vkCode, unsigned int scanCode)
{
	// TODO: OnKeyUp Implementation
}

void Game::OnRButtonUp(unsigned int nFlags, int x, int y)
{
	// TODO: OnRButtonUp Implementation
}

bool bDrawing = false;

void Game::OnMouseMove(unsigned int nFlags, int x, int y)
{
	if (bDrawing)
	{
		IntVector2 selectedCell = { (int32)(x / CELL_SIZE), (int32)(y / CELL_SIZE) };
		gMap[selectedCell.Y][selectedCell.X] = cell;
	}
}

void Game::OnLButtonUp(unsigned int nFlags, int x, int y)
{
	bDrawing = false;
}

void Game::OnLButtonDown(unsigned int nFlags, int x, int y)
{
	bDrawing = true;
}

void Game::OnRButtonDown(unsigned int nFlags, int x, int y)
{
	wchar_t text[256] = { 0, };
	swprintf(text, L"Mouse Click: %d, %d\n", x, y);
	OutputDebugString(text);

	mMousePos.X = x;
	mMousePos.Y = y;

	// find path
	IntVector2 currentCell = { (int32)(mImagePos.X / CELL_SIZE), (int32)(mImagePos.Y / CELL_SIZE) };
	IntVector2 targetCell = { (int32)(mMousePos.X / CELL_SIZE), (int32)(mMousePos.Y / CELL_SIZE) };
	std::list<IntVector2> cellPath;

	int distance = FindPath(&cellPath, (const uint8*)gMap, currentCell, targetCell);

	if (distance <= 0)
	{
		return;
	}

	mPath.clear();

	for (IntVector2& cell : cellPath)
	{
		FloatVector2 pos = { (float)(cell.X * CELL_SIZE + CELL_SIZE / 2), (float)(cell.Y * CELL_SIZE + CELL_SIZE / 2) };
		mPath.push_back(pos);
	}

	mNextPos = mPath.front();
	mPath.pop_front();

	mDirectionVector.X = mNextPos.X - mImagePos.X;
	mDirectionVector.Y = mNextPos.Y - mImagePos.Y;

	float d = sqrtf((float)(mDirectionVector.X * mDirectionVector.X + mDirectionVector.Y * mDirectionVector.Y));
	mDirectionVector.X = mDirectionVector.X / d;
	mDirectionVector.Y = mDirectionVector.Y / d;
}

bool Game::OnSysKeyDown(unsigned int vkCode, unsigned int scanCode, bool bAltKeyDown)
{
	// TODO: OnSysKeyDown Implementation
	return false;
}

void Game::OnUpdateWindowSize()
{
	if (mDDrawDevice != nullptr)
	{
		mDDrawDevice->OnUpdateWindowSize();
	}
}

void Game::OnUpdateWindowPos()
{
	if (mDDrawDevice != nullptr)
	{
		mDDrawDevice->OnUpdateWindowPos();
	}
}

void Game::onGameFrame(ULONGLONG currentTick)
{
	float dSquare = (mNextPos.X - mImagePos.X) * (mNextPos.X - mImagePos.X) + (mNextPos.Y - mImagePos.Y) * (mNextPos.Y - mImagePos.Y);

	float speed = 4.0f;

	if (dSquare > speed * speed)
	{
		mImagePos.X += mDirectionVector.X * speed;
		mImagePos.Y += mDirectionVector.Y * speed;
	}
	else
	{
		mImagePos.X = mNextPos.X;
		mImagePos.Y = mNextPos.Y;

		if (!mPath.empty())
		{
			mNextPos = mPath.front();
			mPath.pop_front();

			mDirectionVector.X = mNextPos.X - mImagePos.X;
			mDirectionVector.Y = mNextPos.Y - mImagePos.Y;

			float d = sqrtf((float)(mDirectionVector.X * mDirectionVector.X + mDirectionVector.Y * mDirectionVector.Y));
			mDirectionVector.X = mDirectionVector.X / d;
			mDirectionVector.Y = mDirectionVector.Y / d;
		}
	}
}

void Game::drawScene()
{
	if (mDDrawDevice->BeginDraw())
	{
		mDDrawDevice->Clear();

		mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);
		//mDDrawDevice->DrawRect(8, 8, 16, 16, 0xffff0000);

		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMap, 0xff000000);

		const GraphicFrame* frame = mImage->GetFrame();
		const uint8* compressedImage = mImage->GetCompressedImage();
		const Palette* palette = mImage->GetPalette();

		mDDrawDevice->DrawGRP(mImagePos.X, mImagePos.Y, frame, compressedImage, palette, false, true);
		mDDrawDevice->DrawBound(mImagePos.X - mUnitSize.Left, mImagePos.Y - mUnitSize.Top, mImagePos.X + mUnitSize.Right, mImagePos.Y + mUnitSize.Bottom, 0xff00ff00);

		mDDrawDevice->EndDraw();
	}

	HDC hdc = nullptr;
	if (mDDrawDevice->BeginGDI(&hdc))
	{
		mDDrawDevice->DrawInfo(hdc);
		mDDrawDevice->EndGDI(hdc);
	}

	mDDrawDevice->OnDraw();
	mDDrawDevice->CheckFPS();
}
