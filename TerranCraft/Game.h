#pragma once

#include <list>

#include "../Common/typedef.h"

class DDrawDevice;
class Image;

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
	DDrawDevice* mDDrawDevice;
	HWND mhWnd;

	enum { DEFAULT_GAME_FPS = 24 };
	uint32 mGameFPS;
	float mTicksPerFrame;

	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mPrevCounter;

	Image* mImage;

	FloatVector2 mImagePos = { 16, 16 };
	FloatVector2 mMousePos = { 0, 0 };

	std::list<IntVector2> cellPath;
	std::list<FloatVector2> mPath;
	FloatVector2 mNextPos = { 0, 0 };

	FloatVector2 mDirectionVector = { 0, 0 };

	IntRect mUnitSize = { 8, 8, 9, 10 };
};

extern Game* gGame;
