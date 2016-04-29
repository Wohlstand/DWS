#pragma once
#include "stdafx.h"

#define POINT_LOC_IN_RECT(_RECT_, _POINT_) (_RECT_.left < _POINT_.x && _RECT_.right > _POINT_.x && _RECT_.top < _POINT_.y && _RECT_.bottom > _POINT_.y)

enum TextPosition
{
	LEFT = 0x00000000L,
	CENTER = 0x00000001L,
	RIGHT = 0x00000002L
};

VOID SetTextAlignment(HWND textHwnd, TextPosition pos);
RECT GetLocalCoordinates(HWND hWnd);
POINT GET_POINT_LPARAM(LPARAM lp);