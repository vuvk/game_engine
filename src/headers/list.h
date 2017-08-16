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
#include "types.h"

typedef struct SList_tag
{
    void* value;

    struct SList_tag* prev;
    struct SList_tag* next;
} SListElement;

typedef struct
{
    uint32 size;

    SListElement* first;        /* head */
    SListElement* last;         /* tail */
} SList;

/** create list and return pointer to list */
SList* ListCreate ();
/** delete all elements in list */
void ListClear (SList* list);
/** clear and destroy list */
void ListDestroy (SList** list);

/** add value to exists list, return false if not */
bool ListAddElement (SList* list, void* value);
/** delete element from list */
bool ListDeleteElement (SList* list, SListElement* element);
/** delete value from list */
void ListDeleteElementByValue (SList* list, void* value);
/** delete value from list by position in list */
void ListDeleteElementByNumber (SList* list, uint32 numOfElement);

/* GETTERS */
/** return element by value */
SListElement* ListGetElementByValue (SList* list, const void* value);
/** return element by number in list */
SListElement* ListGetElementByNumber (SList* list, uint32 numOfElement);
/** return value by number in list */
void* ListGetValueByNumber (SList* list, uint32 numOfElement);
/** return number of value in list (if exists), else return -1 */
int32 ListGetNumberByValue (SList* list, void* value);

/** get count of elements in list */
uint32 ListGetSize (SList* list);

/* SETTERS */
/** set value of element by position in dictionary */
void ListSetValueByNumber(SList* list, uint32 numOfElement, void* value);
/** change value of element. Return false if value not changed */
bool ListChangeValue(SList* list, const void* oldValue, void* newValue);


/* tests */
#ifdef _DEBUG
//#define _LIST_PRINT_DEBUG
#include <assert.h>
#include "SDL2/SDL.h"
void ListTest();
#endif // _DEBUG
