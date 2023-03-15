#pragma once
#include <Windows.h>

#define MAX_LOADSTRING 100
#define TILE_SIZE 55
#define BREAK 6

enum class Level { EASY, MEDIUM, HARD };
enum class LetterStatus { EMPTY, INCORRECT, NOT_IN_PLACE, CORRECT };

struct Buffer
{
	HDC offDC;
	HBITMAP offOldBitmap;
	HBITMAP offBitmap;
};