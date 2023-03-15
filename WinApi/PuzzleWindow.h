#pragma once
#include <Windows.h>
#include <string>
#include "enums.h"


class PuzzleWindow
{
	Level* currentLevel;
	std::string* currentWord;
	HWND hWnd;
	wchar_t content[10][5];
	LetterStatus status[10][5];
	WCHAR szTitlePuzzle[MAX_LOADSTRING];
	WCHAR szWindowClassSmall[MAX_LOADSTRING];
	static bool is_class_registered(HINSTANCE, LPCWSTR);
	Buffer b;
	ATOM registerPuzzleClass(HINSTANCE hInstance);
	void initPuzzleInstance(HINSTANCE hInstance, int nCmdShow, HWND keyboardWindow, int x, int y);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WndProcPuzzle(UINT message, WPARAM wParam, LPARAM lParam);
	void drawPuzzle(Level L, HDC hdc, const HBRUSH* brushes);
	void drawOverlay(HDC hdc, HDC bufferhdc);

	const int animationTickrate = 10;
	const int ticksPerLetter = 20;
	bool isAnimated[10] = { false };
	int animationTick[10] = { 0 };
public:
	bool finished = false, win = false;
	int currRow, currColumn;
	PuzzleWindow(HINSTANCE hInstance, int nCmdShow, HWND keyboardWindow, Level* currentLevel, std::string* currentWord);
	inline HWND getHWND() { return this->hWnd; };
	inline wchar_t getLetter(int y, int x) { return content[y][x]; }
	inline LetterStatus getLetterStatus(int y, int x) { return status[y][x]; }
	inline wchar_t* getWord(int level) { return content[level]; }
	void setContent(int y, int x, wchar_t c, LetterStatus L);
	void startAnimation(int level);
	void resetAllAnimation();
	inline void setWin(bool b) { win = b; };
};

