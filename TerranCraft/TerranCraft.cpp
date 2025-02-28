// TerranCraft.cpp : Defines the entry point for the application.
//

#include "pch.h"

#include "framework.h"
#include "TerranCraft.h"
#include "Game.h"

#include "PathFinder.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND ghMainWindow = nullptr;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TERRANCRAFT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TERRANCRAFT));

	MSG msg;

	gGame = new Game();
	gGame->Initalize(ghMainWindow);

	// Main message loop:
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			gGame->Process();
		}
	}

	if (gGame != nullptr)
	{
		delete gGame;
		gGame = nullptr;
	}

#ifdef _DEBUG
	_ASSERT(_CrtCheckMemory());
#endif // _DEBUG

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TERRANCRAFT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TERRANCRAFT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	constexpr int WINDOW_POS_X = 0;
	constexpr int WINDOW_POS_Y = 0;

	constexpr int CLIENT_WIDTH = 600;
	constexpr int CLIENT_HEIGHT = 700;

	RECT rect = { 0, 0, CLIENT_WIDTH, CLIENT_HEIGHT };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, TRUE);

	const int windowWidth = rect.right - rect.left;
	const int windowHeight = rect.bottom - rect.top;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		WINDOW_POS_X, WINDOW_POS_Y, windowWidth, windowHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ghMainWindow = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_FILE_OPEN:
		{
			// open file
			OPENFILENAME ofn = { 0, };
			WCHAR szFile[MAX_PATH] = { 0, };
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Map File(*.dat)\0*.dat\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn))
			{
				FILE* fp = nullptr;
				_wfopen_s(&fp, ofn.lpstrFile, L"rb");
				if (fp != nullptr)
				{
					fread(gMap, sizeof(gMap), 1, fp);
					fclose(fp);
				}
			}
		}
		break;
		case ID_FILE_SAVE:
		{
			// save file
			OPENFILENAME ofn = { 0, };
			WCHAR szFile[MAX_PATH] = { 0, };
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Map File(*.dat)\0*.dat\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			

			if (GetSaveFileName(&ofn))
			{
				FILE* fp = nullptr;
				_wfopen_s(&fp, ofn.lpstrFile, L"wb");
				if (fp != nullptr)
				{
					fwrite(gMap, sizeof(gMap), 1, fp);
					fclose(fp);
				}
			}
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
		if (gGame != nullptr)
		{
			gGame->OnUpdateWindowSize();
		}
		break;
	case WM_MOVE:
		if (gGame != nullptr)
		{
			gGame->OnUpdateWindowPos();
		}
		break;
	case WM_KEYDOWN:
	{
		unsigned int scanCode = (0x00ff0000 & lParam) >> 16;
		unsigned int vkCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);

		if (!(lParam & 0x40000000))
		{
			if (gGame != nullptr)
			{
				gGame->OnKeyDown(vkCode, scanCode);
			}
		}
	}
	break;
	case WM_KEYUP:
	{
		unsigned int scanCode = (0x00ff0000 & lParam) >> 16;
		unsigned int vkCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);

		if (gGame != nullptr)
		{
			gGame->OnKeyUp(vkCode, scanCode);
		}
	}
	break;
	// mouse move
	case WM_MOUSEMOVE:
		if (gGame != nullptr)
		{
			gGame->OnMouseMove((uint32)wParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_LBUTTONUP:
		if (gGame != nullptr)
		{
			gGame->OnLButtonUp((uint32)wParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_LBUTTONDOWN:
		if (gGame != nullptr)
		{
			gGame->OnLButtonDown((uint32)wParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_RBUTTONUP:
		if (gGame != nullptr)
		{
			gGame->OnRButtonUp((uint32)wParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_RBUTTONDOWN:
		if (gGame != nullptr)
		{
			gGame->OnRButtonDown((uint32)wParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_SYSKEYDOWN:
	{
		unsigned int scanCode = (0x00ff0000 & lParam) >> 16;
		unsigned int vkCode = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);
		bool bAltKeyDown = false;
		if (HIWORD(lParam) & KF_ALTDOWN)
		{
			bAltKeyDown = TRUE;
		}
		if (!gGame->OnSysKeyDown(vkCode, scanCode, bAltKeyDown))
		{
			DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
