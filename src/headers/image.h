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
#include "texture.h"
#include "material.h"

/** this is hidden type for 2d image */
typedef void* aImage;


/** create empty image */
aImage ImageCreate(const char* name);
/** destroy image */
void ImageDestroy(aImage* image);

/* GETTERS */
/** get name of image */
char* ImageGetName(aImage image);
/** get pivot point of image */
bool ImageGetPivot(aImage image, float* x, float* y);
SVector2f ImageGetPivotv(aImage image);
float ImageGetPivotX(aImage image);
float ImageGetPivotY(aImage image);
/** get size of image */
bool ImageGetSize(aImage image, uint16* width, uint16* height);
uint16 ImageGetWidth(aImage image);
uint16 ImageGetHeight(aImage image);
/** get pointer to texture */
aTexture ImageGetTexture(aImage image);
/** get pointer to material */
aMaterial ImageGetMaterial(aImage image);
/** get pointer to image by name */
aImage ImageGetByName(const char* name);
/** get texture-identificator in OpenGL */
uint32 ImageGetGLTextureID(aImage image);

/** return true if void is image */
bool ObjectIsImage(void* object);
/* exit from function if pointer is invalid */
#define IS_IMAGE_VALID(image)        \
        if (!ObjectIsImage(image))   \
            return 0;

/* SETTERS */
/** set name of image */
bool ImageSetName(aImage image, const char* name);
/** set pivot point of image */
bool ImageSetPivot(aImage image, float x, float y);
bool ImageSetPivotv(aImage image, SVector2f pivot);
bool ImageSetPivotX(aImage image, float x);
bool ImageSetPivotY(aImage image, float y);
/** set size of image */
bool ImageSetWidth(aImage image, uint16 width);
bool ImageSetHeight(aImage image, uint16 height);
bool ImageSetSize(aImage image, uint16 width, uint16 height);
/** set texture for image */
bool ImageSetTexture(aImage image, aTexture texture);
/** set material for image */
bool ImageSetMaterial(aImage image, aMaterial material);

/** get count of created images */
uint32 ImagesGetCount();
/**  clear images list */
void ImagesDestroyAll();

/** draw image in position (0, 0) */
void ImageDraw(aImage image);


/*  TESTS!!! */
#ifdef _DEBUG
#include <assert.h>
void ImageTest();
#endif // _DEBUG
