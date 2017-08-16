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

/** initialize resource manager */
bool RM_Init();
/** return OpenGL ID of loaded texture. -1 if error.
    write to imgWidth and imgHeight size of image in pixels */
int32 RM_LoadGLTexture (const char* fileName, int32* imgWidth, int32* imgHeight, bool buildMipMaps);
/** load md2 mesh from file (buffer) to MeshMD2 class */
bool RM_LoadMeshMd2(const char* fileName, void** meshMd2);
/** load bsp mesh from file (buffer) to MeshBsp class */
bool RM_LoadMeshBsp(const char* fileName, void** meshBsp);
