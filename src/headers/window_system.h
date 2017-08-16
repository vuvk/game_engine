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

typedef enum
{
    wsNotCreated,
    wsCreated
} EWindowState;

/** window "constructor" */
int WindowCreate (int32 windowWidth, int32 windowHeight, bool resizable, bool fullScreenMode);
/** window "destructor" */
void WindowDestroy();

// windows setters
void WindowSetSize (int32 windowWidth, int32 windowHeight);
void WindowSetTitle (const char* windowTitle);
void WindowSetFullScreenMode (bool fullScreenMode, bool useDesktopResolution);
//void EngineSetWindowResizable (bool isResizable);

//windows getters
void WindowGetSize (int32* windowWidth, int32* windowHeight);
const char* WindowGetTitle();
int WindowGetWidth();
int WindowGetHeight();

// system
void WindowUpdateState();
EWindowState WindowGetState();
SDL_Window* WindowGetPointer();
