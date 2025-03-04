#pragma once

#include <list>

#include "../Common/typedef.h"

class DDrawDevice;
class Image;
class Unit;

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

private:
	DDrawDevice* mDDrawDevice = nullptr;
	HWND mhWnd = nullptr;

	enum { DEFAULT_GAME_FPS = 24 };
	uint32 mGameFPS = DEFAULT_GAME_FPS;
	float mTicksPerFrame = 1000.0f / mGameFPS;

	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevCounter;

	Unit* mUnit = nullptr;

	std::list<IntVector2> mCellPath;
};

extern Game* gGame;
