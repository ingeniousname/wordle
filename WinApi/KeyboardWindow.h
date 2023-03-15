#pragma once
#include <Windows.h>
#include "enums.h"

class Game;


class KeyboardWindow
{
	Game* g;
	HWND hWnd;
	ULONGLONG TimeStart;
	int width, height;
	WCHAR szTitleKeybord[MAX_LOADSTRING];
	WCHAR szWindowClassMain[MAX_LOADSTRING];
	const wchar_t layout[26] = { 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M' };
	LetterStatus keyboardStatus[4][26] = {LetterStatus::EMPTY};
	Buffer b;
	void drawKeyboard(HDC hdc);
	ATOM registerKeyboardClass(HINSTANCE hInstance);
	void initKeyboardInstance(HINSTANCE hInstance, int nCmdShow);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProcKeyboard(UINT message, WPARAM wParam, LPARAM lParam);


public:
	KeyboardWindow(HINSTANCE hInstance, int nCmdShow, Game* game);
	inline HWND getHWND() { return this->hWnd; };
	void setContentStatus(int k, int idx, LetterStatus S);
	inline LetterStatus getContentStatus(int k, int idx) { return keyboardStatus[k][idx]; }
	inline wchar_t getLayout(int idx) { return layout[idx]; }
};

