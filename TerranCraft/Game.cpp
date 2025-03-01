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
#include "../Util/Util.h"

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
		break;
	default:
		break;
	}
}

void Game::OnKeyUp(unsigned int vkCode, unsigned int scanCode)
{
	// TODO: OnKeyUp Implementation
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

void Game::OnRButtonUp(unsigned int nFlags, int x, int y)
{
	// TODO: OnRButtonUp Implementation
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

	cellPath.clear();
	//int distance = FindPath(&cellPath, (const uint8*)gMap, currentCell, targetCell);
	int distance = FindPathWithUnitSize(&cellPath, (const uint8*)gMap, currentCell, targetCell, mUnitSize);

	if (distance <= 0)
	{
		return;
	}

#if 0
	bool bCollide = false;
	for (const IntVector2& cell : cellPath)
	{
		IntVector2 unitPos = { cell.X * CELL_SIZE + CELL_SIZE / 2, cell.Y * CELL_SIZE + CELL_SIZE / 2 };
		IntRect unitBound = { unitPos.X - mUnitSize.Left, unitPos.Y - mUnitSize.Top, unitPos.X + mUnitSize.Right, unitPos.Y + mUnitSize.Bottom };
		IntRect uintCellBound = { unitBound.Left / CELL_SIZE, unitBound.Top / CELL_SIZE, unitBound.Right / CELL_SIZE, unitBound.Bottom / CELL_SIZE };

		for (int y = uintCellBound.Top; y <= uintCellBound.Bottom; y++)
		{
			for (int x = uintCellBound.Left; x <= uintCellBound.Right; x++)
			{
				if (gMap[y][x] != 0)
				{
					//cellPath.clear();
					bCollide = true;
					goto LB_LOOP;
				}
			}
		}
	}

LB_LOOP:

	if (bCollide)
	{
		return;
	}
#endif

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

	FloatVector2 newImagePos = mImagePos;

	if (dSquare > speed * speed)
	{
		newImagePos.X += mDirectionVector.X * speed;
		newImagePos.Y += mDirectionVector.Y * speed;
	}
	else
	{
		newImagePos.X = mNextPos.X;
		newImagePos.Y = mNextPos.Y;

		if (!mPath.empty())
		{
			mNextPos = mPath.front();
			mPath.pop_front();

			mDirectionVector.X = mNextPos.X - newImagePos.X;
			mDirectionVector.Y = mNextPos.Y - newImagePos.Y;

			float d = sqrtf((float)(mDirectionVector.X * mDirectionVector.X + mDirectionVector.Y * mDirectionVector.Y));
			mDirectionVector.X = mDirectionVector.X / d;
			mDirectionVector.Y = mDirectionVector.Y / d;
		}
	}

	mImagePos.X = newImagePos.X;
	mImagePos.Y = newImagePos.Y;

#if 0
#pragma region check collision
	bool bCollided = false;

	IntRect unitRect = { (int32)mImagePos.X - mUnitSize.Left, (int32)mImagePos.Y - mUnitSize.Top, (int32)mImagePos.X + mUnitSize.Right, (int32)mImagePos.Y + mUnitSize.Bottom };
	IntRect unitCellRect = { unitRect.Left / CELL_SIZE, unitRect.Top / CELL_SIZE, unitRect.Right / CELL_SIZE, unitRect.Bottom / CELL_SIZE };

	for (int y = unitCellRect.Top; y <= unitCellRect.Bottom; y++)
	{
		for (int x = unitCellRect.Left; x <= unitCellRect.Right; x++)
		{
			if (gMap[y][x] != 0)
			{
				bCollided = true;
				goto LB_COLLIDED;
			}
		}
	}

LB_COLLIDED:

	if (!bCollided)
	{
		mImagePos.X = newImagePos.X;
		mImagePos.Y = newImagePos.Y;

		OutputDebugString(L"Move\n");
	}
	else
	{
		mPath.clear();
		mNextPos = mImagePos;
		mDirectionVector = { 0, 0 };

		OutputDebugString(L"Collided\n");
	}

#pragma endregion
#endif
}

void Game::drawScene()
{
	if (mDDrawDevice->BeginDraw())
	{
		mDDrawDevice->Clear();

		mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);

		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMap, 0xff000000);

		mDDrawDevice->DrawPath(cellPath, CELL_SIZE, 0xffff0000);
		mDDrawDevice->DrawPathWithUnitSize(cellPath, CELL_SIZE, mUnitSize, 0xff00ff00);

		const GraphicFrame* frame = mImage->GetFrame();
		const uint8* compressedImage = mImage->GetCompressedImage();
		const Palette* palette = mImage->GetPalette();

		mDDrawDevice->DrawGRP(mImagePos.X, mImagePos.Y, frame, compressedImage, palette, false, true);

		IntRect unitBound = { (int32)mImagePos.X - mUnitSize.Left, (int32)mImagePos.Y - mUnitSize.Top, (int32)mImagePos.X + mUnitSize.Right, (int32)mImagePos.Y + mUnitSize.Bottom };
		mDDrawDevice->DrawBound(unitBound, 0xff00ff00);

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
