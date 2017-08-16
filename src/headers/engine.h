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

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

/** state of engine */
typedef enum
{
    esNotRunning,
    esRunning
} EEngineState;

/** start Antoshka Engine and initialize all subsystems */
int EngineStart (int32 windowWidth, int32 windowHeight, bool resizable, bool fullScreenMode);
/** stop Antoshka Engine */
void EngineStop();

void EngineUpdateTime();
void EngineUpdateFPS();

// timing
void EngineSetLimitFPS (uint16 limitFPS);
uint16 EngineGetLimitFPS();
uint16 EngineGetFPS();
float EngineGetDeltaTime();

// system
EEngineState EngineGetStatus();

const char* EngineGetName();
const char* EngineGetVersion();
