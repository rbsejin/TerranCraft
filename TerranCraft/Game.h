#pragma once

class DDrawDevice;

class Game
{
public:
	Game();
	~Game();

	bool Initalize(HWND hWnd);
	void Cleanup();
	void Process();

	void OnKeyDown(unsigned int vkCode, unsigned int scanCode);
	void OnKeyUp(unsigned int vkCode, unsigned int scanCode);
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

#pragma endregion
};

extern Game* gGame;
