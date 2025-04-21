#include "pch.h"

#include <time.h>
#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/Graphic.h"
#include "Game.h"
#include "Unit.h"
#include "Bullet.h"
#include "Sprite.h"
#include "Image.h"
#include "PathFinder.h"
#include "../Util/Util.h"
#include "Camera.h"
#include "Map.h"
#include "AnimationController.h"
#include "BWFile.h"
#include "Arrangement.h"
#include "../BWLib/WeaponType.h"
#include "../BWLib/UpgradeType.h"
#include "../ImageData/Palette.h"
#include "Camera.h"

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

	mCamera = new Camera();

	mDDrawDevice = new DDrawDevice();
	if (!mDDrawDevice->Initialize(hWnd))
	{
		goto LB_RETURN;
	}

#pragma region Load Resources
	Arrangement::Instance.Load();
	loadImages();
	AnimationController::Instance.Load("../data/STARDAT/scripts/iscript.bin");

	loadMap();
	loadGRP(&mButtonsGRP, "../data/STARDAT/unit/cmdbtns/cmdicons.grp");
	loadGRP(&mTCmdBtnsGRP, "../data/STARDAT/unit/cmdbtns/tcmdbtns.grp");
	loadPCX(&mTConsolePCX, "../data/STARDAT/game/tconsole.pcx");
	loadPCX(&mTUnitPCX, "../data/STARDAT/game/tunit.pcx");
	loadPCX(&mTSelectPCX, "../data/STARDAT/game/tselect.pcx");
	loadPCX(&mTWirePCX, "../data/STARDAT/game/twire.pcx");

	pcxToPaletteEntries(mTUnitPCX, Palette::sTUnitPaletteEntries);
	pcxToPaletteEntries(mTSelectPCX, Palette::sTSelectPaletteEntries);
	pcxToPaletteEntries(mTWirePCX, Palette::sTWirePaletteEntries);

	Palette::LoadPal(Palette::sOfireData, "../data/Palettes/ofire.pal");
	Palette::LoadPal(Palette::sGfireData, "../data/Palettes/gfire.pal");
	Palette::LoadPal(Palette::sBfireData, "../data/Palettes/bfire.pal");
	//Palette::LoadPal(Palette::sBexplData, "../data/Palettes/bexpl.pal");

#pragma region wirefram
	{
		char paths[WIRE_FRAME_COUNT][MAX_PATH] = {
			"../data/STARDAT/unit/wirefram/grpwire.grp", // multi selection wireframe
			"../data/STARDAT/unit/wirefram/tranwire.grp", // transport wireframe
			"../data/STARDAT/unit/wirefram/wirefram.grp" // single selection wireframe
		};

		for (int32 i = 0; i < WIRE_FRAME_COUNT; i++)
		{
			loadGRP(mWireframeGRPs + i, paths[i]);
		}
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
	int32 index = 0;
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

		loadGRP(mCursorGRPs + index, cursorFilename);
	}

	fclose(cursorListFile);
#pragma endregion

#pragma region CursorMarker
	{
		mCursorMarkerSprite = new Sprite();
		mCursorMarkerSprite->Initalize(BW::SpriteNumber::Cursor_Marker);
		Image* primaryImage = mCursorMarkerSprite->GetPrimaryImage();
		primaryImage->SetHidden(true);
	}
#pragma endregion

#pragma endregion

	BW::UnitType unitTypes[4] = {
		BW::UnitType::Terran_SCV,
		BW::UnitType::Terran_Marine,
		BW::UnitType::Terran_Vulture,
		BW::UnitType::Terran_Command_Center
	};

	for (int32 j = 0; j < 4; j++)
	{
		for (int32 i = 0; i < 6; i++)
		{
			Unit* unit = new Unit();

			if (!unit->Initialize(unitTypes[j]))
			{
				goto LB_RETURN;
			}

			unit->SetPosition({ 128 + i * 128.f, (j + 1) * 64.f });
			Units.push_back(unit);
			Thingies.push_back(unit);
		}
	}

	mButtonset = Arrangement::Instance.GetButtonSet();

	for (int32 i = 0; i < SELECTION_CIRCLE_IMAGE_COUNT; i++)
	{
		BW::ImageNumber selectionCircle = (BW::ImageNumber)((uint16)BW::ImageNumber::IMG_SELECT_022 + i);

		Image* selectionCircleImage = new Image();
		if (!selectionCircleImage->Initialize(selectionCircle, nullptr))
		{
			__debugbreak();
			delete selectionCircleImage;
			continue;
		}

		mSelectionCircleImages[i] = selectionCircleImage;
	}

	srand((unsigned int)time(nullptr));

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::pcxToPaletteEntries(PCXImage* pcx, PALETTEENTRY* pDest)
{
	PALETTEENTRY* palette = pcx->PaletteData;

	uint8* buffer = pcx->Data;
	int32 width = pcx->Width;
	int32 x = 0;

	while (x < width)
	{
		uint8 byte = *buffer++;
		if ((byte & 0xc0) == 0xc0)
		{
			int32 count = byte & 0x3f;
			uint8 index = *buffer++;
			PALETTEENTRY color = pcx->PaletteData[index];

			if (*(uint32*)&color != 0x00000000)
			{
				for (int32 i = 0; i < count; i++)
				{
					*(pDest + (x + i)) = color;
				}
			}

			x += count;
		}
		else
		{
			PALETTEENTRY color = pcx->PaletteData[byte];
			*(pDest + x) = color;
			x++;
		}
	}
}

void Game::Cleanup()
{
	for (int32 i = 0; i < SELECTION_CIRCLE_IMAGE_COUNT; i++)
	{
		delete mSelectionCircleImages[i];
		mSelectionCircleImages[i] = nullptr;
	}

	destroyGRP(&mTCmdBtnsGRP);

	for (int32 i = 0; i < WIRE_FRAME_COUNT; i++)
	{
		destroyGRP(mWireframeGRPs + i);
	}

	delete mCursorMarkerSprite;
	mCursorMarkerSprite = nullptr;

	AnimationController::Instance.Destroy();

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

	destroyGRP(&mButtonsGRP);
	destroyPCX(&mTSelectPCX);
	destroyPCX(&mTUnitPCX);
	destroyPCX(&mTConsolePCX);
	destroyPCX(&mTWirePCX);
	destroyMap();

	Bullets.clear();
	Units.clear();

	for (Thingy* thingy : Thingies)
	{
		delete thingy;
	}
	Thingies.clear();

	for (uint32 i = 0; i < mImageCount; i++)
	{
		free(GRPFiles[i]);
		GRPFiles[i] = nullptr;
	}

	delete mDDrawDevice;
	mDDrawDevice = nullptr;

	delete mCamera;
	mCamera = nullptr;

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
		if (mCurrentButtonset == eButtonset::Terran_Structure_Construction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(BW::ImageNumber::Barracks, nullptr);
			mCreatedUnitType = BW::UnitType::Terran_Barracks;
		}
		else
		{
			mCurrentButtonset = eButtonset::Terran_Structure_Construction;
		}
		break;
	case 'C':
		if (mCurrentButtonset == eButtonset::Terran_Structure_Construction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(BW::ImageNumber::Command_Center, nullptr);
			mCreatedUnitType = BW::UnitType::Terran_Command_Center;
		}
		break;
	case 'S':
	{
#if 0
		IntRect countourBounds = mSelectedUnits[0]->GetContourBounds();
		FloatVector2 position = mSelectedUnits[0]->GetPosition();

		IntVector2 scvPosition = { (int32)position.X - countourBounds.Left, (int32)position.Y + countourBounds.Bottom };
		Unit* scv = new Unit();
		scv->Initialize(BW::UnitType::Terran_SCV);
		scv->SetPosition({ (float)scvPosition.X, (float)scvPosition.Y });
		mUnits.push_back(scv);
#endif

		if (mCurrentButtonset == eButtonset::Terran_Structure_Construction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(BW::ImageNumber::Supply_Depot, nullptr);
			mCreatedUnitType = BW::UnitType::Terran_Supply_Depot;
		}
		else
		{
			for (int32 i = 0; i < SelectedUnits.size(); i++)
			{
				Unit* unit = SelectedUnits[i];
				unit->SetStandby();
				unit->ClearOrders();
				Order* order = new Order();
				order->OrderType = BW::OrderType::Stop;
				unit->AddOrder(order);
			}
		}
	}
	break;
	case 'V':
		break;
	case 'A':
	{
		mPlayerOrder = PlayerOrder::Attack;
	}
	break;
	case 'M':
	{
		mPlayerOrder = PlayerOrder::Move;
	}
	break;
	case 'T':
	{
		for (int32 i = 0; i < SelectedUnits.size(); i++)
		{
			uint8 cooldown = SelectedUnits[i]->GetGroundWeaponCooldown();
			cooldown /= 2;
			SelectedUnits[i]->SetGroundWeaponCooldown(cooldown);
		}
	}
	break;
	// delete key
	case VK_DELETE:
	{
	}
	break;
	case VK_ESCAPE:
		if (SelectedUnits.size() > 0)
		{
			mCurrentButtonset = SelectedUnits[0]->GetCurrentButtonset();
		}
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
		Camera* camera = gGame->GetCamera();
		IntVector2 cameraPosition = mCamera->GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		IntVector2 selectedCell = { (int32)(x / CELL_SIZE), (int32)(y / CELL_SIZE) };
		gMiniTiles[selectedCell.Y][selectedCell.X] = cell;
	}
#endif // _DEBUG

	mCursorScreenPos.X = x;
	mCursorScreenPos.Y = y;

	if (!mbSelectable)
	{
		return;
	}

	if (mBuildingPreview != nullptr)
	{
		IntVector2 cameraPosition = mCamera->GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;
		mBuildingPreview->SetOffsets({ x, y });
		return;
	}

	if (nFlags & MK_LBUTTON)
	{
		mCursorBounds.Left = x;
		mCursorBounds.Top = y;

		return;
	}
}

void Game::OnLButtonUp(unsigned int nFlags, int x, int y)
{
	if (!mbSelectable)
	{
		mbSelectable = true;
		return;
	}

	bDrawing = false;

	std::vector<Unit*> tempUnits;
	tempUnits.reserve(Units.size());

	if (mHoveredUnit != nullptr)
	{
		for (Unit* unit : SelectedUnits)
		{
			Sprite* sprite = unit->GetSprite();
			sprite->OnDeselected();
		}
		SelectedUnits.clear();

		Sprite* sprite = mHoveredUnit->GetSprite();
		sprite->OnSelected(0);
		SelectedUnits.push_back(mHoveredUnit);
		mCurrentButtonset = mHoveredUnit->GetCurrentButtonset();
		goto LB_RETURN;
	}

	IntRect bounds = mCursorBounds;
	bounds.Left += mCamera->GetPosition().X;
	bounds.Top += mCamera->GetPosition().Y;
	bounds.Right += mCamera->GetPosition().X;
	bounds.Bottom += mCamera->GetPosition().Y;

	for (Unit* unit : Units)
	{
		if (tempUnits.size() >= MAX_SELECTED_UNIT_COUNT)
		{
			break;
		}

		if (unit->IsBuilding())
		{
			continue;
		}

		Sprite* sprite = unit->GetSprite();
		IntVector2 position = sprite->GetPosition();

		if (IsCollisonRectVsPoint(bounds, position))
		{
			tempUnits.push_back(unit);
		}
	}

	if (tempUnits.size() > 0)
	{
		for (Unit* unit : SelectedUnits)
		{
			Sprite* sprite = unit->GetSprite();
			sprite->OnDeselected();
		}

		for (int32 i = 0; i < tempUnits.size(); i++)
		{
			Sprite* sprite = tempUnits[i]->GetSprite();
			sprite->OnSelected(i);
		}

		std::swap(SelectedUnits, tempUnits);

		// TODO: 여러 유닛 선택했을 때 버튼셋으로 수정할 것
		mCurrentButtonset = SelectedUnits[0]->GetCurrentButtonset();
	}

	//mCursorIndex = 0;

LB_RETURN:
	memset(&mCursorBounds, 0, sizeof(mCursorBounds));
}

void Game::OnLButtonDown(unsigned int nFlags, int x, int y)
{
	bDrawing = true;

	if (mPlayerOrder != PlayerOrder::None)
	{
		IntVector2 cameraPosition = mCamera->GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		switch (mPlayerOrder)
		{
		case PlayerOrder::Attack:
		{
			Target target;
			target.Unit = mHoveredUnit;
			target.Position = { x, y };
			attack(target);
		}
		break;
		case PlayerOrder::Move:
		{
			Target target;
			target.Unit = mHoveredUnit;
			target.Position = { x, y };
			move(target);
			break;
		}
		case PlayerOrder::HoldPosition:
			break;
		case PlayerOrder::Patrol:
			break;
		case PlayerOrder::Stop:
			break;
		case PlayerOrder::Harvest:
			break;
		case PlayerOrder::ReturnCargo:
			break;
		case PlayerOrder::Repair:
			break;
		case PlayerOrder::None:
			break;
		default:
			break;
		}

		mCursorIndex = 0;
		mPlayerOrder = PlayerOrder::None;
		if (mHoveredUnit != nullptr)
		{
			markUnit();
		}
		else
		{
			markCursor();
		}
	}

	if (mBuildingPreview != nullptr)
	{
		build();
		return;
	}

	// Drag
	mCursorBounds.Left = x;
	mCursorBounds.Top = y;
	mCursorBounds.Right = x;
	mCursorBounds.Bottom = y;
}

void Game::OnRButtonUp(unsigned int nFlags, int x, int y)
{
	// TODO: OnRButtonUp Implementation
}

void Game::OnRButtonDown(unsigned int nFlags, int x, int y)
{
	if (!mbSelectable)
	{
		return;
	}

#if 1
	IntVector2 cameraPosition = mCamera->GetPosition();
	x += cameraPosition.X;
	y += cameraPosition.Y;

	Target target = { 0, };
	target.Unit = mHoveredUnit;
	target.Position = { x, y };
	move(target);

	mCursorIndex = 0;
	mPlayerOrder = PlayerOrder::None;
	if (mHoveredUnit != nullptr)
	{
		markUnit();
	}
	else
	{
		markCursor();
	}
#endif
}

void Game::markCursor()
{
	Sprite* sprite = mCursorMarkerSprite;
	Image* primaryImage = sprite->GetPrimaryImage();
	primaryImage->SetHidden(false);

	BW::IScriptAnimation anim = BW::IScriptAnimation::GndAttkInit;
	primaryImage->SetAnim(anim);
	uint16 iscriptHeader = primaryImage->GetIScriptHeader();
	uint16 iscriptOffset = AnimationController::Instance.GetIScriptOffset(iscriptHeader, anim);
	primaryImage->SetIScriptOffset(iscriptOffset);
	primaryImage->SetSleep(0);
}

void Game::updateWireframePalette(const Unit* unit)
{
	BW::UnitType unitType = unit->GetUnitType();
	int32 hp = unit->GetHP();
	int32 maxHP = unit->GetMaxHP();
	int32 lostHP = maxHP - hp;
	float percent = (float)lostHP / maxHP;
	int32 temp = (int32)(percent * 8);

	PALETTEENTRY green = Palette::sTWirePaletteEntries[1];
	PALETTEENTRY yellow = Palette::sTWirePaletteEntries[0];
	PALETTEENTRY red = Palette::sTWirePaletteEntries[10];

	PALETTEENTRY paletteEntries[4] = { green, green, green, green };
	for (int32 i = 0; i < 4; i++)
	{
		uint8 wireIndex = 0;
		if (temp == 0)
		{
			break;
		}
		else if (temp >= 2)
		{
			wireIndex = 1;
			temp -= 2;
			paletteEntries[i] = red;
		}
		else
		{
			wireIndex = 0;
			temp = 0;
			paletteEntries[i] = yellow;
		}
	}
	Palette::SetEntries(Palette::sData, 208, 4, paletteEntries);
}

void Game::move(Target target)
{
	FloatVector2 targetPosition = { (float)target.Position.X, (float)target.Position.Y };
	Unit* targetUnit = target.Unit;
	if (targetUnit != nullptr)
	{
		targetPosition = targetUnit->GetPosition();
	}

	for (uint32 i = 0; i < SelectedUnits.size(); i++)
	{
		Unit* unit = SelectedUnits[i];
		unit->SetStandby();
		unit->ClearOrders();
		Order* order = new Order();
		order->OrderType = BW::OrderType::Move;
		order->Target.Position.X = (int32)targetPosition.X;
		order->Target.Position.Y = (int32)targetPosition.Y;
		unit->AddOrder(order);
	}
}

void Game::attack(Target target)
{
	FloatVector2 targetPosition = { (float)target.Position.X, (float)target.Position.Y };
	Unit* targetUnit = target.Unit;
	if (targetUnit != nullptr)
	{
		targetPosition = targetUnit->GetPosition();
	}

	for (int32 i = 0; i < SelectedUnits.size(); i++)
	{
		// turn to target
		Unit* unit = SelectedUnits[i];
		unit->SetStandby();
		unit->ClearOrders();
		Order* order = new Order();
		if (target.Unit != nullptr)
		{
			order->OrderType = BW::OrderType::AttackUnit;
		}
		else
		{
			order->OrderType = BW::OrderType::AttackMove;
		}
		order->Target = target;
		unit->AddOrder(order);
	}
}

void Game::build()
{
	Unit* scv = SelectedUnits[0];
	scv->ClearOrders();
	Order* order = new Order();
	order->OrderType = BW::OrderType::PlaceBuilding;
	scv->AddOrder(order);

	Unit* unit = new Unit();
	unit->Initialize(mCreatedUnitType);

	IntVector2 position = mBuildingPreview->GetPosition();
	unit->SetPosition({ (float)position.X, (float)position.Y });

	order = new Order();
	order->OrderType = BW::OrderType::ConstructingBuilding;
	unit->AddOrder(order);

	Units.push_back(unit);
	Thingies.push_back(unit);
	delete mBuildingPreview;
	mBuildingPreview = nullptr;
}

void Game::markUnit()
{
	mbSelectable = false;
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
#pragma region Keyboard
	if (mbPressedLeft)
	{
		mCamera->MoveViewPort(-CELL_SIZE, 0);
	}

	if (mbPressedRight)
	{
		mCamera->MoveViewPort(CELL_SIZE, 0);
	}

	if (mbPressedUp)
	{
		mCamera->MoveViewPort(0, -CELL_SIZE / 2);
	}

	if (mbPressedDown)
	{
		mCamera->MoveViewPort(0, CELL_SIZE / 2);
	}
#pragma endregion

	// Unit Order Update
	std::list<Unit*> tempUnits = Units;
	for (Unit* unit : tempUnits)
	{
		unit->Update();
	}

	// Bullet Update
	std::list<Bullet*> tempBullets = Bullets;
	for (Bullet* bullet : tempBullets)
	{
		bullet->Update();
	}

	for (auto iter = Units.begin(); iter != Units.end(); ++iter)
	{
		Unit* unit = *iter;
		int32 hp = unit->GetHP();
		if (hp > 0)
		{
			continue;
		}

		unit->SetStandby();
		unit->ClearOrders();
		Order* order = new Order();
		order->OrderType = BW::OrderType::Die;
		unit->AddOrder(order);
	}

	for (auto iter = Thingies.begin(); iter != Thingies.end(); ++iter)
	{
		Thingy* thingy = *iter;
		const Sprite* sprite = thingy->GetSprite();
		if (sprite == nullptr)
		{
			continue;
		}

		const std::list<Image*>* images = sprite->GetImages();
		// TODO: intrusive list로 변경후에 하나의 for문으로 변경해야함.
		std::list<Image*> tempImages = *images;
		for (Image* image : tempImages)
		{
			AnimationController::Instance.UpdateImageFrame(thingy, image);
		}
	}

	{
		auto iter = Thingies.begin();
		while (iter != Thingies.end())
		{
			Thingy* thingy = *iter;
			Sprite* sprite = thingy->GetSprite();
			if (sprite == nullptr)
			{
				auto it = Units.begin();
				while (it != Units.end())
				{
					Unit* unit = *it;
					if (unit == thingy)
					{
						it = Units.erase(it);
					}
					else
					{
						Target target = unit->GetOrderTarget();
						if (target.Unit == thingy)
						{
							unit->SetStandby();
							if (!unit->IsOrderQueueEmpty())
							{
								unit->RemoveOrder();
							}
						}

						target = unit->GetMoveTarget();
						if (target.Unit == thingy)
						{
							unit->SetStandby();
							if (!unit->IsOrderQueueEmpty())
							{
								unit->RemoveOrder();
							}
						}

						++it;
					}
				}

				//sUnits.erase(std::remove(sUnits.begin(), sUnits.end(), thingy), sUnits.end());
				//sSelectedUnits.erase(std::remove(sSelectedUnits.begin(), sSelectedUnits.end(), thingy), sSelectedUnits.end());
				Bullets.erase(std::remove(Bullets.begin(), Bullets.end(), thingy), Bullets.end());
				iter = Thingies.erase(iter);
				delete thingy;
				thingy = nullptr;
				continue;
			}

			const std::list<Image*>* images = sprite->GetImages();
			for (Image* image : *images)
			{
				image->UpdateGraphicData();
			}

			++iter;
		}
	}

	if (mHoveredUnit == nullptr)
	{
		if (mPlayerOrder == PlayerOrder::None)
		{
			mCursorIndex = 0;
		}
		else
		{
			mCursorIndex = 14;
		}
	}
	else
	{
		if (mPlayerOrder == PlayerOrder::None)
		{
			mCursorIndex = 3;
		}
		else
		{
			mCursorIndex = 15;
		}
	}

	mCursorFrame++;
	if (mCursorFrame >= mCursorGRPs[mCursorIndex]->FrameCount * 3)
	{
		mCursorFrame = 0;
	}

	mHoveredUnit = nullptr;

	for (Unit* unit : Units)
	{
		IntRect unitBound;
		unitBound.Left = (int32)unit->GetPosition().X - unit->GetContourBounds().Left;
		unitBound.Top = (int32)unit->GetPosition().Y - unit->GetContourBounds().Top;
		unitBound.Right = (int32)unit->GetPosition().X + unit->GetContourBounds().Right;
		unitBound.Bottom = (int32)unit->GetPosition().Y + unit->GetContourBounds().Bottom;

		IntVector2 cursorMapPosition = { mCursorScreenPos.X, mCursorScreenPos.Y };
		cursorMapPosition.X += mCamera->GetPosition().X;
		cursorMapPosition.Y += mCamera->GetPosition().Y;

		if (IsCollisonRectVsPoint(unitBound, cursorMapPosition))
		{
			mHoveredUnit = unit;
			// TODO: 아군: 3, 적군: 4, 중립: 5
			break;
		}
	}

	// Cursor Marker
	IntVector2 cameraPosition = mCamera->GetPosition();
	mCursorMarkerSprite->SetPosition({ cameraPosition.X + mCursorScreenPos.X, cameraPosition.Y + mCursorScreenPos.Y });
	Image* cursorMarkerImage = mCursorMarkerSprite->GetPrimaryImage();
	AnimationController::Instance.UpdateImageFrame(nullptr, cursorMarkerImage);
	cursorMarkerImage->UpdateGraphicData();

	if (mBuildingPreview != nullptr)
	{
		mBuildingPreview->UpdateGraphicData();
	}
}

void Game::drawScene()
{
	if (mDDrawDevice->BeginDraw())
	{
		mDDrawDevice->Clear();

#if 1
		mDDrawDevice->DrawMap(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, (const uint8*)gMiniTiles, mMapImage, mMapImageWidth);

#ifdef _DEBUG
		if (mDDrawDevice->IsGridOn)
		{
			mDDrawDevice->DrawGrid(CELL_SIZE, MAP_WIDTH, MAP_HEIGHT, 0xff000000);
		}

		if (mDDrawDevice->IsPathOn)
		{
			for (uint32 i = 0; i < SelectedUnits.size(); i++)
			{
				Unit* unit = SelectedUnits[i];
				IntRect countourBounds = unit->GetContourBounds();
				mDDrawDevice->DrawPath(CellPath, CELL_SIZE, countourBounds, 0xff00ff00);
			}
		}
#endif // _DEBUG

		for (Thingy* thingy : Thingies)
		{
			const Sprite* sprite = thingy->GetSprite();
			if (sprite == nullptr)
			{
				continue;
			}

			int32 spriteID = (int32)sprite->GetSpriteID();

			{
				int32 index = 0; // 0: green, 8: yellow, 16: red
				constexpr int32 PALETTE_ENTRY_COUNT = 7;
				PALETTEENTRY paletteEntries[PALETTE_ENTRY_COUNT] = { 0, };
				Palette::SetEntries(Palette::sData, 1, PALETTE_ENTRY_COUNT, Palette::sTSelectPaletteEntries + index);
			}
			{
				int32 index = 16;
				constexpr int32 PALETTE_ENTRY_COUNT = 8;
				Palette::SetEntries(Palette::sData, 8, PALETTE_ENTRY_COUNT, Palette::sTUnitPaletteEntries + index);
			}

			sprite->Draw(mDDrawDevice);

			if (sprite->IsSelected())
			{
				Image* selectionCircleImage = sprite->GetSelectionCircleImage();
				IntVector2 position = selectionCircleImage->GetScreenPosition();
				IntRect grpBound = selectionCircleImage->GetGRPBounds();
				int32 width = grpBound.Right;
				int32 height = grpBound.Bottom;

				const SpriteData* spriteData = Arrangement::Instance.GetSpriteData();
				int32 healthBarIndex = (int32)spriteID - (SpriteData::SPRITE_COUNT - SpriteData::HEALTH_BAR_COUNT);
				int32 healthBar = spriteData->HealthBars[healthBarIndex];
				{
					int32 hpBar = (healthBar - 1) / 3;
					if (hpBar < 6)
					{
						hpBar = 6;
					}
					int32 hpBarWidth = hpBar * 3;
					int32 x = position.X + (width - hpBarWidth) / 2;
					mDDrawDevice->DrawRect(x, position.Y + height + 10, hpBarWidth, 3, 0xffffffff);

					int32 maxHP = ((Unit*)thingy)->GetMaxHP();
					int32 unitHP = thingy->GetHP();

					hpBar = (int32)(ceilf((float)unitHP / maxHP * hpBar));
					hpBarWidth = hpBar * 3;

					mDDrawDevice->DrawRect(x, position.Y + height + 10, hpBarWidth, 3, 0xff00ff00);
				}
			}
		}

		// Building Prreview
		if (mBuildingPreview != nullptr)
		{
			const GRPFrame* frame = mBuildingPreview->GetCurrentFrame();
			const uint8* compressedImage = mBuildingPreview->GetCompressedImage();
			//const Palette* palette = mBuildingPreview->GetPalette();
			PALETTEENTRY* palette = Palette::sData;
			IntVector2 screenPosition = mBuildingPreview->GetScreenPosition();
			mDDrawDevice->DrawGRPWithBlending(screenPosition.X, screenPosition.Y, frame, compressedImage, palette);
		}

		if (mTConsolePCX != nullptr)
		{
			IntVector2 size = mCamera->GetSize();
			mConsoleX = (size.X - mTConsolePCX->Width) / 2;
			mConsoleY = size.Y - mTConsolePCX->Height;
			mDDrawDevice->DrawPCX(mConsoleX, mConsoleY, mTConsolePCX->Data, mTConsolePCX->Width, mTConsolePCX->Height, mTConsolePCX->PaletteData);
		}

		if (mButtonsGRP != nullptr)
		{
			int32 x = mConsoleX + 508;
			int32 y = mConsoleY + 358;

			auto buttonList = mButtonset->ButtonLists[(uint32)mCurrentButtonset];

			for (uint32 i = 0; i < buttonList->ButtonCount; i++)
			{
				ButtonSet::ButtonList::ButtonInfo buttonInfo = buttonList->ButtonInfos[i];
				int location = buttonInfo.Location;
				int32 frameIndex = (int32)buttonList->ButtonInfos[i].Icon;
				GRPFrame* frame = mButtonsGRP->Frames + frameIndex;
				uint8* compressedImage = (uint8*)mButtonsGRP + frame->DataOffset;

				Palette::SetEntries(Palette::sData, 1, 16, Palette::sIconData);

				mDDrawDevice->DrawGRP(x + (location - 1) % 3 * 47, y + (location - 1) / 3 * 41, frame, compressedImage, Palette::sData);
			}
		}

		mDDrawDevice->DrawBound(mCursorBounds, 0xff00ff00);

#pragma region UI
		// Wireframe
		if (SelectedUnits.size() == 1)
		{
			Unit* unit = SelectedUnits[0];
			BW::UnitType unitType = unit->GetUnitType();
			int32 index = (uint32)unitType;

			{
				GRPHeader* wireframeGRP = mWireframeGRPs[2];
				GRPFrame* frame = wireframeGRP->Frames + index;
				uint8* compressedImage = (uint8*)wireframeGRP + frame->DataOffset;

				int32 x = mConsoleX + 177;
				int32 y = mConsoleY + 389;

				updateWireframePalette(unit);
				mDDrawDevice->DrawGRP(x, y, frame, compressedImage, Palette::sData);
			}

			{
				int32 icons[4] = { 0, };
				int32 upgradeCount = 0;

				int32 x = mConsoleX + 240;
				int32 y = mConsoleY + 440;

				const UnitData* unitData = Arrangement::Instance.GetUnitData();
				const UpgradeData* upgardeData = Arrangement::Instance.GetUpgradeData();
				const WeaponData* weaponData = Arrangement::Instance.GetWeaponData();
				int32 armorUpgradeIndex = unitData->ArmorUpgrades[index];
				int32 armorIconIndex = upgardeData->Icons[armorUpgradeIndex];
				if (armorIconIndex != 0)
				{
					icons[upgradeCount++] = armorIconIndex;
				}

				if (unitData->ShieldEnables[index] != 0)
				{
					icons[upgradeCount++] = upgardeData->Icons[15]; // Protoss Plasma Shields
				}

				int32 groundWeaponIndex = unitData->GroundWeapons[index];
				if (groundWeaponIndex != (int32)BW::WeaponType::None)
				{
					int32 groundWeaponIconIndex = weaponData->Icons[groundWeaponIndex];
					icons[upgradeCount++] = groundWeaponIconIndex;
				}

				int32 airWeaponIndex = unitData->AirWeapons[index];
				if (airWeaponIndex != (int32)BW::WeaponType::None && groundWeaponIndex != airWeaponIndex)
				{
					int32 airWeaponIconIndex = weaponData->Icons[airWeaponIndex];
					icons[upgradeCount++] = airWeaponIconIndex;
				}

				for (int32 i = 0; i < upgradeCount; i++)
				{
					if (icons[i] != 0)
					{
						if (mButtonsGRP != nullptr)
						{
							GRPFrame* frame = mButtonsGRP->Frames + icons[i];
							uint8* compressedImage = (uint8*)mButtonsGRP + frame->DataOffset;
							Palette::SetEntries(Palette::sData, 1, 16, Palette::sIconWireData);
							mDDrawDevice->DrawGRP(x + 2, y + 2, frame, compressedImage, Palette::sData);
						}
					}

					{
						GRPFrame* frame = mTCmdBtnsGRP->Frames + 12;
						uint8* compressedImage = (uint8*)mTCmdBtnsGRP + frame->DataOffset;
						mDDrawDevice->DrawGRP(x, y, frame, compressedImage, Palette::sData);
					}

					x += 40;
				}
			}
		}
		else
		{
			for (int32 i = 0; i < SelectedUnits.size(); i++)
			{
				Unit* unit = SelectedUnits[i];
				BW::UnitType unitType = unit->GetUnitType();
				int32 index = (uint32)unitType;

				{
					int32 col = i / 2;
					int32 row = i % 2;

					int32 x = mConsoleX + 167 + 37 * col;
					int32 y = mConsoleY + 396 + 37 * row;

					GRPFrame* frame = mTCmdBtnsGRP->Frames + 2;
					uint8* compressedImage = (uint8*)mTCmdBtnsGRP + frame->DataOffset;
					mDDrawDevice->DrawGRP(x, y, frame, compressedImage, Palette::sData);

					x += frame->Width / 2;
					y += frame->Height / 2;

					updateWireframePalette(unit);

					GRPHeader* wireframeGRP = mWireframeGRPs[0];
					GRPFrame* wireframe = wireframeGRP->Frames + index;
					uint8* wireCompressedImage = (uint8*)wireframeGRP + wireframe->DataOffset;
					int32 wireFrameX = x - wireframeGRP->Width / 2 + wireframe->X;
					int32 wireFrameY = y - wireframeGRP->Height / 2 + wireframe->Y;
					mDDrawDevice->DrawGRP(wireFrameX, wireFrameY, wireframe, wireCompressedImage, Palette::sData);
				}
			}
		}

		// Cursor Marker
		{
			Image* primaryImage = mCursorMarkerSprite->GetPrimaryImage();
			if (!primaryImage->IsHidden())
			{
				const GRPFrame* frame = primaryImage->GetCurrentFrame();
				const uint8* compressedImage = primaryImage->GetCompressedImage();
				IntVector2 screenPosition = primaryImage->GetScreenPosition();
				IntRect grpBound = primaryImage->GetGRPBounds();
				mDDrawDevice->DrawGRP2(screenPosition.X, screenPosition.Y, frame, grpBound, compressedImage, Palette::sData);
			}
		}

		// Cursor
		if (mCursorGRPs[mCursorIndex] != nullptr)
		{
			GRPHeader* cursorGRP = mCursorGRPs[mCursorIndex];
			GRPFrame* frame = cursorGRP->Frames + mCursorFrame / 3;
			uint8* compressedImage = (uint8*)cursorGRP + frame->DataOffset;

			int32 x = mCursorScreenPos.X - cursorGRP->Width / 2 + frame->X;
			int32 y = mCursorScreenPos.Y - cursorGRP->Height / 2 + frame->Y;

			mDDrawDevice->DrawGRP(x, y, frame, compressedImage, Palette::sData);
		}
#pragma endregion

#endif

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

	fread(mtmxData, sizeof(uint16), (size_t)(width * height), fp);

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

void Game::destroyMap()
{
	delete[] mMapImage;
	mMapImage = nullptr;

	mMapImageWidth = 0;
	mMapImageHeight = 0;
}

bool Game::loadPCX(PCXImage** outPCXImage, const char* filepath)
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

	Chunk* chunk = (Chunk*)malloc(fileSize + sizeof(Chunk) - 1);
	memset(chunk, 0, fileSize + sizeof(Chunk) - 1);

	chunk->Length = fileSize;
	fread(chunk->Data, 1, fileSize, fp);

	fclose(fp);
#pragma endregion

	PCXHeader header = { 0, };
	int32 width = 0;
	int32 height = 0;

	if (chunk->Length <= sizeof(PCXHeader) + 0x300)
	{
		goto LB_RETURN;
	}

	memcpy(&header, chunk->Data, sizeof(PCXHeader));
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

	size_t pcxSize = sizeof(PCXImage) + chunk->Length - sizeof(PCXHeader) - 0x300 - 1;
	*outPCXImage = (PCXImage*)malloc(pcxSize);
	memset(*outPCXImage, 0, pcxSize);

	(*outPCXImage)->Width = (int32)header.BytesPerLine;
	(*outPCXImage)->Height = (int32)height;

	size_t dataSize = chunk->Length - sizeof(PCXHeader) - 0x300;
	memcpy((*outPCXImage)->Data, chunk->Data + sizeof(PCXHeader), dataSize);

	const uint8* paletteCurrent = chunk->Data + chunk->Length - 0x300;

	if (*(paletteCurrent - 1) != 0x0c)
	{
		goto LB_RETURN;
	}

	for (int32 i = 0; i < 256; i++)
	{
		(*outPCXImage)->PaletteData[i].peRed = paletteCurrent[3 * i];
		(*outPCXImage)->PaletteData[i].peGreen = paletteCurrent[3 * i + 1];
		(*outPCXImage)->PaletteData[i].peBlue = paletteCurrent[3 * i + 2];
		(*outPCXImage)->PaletteData[i].peFlags = 0;
	}

	free(chunk);
	chunk = nullptr;

	bResult = true;

LB_RETURN:
	return bResult;
}

void Game::destroyPCX(PCXImage** pcxImage)
{
	free(*pcxImage);
	*pcxImage = nullptr;
}

bool Game::loadGRP(GRPHeader** outGRPHeader, const char* filepath)
{
	bool bResult = false;

	FILE* fp = nullptr;
	fopen_s(&fp, filepath, "rb");

	if (fp == nullptr)
	{
		goto LB_RETURN;
	}

	fseek(fp, 0, SEEK_END);
	int32 fileSize = ftell(fp);

	rewind(fp);

	*outGRPHeader = (GRPHeader*)malloc(fileSize);
	fread(*outGRPHeader, fileSize, 1, fp);

	fclose(fp);

	bResult = true;

LB_RETURN:
	return false;
}

void Game::destroyGRP(GRPHeader** grpHeader)
{
	free(*grpHeader);
	*grpHeader = nullptr;
}

bool Game::loadImages()
{
	bool bResult = false;

#pragma region Load GRP
	{
		constexpr size_t BUFFER_SIZE = 1024;
		char buffer[BUFFER_SIZE] = { 0, };
		int index = 0;

		mImageCount = (uint32)Arrangement::Instance.GetImageCount();

		for (uint32 i = 0; i < mImageCount; i++)
		{
			const ImageData* imageData = Arrangement::Instance.GetImageData();
			const char* grpFilename = Arrangement::Instance.GetImageName(i);
			char grpListFilename[MAX_PATH] = { 0, };
			sprintf(grpListFilename, "%s/%s", "../data/STARDAT/unit/", grpFilename);

			// Load GRP files
			loadGRP(GRPFiles + i, grpListFilename);
		}
	}
#pragma endregion

	bResult = true;

	return bResult;
}