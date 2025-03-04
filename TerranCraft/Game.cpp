#include "pch.h"

#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/ImageResource.h"
#include "../ImageData/Graphic.h"
#include "Game.h"
#include "Unit.h"
#include "Sprite.h"
#include "Image.h"
#include "Map.h"
#include "PathFinder.h"
#include "../Util/Util.h"

Game* gGame = nullptr;

Game::Game()
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

	mUnit = new Unit();
	if (!mUnit->Initialize(BW::UnitType::Terran_Marine))
	{
		goto LB_RETURN;
	}

	srand((unsigned int)time(nullptr));

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::Cleanup()
{
	delete mUnit;
	mUnit = nullptr;

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
		memset(gMap, 0, (size_t)MAP_WIDTH * MAP_HEIGHT * sizeof(uint8));
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
	mUnit->SetMoveTarget({ x, y });
	FloatVector2 position = mUnit->GetPosition();

	// find path
	IntVector2 currentCell = { (int32)(position.X / CELL_SIZE), (int32)(position.Y / CELL_SIZE) };
	IntVector2 targetCell = { x / CELL_SIZE, y / CELL_SIZE };

	mCellPath.clear();
	IntRect countourBounds = mUnit->GetCountourBounds();
	int32 length = FindPathWithUnitSize(&mCellPath, (const uint8*)gMap, currentCell, targetCell, countourBounds);

	if (length <= 0)
	{
		return;
	}

	std::list<IntVector2>* path = mUnit->GetPath();
	path->clear();

	for (IntVector2& cell : mCellPath)
	{
		IntVector2 pos = { cell.X * CELL_SIZE + CELL_SIZE / 2, cell.Y * CELL_SIZE + CELL_SIZE / 2 };
		path->push_back(pos);
	}

	IntVector2 nextMovementWaypoint = path->front();
	path->pop_front();
	mUnit->SetNextMovementWaypoint(nextMovementWaypoint);

	float distanceX = nextMovementWaypoint.X - position.X;
	float distanceY = nextMovementWaypoint.Y - position.Y;

	float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
	FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
	mUnit->SetCurrentVelocity(currentVelocity);
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
	if (mUnit != nullptr)
	{
		IntVector2 nextMovementWaypoint = mUnit->GetNextMovementWaypoint();
		FloatVector2 position = mUnit->GetPosition();

		float distanceX = nextMovementWaypoint.X - position.X;
		float distanceY = nextMovementWaypoint.Y - position.Y;
		float distanceSquare = distanceX * distanceX + distanceY * distanceY;

		int32 speed = mUnit->GetCurrentSpeed();
		FloatVector2 newPosition = position;

		if (distanceSquare > speed * speed)
		{
			FloatVector2 currentVelocity = mUnit->GetCurrentVelocity();
			newPosition.X += currentVelocity.X * speed;
			newPosition.Y += currentVelocity.Y * speed;
		}
		else
		{
			newPosition.X = (float)nextMovementWaypoint.X;
			newPosition.Y = (float)nextMovementWaypoint.Y;

			std::list<IntVector2>* path = mUnit->GetPath();

			if (!path->empty())
			{
				nextMovementWaypoint = path->front();
				path->pop_front();
				mUnit->SetNextMovementWaypoint(nextMovementWaypoint);

				distanceX = nextMovementWaypoint.X - newPosition.X;
				distanceY = nextMovementWaypoint.Y - newPosition.Y;

				float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
				FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
				mUnit->SetCurrentVelocity(currentVelocity);
			}
		}

		mUnit->SetPosition(newPosition);

		Sprite* sprite = mUnit->GetSprite();
		IntVector2 spritePosition = { (int32)newPosition.X, (int32)newPosition.Y };
		sprite->SetPosition(spritePosition);

		const std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			image->Update();
		}
	}
}

void Game::drawScene()
{
	if (mDDrawDevice->BeginDraw())
	{
		mDDrawDevice->Clear();

		mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);

		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMap, 0xff000000);

		IntRect countourBounds = mUnit->GetCountourBounds();
		mDDrawDevice->DrawPath(mCellPath, CELL_SIZE, countourBounds, 0xff00ff00);

		if (mUnit != nullptr)
		{
			const Sprite* sprite = mUnit->GetSprite();

#ifdef _DEBUG
			if (sprite == nullptr)
			{
				__debugbreak();
			}
#endif // _DEBUG

			const std::list<Image*>* images = sprite->GetImages();
			for (const Image* image : *images)
			{
#ifdef _DEBUG
				if (image == nullptr)
				{
					__debugbreak();
				}
#endif // _DEBUG

				const GRPFrame* frame = image->GetCurrentFrame();
				const uint8* compressedImage = image->GetCompressedImage();
				const Palette* palette = image->GetPaletteIndex();
				IntVector2 screenPosition = image->GetScreenPosition();

				mDDrawDevice->DrawGRP(screenPosition.X, screenPosition.Y, frame, compressedImage, palette);

				FloatVector2 unitPosition = mUnit->GetPosition();
				IntRect unitBound = { (int32)unitPosition.X - countourBounds.Left, (int32)unitPosition.Y - countourBounds.Top, (int32)unitPosition.X + countourBounds.Right, (int32)unitPosition.Y + countourBounds.Bottom };
				mDDrawDevice->DrawBound(unitBound, 0xff00ff00);
			}
		}

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
