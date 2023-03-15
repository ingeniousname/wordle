#include "KeyboardWindow.h"
#include "resource.h"
#include "Game.h"
#include "Utils.h"
#include "enums.h"


void KeyboardWindow::drawKeyboard(HDC hdc)
{
	int widthLayout[3] = { 10, 9, 7 };
	int idx = 0;
	int letteridx = 0;
	int widthIdx = 0;

	HPEN pen = CreatePen(PS_SOLID, 1, RGB(164, 174, 196));
	HPEN nopen = CreatePen(PS_NULL, 1, RGB(164, 174, 196));
	HPEN oldPen = reinterpret_cast<HPEN>(SelectObject(hdc, pen));

	RECT rc;
	int offset = 0;
	while (widthIdx < 3)
	{
		rc.left = 2 * BREAK + idx * TILE_SIZE + (idx - 1) * BREAK + offset;
		rc.right = rc.left + TILE_SIZE;
		rc.top = 2 * BREAK + widthIdx * TILE_SIZE + (widthIdx - 1) * BREAK;
		rc.bottom = rc.top + TILE_SIZE;
		oldPen = reinterpret_cast<HPEN>(SelectObject(hdc, pen));
		

		if (*g->currLevel == Level::EASY)
		{
			SelectObject(hdc, Utils::brushes[static_cast<int>(keyboardStatus[0][letteridx])]);
			RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, (rc.right - rc.left) / 5, (rc.bottom - rc.top) / 5);
		}
		else if (*g->currLevel == Level::MEDIUM)
		{
			SelectObject(hdc, Utils::brushes[0]);
			RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, (rc.right - rc.left) / 5, (rc.bottom - rc.top) / 5);
			oldPen = reinterpret_cast<HPEN>(SelectObject(hdc, nopen));

			for (int i = 0; i < 2; i++)
			{
				if (keyboardStatus[i][letteridx] != LetterStatus::EMPTY)
				{
					SelectObject(hdc, Utils::brushes[static_cast<int>(keyboardStatus[i][letteridx])]);
					RoundRect(hdc, rc.left + i * TILE_SIZE / 2,
							   rc.top, 
							   rc.right - (1 - i) * TILE_SIZE / 2, 
							   rc.bottom, (rc.right - rc.left) / 10, (rc.bottom - rc.top) / 10);
				}
				
			}
		}
		else
		{
			
			SelectObject(hdc, Utils::brushes[0]);
			RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, (rc.right - rc.left) / 5, (rc.bottom - rc.top) / 5);
			oldPen = reinterpret_cast<HPEN>(SelectObject(hdc, nopen));

			for (int i = 0; i < 4; i++)
			{
				if (keyboardStatus[i][letteridx] != LetterStatus::EMPTY)
				{
					SelectObject(hdc, Utils::brushes[static_cast<int>(keyboardStatus[i][letteridx])]);
					RoundRect(hdc, rc.left + (i % 2) * TILE_SIZE / 2,
						rc.top + (i / 2) * TILE_SIZE / 2,
						rc.right - (1 - (i % 2)) * TILE_SIZE / 2, 
						rc.bottom - (1 - (i / 2)) * TILE_SIZE / 2,
						(rc.right - rc.left) / 20, (rc.bottom - rc.top) / 20);
				}
			}
		}

		Utils::drawLetter(hdc, rc, layout[letteridx], static_cast<int>(keyboardStatus[0][letteridx]), Utils::colors);





		idx++;
		if (idx == widthLayout[widthIdx])
		{
			idx = 0;
			widthIdx++;
			offset = (10 - widthLayout[widthIdx]) * (TILE_SIZE + BREAK) / 2;
		}
		letteridx++;

	}
}

ATOM KeyboardWindow::registerKeyboardClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = (CreateSolidBrush(RGB(255, 255, 255)));
	wcex.lpszClassName = szWindowClassMain;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TUTORIAL);
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORDLE));
	wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WORDLE));

	return RegisterClassExW(&wcex);
}

void KeyboardWindow::initKeyboardInstance(HINSTANCE hInstance, int nCmdShow)
{
	RECT windowRC;
	windowRC.left = windowRC.top = 0;
	windowRC.right = TILE_SIZE * 10 + BREAK * 11;
	windowRC.bottom = TILE_SIZE * 3 + BREAK * 7;
	AdjustWindowRect(&windowRC, WS_VISIBLE | WS_CAPTION, false);
	int sizeX = windowRC.right - windowRC.left;
	int sizeY = windowRC.bottom - windowRC.top;


	width = sizeX;
	height = sizeY;
	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

	hWnd = CreateWindowExW(0, szWindowClassMain, szTitleKeybord, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, rc.right / 2 - sizeX / 2, rc.bottom * 2 / 3,
		sizeX, sizeY, nullptr, nullptr, hInstance, reinterpret_cast<LPVOID>(this));
	if (!hWnd) throw new std::exception("Keyboard HWND failed to launch!");

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// https://stackoverflow.com/questions/3970066/creating-a-transparent-window-in-c-win32
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, 100, LWA_ALPHA);
}

LRESULT KeyboardWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	KeyboardWindow* w = nullptr;
	if (message == WM_NCCREATE) {
		auto pcs = reinterpret_cast<LPCREATESTRUCTW>(lParam);
		w = reinterpret_cast<KeyboardWindow*>(pcs->lpCreateParams);
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(w));
		w->hWnd = hWnd;
	}
	else w = reinterpret_cast<KeyboardWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
	if (w) {
		auto r = w->WndProcKeyboard(message, wParam, lParam);
		if (message == WM_NCDESTROY) {
			w->hWnd = nullptr;
			SetWindowLongPtrW(hWnd, GWLP_USERDATA, 0);
		}
		return r;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

LRESULT KeyboardWindow::WndProcKeyboard(UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool keyboardFocused = true;
	switch (message)
	{
	case WM_COMMAND:
	{

		int wmId = LOWORD(wParam);
		HMENU menu = GetMenu(hWnd);
		switch (wmId)
		{
		case ID_CHEAT:
			g->cheat();
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_DIFFICULTY_EASY:
			g->changeLevel(Level::EASY);
			break;
		case ID_DIFFICULTY_MEDIUM:
			g->changeLevel(Level::MEDIUM);
			break;
		case ID_DIFFICULTY_HARD:
			g->changeLevel(Level::HARD);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_CREATE:
	{
		HDC hdc = GetDC(hWnd);
		b.offDC = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);
		SetTimer(hWnd, 1, 1, NULL);
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

	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		b.offOldBitmap = (HBITMAP)SelectObject(b.offDC, b.offBitmap);
		RECT rc;
		GetClientRect(hWnd, &rc);
		FillRect(b.offDC, &rc, CreateSolidBrush(RGB(255, 255, 255)));
		drawKeyboard(b.offDC);

		BitBlt(hdc, 0, 0, rc.right, rc.bottom, b.offDC, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_TIMER:
	{

		ULONGLONG time = GetTickCount64();
		std::wstring s = szTitleKeybord;
		s += L": ";
		s += std::to_wstring((time - TimeStart) / 1000) + L"." + std::to_wstring((time - TimeStart) % 1000);
		SetWindowText(hWnd, s.c_str());
		break;
	}
	case WM_KEYDOWN:
	{
		if (keyboardFocused)
		{
			for (int i = 0; i < 4; i++)
				break;
				g->parseInput(hWnd, wParam);
		}
		break;
	}
	case WM_ERASEBKGND:
		return 1;
	case WM_SETFOCUS:
		keyboardFocused = true;
		break;
	case WM_KILLFOCUS:
		keyboardFocused = false;
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


KeyboardWindow::KeyboardWindow(HINSTANCE hInstance, int nCmdShow, Game* game)
{
	this->g = game;
	for(int k = 0; k < 4; k++)
		for (int i = 0; i < 26; i++)
			keyboardStatus[k][i] = LetterStatus::EMPTY;
	

	LoadStringW(hInstance, IDS_APP_TITLE, szTitleKeybord, MAX_LOADSTRING);
	LoadStringW(hInstance, IDS_KEYBORDCLASS, szWindowClassMain, MAX_LOADSTRING);

	this->TimeStart = GetTickCount64();

	registerKeyboardClass(hInstance);
	initKeyboardInstance(hInstance, nCmdShow);

	
}

void KeyboardWindow::setContentStatus(int k, int idx, LetterStatus S)
{
	this->keyboardStatus[k][idx] = S;
}
