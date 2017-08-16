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


/** material types */
#define MT_DEFAULT        0
#define MT_FLAT_SHADING   1
#define MT_ALPHA_BLENDED  2
#define MT_POINT_FILTER   3      // oldschool filter

/** which side NOT draw (CULLING FACE)*/
#define CF_NONE   0
#define CF_BACK   1
#define CF_FRONT  2
#define CF_FRONT_AND_BACK 3

#define MAX_MATERIAL_LAYERS 4

/** this is hidden type for material */
typedef void* aMaterial;

/** create empty material */
aMaterial MaterialCreate(const char* name);
/** destroy material */
void MaterialDestroy(aMaterial* material);

/** apply material's properties */
bool MaterialApply(aMaterial material);

/* GETTERS */
/** get name of material */
const char* MaterialGetName(aMaterial material);
/** get pointer to texture in default layer (0) */
#define MaterialGetTexture(material) MaterialGetTextureEx(material, 0)
/** get pointer to texture in layer (0..MAX_MATERIAL_LAYERS - 1) */
aTexture MaterialGetTextureEx(aMaterial material, uint8 layer);
/** get culling type. return -1 if error */
int8 MaterialGetCullFace(aMaterial material);
/** get material type. return -1 if error */
int8 MaterialGetType(aMaterial material);
/** get pointer to glProgram */
void* MaterialGetGLProgram(aMaterial material);
/** get texture-identificator for material from OpenGL in first layer */
#define MaterialGetGLTextureID(material) MaterialGetGLTextureIDEx(material, 0)
/** get texture-identificator for material from OpenGL in custom layer (use 0..MAX_MATERIAL_LAYERS - 1) */
uint32 MaterialGetGLTextureIDEx(aMaterial material, uint8 layer);
/** get pointer to material by name */
aMaterial MaterialGetByName(const char* name);

/** return true if void is material */
bool ObjectIsMaterial(void* object);
/* exit from function if pointer is invalid */
#define IS_MATERIAL_VALID(material)        \
        if (!ObjectIsMaterial(material))   \
            return 0;

/* SETTERS */
/** set name of material */
bool MaterialSetName(aMaterial material, const char* name);
/** set texture for material (first layer) */
#define MaterialSetTexture(material, texture) MaterialSetTextureEx(material, texture, 0)
/** set texture for material in custom layer (use 0..MAX_MATERIAL_LAYERS - 1) */
bool MaterialSetTextureEx(aMaterial material, aTexture texture, uint8 layer);
/** set culling type */
bool MaterialSetCullFace(aMaterial material, int8 cullFace);
/** set material type. return -1 if error */
bool MaterialSetType(aMaterial material, int8 type);
/** set pointer to glProgram */
bool MaterialSetGLProgram(aMaterial material, void* glProgram);

/** get count of created materials */
uint32 MaterialsGetCount();
/**  clear materials list */
void MaterialsDestroyAll();




/* TESTS! */
#ifdef _DEBUG
#include <assert.h>
#include "texture.h"
void MaterialTest();
#endif // _DEBUG
