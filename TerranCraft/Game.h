#pragma once

#include "../Common/typedef.h"
#include "../ImageData/Graphic.h"
#include "../BWLib/UnitType.h"
#include "../BWLib/ImageType.h"
#include "../BWLib/ButtonsetType.h"
#include <vector>
#include <list>
#include "Order.h"

class DDrawDevice;
class Camera;
class Thingy;
class Unit;
class Bullet;
class Sprite;
class Image;
class AnimationController;
class ResourceManager;
class PaletteManager;

struct ImageData;
struct SpriteData;
struct UnitData;
struct ButtonsetData;

enum class ePlayerOrder
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

enum class eCursor
{
	Arrow,
	Drag,
	Mag,
	Scroll,
	Target
};

enum class eCursorTarget
{
	Green, // Ally
	Red, // Enemy
	Yellow // Neutral
};

enum class eGameSpeed
{
	Slowest = 6,
	Slower = 9,
	Slow = 12,
	Normal = 15,
	Fast = 18,
	Faster = 21,
	Fastest = 24
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

	Camera* GetCamera() { return mCamera; }
	AnimationController* GetAnimationController() { return mAnimationController; }
	ResourceManager* GetResourceManager() { return mResourceManager; }
	PaletteManager* GetPaletteManager() { return mPaletteManager; }

private:
	void onGameFrame(ULONGLONG currentTick);
	void drawScene();

	void move(Target target);
	void attack(Target target);
	void build();
	void markUnit();
	void markCursor();
	void updateWireframePalette(const Unit* unit);

public:
	std::list<Thingy*> Thingies;
	std::list<Unit*> Units;
	std::list<Bullet*> Bullets;
	std::vector<Unit*> SelectedUnits;
	std::list<Int32Vector2> CellPath;

private:
	DDrawDevice* mDDrawDevice = nullptr;
	HWND mhWnd = nullptr;

	Camera* mCamera = nullptr;
	AnimationController* mAnimationController = nullptr;
	ResourceManager* mResourceManager = nullptr;
	PaletteManager* mPaletteManager = nullptr;

	eGameSpeed mGameSpeed = eGameSpeed::Fastest;
	float mTicksPerFrame = 1000.0f / (uint32)mGameSpeed;

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

	// UI
	Int32Vector2 mConsolePos = { 0, };
	const ButtonsetData* mButtonset = nullptr;
	eButtonset mCurrentButtonset = eButtonset::None;

	// Cursor
	int32 mCursorIndex = 0;
	int32 mCursorFrame = 0;
	Int32Vector2 mCursorScreenPos = { 0, };
	Int32Rect mCursorBounds = { 0, };
	Sprite* mCursorMarkerSprite = nullptr;
	ePlayerOrder mPlayerOrder = ePlayerOrder::None;
	bool mbSelectable = true;
	
	Image* mBuildingPreview = nullptr;

	std::list<Order> mUnitOrders;
	enum { SELECTION_CIRCLE_IMAGE_COUNT = 10 };
	Image* mSelectionCircleImages[SELECTION_CIRCLE_IMAGE_COUNT] = { 0, };

	eUnit mCreatedUnitType = eUnit::None;
};

extern Game* gGame;
