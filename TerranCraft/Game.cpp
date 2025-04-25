#include "pch.h"

#include <time.h>
#include "../DDrawLib/DDrawDevice.h"
#include "../ImageData/Graphic.h"
#include "../ImageData/PaletteManager.h"
#include "../BWLib/SpecialAbilityFlags.h"
#include "Game.h"
#include "Unit.h"
#include "Bullet.h"
#include "Sprite.h"
#include "Image.h"
#include "PathFinder.h"
#include "../Util/Util.h"
#include "Camera.h"
#include "AnimationController.h"
#include "ResourceManager.h"
#include "BWFile.h"
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

	mPathFinder = new PathFinder();

#pragma region Load Resources
	mResourceManager = new ResourceManager();
	mResourceManager->Load();
	mAnimationController = new AnimationController();
	mAnimationController->Load("../data/STARDAT/scripts/iscript.bin");

	mPaletteManager = new PaletteManager();
	const PCXImage* tunitPCX = mResourceManager->GetTUnitPCX();
	mPaletteManager->pcxToPaletteEntries(tunitPCX, mPaletteManager->TUnitPaletteEntries);

	const PCXImage* tselectPCX = mResourceManager->GetTSelectPCX();
	mPaletteManager->pcxToPaletteEntries(tselectPCX, mPaletteManager->TSelectPaletteEntries);

	const PCXImage* twirePCX = mResourceManager->GetTWirePCX();
	mPaletteManager->pcxToPaletteEntries(twirePCX, mPaletteManager->TWirePaletteEntries);

	mPaletteManager->LoadPals();
	//mPaletteManager->LoadPal(mPaletteManager->sBexplData, "../data/Palettes/bexpl.pal");

#pragma endregion

#pragma region CursorMarker
	{
		mCursorMarkerSprite = new Sprite();
		mCursorMarkerSprite->Initalize(eSprite::CursorMarker);
		Image* primaryImage = mCursorMarkerSprite->GetPrimaryImage();
		primaryImage->SetHidden(true);
	}
#pragma endregion

	eUnit unitTypes[4] = {
		eUnit::TerranMarine,
		eUnit::TerranBattlecruiser,
		eUnit::TerranSCV,
		eUnit::TerranVulture,
	};

	for (int32 j = 0; j < 3; j++)
	{
		for (int32 i = 0; i < 1; i++)
		{
			Unit* unit = new Unit();

			if (!unit->Initialize(unitTypes[j]))
			{
				goto LB_RETURN;
			}

			//unit->SetPosition({ 128 + i * 128.f, (j * 3 + 1) * 64.f });
			unit->SetHP(1000);
			unit->SetPosition({ 128 + i * 128.f, (j + 1) * 64.f });
			Units.push_back(unit);
			Thingies.push_back(unit);
		}
	}

	mButtonsetData = mResourceManager->GetButtonSet();

	for (int32 i = 0; i < SELECTION_CIRCLE_IMAGE_COUNT; i++)
	{
		eImage selectionCircle = (eImage)((uint16)eImage::IMGSELECT022 + i);

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

void Game::Cleanup()
{
	for (int32 i = 0; i < SELECTION_CIRCLE_IMAGE_COUNT; i++)
	{
		delete mSelectionCircleImages[i];
		mSelectionCircleImages[i] = nullptr;
	}

	delete mCursorMarkerSprite;
	mCursorMarkerSprite = nullptr;

	mAnimationController->Destroy();

	delete mBuildingPreview;
	mBuildingPreview = nullptr;

	Bullets.clear();
	Units.clear();

	for (Thingy* thingy : Thingies)
	{
		delete thingy;
	}
	Thingies.clear();

	delete mPaletteManager;
	mPaletteManager = nullptr;

	delete mResourceManager;
	mResourceManager = nullptr;

	delete mAnimationController;
	mAnimationController = nullptr;

	delete mPathFinder;
	mPathFinder = nullptr;

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
#if 0
#ifdef _DEBUG
	case 'C':
		memset(gMiniTiles, 1, (size_t)MAP_WIDTH * MAP_HEIGHT * sizeof(uint8));
		break;
	case 'W':
		cell = 0;
		break;
	case 'E':
		cell = 1;
		break;
	case 'H':
		mDDrawDevice->IsVF4On = !mDDrawDevice->IsVF4On;
		break;
	case 'P':
		mDDrawDevice->IsPathOn = !mDDrawDevice->IsPathOn;
		break;
	case 'G':
		mDDrawDevice->IsGridOn = !mDDrawDevice->IsGridOn;
		break;
	case 'B':
		mDDrawDevice->IsBoundOn = !mDDrawDevice->IsBoundOn;
		break;
	case 'D':
		bEnableDrawing = !bEnableDrawing;
		break;
#endif // _DEBUG
#endif
	case 'B':
		if (mCurrentButtonset == eButtonset::TerranStructureConstruction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(eImage::Barracks, nullptr);
			mCreatedUnitType = eUnit::TerranBarracks;
		}
		else
		{
			mCurrentButtonset = eButtonset::TerranStructureConstruction;
		}
		break;
	case 'C':
		if (mCurrentButtonset == eButtonset::TerranStructureConstruction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(eImage::CommandCenter, nullptr);
			mCreatedUnitType = eUnit::TerranCommandCenter;
		}
		break;
	case 'S':
	{
#if 0
		IntRect countourBounds = mSelectedUnits[0]->GetContourBounds();
		FloatVector2 position = mSelectedUnits[0]->GetPosition();

		IntVector2 scvPosition = { (int32)position.X - countourBounds.Left, (int32)position.Y + countourBounds.Bottom };
		Unit* scv = new Unit();
		scv->Initialize(UnitType::Terran_SCV);
		scv->SetPosition({ (float)scvPosition.X, (float)scvPosition.Y });
		mUnits.push_back(scv);
#endif

		if (mCurrentButtonset == eButtonset::TerranStructureConstruction)
		{
			delete mBuildingPreview;
			mBuildingPreview = new Image();
			mBuildingPreview->Initialize(eImage::SupplyDepot, nullptr);
			mCreatedUnitType = eUnit::TerranSupplyDepot;
		}
		else
		{
			for (int32 i = 0; i < SelectedUnits.size(); i++)
			{
				Unit* unit = SelectedUnits[i];
				unit->SetStandby();
				unit->ClearOrders();
				Order* order = new Order();
				order->OrderType = eOrder::Stop;
				unit->AddOrder(order);
			}
		}
		break;
	}
	case 'V':
		break;
	case 'A':
	{
		mPlayerOrder = ePlayerOrder::Attack;
		break;
	}
	case 'M':
	{
		mPlayerOrder = ePlayerOrder::Move;
		break;
	}
	case 'T':
	{
		for (int32 i = 0; i < SelectedUnits.size(); i++)
		{
			uint8 cooldown = SelectedUnits[i]->GetGroundWeaponCooldown();
			cooldown /= 2;
			SelectedUnits[i]->SetGroundWeaponCooldown(cooldown);
		}
		break;
	}
	// minus key
	case VK_OEM_MINUS:
	{
		int32 gameSpeed = (int32)mGameSpeed - 3;
		if (gameSpeed < (int32)eGameSpeed::Slowest)
		{
			gameSpeed = (int32)eGameSpeed::Slowest;
		}
		mGameSpeed = (eGameSpeed)gameSpeed;
		mTicksPerFrame = 1000.0f / (uint32)mGameSpeed;
		break;
	}
	case VK_OEM_PLUS:
	{
		int32 gameSpeedPlus = (int32)mGameSpeed + 3;
		if (gameSpeedPlus > (int32)eGameSpeed::Fastest)
		{
			gameSpeedPlus = (int32)eGameSpeed::Fastest;
		}
		mGameSpeed = (eGameSpeed)gameSpeedPlus;
		mTicksPerFrame = 1000.0f / (uint32)mGameSpeed;
		break;
	}
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
		Int32Vector2 cameraPosition = mCamera->GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		Int32Vector2 selectedCell = { (int32)(x / PathFinder::CELL_SIZE), (int32)(y / PathFinder::CELL_SIZE) };
		mPathFinder->SetMiniTile(selectedCell.X, selectedCell.Y, cell);
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
		Int32Vector2 cameraPosition = mCamera->GetPosition();
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

	Int32Rect bounds = mCursorBounds;
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

		uint32 specialAbilityFlags = unit->GetSpecialAbilityFlags();

		if (specialAbilityFlags & (uint32)eSpecialAbilityFlags::Building)
		{
			continue;
		}

		Sprite* sprite = unit->GetSprite();
		Int32Vector2 position = sprite->GetPosition();

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

	if (mPlayerOrder != ePlayerOrder::None)
	{
		Int32Vector2 cameraPosition = mCamera->GetPosition();
		x += cameraPosition.X;
		y += cameraPosition.Y;

		switch (mPlayerOrder)
		{
		case ePlayerOrder::Attack:
		{
			Target target;
			target.Unit = mHoveredUnit;
			target.Position = { x, y };
			attack(target);
		}
		break;
		case ePlayerOrder::Move:
		{
			Target target;
			target.Unit = mHoveredUnit;
			target.Position = { x, y };
			move(target);
			break;
		}
		case ePlayerOrder::HoldPosition:
			break;
		case ePlayerOrder::Patrol:
			break;
		case ePlayerOrder::Stop:
			break;
		case ePlayerOrder::Gather:
			break;
		case ePlayerOrder::ReturnCargo:
			break;
		case ePlayerOrder::Repair:
			break;
		case ePlayerOrder::None:
			break;
		default:
			break;
		}

		mCursorIndex = 0;
		mPlayerOrder = ePlayerOrder::None;
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
	Int32Vector2 cameraPosition = mCamera->GetPosition();
	x += cameraPosition.X;
	y += cameraPosition.Y;

	Target target = { 0, };
	target.Unit = mHoveredUnit;
	target.Position = { x, y };
	move(target);

	mCursorIndex = 0;
	mPlayerOrder = ePlayerOrder::None;
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
	primaryImage->UpdateAnim(eAnim::GndAttkInit);
}

void Game::updateWireframePalette(const Unit* unit)
{
	eUnit unitType = unit->GetUnitType();
	int32 hp = unit->GetHP();
	int32 maxHP = unit->GetMaxHP();
	int32 lostHP = maxHP - hp;
	float percent = (float)lostHP / maxHP;
	int32 temp = (int32)(percent * 8);

	PaletteManager* paletteManager = gGame->GetPaletteManager();
	PALETTEENTRY green = mPaletteManager->TWirePaletteEntries[1];
	PALETTEENTRY yellow = mPaletteManager->TWirePaletteEntries[0];
	PALETTEENTRY red = mPaletteManager->TWirePaletteEntries[10];

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
	mPaletteManager->SetEntries(mPaletteManager->Data, 208, 4, paletteEntries);
}

void Game::move(Target target)
{
	for (uint32 i = 0; i < SelectedUnits.size(); i++)
	{
		Unit* unit = SelectedUnits[i];
		unit->SetStandby();
		unit->ClearOrders();
		Order* order = new Order();
		order->OrderType = eOrder::Move;
		order->Target = target;
		unit->AddOrder(order);
	}
}

void Game::attack(Target target)
{
	for (uint32 i = 0; i < SelectedUnits.size(); i++)
	{
		Unit* unit = SelectedUnits[i];
		unit->SetStandby();
		unit->ClearOrders();
		Order* order = new Order();

		if (target.Unit != nullptr)
		{
			order->OrderType = eOrder::AttackUnit;
		}
		else
		{
			order->OrderType = eOrder::AttackMove;
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
	order->OrderType = eOrder::PlaceBuilding;
	scv->AddOrder(order);

	Unit* unit = new Unit();
	unit->Initialize(mCreatedUnitType);

	Int32Vector2 position = mBuildingPreview->GetPosition();
	unit->SetPosition({ (float)position.X, (float)position.Y });

	order = new Order();
	order->OrderType = eOrder::ConstructingBuilding;
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
		mCamera->MoveViewPort(-PathFinder::CELL_SIZE, 0);
	}

	if (mbPressedRight)
	{
		mCamera->MoveViewPort(PathFinder::CELL_SIZE, 0);
	}

	if (mbPressedUp)
	{
		mCamera->MoveViewPort(0, -PathFinder::CELL_SIZE / 2);
	}

	if (mbPressedDown)
	{
		mCamera->MoveViewPort(0, PathFinder::CELL_SIZE / 2);
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
		order->OrderType = eOrder::Die;
		unit->AddOrder(order);
	}

	for (auto iter = Thingies.begin(); iter != Thingies.end(); ++iter)
	{
		Thingy* thingy = *iter;
		Sprite* sprite = thingy->GetSprite();
		if (sprite == nullptr)
		{
			continue;
		}

		const std::list<Image*>* images = sprite->GetImages();
		// TODO: intrusive list로 변경후에 하나의 for문으로 변경해야함.
		std::list<Image*> tempImages = *images;
		for (Image* image : tempImages)
		{
			mAnimationController->UpdateImageFrame(thingy, image);
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

				Bullets.erase(std::remove(Bullets.begin(), Bullets.end(), thingy), Bullets.end());
				iter = Thingies.erase(iter);
				delete thingy;
				thingy = nullptr;
				continue;
			}

			const std::list<Image*>* images = sprite->GetImages();
			std::list<Image*> tempImages = *images;
			for (Image* image : tempImages)
			{
				image->UpdateGraphicData();
			}

			Image* selectionCircleImage = sprite->GetSelectionCircleImage();
			if (selectionCircleImage != nullptr)
			{
				selectionCircleImage->UpdateGraphicData();
			}

			++iter;
		}
	}

	if (mHoveredUnit == nullptr)
	{
		if (mPlayerOrder == ePlayerOrder::None)
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
		if (mPlayerOrder == ePlayerOrder::None)
		{
			mCursorIndex = 3;
		}
		else
		{
			mCursorIndex = 15;
		}
	}

	mCursorFrame++;

	const GRPHeader* cursorGRP = mResourceManager->GetCursorGRP(mCursorIndex);
	if (mCursorFrame >= cursorGRP->FrameCount * 3)
	{
		mCursorFrame = 0;
	}

	mHoveredUnit = nullptr;

	for (Unit* unit : Units)
	{
		Int32Rect unitBound;
		unitBound.Left = (int32)unit->GetPosition().X - unit->GetContourBounds().Left;
		unitBound.Top = (int32)unit->GetPosition().Y - unit->GetContourBounds().Top;
		unitBound.Right = (int32)unit->GetPosition().X + unit->GetContourBounds().Right;
		unitBound.Bottom = (int32)unit->GetPosition().Y + unit->GetContourBounds().Bottom;

		Int32Vector2 cursorMapPosition = { mCursorScreenPos.X, mCursorScreenPos.Y };
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
	Int32Vector2 cameraPosition = mCamera->GetPosition();
	mCursorMarkerSprite->SetPosition({ cameraPosition.X + mCursorScreenPos.X, cameraPosition.Y + mCursorScreenPos.Y });
	Image* cursorMarkerImage = mCursorMarkerSprite->GetPrimaryImage();
	mAnimationController->UpdateImageFrame(nullptr, cursorMarkerImage);
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
		const uint32* mapImage = mResourceManager->GetMapImage();
		uint32 mapImageWidth = mResourceManager->GetMapImageWidth();
		const uint8* gMiniTiles = mPathFinder->GetMiniTiles();
		mDDrawDevice->DrawMap(PathFinder::CELL_SIZE, PathFinder::MAP_WIDTH, PathFinder::MAP_HEIGHT, (const uint8*)gMiniTiles, mapImage, mapImageWidth);

#ifdef _DEBUG
		if (mDDrawDevice->IsGridOn)
		{
			mDDrawDevice->DrawGrid(PathFinder::CELL_SIZE, PathFinder::MAP_WIDTH, PathFinder::MAP_HEIGHT, 0xff000000);
		}

		if (mDDrawDevice->IsPathOn)
		{
			for (uint32 i = 0; i < SelectedUnits.size(); i++)
			{
				Unit* unit = SelectedUnits[i];
				Int32Rect countourBounds = unit->GetContourBounds();
				mDDrawDevice->DrawPath(CellPath, PathFinder::CELL_SIZE, countourBounds, 0xff00ff00);
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
				mPaletteManager->SetEntries(mPaletteManager->Data, 1, PALETTE_ENTRY_COUNT, mPaletteManager->TSelectPaletteEntries + index);
			}
			{
				int32 index = 16;
				constexpr int32 PALETTE_ENTRY_COUNT = 8;
				mPaletteManager->SetEntries(mPaletteManager->Data, 8, PALETTE_ENTRY_COUNT, mPaletteManager->TUnitPaletteEntries + index);
			}

			sprite->Draw(mDDrawDevice);

			if (sprite->IsSelected())
			{
				Image* selectionCircleImage = sprite->GetSelectionCircleImage();
				Int32Vector2 position = selectionCircleImage->GetScreenPosition();
				Int32Rect grpBound = selectionCircleImage->GetGRPBounds();
				int32 width = grpBound.Right;
				int32 height = grpBound.Bottom;

				const SpriteData* spriteData = mResourceManager->GetSpriteData();
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
			Int32Vector2 screenPosition = mBuildingPreview->GetScreenPosition();
			mDDrawDevice->DrawGRPWithBlending(screenPosition.X, screenPosition.Y, frame, compressedImage, mPaletteManager->Data);
		}

		const PCXImage* tconsolePCX = mResourceManager->GetTConsolePCX();
		if (tconsolePCX != nullptr)
		{
			Int32Vector2 size = mCamera->GetSize();
			mConsolePos.X = (size.X - tconsolePCX->Width) / 2;
			mConsolePos.Y = size.Y - tconsolePCX->Height;
			mDDrawDevice->DrawPCX(mConsolePos.X, mConsolePos.Y, tconsolePCX->Data, tconsolePCX->Width, tconsolePCX->Height, tconsolePCX->PaletteData);
		}

		const GRPHeader* buttonsGRP = mResourceManager->GetButtonsGRP();
		if (buttonsGRP != nullptr)
		{
			int32 x = mConsolePos.X + 508;
			int32 y = mConsolePos.Y + 358;

			const Buttonset* buttonset = mButtonsetData->Buttonsets[(uint32)mCurrentButtonset];

			for (uint32 i = 0; i < buttonset->ButtonCount; i++)
			{
				const ButtonInfo* buttonInfo = buttonset->ButtonInfos + i;
				uint32 condition = buttonInfo->Condition;

				//if (condition != 0x4282d0)
				//{
				//	continue;
				//}

				uint16 location = buttonInfo->Location;
				int32 frameIndex = (int32)buttonset->ButtonInfos[i].Icon;
				const GRPFrame* frame = buttonsGRP->Frames + frameIndex;
				const uint8* compressedImage = (uint8*)buttonsGRP + frame->DataOffset;

				mPaletteManager->SetEntries(mPaletteManager->Data, 1, 16, mPaletteManager->IconData);

				mDDrawDevice->DrawGRP(x + (location - 1) % 3 * 47, y + (location - 1) / 3 * 41, frame, compressedImage, mPaletteManager->Data);
			}
		}

		mDDrawDevice->DrawBound(mCursorBounds, 0xff00ff00);

#pragma region UI
		// Wireframe
		if (SelectedUnits.size() == 1)
		{
			Unit* unit = SelectedUnits[0];
			eUnit unitType = unit->GetUnitType();
			int32 index = (uint32)unitType;

			{
				const GRPHeader* wireframeGRP = mResourceManager->GetWireframeGRP(2);
				const GRPFrame* frame = wireframeGRP->Frames + index;
				uint8* compressedImage = (uint8*)wireframeGRP + frame->DataOffset;

				int32 x = mConsolePos.X + 177;
				int32 y = mConsolePos.Y + 389;

				updateWireframePalette(unit);
				mDDrawDevice->DrawGRP(x, y, frame, compressedImage, mPaletteManager->Data);
			}

			{
				int32 icons[4] = { 0, };
				int32 upgradeCount = 0;

				int32 x = mConsolePos.X + 240;
				int32 y = mConsolePos.Y + 440;

				const UnitData* unitData = mResourceManager->GetUnitData();
				const UpgradeData* upgardeData = mResourceManager->GetUpgradeData();
				const WeaponData* weaponData = mResourceManager->GetWeaponData();
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
				if (groundWeaponIndex != (int32)eWeapon::None)
				{
					int32 groundWeaponIconIndex = weaponData->Icons[groundWeaponIndex];
					icons[upgradeCount++] = groundWeaponIconIndex;
				}

				int32 airWeaponIndex = unitData->AirWeapons[index];
				if (airWeaponIndex != (int32)eWeapon::None && groundWeaponIndex != airWeaponIndex)
				{
					int32 airWeaponIconIndex = weaponData->Icons[airWeaponIndex];
					icons[upgradeCount++] = airWeaponIconIndex;
				}

				for (int32 i = 0; i < upgradeCount; i++)
				{
					if (icons[i] != 0)
					{
						if (buttonsGRP != nullptr)
						{
							const GRPFrame* frame = buttonsGRP->Frames + icons[i];
							uint8* compressedImage = (uint8*)buttonsGRP + frame->DataOffset;
							mPaletteManager->SetEntries(mPaletteManager->Data, 1, 16, mPaletteManager->IconWireData);
							mDDrawDevice->DrawGRP(x + 2, y + 2, frame, compressedImage, mPaletteManager->Data);
						}
					}

					{
						const GRPHeader* tcmdBtnsGRP = mResourceManager->GetTCmdBtnsGRP();
						const GRPFrame* frame = tcmdBtnsGRP->Frames + 12;
						uint8* compressedImage = (uint8*)tcmdBtnsGRP + frame->DataOffset;
						mDDrawDevice->DrawGRP(x, y, frame, compressedImage, mPaletteManager->Data);
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
				eUnit unitType = unit->GetUnitType();
				int32 index = (uint32)unitType;

				{
					int32 col = i / 2;
					int32 row = i % 2;

					int32 x = mConsolePos.X + 167 + 37 * col;
					int32 y = mConsolePos.Y + 396 + 37 * row;

					const GRPHeader* tcmdBtnsGRP = mResourceManager->GetTCmdBtnsGRP();
					const GRPFrame* frame = tcmdBtnsGRP->Frames + 2;
					uint8* compressedImage = (uint8*)tcmdBtnsGRP + frame->DataOffset;
					mDDrawDevice->DrawGRP(x, y, frame, compressedImage, mPaletteManager->Data);

					x += frame->Width / 2;
					y += frame->Height / 2;

					updateWireframePalette(unit);

					const GRPHeader* wireframeGRP = mResourceManager->GetWireframeGRP(0);
					const GRPFrame* wireframe = wireframeGRP->Frames + index;
					uint8* wireCompressedImage = (uint8*)wireframeGRP + wireframe->DataOffset;
					int32 wireFrameX = x - wireframeGRP->Width / 2 + wireframe->X;
					int32 wireFrameY = y - wireframeGRP->Height / 2 + wireframe->Y;
					mDDrawDevice->DrawGRP(wireFrameX, wireFrameY, wireframe, wireCompressedImage, mPaletteManager->Data);
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
				Int32Vector2 screenPosition = primaryImage->GetScreenPosition();
				Int32Rect grpBound = primaryImage->GetGRPBounds();
				mDDrawDevice->DrawGRP2(screenPosition.X, screenPosition.Y, frame, grpBound, compressedImage, mPaletteManager->Data);
			}
		}

		// Cursor
		const GRPHeader* cursorGRP = mResourceManager->GetCursorGRP(mCursorIndex);
		if (cursorGRP != nullptr)
		{
			const GRPFrame* frame = cursorGRP->Frames + mCursorFrame / 3;
			uint8* compressedImage = (uint8*)cursorGRP + frame->DataOffset;

			int32 x = mCursorScreenPos.X - cursorGRP->Width / 2 + frame->X;
			int32 y = mCursorScreenPos.Y - cursorGRP->Height / 2 + frame->Y;

			mDDrawDevice->DrawGRP(x, y, frame, compressedImage, mPaletteManager->Data);
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
