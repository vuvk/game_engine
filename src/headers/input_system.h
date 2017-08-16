/*
	It's part of bodhi GameEngine.
    Copyright (C) 2017 Anton "Vuvk" Shcherbatykh

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "SDL2/SDL.h"

#include "types.h"
#include "vector.h"

/** Update state of input. Use this on every step! */
void InputUpdateState();

/*----------*/
/* KEYBOARD */
/*----------*/
/** Is any key down? */
bool InputIsKeyDown();
/** Is any key up? */
bool InputIsKeyUp();
/** Is this key pressed? */
bool InputIsKeyPressed(SDL_Keycode keyCode);
/** Return code of last pressed key */
SDL_Keycode InputGetLastKeyCode();
/** Return code of last pressed key */
SDL_Scancode InputGetLastScanCode();
/** Clear table of pressed keys */
void InputClearTableOfPressedKeys();

/*-------*/
/* MOUSE */
/*-------*/
/** mouse moving now? */
bool InputIsMouseMove();
/** Is any mouse button down? */
bool InputIsMouseButtonDown();
/** Is any mouse button up? */
bool InputIsMouseButtonUp();
/** get mouse position */
SVector2i InputGetMousePos();
int32 InputGetMousePosX();
int32 InputGetMousePosY();
/** Is mouse wheel? */
bool InputIsMouseWheel();
/** get mouse wheel direction */
int32 InputGetMouseWheelDir();
bool InputIsMouseWheelUp();
bool InputIsMouseWheelDown();
/** set mouse position */
void InputSetMousePosi(int32 mousePositionX, int32 mousePositionY);
void InputSetMousePosv(SVector2i mousePosition);
void InputSetMousePosX(int32 mousePositionX);
void InputSetMousePosY(int32 mousePositionY);
/* get mouse button state */
/** get last pressed button */
bool InputIsMouseButtonLeftClicked();
bool InputIsMouseButtonRightClicked();
bool InputIsMouseButtonMiddleClicked();
/** what button is held down */
bool InputIsMouseButtonLeftHeld();
bool InputIsMouseButtonRightHeld();
bool InputIsMouseButtonMiddleHeld();
