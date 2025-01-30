#include "Utils.h"
#include "enums.h"
#include <tchar.h>
#include <fstream>
#include <filesystem>

HBRUSH Utils::brushes[4];
HPEN Utils::pens[4];
COLORREF Utils::colors[4];

void Utils::initData()
{
	colors[0] = RGB(251, 252, 255);
	colors[1] = RGB(164, 174, 196);
	colors[2] = RGB(243, 194, 55) ;
	colors[3] = RGB(121, 184, 81) ;
	for (int i = 0; i < 4; i++)
	{
		brushes[i] = CreateSolidBrush(colors[i]);
		pens[i] = CreatePen(PS_SOLID, 1, colors[i]);
	}
}

void Utils::deleteData()
{
	for (int i = 0; i < 4; i++)
	{
		DeleteObject(brushes[i]);
		DeleteObject(pens[i]);
	}
}

void Utils::drawLetter(HDC hdc, RECT rc, wchar_t c, int bkndType, COLORREF* colors)
{
	const int fontSize = 18;
	HFONT letterFont = CreateFont(
		-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), // Height

		0,
		0,
		0,
		FW_BOLD,
		false,
		FALSE,
		0,
		EASTEUROPE_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T(" Verdana "));


	HFONT oldFont = (HFONT)SelectObject(hdc, letterFont);
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, &c, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	oldFont = (HFONT)SelectObject(hdc, oldFont);
	DeleteObject(letterFont);
}

std::string Utils::getRandomWord(const std::set<std::string>& dict)
{
	//https://stackoverflow.com/questions/158836/random-element-in-a-map
	int pos = rand() % dict.size();
	auto it = dict.begin();
	std::advance(it, pos);
	return *it;
}

bool Utils::isInDictionary(const std::set<std::string>& dict, std::wstring word)
{
	return dict.find(std::string(word.begin(), word.end())) != dict.end();
}

bool Utils::containsLetter(std::string word, wchar_t c)
{
	for (int i = 0; i < 5; i++)
		if (word[i] == c) return true;
	return false;
}

std::set<std::string> Utils::readAndParseWordFile(std::string filename)
{
	std::ifstream inFile(filename);
	if (inFile.fail())
		throw new std::exception(filename.c_str());
	std::set<std::string> gameDict;
	while (!inFile.eof())
	{
		std::string s;
		inFile >> s;
		gameDict.insert(s);

	}
	inFile.close();
	return gameDict;
}

void Utils::writeToIniFile(Level L)
{
	std::wstring ws;
	switch (L)
	{
	case Level::EASY:
		ws = L"EASY";
		break;
	case Level::MEDIUM:
		ws = L"MEDIUM";
		break;
	case Level::HARD:
		ws = L"HARD";
		break;
	}
	WritePrivateProfileString(L"WORDLE", L"LEVEL", ws.c_str(), L"./wordle.ini");
}

Level Utils::readIniFile()
{
	wchar_t level[20];
	GetPrivateProfileString(L"WORDLE", L"LEVEL", L"EASY", level, 20, L"./wordle.ini");
	if (wcscmp(level, L"EASY") == 0)
		return Level::EASY;
	else if (wcscmp(level, L"MEDIUM") == 0)
		return Level::MEDIUM;
	else return Level::HARD;
}

void Utils::printWhiteText(HDC hdc, RECT rc, std::string text)
{
	const int fontSize = 24;
	HFONT letterFont = CreateFont(
		-MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72), // Height

		0,
		0,
		0,
		FW_BOLD,
		false,
		FALSE,
		0,
		EASTEUROPE_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T(" Verdana "));

	SetTextColor(hdc, RGB(255, 2552, 255));
	HFONT oldFont = (HFONT)SelectObject(hdc, letterFont);
	SelectObject(hdc, letterFont);
	SetBkMode(hdc, TRANSPARENT);

	wchar_t* t = new wchar_t[6];
	mbstowcs(t, text.c_str(), 5);
	text[5] = '\0';


	DrawText(hdc, t, 5, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	oldFont = (HFONT)SelectObject(hdc, oldFont);
	DeleteObject(oldFont);
	delete[] t;

}

std::wstring Utils::toLowerCase(std::wstring w)
{
	std::wstring ws;
	for (wchar_t c : w)
		ws.push_back(tolower(c));
	return ws;
}

