
#include <string>
#include <ctime>
#include <cctype>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <tchar.h>
#include <vector>
#include <codecvt>
#include <locale>
#include <map>
#include <set>
#include "framework.h"
#include "tutorial.h"
#include "Game.h"
#include "Utils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	srand(time(NULL));
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	Utils::initData();
	Game G(hInstance, nCmdShow);

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TUTORIAL));
	MSG msg;


	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Utils::deleteData();
	return static_cast<int>(msg.wParam);
}