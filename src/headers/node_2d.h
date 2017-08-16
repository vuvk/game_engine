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
#include "list.h"
#include "types.h"
#include "image.h"
#include "text.h"

typedef void* aNode2D;


/** create an empty node2d */
aNode2D Node2DCreate(const char* name);
/** destroy node2d */
void Node2DDestroy(aNode2D* node2D);
/** clear all links to used images */
bool Node2DClearAllImageLinks(aNode2D node2D);
/** clear all links to used texts */
bool Node2DClearAllTextLinks(aNode2D node2D);

/* manipulation with resources */
/** add link to image for using in node. return true if all's OK */
bool Node2DAddLinkToImage(aNode2D node2D, aImage image);
/** add link to text for using in node. return true if all's OK */
bool Node2DAddLinkToText(aNode2D node2D, aText text);
/** remove link to image from node */
bool Node2DDeleteLinkToImage(aNode2D node2D, aImage image);
/** remove link to image from node by number */
bool Node2DDeleteLinkToImageByNumber(aNode2D node2D, uint32 numOfImage);
/** remove link to text from node */
bool Node2DDeleteLinkToText(aNode2D node2D, aText text);
/** remove link to text from node by number */
bool Node2DDeleteLinkToTextByNumber(aNode2D node2D, uint32 numOfText);

/* GETTERS */
/** get name of node */
const char* Node2DGetName(aNode2D node2D);
/** get all pointers to results by name */
void Nodes2DGetByName(const char* name, SList* results);
/** get visibility of node */
bool Node2DIsVisible(aNode2D node2D);

/* get transforms */
/** get pivot point of node */
bool Node2DGetPivot(aNode2D node2D, float* x, float* y);
SVector2f Node2DGetPivotv(aNode2D node2D);
float Node2DGetPivotX(aNode2D node2D);
float Node2DGetPivotY(aNode2D node2D);
/** get position of node */
bool Node2DGetPos(aNode2D node2D, float* x, float* y);
SVector2f Node2DGetPosv(aNode2D node2D);
float Node2DGetPosX(aNode2D node2D);
float Node2DGetPosY(aNode2D node2D);
/** get scale of node */
bool Node2DGetScale(aNode2D node2D, float* x, float* y);
SVector2f Node2DGetScalev(aNode2D node2D);
float Node2DGetScaleX(aNode2D node2D);
float Node2DGetScaleY(aNode2D node2D);
/** get rotation of node */
float Node2DGetRotation(aNode2D node2D);

/** get first image used in node */
#define Node2DGetImage(node2D) Node2DGetImageByNumber(node2D, 0)
/** get image by number used in node */
aImage Node2DGetImageByNumber(aNode2D node2D, uint32 numOfImage);
/** get first text used in node */
#define Node2DGetText(node2D) Node2DGetTextByNumber(node2D, 0)
/** get text by number used in node */
aText Node2DGetTextByNumber(aNode2D node2D, uint32 numOfText);
/** get position of image in images list */
uint32 Node2DGetNumberOfImage(aNode2D node2D, aImage image);
/** get position of text in texts list */
uint32 Node2DGetNumberOfText(aNode2D node2D, aText text);
/** get count of images used in node */
uint32 Node2DGetCountOfImageLinks(aNode2D node2D);
/** get count of texts used in node */
uint32 Node2DGetCountOfTextLinks(aNode2D node2D);

/** return true if pointer is node2d */
bool ObjectIsNode2D(void* object);
/* exit from function if pointer is invalid */
#define IS_NODE2D_VALID(node)        \
        if (!ObjectIsNode2D(node))   \
            return 0;

/* SETTERS */
/** set name of node2d */
bool Node2DSetName(aNode2D node2D, const char* name);
/** set visibility of node */
bool Node2DSetVisible(aNode2D node2D, bool isVisible);

/* set transforms */
/** set pivot point of node */
bool Node2DSetPivot(aNode2D node2D, float x, float y);
bool Node2DSetPivotv(aNode2D node2D, SVector2f pivot);
bool Node2DSetPivotX(aNode2D node2D, float x);
bool Node2DSetPivotY(aNode2D node2D, float y);
/** set position of node */
bool Node2DSetPos(aNode2D node2D, float x, float y);
bool Node2DSetPosv(aNode2D node2D, SVector2f position);
bool Node2DSetPosX(aNode2D node2D, float x);
bool Node2DSetPosY(aNode2D node2D, float y);
/** set scale of node */
bool Node2DSetScale(aNode2D node2D, float x, float y);
bool Node2DSetScalev(aNode2D node2D, SVector2f scale);
bool Node2DSetScaleX(aNode2D node2D, float x);
bool Node2DSetScaleY(aNode2D node2D, float y);
/** set rotation of node. Angle in degrees */
bool Node2DSetRotationInDeg(aNode2D node2D, float angle);
/** set rotation of node. Angle in radians */
bool Node2DSetRotationInRad(aNode2D node2D, float angle);


/** get count of created nodes2d */
uint32 Nodes2DGetCount();
/**  clear node2d list */
void Nodes2DDestroyAll();
