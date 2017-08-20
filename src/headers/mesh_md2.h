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


typedef void* aMeshMd2;

/** create empty mesh */
aMeshMd2 MeshMd2Create(const char* name);
/** load mesh from file */
bool MeshMd2LoadFromFile (aMeshMd2 meshMd2, const char* fileName);
/**  create mesh from file */
aMeshMd2 MeshMd2CreateFromFile(const char* name, const char* fileName);
/** destroy mesh */
void MeshMd2Destroy(aMeshMd2* meshMd2);

/* GETTERS */
/** get name of mesh */
char* MeshMd2GetName(aMeshMd2 meshMd2);
/** get fileName of mesh */
char* MeshMd2GetFileName(aMeshMd2 meshMd2);
/** return pointer if file already loaded */
aMeshMd2 MeshMd2GetByFileName(const char* fileName);
/** get pointer to mesh by name */
aMeshMd2 MeshMd2GetByName(const char* name);

/* get transforms */
/** get pivot point of mesh */
bool MeshMd2GetPivotf(aMeshMd2 meshMd2, float* x, float* y, float* z);
SVector3f MeshMd2GetPivotv(aMeshMd2 meshMd2);
float MeshMd2GetPivotX(aMeshMd2 meshMd2);
float MeshMd2GetPivotY(aMeshMd2 meshMd2);
float MeshMd2GetPivotZ(aMeshMd2 meshMd2);
/** get position of mesh */
bool MeshMd2GetPosf(aMeshMd2 meshMd2, float* x, float* y, float* z);
SVector3f MeshMd2GetPosv(aMeshMd2 meshMd2);
float MeshMd2GetPosX(aMeshMd2 meshMd2);
float MeshMd2GetPosY(aMeshMd2 meshMd2);
float MeshMd2GetPosZ(aMeshMd2 meshMd2);
/** get scale of mesh */
bool MeshMd2GetScalef(aMeshMd2 meshMd2, float* x, float* y, float* z);
SVector3f MeshMd2GetScalev(aMeshMd2 meshMd2);
float MeshMd2GetScaleX(aMeshMd2 meshMd2);
float MeshMd2GetScaleY(aMeshMd2 meshMd2);
float MeshMd2GetScaleZ(aMeshMd2 meshMd2);
/** get rotation of mesh */
bool MeshMd2GetRotation(aMeshMd2 meshMd2, float* pitch, float* yaw, float* roll);
float MeshMd2GetPitch(aMeshMd2 meshMd2);
float MeshMd2GetYaw(aMeshMd2 meshMd2);
float MeshMd2GetRoll(aMeshMd2 meshMd2);


/** return true if void is md2 mesh */
bool ObjectIsMeshMd2(void* object);
/* exit from function if pointer is invalid */
#define IS_MESHMD2_VALID(meshMd2)        \
        if (!ObjectIsMeshMd2(meshMd2))   \
            return 0;


/* SETTERS */
/** set name of mesh */
bool MeshMd2SetName(aMeshMd2 meshMd2, const char* name);

/* set transforms */
/** set pivot point of mesh */
bool MeshMd2SetPivotf(aMeshMd2 meshMd2, float x, float y, float z);
bool MeshMd2SetPivotv(aMeshMd2 meshMd2, SVector3f pivot);
bool MeshMd2SetPivotX(aMeshMd2 meshMd2, float x);
bool MeshMd2SetPivotY(aMeshMd2 meshMd2, float y);
bool MeshMd2SetPivotZ(aMeshMd2 meshMd2, float z);
/** set position of mesh */
bool MeshMd2SetPosf(aMeshMd2 meshMd2, float x, float y, float z);
bool MeshMd2SetPosv(aMeshMd2 meshMd2, SVector3f position);
bool MeshMd2SetPosX(aMeshMd2 meshMd2, float x);
bool MeshMd2SetPosY(aMeshMd2 meshMd2, float y);
bool MeshMd2SetPosZ(aMeshMd2 meshMd2, float z);
/** set scale of mesh */
bool MeshMd2SetScalef(aMeshMd2 meshMd2, float x, float y, float z);
bool MeshMd2SetScalev(aMeshMd2 meshMd2, SVector3f scale);
bool MeshMd2SetScaleX(aMeshMd2 meshMd2, float x);
bool MeshMd2SetScaleY(aMeshMd2 meshMd2, float y);
bool MeshMd2SetScaleZ(aMeshMd2 meshMd2, float z);
/** set rotation of mesh. Angle in degrees */
bool MeshMd2SetRotationInDeg(aMeshMd2 meshMd2, float pitch, float yaw, float roll);
bool MeshMd2SetPitchInDeg(aMeshMd2 meshMd2, float pitch);
bool MeshMd2SetYawInDeg(aMeshMd2 meshMd2, float yaw);
bool MeshMd2SetRollInDeg(aMeshMd2 meshMd2, float roll);
/** set rotation of mesh. Angle in radians */
bool MeshMd2SetRotationInRad(aMeshMd2 meshMd2, float pitch, float yaw, float roll);
bool MeshMd2SetPitchInRad(aMeshMd2 meshMd2, float pitch);
bool MeshMd2SetYawInRad(aMeshMd2 meshMd2, float yaw);
bool MeshMd2SetRollInRad(aMeshMd2 meshMd2, float roll);


/** get count of loaded meshes */
uint32 MeshesMd2GetCount();
/**  clear meshes list */
void MeshesMd2DestroyAll();
