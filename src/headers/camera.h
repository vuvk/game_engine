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

/*
    Thanks to :
        Wingman (http://masandilov.ru/opengl)
*/

#pragma once

#include "types.h"



typedef void* aCamera;

/** create a camera */
aCamera CameraCreate(const char* name);
/** destroy a camera */
void CameraDestroy(aCamera* camera);

/* GETTERS */
/** get name of camera */
const char* CameraGetName(aCamera camera);
/* get transforms */
/** get position of camera */
bool CameraGetPosf(aCamera camera, float* x, float* y, float* z);
SVector3f CameraGetPosv(aCamera camera);
float CameraGetPosX(aCamera camera);
float CameraGetPosY(aCamera camera);
float CameraGetPosZ(aCamera camera);
/** get target of camera */
bool CameraGetTargetf(aCamera camera, float* x, float* y, float* z);
SVector3f CameraGetTargetv(aCamera camera);
float CameraGetTargetX(aCamera camera);
float CameraGetTargetY(aCamera camera);
float CameraGetTargetZ(aCamera camera);
/** get up vector of camera */
bool CameraGetUpVectorf(aCamera camera, float* x, float* y, float* z);
SVector3f CameraGetUpVectorv(aCamera camera);
float CameraGetUpVectorX(aCamera camera);
float CameraGetUpVectorY(aCamera camera);
float CameraGetUpVectorZ(aCamera camera);


/** return true if pointer is camera */
bool ObjectIsCamera(void* object);
/* exit from function if pointer is invalid */
#define IS_CAMERA_VALID(camera)        \
        if (!ObjectIsCamera(camera))   \
            return 0;


/* SETTERS */
/** set name of camera */
bool CameraSetName(aCamera camera, const char* name);
/* set transforms */
/** set position of camera */
bool CameraSetPosf(aCamera camera, float x, float y, float z);
bool CameraSetPosv(aCamera camera, SVector3f position);
bool CameraSetPosX(aCamera camera, float x);
bool CameraSetPosY(aCamera camera, float y);
bool CameraSetPosZ(aCamera camera, float z);
/** set target of camera */
bool CameraSetTargetf(aCamera camera, float x, float y, float z);
bool CameraSetTargetv(aCamera camera, SVector3f target);
bool CameraSetTargetX(aCamera camera, float x);
bool CameraSetTargetY(aCamera camera, float y);
bool CameraSetTargetZ(aCamera camera, float z);
/** set upvector of camera */
bool CameraSetUpVectorf(aCamera camera, float x, float y, float z);
bool CameraSetUpVectorv(aCamera camera, SVector3f upVector);
bool CameraSetUpVectorX(aCamera camera, float x);
bool CameraSetUpVectorY(aCamera camera, float y);
bool CameraSetUpVectorZ(aCamera camera, float z);


// Тут изменяется положение, направление и верт. вектор камеры.
bool CameraPlacev (aCamera camera, SVector3f position, SVector3f target, SVector3f upVector);
bool CameraPlacef (aCamera camera,
                   float positionX, float positionY, float positionZ,
                   float targetX,   float targetY,   float targetZ,
                   float upVectorX, float upVectorY, float upVectorZ);
// Эта ф-я вращает камеру вокруг позиции в зависимости от переданных параметров.
bool CameraRotatef(aCamera camera, float angle, float x, float y, float z);
bool CameraRotatev(aCamera camera, float angle, SVector3f axis);
/** move camera to forward (speed > 0) or backward (speed < 0) */
bool CameraMove(aCamera camera, float speed);
/** move camera to right (speed > 0) or left (speed < 0) */
bool CameraStrafe(aCamera camera, float speed);
/** change view direction of camera by nouse */
bool CameraSetViewByMouse(aCamera camera);

