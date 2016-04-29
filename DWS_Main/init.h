#pragma once

#include "stdafx.h"

static STR szAppName[] = TEXT("DWSExtended");
String DWSCation = TEXT("DWS Extended Pre-Alpha");
int _iCmdShow = 0;

#define CreateLogBox(hWnd, xPos,yPos,xUnit,yUnit,ID) CreateWindow(TEXT("EDIT"), 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY, xPos, yPos, xUnit, yUnit, hWnd, (HMENU)ID, (HINSTANCE)GetModuleHandle(NULL), 0)
#define CreateButton(hWnd, xPos, yPos, xUnit, yUnit, ID, Text) 	CreateWindow(TEXT("BUTTON"), Text, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, xPos, yPos, xUnit, yUnit, hWnd, (HMENU)ID, (HINSTANCE)GetModuleHandle(NULL), 0);
#define CreateStatic(hWnd, Text, xPos, yPos, xUint, yUint, ID) CreateWindow(TEXT("static"), Text, WS_CHILD | WS_VISIBLE | WS_TABSTOP, xPos, yPos, xUint, yUint, hWnd, (HMENU)ID, (HINSTANCE)GetModuleHandle(NULL), NULL);

enum Elements
{
	MainButton = 5000,
	StatusBar,
	LogBox
};

typedef struct
{
	POINT _MinSize;
	POINT _MaxSize;
} mWindowSize;