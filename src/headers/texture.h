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


/** this is hidden type for texture */
typedef void* aTexture;

/** create empty texture */
aTexture TextureCreate(const char* name);
/** load texture from file to OpenGL */
bool TextureLoadFromFile (aTexture texture, const char* fileName, bool buildMipMaps);
/**  create texture from file */
aTexture TextureCreateFromFile(const char* name, const char* fileName, bool buildMipMaps);
/** destroy texture and unload from OpenGL */
void TextureDestroy(aTexture* texture);

/* GETTERS */
/** get name of texture */
char* TextureGetName(aTexture texture);
/** get fileName of texture */
char* TextureGetFileName(aTexture texture);
/** get size of texture (as file) */
bool TextureGetSize(aTexture texture, uint16* width, uint16* height);
/** get width of texture (as file) */
uint16 TextureGetWidth(aTexture texture);
/** get height of texture (as file) */
uint16 TextureGetHeight(aTexture texture);
/** get texture-identificator in OpenGL */
uint32 TextureGetGLTextureID(aTexture texture);
/** return pointer if file already loaded to OpenGL */
aTexture TextureGetByFileName(const char* fileName);
/** get pointer to texture by name */
aTexture TextureGetByName(const char* name);

/** return true if void is texture */
bool ObjectIsTexture(void* object);
/* exit from function if pointer is invalid */
#define IS_TEXTURE_VALID(texture)        \
        if (!ObjectIsTexture(texture))   \
            return 0;

/* SETTERS */
/** set name of texture */
bool TextureSetName(aTexture texture, const char* name);

/** get count of loaded textures */
uint32 TexturesGetCount();
/** get count of loaded textures to OpenGL */
uint32 TexturesGetCountInOpenGL();
/**  clear textures list and unload from OpenGL */
void TexturesDestroyAll();


/*  TESTS!!! */
#ifdef _DEBUG
#include <assert.h>
void TextureTest();
#endif // _DEBUG
