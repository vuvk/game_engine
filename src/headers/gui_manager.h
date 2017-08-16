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

/*
  Thanks to :
    Wingman (http://masandilov.ru/opengl/masking-ortho)
*/


#pragma once

#include "image.h"
#include "font.h"

/** before drawing 2d objects you need start 2d mode */
void GuiBegin();
/** after drawing 2d objects you need stop 2d mode */
void GuiEnd();

/* DRAWING */
/** draw image in position */
void GuiDrawImage(aImage image, float x, float y);

/** draw text in position */
void GuiDrawText(aFont font, float x, float y, const char* text);
