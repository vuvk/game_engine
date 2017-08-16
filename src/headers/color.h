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

#include "types.h"

typedef struct
{
    uint8 r, g, b;
} RGBColor;
typedef struct
{
    uint8 r, g, b, a;
} RGBAColor;

typedef struct
{
    float r, g, b;
} RGBColorf;
typedef struct
{
    float r, g, b, a;
} RGBAColorf;

/* "constructors" */
RGBColor   NewRGBColor(uint8 r, uint8 g, uint8 b);
RGBColorf  NewRGBColorf(float r, float g, float b);
RGBAColor  NewRGBAColor(uint8 r, uint8 g, uint8 b, uint8 a);
RGBAColorf NewRGBAColorf(float r, float g, float b, float a);

/* equals */
bool IsEqualRGBColor(RGBColor col1, RGBColor col2);
bool IsEqualRGBColorf(RGBColorf col1, RGBColorf col2);
bool IsEqualRGBAColor(RGBAColor col1, RGBAColor col2);
bool IsEqualRGBAColorf(RGBAColorf col1, RGBAColorf col2);

/* math operations */
/* addition */
RGBColor   AddRGBColor(RGBColor col1, RGBColor col2);
RGBColorf  AddRGBColorf(RGBColorf col1, RGBColorf col2);
RGBAColor  AddRGBAColor(RGBAColor col1, RGBAColor col2);
RGBAColorf AddRGBAColorf(RGBAColorf col1, RGBAColorf col2);
/* subtract */
RGBColor   SubRGBColor(RGBColor col1, RGBColor col2);
RGBColorf  SubRGBColorf(RGBColorf col1, RGBColorf col2);
RGBAColor  SubRGBAColor(RGBAColor col1, RGBAColor col2);
RGBAColorf SubRGBAColorf(RGBAColorf col1, RGBAColorf col2);
/* multiplication by scalar */
RGBColor   MultRGBColor(RGBColor col, float scalar);
RGBColorf  MultRGBColorf(RGBColorf col, float scalar);
RGBAColor  MultRGBAColor(RGBAColor col, float scalar);
RGBAColorf MultRGBAColorf(RGBAColorf col, float scalar);
/* division by scalar */
RGBColor   DivRGBColor(RGBColor col, float scalar);
RGBColorf  DivRGBColorf(RGBColorf col, float scalar);
RGBAColor  DivRGBAColor(RGBAColor col, float scalar);
RGBAColorf DivRGBAColorf(RGBAColorf col, float scalar);
