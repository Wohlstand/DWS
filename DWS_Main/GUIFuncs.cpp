#include "GUIFuncs.h"

VOID SetTextAlignment(HWND textHwnd, TextPosition pos)
{
	LONG new_style = pos;
	if (new_style == SS_LEFT) new_style = SS_LEFTNOWORDWRAP;
	auto s = GetWindowLongPtr(textHwnd, GWL_STYLE);
	s = (s & ~(SS_LEFT | SS_CENTER | SS_RIGHT | SS_LEFTNOWORDWRAP)) | new_style;
	SetWindowLongPtr(textHwnd, GWL_STYLE, s);
	InvalidateRect(textHwnd, NULL, TRUE);
}

RECT GetLocalCoordinates(HWND hWnd)
{
	RECT Rect;
	GetWindowRect(hWnd, &Rect);
	MapWindowPoints(HWND_DESKTOP, GetParent(hWnd), (LPPOINT)&Rect, 2);
	return Rect;
}

POINT GET_POINT_LPARAM(LPARAM lp)
{
	POINT cor;
	cor.x = ((int)(short)LOWORD(lp));
	cor.y = ((int)(short)HIWORD(lp));
	return cor;
}