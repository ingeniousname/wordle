#include "PuzzleWindow.h"
#include "Utils.h"
#include "resource.h"
#include "enums.h"
#include <cmath>





bool PuzzleWindow::is_class_registered(HINSTANCE hInst, LPCWSTR cName)
{
	WNDCLASSEXW wcx;
	return GetClassInfoExW(hInst, cName, &wcx);
}

ATOM PuzzleWindow::registerPuzzleClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = 0;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	wcex.lpszClassName = szWindowClassSmall;
	wcex.lpszMenuName = nullptr;
	wcex.hIcon = nullptr;
	wcex.hIconSm = nullptr;

	return RegisterClassExW(&wcex);
}

void PuzzleWindow::initPuzzleInstance(HINSTANCE hInstance, int nCmdShow, HWND keyboardWindow, int x, int y)
{
	CreateWindowExW(0, szWindowClassSmall, szTitlePuzzle, WS_VISIBLE | WS_CAPTION, x, y,
		200, 200, keyboardWindow, nullptr, hInstance, reinterpret_cast<LPVOID>(this));
	//SetWindowLong(hWnd, GWL_STYLE, 0);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

}

LRESULT PuzzleWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PuzzleWindow* w = nullptr;
	if (message == WM_NCCREATE) {
		auto pcs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
		w = reinterpret_cast<PuzzleWindow*>(pcs->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
		w->hWnd = hWnd;
	}
	else w = reinterpret_cast<PuzzleWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (w) {
		auto r = w->WndProcPuzzle(message, wParam, lParam);
		if (message == WM_NCDESTROY) {
			w->hWnd = nullptr; 
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		}
		return r;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

LRESULT PuzzleWindow::WndProcPuzzle(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{

		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				content[i][j] = ' ';
				status[i][j] = LetterStatus::EMPTY;
			}
		}

		currColumn = 0;
		currRow = 0;

		b.offBitmap = b.offOldBitmap = NULL;
		HDC hdc = GetDC(hWnd);
		b.offDC = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		break;
	}
	case WM_SIZE:
	{
		int clientWidth = LOWORD(lParam);
		int clientHeight = HIWORD(lParam);
		HDC hdc = GetDC(hWnd);
		if (b.offOldBitmap != NULL) {
			SelectObject(b.offDC, b.offOldBitmap);

		}
		if (b.offBitmap != NULL) {
			DeleteObject(b.offBitmap);
		}
		b.offBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		b.offOldBitmap = (HBITMAP)SelectObject(b.offDC, b.offBitmap);
		ReleaseDC(hWnd, hdc);
		InvalidateRect(hWnd, NULL, TRUE);

	}
	break;
	case WM_TIMER:
	{
		if (isAnimated[wParam])
		{
			if (animationTick[wParam] == ticksPerLetter * 3)
			{
				isAnimated[wParam] = false;
				animationTick[wParam] = 0;
			}
			else animationTick[wParam]++;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		b.offOldBitmap = (HBITMAP)SelectObject(b.offDC, b.offBitmap);
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(b.offDC, &rc, CreateSolidBrush(RGB(255, 255, 255)));
		drawPuzzle(*currentLevel, b.offDC, Utils::brushes);

		BitBlt(hdc, 0, 0, rc.right, rc.bottom, b.offDC, 0, 0, SRCCOPY);
		if (finished)
			drawOverlay(hdc, b.offDC);
		EndPaint(hWnd, &ps);

		break;
	}
	case WM_ERASEBKGND:
		return 0;
	case WM_NCHITTEST:
		return HTCAPTION;
		break;
	case WM_DESTROY:
	{
		if (b.offOldBitmap != NULL) {
			SelectObject(b.offDC, b.offOldBitmap);
		}

		if (b.offDC != NULL) {
			DeleteDC(b.offDC);
		}

		if (b.offBitmap != NULL) {
			DeleteObject(b.offBitmap);
		}

		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void PuzzleWindow::drawPuzzle(Level L, HDC hdc, const HBRUSH* brushes)
{
	RECT rc;
	int boardHeight = 6 + static_cast<int>(L) * 2;



	for (int y = 0; y < boardHeight; y++)
	{
		for (int x = 0; x < 5; x++)
		{
			SelectObject(hdc, brushes[static_cast<int>(status[y][x])]);
			if(status[y][x] == LetterStatus::EMPTY) SelectObject(hdc, Utils::pens[1]);
			else SelectObject(hdc, Utils::pens[static_cast<int>(status[y][x])]);
			int tick = ticksPerLetter;
			if (isAnimated[y])
			{
				tick = animationTick[y] - x * ticksPerLetter / 2;
				if (tick > ticksPerLetter) tick = ticksPerLetter;
				if (tick < 0) tick = 0;
				if (tick < ticksPerLetter / 2)
				{
					SelectObject(hdc, brushes[0]);
					SelectObject(hdc, Utils::pens[1]);
				}
			}
			
			int offset = int(TILE_SIZE - float(abs(ticksPerLetter / 2 - tick)) / (ticksPerLetter / 2) * TILE_SIZE);

			rc.left = 2 * BREAK + x * TILE_SIZE + (x - 1) * BREAK;
			rc.right = rc.left + TILE_SIZE;
			rc.top = 2 * BREAK + y * TILE_SIZE + (y - 1) * BREAK;
			rc.bottom = rc.top + TILE_SIZE;


			RoundRect(hdc, rc.left, rc.top + offset / 2, rc.right, rc.bottom - offset / 2, (rc.right - rc.left) / 5, (rc.bottom - rc.top - offset) / 5);
			Utils::drawLetter(hdc, rc, content[y][x], static_cast<int>(status[y][x]), Utils::colors);
		}
	}
}

void PuzzleWindow::drawOverlay(HDC hdc, HDC bufferhdc)
{
	for (int i = 0; i < 10; i++)
		if (isAnimated[i]) return;
	HBRUSH oldBrush;
	if(win)
		oldBrush = reinterpret_cast<HBRUSH>(SelectObject(bufferhdc, CreateSolidBrush(RGB(0, 255, 0))));
	else oldBrush = reinterpret_cast<HBRUSH>(SelectObject(bufferhdc, CreateSolidBrush(RGB(255, 0, 0))));
	
	RECT rc;
	GetClientRect(hWnd, &rc);
	HPEN old_pen = reinterpret_cast<HPEN>(SelectObject(bufferhdc, CreatePen(PS_NULL, 0, RGB(255, 255, 255))));
	Rectangle(bufferhdc, -2, -2, rc.right + 2, rc.bottom + 2);
	BLENDFUNCTION B = { AC_SRC_OVER, 0, 150, AC_SRC_OVER};
	AlphaBlend(hdc, 0, 0, rc.right, rc.bottom, bufferhdc, 0, 0, rc.right, rc.bottom, B);
	if (!win)
		Utils::printWhiteText(hdc, rc, *currentWord);


	old_pen = reinterpret_cast<HPEN>(SelectObject(bufferhdc, old_pen));
	oldBrush = reinterpret_cast<HBRUSH>(SelectObject(bufferhdc, oldBrush));
	DeleteObject(old_pen);
	DeleteObject(oldBrush);
}

PuzzleWindow::~PuzzleWindow()
{
	if (IsWindow(hWnd))
		DestroyWindow(hWnd);
}

PuzzleWindow::PuzzleWindow(HINSTANCE hInstance, int nCmdShow, HWND keyboardWindow, Level* currLevel, std::string* currentWord)
{
	this->currentLevel = currLevel;
	this->currentWord = currentWord;
	currColumn = currRow = 0;


	LoadStringW(hInstance, IDS_PUZZLE_TITLE, szTitlePuzzle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDS_PUZZLECLASS, szWindowClassSmall, MAX_LOADSTRING);

	if(!is_class_registered(hInstance, szWindowClassSmall))
		registerPuzzleClass(hInstance);
	initPuzzleInstance(hInstance, nCmdShow, keyboardWindow, 200, 200);



}

void PuzzleWindow::setContent(int y, int x, wchar_t c, LetterStatus L)
{
	if (c != NULL)
		content[y][x] = c;
	status[y][x] = L;
}

void PuzzleWindow::startAnimation(int level)
{
	SetTimer(hWnd, level, animationTickrate, NULL);
	this->isAnimated[level] = true;
	this->animationTick[level] = 0;
}

void PuzzleWindow::resetAllAnimation()
{
	for (int i = 0; i < 10; i++)
		isAnimated[i] = false;
}
