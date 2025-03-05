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
#include "PathFinder.h"
#include "../Util/Util.h"
#include "Camera.h"
#include "Map.h"

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

	readMap();

	srand((unsigned int)time(nullptr));

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::Cleanup()
{
	delete[] mMapImage;

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

uint8 cell = 0;

void Game::OnKeyDown(unsigned int vkCode, unsigned int scanCode)
{
	switch (vkCode)
	{
	case VK_LEFT:
		mbPressedLeft = true;
		break;
	case VK_RIGHT:
		mbPressedRight = true;
		break;
	case VK_UP:
		mbPressedUp = true;
		break;
	case VK_DOWN:
		mbPressedDown = true;
		break;
	case 'C':
		memset(gMiniTiles, 1, (size_t)MAP_WIDTH * MAP_HEIGHT * sizeof(uint8));
		break;
	case 'W':
		cell = 0;
		break;
	case 'E':
		cell = 1;
		break;
#ifdef _DEBUG
	case 'H':
		mDDrawDevice->IsVF4On = !mDDrawDevice->IsVF4On;
		break;
	case 'P':
		mDDrawDevice->IsPathOn = !mDDrawDevice->IsPathOn;
		break;
	case 'G':
		mDDrawDevice->IsGridOn = !mDDrawDevice->IsGridOn;
		break;
#endif // _DEBUG
	default:
		break;
	}
}

void Game::OnKeyUp(unsigned int vkCode, unsigned int scanCode)
{
	switch (vkCode)
	{
	case VK_LEFT:
		mbPressedLeft = false;
		break;
	case VK_RIGHT:
		mbPressedRight = false;
		break;
	case VK_UP:
		mbPressedUp = false;
		break;
	case VK_DOWN:
		mbPressedDown = false;
		break;
	default:
		break;
	}
}

bool bDrawing = false;

void Game::OnMouseMove(unsigned int nFlags, int x, int y)
{
	if (bDrawing)
	{
		IntVector2 cameraPosition = Camera::Instance.GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		IntVector2 selectedCell = { (int32)(x / CELL_SIZE), (int32)(y / CELL_SIZE) };
		gMiniTiles[selectedCell.Y][selectedCell.X] = cell;
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
	IntVector2 cameraPosition = Camera::Instance.GetPosition();
	x += cameraPosition.X;
	y += cameraPosition.Y;

	mUnit->SetMoveTarget({ x, y });
	FloatVector2 position = mUnit->GetPosition();

	// find path
	IntVector2 currentCell = { (int32)(position.X / CELL_SIZE), (int32)(position.Y / CELL_SIZE) };
	IntVector2 targetCell = { x / CELL_SIZE, y / CELL_SIZE };

	mCellPath.clear();
	IntRect countourBounds = mUnit->GetCountourBounds();
	int32 length = FindPathWithUnitSize(&mCellPath, (const uint8*)gMiniTiles, currentCell, targetCell, countourBounds);

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
	// Keyboard
	if (mbPressedLeft)
	{
		Camera::Instance.MoveViewPort(-CELL_SIZE, 0);
	}
	if (mbPressedRight)
	{
		Camera::Instance.MoveViewPort(CELL_SIZE, 0);
	}
	if (mbPressedUp)
	{
		Camera::Instance.MoveViewPort(0, -CELL_SIZE / 2);
	}
	if (mbPressedDown)
	{
		Camera::Instance.MoveViewPort(0, CELL_SIZE / 2);
	}

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

		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMiniTiles, mMapImage, mMapImageWidth);

		IntRect countourBounds = mUnit->GetCountourBounds();

#ifdef _DEBUG
		if (mDDrawDevice->IsGridOn)
		{
			mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);
		}

		if (mDDrawDevice->IsPathOn)
		{
			mDDrawDevice->DrawPath(mCellPath, CELL_SIZE, countourBounds, 0xff00ff00);
		}
#endif // _DEBUG

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
				unitBound.Left -= Camera::Instance.GetPosition().X;
				unitBound.Right -= Camera::Instance.GetPosition().X;
				unitBound.Top -= Camera::Instance.GetPosition().Y;
				unitBound.Bottom -= Camera::Instance.GetPosition().Y;
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

void Game::readMap()
{
#pragma region Read Files
	FILE* fp = nullptr;

	fopen_s(&fp, "../data/jungle.CV5", "rb");
	if (fp == nullptr)
	{
		return;
	}
	CV5* cv5 = new CV5;
	memset(cv5, 0, sizeof(CV5));
	fread(cv5, sizeof(CV5), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/jungle.VF4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VF4* vf4 = new VF4;
	memset(vf4, 0, sizeof(VF4));
	fread(vf4, sizeof(VF4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/jungle.VX4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VX4* vx4 = new VX4;
	memset(vx4, 0, sizeof(VX4));
	fread(vx4, sizeof(VX4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/jungle.VR4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VR4* vr4 = new VR4;
	memset(vr4, 0, sizeof(VR4));
	fread(vr4, sizeof(VR4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/jungle.WPE", "rb");
	if (fp == nullptr)
	{
		return;
	}
	WPE* wpe = new WPE;
	memset(wpe, 0, sizeof(WPE));
	fread(wpe, sizeof(WPE), 1, fp);
	fclose(fp);

	const int32 width = 128;
	const int32 height = 128;
	uint16* mtmxData = new uint16[width * height];
	memset(mtmxData, 0, sizeof(uint16) * width * height);
	fopen_s(&fp, "../data/MTXM", "rb");

	if (fp == nullptr)
	{
		return;
	}

	fread(mtmxData, sizeof(uint16), width * height, fp);

	fclose(fp);
#pragma endregion

	mMapImageWidth = width * 32;
	mMapImageHeight = height * 32;
	mMapImage = new uint32[mMapImageWidth * mMapImageHeight];

	for (int32 y = 0; y < height; y++)
	{
		for (int32 x = 0; x < width; x++)
		{
			int32 group = mtmxData[y * width + x] >> 4;
			int32 index = mtmxData[y * width + x] & 0x0f;
			int32 megatile = cv5->Group[group].MegatileIndex[index];

			for (int32 subY = 0; subY < 4; subY++)
			{
				for (int32 subX = 0; subX < 4; subX++)
				{
					int minitileIndex = vx4->Data[megatile].VR4Index[subY * 4 + subX] >> 1; // 상위 15비트는 이미지
					bool flipped = vx4->Data[megatile].VR4Index[subY * 4 + subX] & 0x01; // 하위 1비트는 좌우 반전 여부

					gMiniTiles[y * 4 + subY][x * 4 + subX] = (uint8)vf4->Data[megatile].Flags[subY * 4 + subX];

					const VR4Data* vr4Data = vr4->Image + minitileIndex;

					const int32 offsetX = x * 32 + subX * 8;
					const int32 offsetY = y * 32 + subY * 8;

					for (int32 j = 0; j < 8; j++)
					{
						for (int32 i = 0; i < 8; i++)
						{
							int32 drawX = offsetX + (flipped ? 7 - i : i);
							int32 drawY = offsetY + j;
							const WPEData* wpeData = wpe->Data + vr4Data->Color[j * 8 + i];
							uint32 color = 0xff000000 | wpeData->R << 16 | wpeData->G << 8 | wpeData->B;
							mMapImage[drawY * mMapImageWidth + drawX] = color;
						}
					}

				}
			}
		}
	}

	delete[] mtmxData;
	delete wpe;
	delete vr4;
	delete vx4;
	delete vf4;
	delete cv5;
}
