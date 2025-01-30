#pragma once
#include "enums.h"
#include "KeyboardWindow.h"
#include "PuzzleWindow.h"
#include <set>
#include <string>


class Game
{
	std::set<std::string> gameDict;
	std::string gameWords[4];
	KeyboardWindow* keyboardWindow;
	PuzzleWindow* puzzleWindow[4];


public:
	Level* currLevel;
	Game(HINSTANCE hInstance, int nCmdShow);
	~Game() { for (int i = 0; i < 4; i++) delete puzzleWindow[i]; delete keyboardWindow; }
	void changeLevel(Level L);
	void parseInput(HWND keyboardHWND, WPARAM wParam);
	void cheat();
};

