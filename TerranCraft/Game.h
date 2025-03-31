#pragma once

#include <list>

#include "../Common/typedef.h"
#include "../ImageData/Graphic.h"
#include "CommandIcon.h"
#include <vector>

class DDrawDevice;
class Image;
class Unit;
struct Chunk;

class Sprite;
struct ImageData;
struct SpriteData;
struct UnitData;

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
	bool loadPCX(const char* filepath);

private:
	DDrawDevice* mDDrawDevice = nullptr;
	HWND mhWnd = nullptr;

	enum { DEFAULT_GAME_FPS = 24 };
	uint32 mGameFPS = DEFAULT_GAME_FPS;
	float mTicksPerFrame = 1000.0f / mGameFPS;

	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevCounter;

	std::list<Unit*> mUnits;
	enum { MAX_SELECTED_UNIT_COUNT = 12 };
	std::vector<Unit*> mSelectedUnits;
	Unit* mHoveredUnit = nullptr;

	std::list<IntVector2> mCellPath;

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
	Chunk* mChunk = nullptr;
	RGBColor mPalette[256] = { 0, };
	int32 mImageWidth = 0;
	int32 mImageHeight = 0;

	// UI
	GRPHeader* mButtonsGRP;
	enum { BUTTON_COUNT = 9 };
	enum { BUTTONSET_COUNT = 250 };
	Buttonset mButtonsets[BUTTONSET_COUNT] = { 0, };
	eButtonset mCurrentButtonset = eButtonset::Marine;

	// Cursor
	enum { CURSOR_IMAGE_COUNT = 19 };
	GRPHeader* mCursorGRPs[CURSOR_IMAGE_COUNT] = { 0, };
	int32 mCursorIndex = 0;
	IntVector2 mCursorScreenPos = { 0, };
	IntRect mCursorBounds = { 0, };
	
	Image* mBuildingPreview = nullptr;

	// Test
	Unit* mTestUnit = nullptr;
	Sprite* mTestSprite = nullptr;
	Image* mTestImage = nullptr;
	int32 mTestX = 0;
	int32 mTestY = 0;
};

extern Game* gGame;
