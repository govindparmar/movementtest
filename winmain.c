// Windows functions
#include <Windows.h>
// String functions... seems necessary for when the game is actually complete
// Which is to say, these functions may be necessary for, say, character dialogs and stuff.
#include <strsafe.h>
// To use the bitmaps
#include "resource.h"

// Window class name
TCHAR szClassName[] = TEXT("MvmntTestWnd");
// Callback functions:
// Message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
// Child window processing callback (set fonts)
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);

// Global variables:
// The ball
HBITMAP g_hBall;
// The brush
HBRUSH g_hBrush;
// The ball location bounding rect
RECT g_ballLoc;

/**
 * Fills out the members of a WNDCLASSEX structure and registers the window class.
 * @param *wcex A pointer to the WNDCLASSEX structure whose members will be populated.
 * @param hInstance The value to load into the wcex->hInstance field.
 * @return The atom from registering the window class.
 */
ATOM RegisterWindow(WNDCLASSEX *wcex, HINSTANCE hInstance)
{
	wcex->cbClsExtra = 0;
	wcex->cbWndExtra = 0;
	wcex->cbSize = sizeof(WNDCLASSEX);
	wcex->hbrBackground = (HBRUSH) COLOR_WINDOW;
	wcex->hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex->hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex->hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wcex->hInstance = hInstance;
	wcex->lpfnWndProc = WindowProc;
	wcex->lpszClassName = szClassName;
	wcex->lpszMenuName = NULL;
	wcex->style = CS_HREDRAW | CS_VREDRAW;

	return RegisterClassEx(wcex);
}

/**
 * The WinMain function (called by OS on program execution).
 * @param hInstance A handle to the instance of *this* execution.
 * @param hPrevInstance A handle to the most recent previous execution of this program.
 * @param lpCmdLine A string containing the command-line arguments to the program (if any).
 * @param nShowCmd The value for ShowWindow if it's ever called (I just use SW_SHOW regardless of this value).
 * @return A value to the operating system. 0 indicates no problems executing; non-0 means some error code to look up.
 */
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	HDC winDC;
	MSG Msg;
	ATOM aRW = RegisterWindow(&wcex, hInstance);
	if(!aRW)
	{
		DWORD errCode = GetLastError();
		MessageBox(0, TEXT("Window registration failed!"), TEXT("Error"), MB_OK | MB_ICONSTOP);
		return errCode;
	}

	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, szClassName, TEXT("Movement Test"), WS_SYSMENU | WS_VISIBLE, 200, 200, 300, 230, NULL, NULL, hInstance, NULL);
	if(!hWnd)
	{
		DWORD errCode = GetLastError();
		MessageBox(0, TEXT("Window registration failed!"), TEXT("Error"), MB_OK | MB_ICONSTOP);
		return errCode;
	}
	g_hBall = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
	g_hBrush = (HBRUSH) COLOR_WINDOW;
	
	ShowWindow(hWnd, SW_SHOW);
	EnumChildWindows(hWnd, EnumChildProc, MAKELPARAM(FALSE, 0));
	UpdateWindow(hWnd);
	winDC = GetDC(hWnd);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return (int) Msg.wParam;
}

/**
 * Erases the ball at its current location and redraws it at a specified location.
 * @param hWnd The handle to the window to perform the drawing operation on.
 * @param x The new x-coordinate of the left edge of the ball.
 * @param y The new y-coordinate of the top edge of the ball.
 */
VOID UpdateBall(HWND hWnd, int x, int y)
{
	HDC hDC, memDC;
	BITMAP bm;
	HBITMAP hOld;
	hDC = GetDC(hWnd);
	memDC = CreateCompatibleDC(hDC);
	hOld = SelectObject(memDC, g_hBall);
	FillRect(hDC, &g_ballLoc, (HBRUSH) COLOR_WINDOW);
	GetObject(g_hBall, sizeof(bm), &bm);	
	BitBlt(hDC, x, y, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, hOld);
	DeleteDC(memDC);
	g_ballLoc.top=y;
	g_ballLoc.left=x;
	g_ballLoc.bottom=y+bm.bmHeight;
	g_ballLoc.right=x+bm.bmWidth;
	
}

/**
 * The procedure that handles window messages.
 * @param hWnd The window whose messages are being processed.
 * @param Msg An identifier for the message (WM_*)
 * @param wParam The meaning of this parameter varies by message.
 * @param lParam The meaning of this parameter varies by message.
 * @return The meaning of a return value varies by message.
 */
LRESULT CALLBACK WindowProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_KEYDOWN:
		switch((int)wParam)
		{
		case VK_UP:
			if(g_ballLoc.top>=10)
			{
				UpdateBall(hWnd, g_ballLoc.left, g_ballLoc.top-5);
			}
			break;
		case VK_LEFT:
			if(g_ballLoc.left>=10)
			{
				UpdateBall(hWnd, g_ballLoc.left-5, g_ballLoc.top);
			}
			break;
		case VK_RIGHT:
			if(g_ballLoc.right<=290)
			{
				UpdateBall(hWnd, g_ballLoc.left+5, g_ballLoc.top);
			}
			break;
		case VK_DOWN:
			if(g_ballLoc.bottom<=190)
			{
				UpdateBall(hWnd, g_ballLoc.left, g_ballLoc.top+5);
			}
			break;
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hWnd, &ps);
			UpdateBall(hWnd, 0, 0);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		DeleteObject(g_hBall);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}

/**
 * Goes through child windows and tells each of them to the default system font.
 * @param hWnd The child window being processed.
 * @param lParam Unused but required for the callback function.
 * @return True always to indicate to the WinAPI function that uses the callback to continue processing child windows.
 */
BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	HFONT hfDefault = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWnd, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(TRUE, 0));
	return TRUE;
}
