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

#include "defines.h"
#include "texture.h"

typedef void* aFont;

/** create empty font */
aFont FontCreate(const char* name);
/** destroy font */
void FontDestroy(aFont* font);

/** generate font from loaded texture */
bool FontGenerateFromTexture(aFont font, aTexture texture,
                             uint16 xCount,  uint16 yCount,
                             int16  xOffset, int16  yOffset);

/* GETTERS */
/** get name of font */
const char* FontGetName(aFont font);
/** get pointer to texture */
aTexture FontGetTexture(aFont font);
/** get texture in OpenGL */
uint32 FontGetGLTextureID(aFont font);
/** get pointer to font by name */
aFont FontGetByName(const char* name);
/** get width of char in pixels */
uint16 FontGetCharWidth(aFont font);
/** get height of char in pixels */
uint16 FontGetCharHeight(aFont font);

/** return true if pointer is font */
bool ObjectIsFont(void* object);
/* exit from function if pointer is invalid */
#define IS_FONT_VALID(font)        \
        if (!ObjectIsFont(font))   \
            return 0;

/* SETTERS */
/** set name of font */
bool FontSetName(aFont font, const char* name);

/** get count of created fonts */
uint32 FontsGetCount();
/**  clear fonts list */
void FontsDestroyAll();



/*  TESTS!!! */
#ifdef _DEBUG
#include <assert.h>
void FontTest();
#endif // _DEBUG
