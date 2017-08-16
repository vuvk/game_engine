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

#define MAX_KEY_LENGTH 64

typedef struct SDict_tag
{
    char key[MAX_KEY_LENGTH];
    void* value;

    struct SDict_tag* prev;
    struct SDict_tag* next;

} SDictElement;

typedef struct
{
    uint32 size;

    SDictElement* first;        /* head */
    SDictElement* last;         /* tail */
} SDictionary;

/** create dictionary and return pointer to list */
SDictionary* DictionaryCreate ();
/** clear all elements in dictionary */
void DictionaryClear (SDictionary* dictionary);
/** clear and destroy dictionary */
void DictionaryDestroy (SDictionary** dictionary);

/** add element to exists dictionary. return false, if key exists or value is NULL */
bool DictionaryAddElement (SDictionary* dictionary, const char* key, void* value);
/** delete element from dictionary */
bool DictionaryDeleteElement (SDictionary* dictionary, SDictElement* element);
/** delete element from dictionary by value */
void DictionaryDeleteElementByValue (SDictionary* dictionary, const void* value);
/** delete element from dictionary by key */
void DictionaryDeleteElementByKey (SDictionary* dictionary, const char* key);
/** delete element from dictionary by position in dictionary */
void DictionaryDeleteElementByNumber (SDictionary* dictionary, uint32 numOfElement);

/* GETTERS */
/** return element from dictionary by key */
SDictElement* DictionaryGetElementByKey (SDictionary* dictionary, const char* key);
/** return element from dictionary by value */
SDictElement* DictionaryGetElementByValue (SDictionary* dictionary, const void* value);
/** return element from dictionary by position in dictionary */
SDictElement* DictionaryGetElementByNumber (SDictionary* dictionary, uint32 numOfElement);

/** return value from dictionary by key */
void* DictionaryGetValueByKey (SDictionary* dictionary, const char* key);
/** return value from dictionary by position in dictionary */
void* DictionaryGetValueByNumber (SDictionary* dictionary, uint32 numOfElement);

/** return key from dictionary by position in dictionary */
char* DictionaryGetKeyByNumber (SDictionary* dictionary, uint32 numOfElement);
/** return key of value from dictionary */
char* DictionaryGetKeyByValue (SDictionary* dictionary, const void* value);

/** return position of key in dictionary (if exists), return -1 if error */
int32 DictionaryGetNumberByKey (SDictionary* dictionary, const char* key);
/** return position of value in dictionary (if exists), return -1 if error */
int32 DictionaryGetNumberByValue (SDictionary* dictionary, const void* value);

/** get count of elements in dictionary */
uint32 DictionaryGetSize (SDictionary* dictionary);


/* SETTERS */
/** set value of element by key */
void DictionarySetValueByKey(SDictionary* dictionary, const char* key, void* value);
/** set value of element by position in dictionary */
void DictionarySetValueByNumber(SDictionary* dictionary, uint32 numOfElement, void* value);

/** set key of element by position in dictionary. return false if key already exists */
bool DictionarySetKeyByNumber(SDictionary* dictionary, uint32 numOfElement, const char* key);
/** set key of element by value. return false if key already exists */
bool DictionarySetKeyByValue(SDictionary* dictionary, const void* value, const char* key);

/** change key of element. Return false if key not changed */
bool DictionaryChangeKey(SDictionary* dictionary, const char* oldKey, const char* newKey);
/** change value of element. Return false if value not changed */
bool DictionaryChangeValue(SDictionary* dictionary, const void* oldValue, void* newValue);


/** sort dictionary by key */
void DictionarySortByKey(SDictionary** dictionary);


/* tests */
#ifdef _DEBUG
//#define _DICTIONARY_PRINT_DEBUG
#include <assert.h>
#include "SDL2/SDL.h"
void DictionaryTest();
#endif // _DEBUG
