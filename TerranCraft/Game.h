#pragma once

#include "../Common/typedef.h"
#include "../ImageData/Graphic.h"
#include "../BWLib/UnitType.h"
#include "../BWLib/ImageNumber.h"
#include "CommandIcon.h"
#include <vector>
#include <list>
#include "Order.h"

class DDrawDevice;
class Image;
class Thingy;
class Unit;
class Bullet;
struct Chunk;


class Sprite;
class Image;
struct ImageData;
struct SpriteData;
struct UnitData;
struct ButtonSet;

enum class PlayerOrder
{
	Attack,
	Move,
	HoldPosition,
	Patrol,
	Stop,
	Harvest,
	ReturnCargo,
	Repair,
	None
};

enum class CursorType
{
	Arrow,
	Drag,
	Mag,
	Scroll,
	Target
};

enum class CursorTargetType
{
	Green, // Ally
	Red, // Enemy
	Yellow // Neutral
};

class Game final
{
public:
	Game();
	~Game();

	bool Initalize(HWND hWnd);
	void Cleanup();
	void Process();

	void OnKeyDown(unsigned int vkCode, unsigned int scanCode);
	void OnKeyUp(unsigned int vkCode, unsigned int scanCode);
	void OnMouseMove(unsigned int nFlags, int x, int y);
	void OnLButtonUp(unsigned int nFlags, int x, int y);
	void OnLButtonDown(unsigned int nFlags, int x, int y);
	void OnRButtonUp(unsigned int nFlags, int x, int y);
	void OnRButtonDown(unsigned int nFlags, int x, int y);
	bool OnSysKeyDown(unsigned int vkCode, unsigned int scanCode, bool bAltKeyDown);
	void OnUpdateWindowSize();
	void OnUpdateWindowPos();

private:
	void onGameFrame(ULONGLONG currentTick);
	void drawScene();

	void loadMap();
	void destroyMap();
	bool loadPCX(PCXImage** outPCXImage, const char* filepath);
	void destroyPCX(PCXImage** pcxImage);
	bool loadGRP(GRPHeader** outGRPHeader, const char* filepath);
	void destroyGRP(GRPHeader** grpHeader);
	bool loadImages();

	void move(int x, int y);
	void move(Target target);
	void attack(int x, int y);
	void attack(Target target);
	void markUnit();
	void markCursor();
	void updateWireframePalette(const Unit* unit);
	void pcxToPaletteEntries(PCXImage* pcx, PALETTEENTRY* pDest);

public:
	static std::list<Thingy*> sThingies;
	//static std::list<Sprite*> sSprites;
	static std::list<Unit*> sUnits;
	static std::list<Bullet*> sBullets;
	static std::vector<Unit*> sSelectedUnits;
	static std::list<IntVector2> sCellPath;

	// GRP Images
	enum { IMAGE_COUNT = BW::ImageNumber::None };
	static GRPHeader* sGRPFiles[IMAGE_COUNT];

private:
	DDrawDevice* mDDrawDevice = nullptr;
	HWND mhWnd = nullptr;

	enum { DEFAULT_GAME_FPS = 24 };
	uint32 mGameFPS = DEFAULT_GAME_FPS;
	float mTicksPerFrame = 1000.0f / mGameFPS;

	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevCounter;

	enum { MAX_SELECTED_UNIT_COUNT = 12 };
	Unit* mHoveredUnit = nullptr;

	// Keyboard
	bool mbPressedLeft = false;
	bool mbPressedRight = false;
	bool mbPressedUp = false;
	bool mbPressedDown = false;

	bool mbPressedShift = false;

	// Map
	uint32* mMapImage = nullptr;
	uint32 mMapImageWidth = 0;
	uint32 mMapImageHeight = 0;

	// PCX
	PCXImage* mTConsolePCX = nullptr;
	PCXImage* mTUnitPCX = nullptr;
	PCXImage* mTSelectPCX = nullptr;
	PCXImage* mTWirePCX = nullptr;

	// UI
	GRPHeader* mButtonsGRP = nullptr;
	const ButtonSet* mButtonset = nullptr;
	eButtonset mCurrentButtonset = eButtonset::None;
	GRPHeader* mTCmdBtnsGRP = nullptr;
	// wireframe
	enum { WIRE_FRAME_COUNT = 3 };
	GRPHeader* mWireframeGRPs[WIRE_FRAME_COUNT] = { 0, };

	// Cursor
	enum { CURSOR_IMAGE_COUNT = 19 };
	GRPHeader* mCursorGRPs[CURSOR_IMAGE_COUNT] = { 0, };
	int32 mCursorIndex = 0;
	int32 mCursorFrame = 0;
	IntVector2 mCursorScreenPos = { 0, };
	IntRect mCursorBounds = { 0, };
	//CursorType mCursorType = CursorType::Arrow;
	Sprite* mCursorMarkerSprite = nullptr;
	PlayerOrder mPlayerOrder = PlayerOrder::None;
	bool mbSelectable = true;
	
	Image* mBuildingPreview = nullptr;

	std::list<Order> mUnitOrders;
	enum { SELECTION_CIRCLE_IMAGE_COUNT = 10 };
	Image* mSelectionCircleImages[SELECTION_CIRCLE_IMAGE_COUNT];
};

extern Game* gGame;
