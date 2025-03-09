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

	const char* consolePath = "../data/STARDAT/game/tconsole.pcx";

	loadPCX(consolePath);

	ImageResource::Instance.Load("../data/grplist.txt", "../data/palettelist.txt");

	Unit* unit = new Unit();
	if (!unit->Initialize(BW::UnitType::Terran_Marine))
	{
		goto LB_RETURN;
	}
	mUnits.push_back(unit);

	Unit* unit2 = new Unit();
	if (!unit2->Initialize(BW::UnitType::Terran_SCV))
	{
		goto LB_RETURN;
	}
	mUnits.push_back(unit2);

	loadMap();

#pragma region cmdicon

	FILE* fp = nullptr;
	fopen_s(&fp, "../data/STARDAT/unit/cmdbtns/cmdicons.grp", "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);

	rewind(fp);

	mButtonsGRP = (GRPHeader*)malloc(fileSize);
	fread(mButtonsGRP, fileSize, 1, fp);

	fclose(fp);

	memset(mButtonsets, 0, sizeof(mButtonsets));
	ButtonInfo button;
	Buttonset* buttonset = mButtonsets;

	// Marine
	{
		buttonset = mButtonsets + eButtonset::Marine;

		// Move
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 1;
		button.Icon = CommandIcon::Move;
		button.Enable = 664;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Stop
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 2;
		button.Icon = CommandIcon::Stop;
		button.Enable = 665;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Attack
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 3;
		button.Icon = CommandIcon::Attack;
		button.Enable = 666;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Patrol
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 4;
		button.Icon = CommandIcon::Patrol;
		button.Enable = 667;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Hold Position
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 5;
		button.Icon = CommandIcon::HoldPosition;
		button.Enable = 668;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Use Stimpack
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 7;
		button.Icon = CommandIcon::UseStimpack;
		button.Enable = 334;
		button.Disable = 346;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;
	}

	// SCV
	{
		buttonset = mButtonsets + eButtonset::SCV;

		// Move
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 1;
		button.Icon = CommandIcon::Move;
		button.Enable = 664;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Stop
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 2;
		button.Icon = CommandIcon::Stop;
		button.Enable = 665;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Attack
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 3;
		button.Icon = CommandIcon::Attack;
		button.Enable = 666;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Repair
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 4;
		button.Icon = CommandIcon::Repair;
		button.Enable = 667;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Gather
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 5;
		button.Icon = CommandIcon::Gather;
		button.Enable = 675;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Return Resoruces
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 6;
		button.Icon = CommandIcon::ReturnResource;
		button.Enable = 676;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Terran Basic Building
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 7;
		button.Icon = CommandIcon::TerranBasicBuilding;
		button.Enable = 677;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Terran Advanced Building
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 8;
		button.Icon = CommandIcon::TerranAdvancedBuilding;
		button.Enable = 678;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Cancel
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::Cancel;
		button.Enable = 669;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;
	}

	// Command Center
	{
		buttonset = mButtonsets + eButtonset::TerranCommandCenter;

		// Move
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 1;
		button.Icon = CommandIcon::Move;
		button.Enable = 664;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// SCV
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 1;
		button.Icon = CommandIcon::SCV;
		button.Enable = 592;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Stop
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 2;
		button.Icon = CommandIcon::Stop;
		button.Enable = 665;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Set Rally Point
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 6;
		button.Icon = CommandIcon::SetRallyPoint;
		button.Enable = 672;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Comsat Station
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 7;
		button.Icon = CommandIcon::ComsatStation;
		button.Enable = 658;
		button.Disable = 708;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Nuclear Silo
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 8;
		button.Icon = CommandIcon::NuclearSilo;
		button.Enable = 659;
		button.Disable = 709;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Land
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::Land;
		button.Enable = 670;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Lift Off
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::LiftOff;
		button.Enable = 671;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Cancel
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::Cancel;
		button.Enable = 693;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Cancel
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::Cancel;
		button.Enable = 694;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;
	}

	// Terran Structure Construction
	{
		buttonset = mButtonsets + eButtonset::TerranStructureConstruction;

		// Command Center
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 1;
		button.Icon = CommandIcon::CommandCenter;
		button.Enable = 646;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Supply Depot
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 2;
		button.Icon = CommandIcon::SupplyDepot;
		button.Enable = 647;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Refinery
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 3;
		button.Icon = CommandIcon::Refinery;
		button.Enable = 648;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Barracks
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 4;
		button.Icon = CommandIcon::Barracks;
		button.Enable = 649;
		button.Disable = 710;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Engineering Bay
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 5;
		button.Icon = CommandIcon::EngineeringBay;
		button.Enable = 650;
		button.Disable = 715;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Missile Tower
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 6;
		button.Icon = CommandIcon::MissileTower;
		button.Enable = 651;
		button.Disable = 717;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Academy
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 7;
		button.Icon = CommandIcon::Academy;
		button.Enable = 652;
		button.Disable = 711;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Bunker
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 8;
		button.Icon = CommandIcon::Bunker;
		button.Enable = 653;
		button.Disable = 718;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;

		// Cancel
		memset(&button, 0, sizeof(ButtonInfo));
		button.Location = 9;
		button.Icon = CommandIcon::Cancel;
		button.Enable = 688;
		button.Disable = 0;
		buttonset->ButtonList[buttonset->ButtonCount++] = button;
	}

#pragma endregion

#pragma region Cursor
	FILE* cursorListFile = nullptr;

	fopen_s(&cursorListFile, "../data/cursorlist.txt", "rb");

	if (cursorListFile == nullptr)
	{
		goto LB_RETURN;
	}

	char buffer[1024] = { 0, };
	int index = 0;
	char cursorFilename[MAX_PATH] = { 0, };

	while (fgets(buffer, 1024, cursorListFile) != nullptr)
	{
		int count = sscanf(buffer, "%d %s", &index, cursorFilename);
		if (count != 2)
		{
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG
			continue;
		}
		if (strlen(cursorFilename) == 0)
		{
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG
			continue;
		}

		FILE* cursorFile = nullptr;

		fopen_s(&cursorFile, cursorFilename, "rb");

		if (cursorFile == nullptr)
		{
#ifdef _DEBUG
			__debugbreak();
#endif // _DEBUG
			continue;
		}

		fseek(cursorFile, 0, SEEK_END);
		int32 fileSize = ftell(cursorFile);

		fseek(cursorFile, 0, SEEK_SET);

		GRPHeader* cursorGRP = (GRPHeader*)malloc(fileSize);
		fread(cursorGRP, fileSize, 1, cursorFile);

		mCursorGRPs[index] = cursorGRP;

		fclose(cursorFile);
	}

	fclose(cursorListFile);

#pragma endregion

	srand((unsigned int)time(nullptr));

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::Cleanup()
{
	for (int i = 0; i < CURSOR_IMAGE_COUNT; i++)
	{
		if (mCursorGRPs[i] != nullptr)
		{
			free(mCursorGRPs[i]);
			mCursorGRPs[i] = nullptr;
		}
	}

	delete mBuildingPreview;
	mBuildingPreview = nullptr;

	free(mButtonsGRP);
	mButtonsGRP = nullptr;

	free(mChunk);
	mChunk = nullptr;

	delete[] mMapImage;
	mMapImage = nullptr;

	for (Unit* unit : mUnits)
	{
		delete unit;
	}
	mUnits.clear();

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
bool bEnableDrawing = false;

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
	case VK_SHIFT:
		mbPressedShift = true;
		break;
#ifdef _DEBUG
		//case 'C':
		//	memset(gMiniTiles, 1, (size_t)MAP_WIDTH * MAP_HEIGHT * sizeof(uint8));
		//	break;
		//case 'W':
		//	cell = 0;
		//	break;
		//case 'E':
		//	cell = 1;
		//	break;
		//case 'H':
		//	mDDrawDevice->IsVF4On = !mDDrawDevice->IsVF4On;
		//	break;
		//case 'P':
		//	mDDrawDevice->IsPathOn = !mDDrawDevice->IsPathOn;
		//	break;
		//case 'G':
		//	mDDrawDevice->IsGridOn = !mDDrawDevice->IsGridOn;
		//	break;
		//case 'B':
		//	mDDrawDevice->IsBoundOn = !mDDrawDevice->IsBoundOn;
		//	break;
		//case 'D':
		//	bEnableDrawing = !bEnableDrawing;
		//	break;
#endif // _DEBUG
	case 'B':
		mCurrentButtonset = eButtonset::TerranStructureConstruction;
		break;
	case 'C':
		delete mBuildingPreview;
		mBuildingPreview = new Image();
		mBuildingPreview->Initialize(BW::ImageNumber::Command_Center, nullptr);
		break;
	case 'S':
	{
		IntRect countourBounds = mSelectedUnits[0]->GetCountourBounds();
		FloatVector2 position = mSelectedUnits[0]->GetPosition();

		IntVector2 scvPosition = { (int32)position.X - countourBounds.Left, (int32)position.Y + countourBounds.Bottom };
		Unit* scv = new Unit();
		scv->Initialize(BW::UnitType::Terran_SCV);
		scv->SetPosition({ (float)scvPosition.X, (float)scvPosition.Y });
		mUnits.push_back(scv);
	}
	break;
	case 'V':
		break;
	case VK_OEM_4:
		mCursorIndex--;
		break;
	case VK_OEM_6:
		mCursorIndex++;
		break;
	case VK_ESCAPE:
		mCurrentButtonset = mSelectedUnits[0]->GetCurrentButtonset();
		break;
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
	case VK_SHIFT:
		mbPressedShift = false;
		break;
	default:
		break;
	}
}

bool bDrawing = false;

void Game::OnMouseMove(unsigned int nFlags, int x, int y)
{
#ifdef _DEBUG
	if (bDrawing && bEnableDrawing)
	{
		IntVector2 cameraPosition = Camera::Instance.GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		IntVector2 selectedCell = { (int32)(x / CELL_SIZE), (int32)(y / CELL_SIZE) };
		gMiniTiles[selectedCell.Y][selectedCell.X] = cell;
	}
#endif // _DEBUG

	mCursorScreenPos.X = x;
	mCursorScreenPos.Y = y;

	if (mBuildingPreview != nullptr)
	{
		IntVector2 cameraPosition = Camera::Instance.GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;
		mBuildingPreview->SetOffsets({ x, y });
		return;
	}

	if (nFlags & MK_LBUTTON)
	{
		if (!IsCollisonRectVsPoint(mCursorBounds, mCursorScreenPos))
		{
			mCursorIndex = 0;
			mHoveredUnit = nullptr;

			mCursorBounds.Left = x;
			mCursorBounds.Top = y;
		}

		mCursorIndex = 1;
		mHoveredUnit = nullptr;
		return;
	}

	for (Unit* unit : mUnits)
	{
		IntRect unitBound;
		unitBound.Left = (int32)unit->GetPosition().X - unit->GetCountourBounds().Left;
		unitBound.Top = (int32)unit->GetPosition().Y - unit->GetCountourBounds().Top;
		unitBound.Right = (int32)unit->GetPosition().X + unit->GetCountourBounds().Right;
		unitBound.Bottom = (int32)unit->GetPosition().Y + unit->GetCountourBounds().Bottom;

		IntVector2 cursorMapPosition = { x, y };
		cursorMapPosition.X += Camera::Instance.GetPosition().X;
		cursorMapPosition.Y += Camera::Instance.GetPosition().Y;

		if (IsCollisonRectVsPoint(unitBound, cursorMapPosition))
		{
			mHoveredUnit = unit;
			// TODO: 아군: 3, 적군: 4, 중립: 5
			mCursorIndex = 3;
			return;
		}
	}

	mCursorIndex = 0;
	mHoveredUnit = nullptr;
}

void Game::OnLButtonUp(unsigned int nFlags, int x, int y)
{
	bDrawing = false;

	std::vector<Unit*> tempUnits;
	tempUnits.reserve(mUnits.size());

	if (mHoveredUnit != nullptr)
	{
		for (Unit* unit : mSelectedUnits)
		{
			Sprite* sprite = unit->GetSprite();
			sprite->SetSelectionIndex(-1);
		}

		mSelectedUnits.clear();
		Sprite* sprite = mHoveredUnit->GetSprite();
		sprite->SetSelectionIndex(0);
		mSelectedUnits.push_back(mHoveredUnit);
		goto LB_RETURN;
	}

	IntRect bounds = mCursorBounds;
	bounds.Left += Camera::Instance.GetPosition().X;
	bounds.Top += Camera::Instance.GetPosition().Y;
	bounds.Right += Camera::Instance.GetPosition().X;
	bounds.Bottom += Camera::Instance.GetPosition().Y;

	for (Unit* unit : mUnits)
	{
		Sprite* sprite = unit->GetSprite();
		IntVector2 position = sprite->GetPosition();

		if (IsCollisonRectVsPoint(bounds, position))
		{
			tempUnits.push_back(unit);
		}
	}

	if (tempUnits.size() > 0)
	{
		for (Unit* unit : mSelectedUnits)
		{
			Sprite* sprite = unit->GetSprite();
			sprite->SetSelectionIndex(-1);
		}

		for (int i = 0; i < tempUnits.size(); i++)
		{
			Sprite* sprite = tempUnits[i]->GetSprite();
			sprite->SetSelectionIndex(i);
		}

		std::swap(mSelectedUnits, tempUnits);
	}

	mCursorIndex = 0;

LB_RETURN:
	memset(&mCursorBounds, 0, sizeof(mCursorBounds));
}

void Game::OnLButtonDown(unsigned int nFlags, int x, int y)
{
	bDrawing = true;

	if (mBuildingPreview != nullptr)
	{
		IntVector2 cameraPosition = Camera::Instance.GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		Unit* unit = new Unit();
		unit->Initialize(BW::UnitType::Terran_Command_Center);

		IntVector2 position = mBuildingPreview->GetPosition();
		unit->SetPosition({ (float)position.X, (float)position.Y });
		mUnits.push_back(unit);
		delete mBuildingPreview;
		mBuildingPreview = nullptr;
		return;
	}

	mCursorBounds.Left = x;
	mCursorBounds.Top = y;
	mCursorBounds.Right = x + 1;
	mCursorBounds.Bottom = y + 1;
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

	for (uint32 i = 0; i < mSelectedUnits.size(); i++)
	{
		Unit* unit = mSelectedUnits[i];

		unit->SetMoveTarget({ x, y });
		FloatVector2 position = unit->GetPosition();

		// find path
		IntVector2 currentCell = { (int32)(position.X / CELL_SIZE), (int32)(position.Y / CELL_SIZE) };
		IntVector2 targetCell = { x / CELL_SIZE, y / CELL_SIZE };

		mCellPath.clear();
		IntRect countourBounds = unit->GetCountourBounds();
		int32 length = FindPathWithUnitSize(&mCellPath, (const uint8*)gMiniTiles, currentCell, targetCell, countourBounds);

		if (length <= 0)
		{
			return;
		}

		std::list<IntVector2>* path = unit->GetPath();
		path->clear();

		for (IntVector2& cell : mCellPath)
		{
			IntVector2 pos = { cell.X * CELL_SIZE + CELL_SIZE / 2, cell.Y * CELL_SIZE + CELL_SIZE / 2 };
			path->push_back(pos);
		}

		IntVector2 nextMovementWaypoint = path->front();
		path->pop_front();
		unit->SetNextMovementWaypoint(nextMovementWaypoint);

		float distanceX = nextMovementWaypoint.X - position.X;
		float distanceY = nextMovementWaypoint.Y - position.Y;

		float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
		FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
		unit->SetCurrentVelocity(currentVelocity);
	}
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

	for (Unit* unit : mUnits)
	{
		IntVector2 nextMovementWaypoint = unit->GetNextMovementWaypoint();
		FloatVector2 position = unit->GetPosition();

		float distanceX = nextMovementWaypoint.X - position.X;
		float distanceY = nextMovementWaypoint.Y - position.Y;
		float distanceSquare = distanceX * distanceX + distanceY * distanceY;

		int32 speed = unit->GetCurrentSpeed();
		FloatVector2 newPosition = position;

		if (distanceSquare > speed * speed)
		{
			FloatVector2 currentVelocity = unit->GetCurrentVelocity();
			newPosition.X += currentVelocity.X * speed;
			newPosition.Y += currentVelocity.Y * speed;
		}
		else
		{
			newPosition.X = (float)nextMovementWaypoint.X;
			newPosition.Y = (float)nextMovementWaypoint.Y;

			std::list<IntVector2>* path = unit->GetPath();

			if (!path->empty())
			{
				nextMovementWaypoint = path->front();
				path->pop_front();
				unit->SetNextMovementWaypoint(nextMovementWaypoint);

				distanceX = nextMovementWaypoint.X - newPosition.X;
				distanceY = nextMovementWaypoint.Y - newPosition.Y;

				float distance = sqrtf(distanceX * distanceX + distanceY * distanceY);
				FloatVector2 currentVelocity = { distanceX / distance, distanceY / distance };
				unit->SetCurrentVelocity(currentVelocity);
			}
		}

		unit->SetPosition(newPosition);

		Sprite* sprite = unit->GetSprite();
		IntVector2 spritePosition = { (int32)newPosition.X, (int32)newPosition.Y };
		sprite->SetPosition(spritePosition);

		const std::list<Image*>* images = sprite->GetImages();
		for (Image* image : *images)
		{
			image->Update();
		}
	}

	if (mBuildingPreview != nullptr)
	{
		mBuildingPreview->Update();
	}
}

void Game::drawScene()
{
	if (mDDrawDevice->BeginDraw())
	{
		mDDrawDevice->Clear();

		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMiniTiles, mMapImage, mMapImageWidth);

#ifdef _DEBUG
		if (mDDrawDevice->IsGridOn)
		{
			mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);
		}

		if (mDDrawDevice->IsPathOn)
		{
			for (uint32 i = 0; i < mSelectedUnits.size(); i++)
			{
				Unit* unit = mSelectedUnits[i];
				IntRect countourBounds = unit->GetCountourBounds();
				mDDrawDevice->DrawPath(mCellPath, CELL_SIZE, countourBounds, 0xff00ff00);
			}
		}
#endif // _DEBUG

		for (Unit* unit : mUnits)
		{

			if (unit != nullptr)
			{
				const Sprite* sprite = unit->GetSprite();

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


					if (image->GetImageID() == BW::ImageNumber::IMG_SELECT_022)
					{
						if (sprite->GetSelectionIndex() != -1)
						{
							const Palette* pal = ImageResource::Instance.GetPalette(5);
							mDDrawDevice->DrawGRP(screenPosition.X, screenPosition.Y, frame, compressedImage, pal);
						}

						continue;
					}

					mDDrawDevice->DrawGRP(screenPosition.X, screenPosition.Y, frame, compressedImage, palette);
#ifdef _DEBUG
					if (mDDrawDevice->IsBoundOn)
					{
						FloatVector2 unitPosition = unit->GetPosition();
						IntRect countourBounds = unit->GetCountourBounds();
						IntRect unitBound = { (int32)unitPosition.X - countourBounds.Left, (int32)unitPosition.Y - countourBounds.Top, (int32)unitPosition.X + countourBounds.Right, (int32)unitPosition.Y + countourBounds.Bottom };
						unitBound.Left -= Camera::Instance.GetPosition().X;
						unitBound.Right -= Camera::Instance.GetPosition().X;
						unitBound.Top -= Camera::Instance.GetPosition().Y;
						unitBound.Bottom -= Camera::Instance.GetPosition().Y;
						mDDrawDevice->DrawBound(unitBound, 0xff00ff00);
					}
#endif // _DEBUG
				}
			}
		}

		// Building Prreview
		if (mBuildingPreview != nullptr)
		{
			const GRPFrame* frame = mBuildingPreview->GetCurrentFrame();
			const uint8* compressedImage = mBuildingPreview->GetCompressedImage();
			const Palette* palette = mBuildingPreview->GetPaletteIndex();
			IntVector2 screenPosition = mBuildingPreview->GetScreenPosition();
			mDDrawDevice->DrawGRPWithBlending(screenPosition.X, screenPosition.Y, frame, compressedImage, palette);
		}

		if (mChunk != nullptr)
		{
			mDDrawDevice->DrawPCX(0, 0, mChunk, mImageWidth, mImageHeight, mPalette);
		}

		if (mButtonsGRP != nullptr)
		{
			int x = 508;
			int y = 358;

			for (int32 i = 0; i < mButtonsets[mCurrentButtonset].ButtonCount; i++)
			{
				ButtonInfo buttonInfo = mButtonsets[mCurrentButtonset].ButtonList[i];
				int location = buttonInfo.Location;
				int32 frameIndex = (int32)mButtonsets[mCurrentButtonset].ButtonList[i].Icon;
				GRPFrame* frame = mButtonsGRP->Frames + frameIndex;
				uint8* compressedImage = (uint8*)mButtonsGRP + frame->DataOffset;
				mDDrawDevice->DrawGRP(x + (location - 1) % 3 * 47, y + (location - 1) / 3 * 41, frame, compressedImage, ImageResource::Instance.GetPalette(5));
			}
		}

		mDDrawDevice->DrawBound(mCursorBounds, 0xff00ff00);

		// Cursor
		if (mCursorGRPs[mCursorIndex] != nullptr)
		{
			GRPFrame* frame = mCursorGRPs[mCursorIndex]->Frames;
			uint8* compressedImage = (uint8*)mCursorGRPs[mCursorIndex] + frame->DataOffset;

			int32 x = mCursorScreenPos.X;
			int32 y = mCursorScreenPos.Y;

			if (mCursorIndex != 0)
			{
				x -= frame->Width / 2;
				y -= frame->Height / 2;
			}

			mDDrawDevice->DrawGRP(x, y, frame, compressedImage, ImageResource::Instance.GetPalette(5));
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

void Game::loadMap()
{
#pragma region Read Files
	FILE* fp = nullptr;

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.CV5", "rb");
	if (fp == nullptr)
	{
		return;
	}
	CV5* cv5 = new CV5;
	memset(cv5, 0, sizeof(CV5));
	fread(cv5, sizeof(CV5), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VF4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VF4* vf4 = new VF4;
	memset(vf4, 0, sizeof(VF4));
	fread(vf4, sizeof(VF4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VX4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VX4* vx4 = new VX4;
	memset(vx4, 0, sizeof(VX4));
	fread(vx4, sizeof(VX4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.VR4", "rb");
	if (fp == nullptr)
	{
		return;
	}
	VR4* vr4 = new VR4;
	memset(vr4, 0, sizeof(VR4));
	fread(vr4, sizeof(VR4), 1, fp);
	fclose(fp);

	fopen_s(&fp, "../data/STARDAT/tileset/jungle.WPE", "rb");
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

bool Game::loadPCX(const char* filepath)
{
	bool bResult = false;

#pragma region Read File
	FILE* fp = nullptr;
	fopen_s(&fp, filepath, "rb");
	if (fp == nullptr)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);
	rewind(fp);

	free(mChunk);
	mChunk = (Chunk*)malloc(fileSize + sizeof(Chunk) - 1);
	memset(mChunk, 0, fileSize + sizeof(Chunk) - 1);

	mChunk->Type[0] = 'P';
	mChunk->Type[1] = 'C';
	mChunk->Type[2] = 'X';
	mChunk->Type[3] = ' ';
	mChunk->Length = fileSize;
	fread(mChunk->Data, 1, fileSize, fp);

	fclose(fp);
#pragma endregion

	PCXHeader header = { 0, };
	int32 width = 0;
	int32 height = 0;

	if (mChunk->Length < sizeof(PCXHeader) + 768 + 1)
	{
		goto LB_RETURN;
	}

	memcpy(&header, mChunk->Data, sizeof(PCXHeader));
	width = header.XMax - header.XMin + 1;
	height = header.YMax - header.YMin + 1;

	if (header.Manufacturer != 0x0a ||
		header.Version != 5 ||
		header.Encoding != 1 ||
		header.BitsPerPixel != 8 ||
		header.PlaneCount != 1 ||
		width < 0 ||
		height < 0 ||
		header.BytesPerLine & 0x01 ||
		header.BytesPerLine < width)
	{
		goto LB_RETURN;
	}


	mImageWidth = header.BytesPerLine;
	mImageHeight = height;

	const uint8* paletteCurrent = mChunk->Data + mChunk->Length - 768;

	if (*(paletteCurrent - 1) != 0x0c)
	{
		goto LB_RETURN;
	}

	for (int32 i = 0; i < 256; i++)
	{
		mPalette[i].R = paletteCurrent[3 * i];
		mPalette[i].G = paletteCurrent[3 * i + 1];
		mPalette[i].B = paletteCurrent[3 * i + 2];
		mPalette[i].Reserved = 0;
	}

	bResult = true;

LB_RETURN:
	return bResult;
}
