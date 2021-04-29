#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <fstream>

using std::wstring;
using std::to_wstring;

#define OPEN_FILE_BUTTON 1

//Variables globales
HDC hdc;
HMENU hMenu;
HINSTANCE hInst;
HWND hEdit;


// The main window cclass name.
static TCHAR szWindowClass[] = _T("DesktopAPP");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");


void AddMenus(HWND hWnd);
void AddControls(HWND hWnd);
void OpenFile(HWND hWnd);
void DisplayFile(wchar_t* path);

LRESULT CALLBACK DialogProcedure(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam

);

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow
)
{

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);
		return 1;
	}

	hInst = hInstance;

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		320, 240,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}

	ShowWindow(hWnd,
		nCmdShow);
	UpdateWindow(hWnd);

	// Main message loop:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CREATE:
		AddControls(hWnd);
		AddMenus(hWnd);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case OPEN_FILE_BUTTON:
			OpenFile(hWnd);
			break;
		case 3:
			TCHAR Instructions[] = _T("Presiona el boton para abrir un archivo y contar sus lineas de acuerdo con el estandar de Angel Bustillos");
			MessageBox(NULL, Instructions, _T("Instrucciones"), MB_OK);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void AddMenus(HWND hWnd)
{
	// Adding Menus Tutorial | Video #2
	hMenu = CreateMenu();
	HMENU hInstrucMenu = CreateMenu();

	AppendMenu(hInstrucMenu, MF_STRING, 3, _T("Instrucciones"));

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hInstrucMenu, _T("Ayuda"));

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd)
{
	CreateWindow(_T("Button"),
		_T("Open"),
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		10, 10,
		100, 25,
		hWnd, (HMENU)OPEN_FILE_BUTTON, NULL, NULL);
	hEdit = CreateWindow(_T("Edit"),
		_T(""),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE,
		10, 40,
		280, 130,
		hWnd, NULL, NULL, NULL);
}

void OpenFile(HWND hWnd)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	wchar_t fileName[256];

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = fileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = _T("Only CPP\0*.cpp\0");
	ofn.nFilterIndex = 1;

	GetOpenFileName(&ofn);
	DisplayFile(ofn.lpstrFile);
}

void DisplayFile(wchar_t* path)
{
	std::fstream file;
	file.open(path);

	if (!file.is_open())
	{
		MessageBox(NULL, _T("Error al cargar archivo"), _T("El archivo no se pudo abrir."), MB_ICONERROR);
		return;
	}

	int counter = 0;
	char line[256];
	int i = 0;
	unsigned int lineType = 0b0;

	file.getline(line, 256);

	do
	{
		i = 0;
		lineType = 0;
		while (line[i] != '\0') {
			if (line[i] == '/') {
				if (line[i] == line[i + 1]) {
					lineType = 0b0010;
					break;
				}
				else if (line[i] == '*'){
					do {
						file.getline(line, 256);
					} while (line[i] == '*' && line[i + 1] == '*');
					break;
				}
			}
			else if ((line[i] == '}') || (line[i] == ')')){
				lineType = 0b0100;
				break;
			}
			else if (line[i] != ' ') {
				lineType = 0b0001;
				break;
			}
			i++;
		}
		if (lineType == 0b0001) {
			counter++;
		}
		file.getline(line, 256);
	} while (!file.eof());

	wstring out = _T("Tu código tiene ");
	wstring data = to_wstring(counter);
	out += data;
	out += _T(" líneas físicas de código.");

	SetWindowText(hEdit, &out[0] );

	file.close();
}