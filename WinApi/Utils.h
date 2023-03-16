#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <set>
#include "enums.h"



class Utils
{
public:
	Utils() = delete;
	Utils(const Utils& U) = delete;
	Utils(const Utils&& U) = delete;

	static HBRUSH brushes[4];
	static HPEN pens[4];
	static COLORREF colors[4];

	static void initData();
	static void deleteData();
	static void drawLetter(HDC hdc, RECT rc, wchar_t c, int bkndType, COLORREF* colors);
	static std::string getRandomWord(const std::set<std::string>& dict);
	static bool isInDictionary(const std::set<std::string>& dict, std::wstring word);
	static bool containsLetter(std::string word, wchar_t c);
	static std::set<std::string> readAndParseWordFile(std::string filename);
	static void writeToIniFile(Level L);
	static Level readIniFile();
	static void printWhiteText(HDC hdc, RECT rc, std::string text);
	static std::wstring toLowerCase(std::wstring w);
};

