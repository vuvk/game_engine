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
#include "font.h"

typedef void* aText;

/** create text class. Set text param as NULL if you want generate static text */
aText TextCreate(const char* name, aFont font, const char* string);
/** destroy text */
void TextDestroy(aText* text);

/* GETTERS */
/** get name of text */
char* TextGetName(aText text);
/** get pointer to font */
aFont TextGetFont(aText text);
/** get dynamic text from class */
char* TextGetString(aText text);
/** get pivot point of text */
bool TextGetPivot(aText text, float* x, float* y);
SVector2f TextGetPivotv(aText text);
float TextGetPivotX(aText text);
float TextGetPivotY(aText text);
/** get pointer to text by name */
aText TextGetByName(const char* name);

/** return true if void is text */
bool ObjectIsText(void* object);
/* exit from function if pointer is invalid */
#define IS_TEXT_VALID(text)        \
        if (!ObjectIsText(text))   \
            return 0;

/* SETTERS */
/** set name of text */
bool TextSetName(aText text, const char* name);
/** set font */
bool TextSetFont(aText text, aFont font);
/** set dynamic text */
bool TextSetString(aText text, const char* string);
/** set static text - fast draw */
bool TextSetStringStatic(aText text, const char* string);
/** set pivot point of text */
bool TextSetPivot(aText text, float x, float y);
bool TextSetPivotv(aText text, SVector2f pivot);
bool TextSetPivotX(aText text, float x);
bool TextSetPivotY(aText text, float y);


/** get count of created texts */
uint32 TextsGetCount();
/**  clear texts list */
void TextsDestroyAll();

/** draw text in position (0, 0) */
void TextDraw(aText text);



/*  TESTS!!! */
#ifdef _DEBUG
#include <assert.h>
void TextTest();
#endif // _DEBUG
