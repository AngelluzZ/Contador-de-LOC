#include <windows.h>
#include <stdlib.h>
#include <string>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include "list.h"
#include "node.h"

using namespace std;

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

//Prototipos
void contador(Node* Inicio, int* total, int* numeroFunciones, wchar_t* path);
void AddMenus(HWND hWnd);
void AddControls(HWND hWnd);
void OpenFile(HWND hWnd);
void DisplayFile(wchar_t* path);

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);

int CALLBACK WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR     lpCmdLine, _In_ int       nCmdShow){

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

	if (!RegisterClassEx(&wcex)){
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

	if (!hWnd){
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
	while (GetMessage(&msg, NULL, 0, 0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){

	switch (message){
	case WM_CREATE:
		AddControls(hWnd);
		AddMenus(hWnd);
		break;
	case WM_COMMAND:
		switch (wParam){
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

void AddMenus(HWND hWnd){
	// Adding Menus Tutorial | Video #2
	hMenu = CreateMenu();
	HMENU hInstrucMenu = CreateMenu();

	AppendMenu(hInstrucMenu, MF_STRING, 3, _T("Instrucciones"));

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hInstrucMenu, _T("Ayuda"));

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd){
	CreateWindow(_T("Button"),
		_T("Open"),
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		10, 10,
		100, 25,
		hWnd, (HMENU)OPEN_FILE_BUTTON, NULL, NULL);
	hEdit = CreateWindow(_T("Edit"),
		_T(""),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL,
		10, 40,
		280, 130,
		hWnd, NULL, NULL, NULL);
}

void OpenFile(HWND hWnd){
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

void DisplayFile(wchar_t* path){
	
	int total, numeroFunciones=0;
	Node* Inicio = CrearNode();
	Node* ptr = Inicio;

	contador(Inicio, &total, &numeroFunciones, path);

	wstring out = _T("Total de LOC: ");
	wstring data = to_wstring(total);
	out += data;
	
	for (int i = 0; i < numeroFunciones; i++) {
		ptr = ptr->next;
		out += _T("\r\nFuncion: ");
		out += (ptr->data.nombre);
		out += _T("  Total de LOC: ");
		data = to_wstring(ptr->data.numeroLineas);
		out += data;
	}

	ptr = Inicio;
	while (ptr->next != NULL)
	{
		TIPO datos;
		if (eliminaNodoLista(ptr, &datos))
		exit(EXIT_FAILURE);
	}
	EliminaNode(Inicio);

	SetWindowText(hEdit, &out[0] );


}

void contador(Node* Inicio, int * total, int* numeroFunciones, wchar_t* path) {
	
	//abrir archivo
	fstream archivo;
	archivo.open(path, fstream::in);
	

	string linea;
	int i, lineasFuncion, llaves, tamanoNombre;
	int lineasTotales = 0,  comentario;
	string aux;
	char* nombreFuncion;
	unsigned int tipoLinea = 0b0;
	Node* ptr = Inicio;
	TIPO datosFuncion;
	//leer primera linea
	getline(archivo, linea);

	while (!archivo.eof()) {
		comentario = linea.find("//");
		//Es funcion ?
		if (linea.find("(") !=  string::npos && linea.find(")") != string::npos && linea.find("{") != string::npos && linea.find("while") == string::npos && linea.find("if") == string::npos && linea.find("for(") == string::npos) {
			*numeroFunciones = *numeroFunciones + 1;

			aux = linea;
			tamanoNombre = linea.find("(");
			i = tamanoNombre;
			aux.erase(i, linea.size());
			i--;
			while (linea[i] != ' '){
				i--;
			}
			aux.erase(0,i);
			nombreFuncion = &aux[0];

			size_t newsize = strlen(nombreFuncion) + 1;
			wchar_t* wcstring = new wchar_t[newsize];

			// Convert char* string to a wchar_t* string.
			size_t convertedChars = 0;
			mbstowcs_s(&convertedChars, wcstring, newsize, nombreFuncion, _TRUNCATE);
			// Display the result and indicate the type of string that it is.
			wcout << wcstring << _T(" (wchar_t *)") << endl;
			datosFuncion.nombre = wcstring;


			llaves = 1; // hay una llave
			lineasFuncion = 1;
			while (llaves != 0) {

				getline(archivo, linea);
				i = 0;

				if (comentario != string::npos) { // tiene comentario
					if (linea.find("{") != string::npos && linea.find("{") < comentario ) {
						lineasFuncion++;
						lineasTotales++;
						llaves++;
					}
					if (linea.find(";") != string::npos && linea.find(";") < comentario) {
						lineasFuncion++;
						lineasTotales++;
					}
					else if (linea.find("}") != string::npos && linea.find("}") < comentario) {
						llaves--;
					}
				}
				else {
					if (linea.find("{") != string::npos) {
						lineasFuncion++;
						lineasTotales++;
						llaves++;
					}
					else if (linea.find(";") != string::npos ) {
						lineasFuncion++;
						lineasTotales++;
					}
					if (linea.find("}") != string::npos) {
						llaves--;
					}
				}
			}

			datosFuncion.numeroLineas = lineasFuncion;
			if (!insertNodoLista(ptr, datosFuncion)) {
				ptr = ptr->next;
			}
			else {
				exit(EXIT_FAILURE);
			}
		}
		else if(linea.find("/*") != string::npos){ //Es banner ?
			do {
				getline(archivo, linea);
					lineasTotales++;
			} while (linea.find("*/") == string::npos);
		}
		else if (comentario != string::npos) { // tiene comentario ?
			if (linea.find("{") != string::npos && linea.find("{") < comentario) {
				lineasTotales++;
			}
			else if (linea.find("#") != string::npos && linea.find("#") < comentario) {
				lineasTotales++;
			}
			else if (linea.find(";") != string::npos && linea.find(";") < comentario) {
				lineasTotales++;
			}
		}
		else {
			if (linea.find("{") != string::npos) {
				lineasTotales++;
			}
			else if (linea.find("#") != string::npos) {
				lineasTotales++;
			}
			else if (linea.find(";") != string::npos) {
				lineasTotales++;
			}
		}

		getline(archivo, linea);
	}

	*total = lineasTotales;

	archivo.close();

	return;
}
