#include "Game.h"
#include "Utils.h"
#include "resource.h"


Game::Game(HINSTANCE hInstance, int nCmdShow)
{
	gameDict = Utils::readAndParseWordFile("Wordle.txt");
	currLevel = new Level(Level::EASY);

	keyboardWindow = new KeyboardWindow(hInstance, nCmdShow, this);
	for (int i = 0; i < 4; i++)
	{
		puzzleWindow[i] = new PuzzleWindow(hInstance, nCmdShow, keyboardWindow->getHWND(), currLevel, &gameWords[i]);
	}
	changeLevel(Utils::readIniFile());
}

void Game::changeLevel(Level L)
{
	*this->currLevel = L;
	std::wstring ws;
	Utils::writeToIniFile(L);

	for (int k = 0; k < 4; k++)
		for (int i = 0; i < 26; i++)
			keyboardWindow->setContentStatus(k, i, LetterStatus::EMPTY);

	InvalidateRect(keyboardWindow->getHWND(), NULL, TRUE);
	for (int i = 0; i < 4; i++)
	{
		for (int y = 0; y < 10; y++)
			for (int x = 0; x < 5; x++)
			{
				puzzleWindow[i]->setContent(y, x, ' ', LetterStatus::EMPTY);
			}
		puzzleWindow[i]->currColumn = puzzleWindow[i]->currRow = 0;
		puzzleWindow[i]->finished = false;
		puzzleWindow[i]->resetAllAnimation();
		puzzleWindow[i]->setWin(false);
	
	}
	

	RECT rc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);

	RECT windowRC;
	windowRC.left = windowRC.top = 0;

	AdjustWindowRect(&windowRC, WS_VISIBLE | WS_CAPTION, false);
	HMENU menu = GetMenu(keyboardWindow->getHWND());
	if (L == Level::EASY)
	{
		CheckMenuItem(menu, ID_DIFFICULTY_EASY, MF_CHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_HARD, MF_UNCHECKED);

		gameWords[0] = Utils::getRandomWord(gameDict);
		InvalidateRect(puzzleWindow[0]->getHWND(), NULL, TRUE);
		for (int i = 1; i < 4; i++)
			ShowWindow(puzzleWindow[i]->getHWND(), SW_HIDE);

		windowRC.right = TILE_SIZE * 5 + BREAK * 5;
		windowRC.bottom = TILE_SIZE * 6 + BREAK * 6 - 24;
		AdjustWindowRect(&windowRC, WS_VISIBLE | WS_CAPTION, false);
		int width = windowRC.right - windowRC.left;
		int height = windowRC.bottom - windowRC.top;
		//int width = TILE_SIZE * 5 + BREAK * 5 + 24;
		//int height = TILE_SIZE * 6 + BREAK * 6 + 48;

		MoveWindow(puzzleWindow[0]->getHWND(), rc.right / 2 - width / 2, rc.bottom / 2 - height / 2, width, height, true);
	}
	else if (L == Level::MEDIUM)
	{
		CheckMenuItem(menu, ID_DIFFICULTY_EASY, MF_UNCHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_MEDIUM, MF_CHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_HARD, MF_UNCHECKED);
		for (int i = 0; i < 2; i++)
		{
			ShowWindow(puzzleWindow[i]->getHWND(), SW_SHOW);
			InvalidateRect(puzzleWindow[i]->getHWND(), NULL, TRUE);
		}
		gameWords[0] = Utils::getRandomWord(gameDict);
		do {
			gameWords[1] = Utils::getRandomWord(gameDict);
		} while (gameWords[0] == gameWords[1]);





		for (int i = 2; i < 4; i++)
			ShowWindow(puzzleWindow[i]->getHWND(), SW_HIDE);

		windowRC.right = TILE_SIZE * 5 + BREAK * 5;
		windowRC.bottom = TILE_SIZE * 8 + BREAK * 8 - 24;
		AdjustWindowRect(&windowRC, WS_VISIBLE | WS_CAPTION, false);
		int width = windowRC.right - windowRC.left;
		int height = windowRC.bottom - windowRC.top;

		//int width = TILE_SIZE * 5 + BREAK * 5 + 24;
		//int height = TILE_SIZE * 8 + BREAK * 8 + 48;

		MoveWindow(puzzleWindow[0]->getHWND(), rc.right / 4 - width / 2, rc.bottom / 2 - height / 2, width, height, true);
		MoveWindow(puzzleWindow[1]->getHWND(), rc.right * 3 / 4 - width / 2, rc.bottom / 2 - height / 2, width, height, true);


	}
	else
	{
		CheckMenuItem(menu, ID_DIFFICULTY_EASY, MF_UNCHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_MEDIUM, MF_UNCHECKED);
		CheckMenuItem(menu, ID_DIFFICULTY_HARD, MF_CHECKED);
		for (int i = 0; i < 4; i++)
		{
			ShowWindow(puzzleWindow[i]->getHWND(), SW_SHOW);
			InvalidateRect(puzzleWindow[i]->getHWND(), NULL, TRUE);
			do
			{
				gameWords[i] = Utils::getRandomWord(gameDict);

			} while (gameWords[i] == gameWords[(i + 1) % 4] || gameWords[i] == gameWords[(i + 2) % 4] || gameWords[i] == gameWords[(i + 3) % 4]);
			
		}



		windowRC.right = TILE_SIZE * 5 + BREAK * 5;
		windowRC.bottom = TILE_SIZE * 10 + BREAK * 10 - 24;
		AdjustWindowRect(&windowRC, WS_VISIBLE | WS_CAPTION, false);
		int width = windowRC.right - windowRC.left;
		int height = windowRC.bottom - windowRC.top;


		//int width = TILE_SIZE * 5 + BREAK * 5 + 24;
		//int height = TILE_SIZE * 10 + BREAK * 10 + 48;

		MoveWindow(puzzleWindow[0]->getHWND(), rc.right / 4 - width / 2, rc.bottom / 4 - height / 2, width, height, true);
		MoveWindow(puzzleWindow[1]->getHWND(), rc.right * 3 / 4 - width / 2, rc.bottom / 4 - height / 2, width, height, true);
		MoveWindow(puzzleWindow[2]->getHWND(), rc.right / 4 - width / 2, rc.bottom * 3 / 4 - height / 2, width, height, true);
		MoveWindow(puzzleWindow[3]->getHWND(), rc.right * 3 / 4 - width / 2, rc.bottom * 3 / 4 - height / 2, width, height, true);


	}



}

void Game::parseInput(HWND keyboardHWND, WPARAM wParam)
{
	wchar_t c = static_cast<wchar_t>(wParam);
	for (int k = 0; k < 4; k++)
	{
		HWND hWnd = puzzleWindow[k]->getHWND();
		if (!IsWindowVisible(hWnd) || puzzleWindow[k]->finished)
			continue;
		std::string currentWord = gameWords[k];

		if (isalpha(c))
		{
			if (puzzleWindow[k]->currColumn < 5 && puzzleWindow[k]->currRow < 6 + static_cast<int>(*currLevel) * 2)
			{
				puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, puzzleWindow[k]->currColumn, toupper(c), LetterStatus::EMPTY);
				puzzleWindow[k]->currColumn++;
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		else if (wParam == VK_RETURN)
		{
			if (puzzleWindow[k]->currColumn == 5 && puzzleWindow[k]->currRow < 6 + static_cast<int>(*currLevel) * 2)
			{
				std::wstring word = L"";
				for (int i = 0; i < 5; i++)
					word.push_back(tolower(puzzleWindow[k]->getLetter(puzzleWindow[k]->currRow, i)));

				if (Utils::toLowerCase(word) == std::wstring(currentWord.begin(), currentWord.end()))
				{
					puzzleWindow[k]->finished = true;
					puzzleWindow[k]->setWin(true);
					for (int i = 0; i < 5; i++)
					{
						puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, i, NULL, LetterStatus::CORRECT);
						int j = 0;
						for (j; j < 26; j++)
							if (keyboardWindow->getLayout(j) == toupper(currentWord[i])) break;
						keyboardWindow->setContentStatus(k, j, LetterStatus::CORRECT);
					}
					this->puzzleWindow[k]->startAnimation(puzzleWindow[k]->currRow);
					InvalidateRect(hWnd, NULL, TRUE);
					InvalidateRect(keyboardHWND, NULL, TRUE);
					continue;
				}

				if (Utils::isInDictionary(gameDict, word))
				{
					for (int i = 0; i < 5; i++)
					{
						int j = 0;
						for (j; j < 26; j++)
							if (keyboardWindow->getLayout(j) == toupper(word[i])) break;

						if ((char)word[i] == currentWord[i])
						{
							puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, i, NULL, LetterStatus::CORRECT);
							if (j < 26 && static_cast<int>(keyboardWindow->getContentStatus(k, j)) < static_cast<int>(LetterStatus::CORRECT))
								keyboardWindow->setContentStatus(k, j, LetterStatus::CORRECT);
						}
						else if (Utils::containsLetter(currentWord, word[i]))
						{
							puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, i, NULL, LetterStatus::NOT_IN_PLACE);
							if (j < 26 && static_cast<int>(keyboardWindow->getContentStatus(k, j)) < static_cast<int>(LetterStatus::NOT_IN_PLACE))
								keyboardWindow->setContentStatus(k, j, LetterStatus::NOT_IN_PLACE);

						}
						else
						{
							puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, i, NULL, LetterStatus::INCORRECT);
							if (j < 26 && static_cast<int>(keyboardWindow->getContentStatus(k, j)) < static_cast<int>(LetterStatus::INCORRECT))
								keyboardWindow->setContentStatus(k, j, LetterStatus::INCORRECT);
						}


					}
					//std::wstring ws = L"";
					//for (int i = 0; i < 26; i++)
					//	ws += std::to_wstring((int)keyboardData.keyboardStatus[i]);
					this->puzzleWindow[k]->startAnimation(puzzleWindow[k]->currRow);
					puzzleWindow[k]->currRow++;
					if (puzzleWindow[k]->currRow == 6 + static_cast<int>(*currLevel) * 2)
						puzzleWindow[k]->finished = true;
				}
				else
				{
					for (int i = 0; i < 5; i++)
						puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, i,  ' ', LetterStatus::EMPTY);
				}

				puzzleWindow[k]->currColumn = 0;
				InvalidateRect(hWnd, NULL, TRUE);
				InvalidateRect(keyboardHWND, NULL, TRUE);
			}
		}
		else if (wParam == VK_BACK)
		{
			if (puzzleWindow[k]->currRow < 6 + static_cast<int>(*currLevel) * 2)
			{
				puzzleWindow[k]->currColumn--;
				if (puzzleWindow[k]->currColumn < 0)
					puzzleWindow[k]->currColumn = 0;

				puzzleWindow[k]->setContent(puzzleWindow[k]->currRow, puzzleWindow[k]->currColumn, ' ', LetterStatus::EMPTY);
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
	}

	
}

void Game::cheat()
{
	for(int i = 0; i < 4; i++)
		if(IsWindowVisible(puzzleWindow[i]->getHWND()))
			SetWindowText(puzzleWindow[i]->getHWND(), std::wstring(gameWords[i].begin(), gameWords[i].end()).c_str());
}
