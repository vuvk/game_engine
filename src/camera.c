#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "vector.h"
#include "matrix.h"
#include "structs_private.h"
#include "camera.h"
#include "window_system.h"
#include "input_system.h"
#include "render_system.h"



extern TMatrix4x4 _world;   // from render_system.h

/** identificator of object as camera */
#define CAM_IDENT 0x5F4D4143   // 'C', 'A', 'M', '_' = 1598898499 = 0x5F4D4143


static inline void CameraUpdate(SCamera* camera)
{
    CameraPlacev (camera, camera->position, camera->view, camera->upVector);
}


aCamera CameraCreate(const char* name)
{
    if (name == NULL)
        return NULL;

    SCamera* cam = calloc(1, sizeof(SCamera));

    cam->ident = CAM_IDENT;
    cam->upVector.y = 1.0f;

    CameraUpdate(cam);

    return cam;
}

void CameraDestroy(aCamera* camera)
{
    if (camera == NULL || !ObjectIsCamera(*camera))
        return;

    /* clear SCamera struct */
    SCamera* cam = (SCamera*)*camera;
    cam->ident = 0;

    free(cam);
    cam = NULL;
    *camera = NULL;
}


bool CameraPlacev (aCamera camera, SVector3f position, SVector3f view, SVector3f upVector)
{
    IS_CAMERA_VALID(camera);

    SCamera* cam = (SCamera*)camera;

    cam->position = position;
    cam->view     = view;
    cam->upVector = upVector;

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}

bool CameraPlacef (aCamera camera,
                   float positionX, float positionY, float positionZ,
                   float viewX,     float viewY,     float viewZ,
                   float upVectorX, float upVectorY, float upVectorZ)
{
    IS_CAMERA_VALID(camera);

    SCamera* cam = (SCamera*)camera;

    cam->position.x = positionX;
    cam->position.y = positionY;
    cam->position.z = positionZ;

    cam->view.x = viewX;
    cam->view.y = viewY;
    cam->view.z = viewZ;

    cam->upVector.x = upVectorX;
    cam->upVector.y = upVectorY;
    cam->upVector.z = upVectorZ;

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}


bool CameraRotatef(aCamera camera, float angle, float x, float y, float z)
{
    IS_CAMERA_VALID(camera);

    if (angle == 0.0f)
        return false;

    SCamera* cam = (SCamera*)camera;

    SVector3f view = Sub(cam->view, cam->position);
    view = NormalizeVector3f(view);

    float cosTheta = (float)SDL_cos(angle);
    float sinTheta = (float)SDL_sin(angle);

    SVector3f newView;

    newView.x  = (cosTheta + (1.0f - cosTheta) * x * x) * view.x;
    newView.x += ((1.0f - cosTheta) * x * y - z * sinTheta) * view.y;
    newView.x += ((1.0f - cosTheta) * x * z + y * sinTheta) * view.z;

    newView.y  = ((1.0f - cosTheta) * x * y + z * sinTheta) * view.x;
    newView.y += (cosTheta + (1.0f - cosTheta) * y * y) * view.y;
    newView.y += ((1.0f - cosTheta) * y * z - x * sinTheta) * view.z;

    newView.z  = ((1.0f - cosTheta) * x * z - y * sinTheta) * view.x;
    newView.z += ((1.0f - cosTheta) * y * z + x * sinTheta) * view.y;
    newView.z += (cosTheta + (1.0f - cosTheta) * z * z) * view.z;

    cam->view.x = cam->position.x + newView.x;
    cam->view.y = cam->position.y + newView.y;
    cam->view.z = cam->position.z + newView.z;

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}

inline bool CameraRotatev(aCamera camera, float angle, SVector3f axis)
{
    return CameraRotatef(camera, angle, axis.x, axis.y, axis.z);
}

bool CameraMove(aCamera camera, float speed)
{
    IS_CAMERA_VALID(camera);

    if (speed == 0.0f)
        return false;

    SCamera* cam = (SCamera*)camera;

    SVector3f direction = Sub(cam->view, cam->position);
    direction = NormalizeVector3f(direction);
    direction = Mult(direction, speed);

    cam->position = Add(cam->position, direction);
    cam->view = Add(cam->view, direction);

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}

bool CameraStrafe(aCamera camera, float speed)
{
    IS_CAMERA_VALID(camera);

    if (speed == 0.0f)
        return false;

    SCamera* cam = (SCamera*)camera;

    SVector3f vCross = CrossVector3f(Sub(cam->view, cam->position), cam->upVector);
    vCross = NormalizeVector3f(vCross);
    vCross = Mult(vCross, speed);

    cam->position = Add(cam->position, vCross);
    cam->view = Add(cam->view, vCross);

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}

static SVector2i mousePos;
bool CameraSetViewByMouse(aCamera camera)
{
    IS_CAMERA_VALID(camera);

    int32 middleX = WindowGetWidth()  >> 1;   // ¬ычисл€ем половину ширины
    int32 middleY = WindowGetHeight() >> 1;   // » половину высоты экрана
    float angleY = 0.0f;    // Ќаправление взгл€да вверх/вниз
    float angleZ = 0.0f;    // «начение, необходимое дл€ вращени€ влево-вправо (по оси Y)
    static float currentRotX = 0.0f;

    // ѕолучаем текущие коорд. мыши
    mousePos = InputGetMousePos();

    // ≈сли курсор осталс€ в том же положении, мы не вращаем камеру
    if ((mousePos.x == middleX) && (mousePos.y == middleY))
        return false;

    SCamera* cam = (SCamera*)camera;

    // “еперь, получив координаты курсора, возвращаем его обратно в середину.
    InputSetMousePosi(middleX, middleY);

    // “еперь нам нужно направление (или вектор), куда сдвинулс€ курсор.
    // ≈го рассчет - простое вычитание. ѕросто возьмите среднюю точку и вычтите из неЄ
    // новую позицию мыши: VECTOR = P1 - P2; где P1 - средн€€ точка (400,300 при 800х600)
    angleY = (float)(middleX - mousePos.x) / 10.0f;
    angleZ = (float)(middleY - mousePos.y) / 10.0f;

    static float lastRotX = 0.0f;
    lastRotX = currentRotX;     // —охран€ем последний угол вращени€
                                // и используем заново currentRotX

    // ≈сли текущее вращение больше 1 градуса, обрежем его, чтобы не вращать слишком быстро
    if (currentRotX > 1.0f)
    {
        currentRotX = 1.0f;

        // врощаем на оставшийс€ угол
        if (lastRotX != 1.0f)
        {
            SVector3f axis = CrossVector3f(Sub(cam->view, cam->position), cam->upVector);
            axis = NormalizeVector3f(axis);
            CameraRotatef(cam, DegToRad(1.0f - lastRotX), axis.x, axis.y, axis.z);
        }
    }
    // ≈сли угол меньше -1.0f, убедимс€, что вращение не продолжитс€
    else
        if (currentRotX < -1.0f)
        {
            currentRotX = -1.0f;
            if (lastRotX != -1.0f)
            {
                SVector3f axis = CrossVector3f(Sub(cam->view, cam->position), cam->upVector);
                axis = NormalizeVector3f(axis);
                CameraRotatef(cam, DegToRad(-1.0f - lastRotX), axis.x, axis.y, axis.z);
            }
        }
        // ≈сли укладываемс€ в пределы 1.0f -1.0f - просто вращаем
        else
        {
            SVector3f axis = CrossVector3f(Sub(cam->view, cam->position), cam->upVector);
            axis = NormalizeVector3f(axis);
            CameraRotatef(cam, DegToRad(angleZ), axis.x, axis.y, axis.z);
        }

    // ¬сегда вращаем камеру вокруг Y-оси
    CameraRotatef(cam, DegToRad(angleY), 0.0f, 1.0f, 0.0f);

    LookAtv_M4x4(_world, cam->position, cam->view, cam->upVector);

    return true;
}

bool ObjectIsCamera(void* object)
{
    if (object != NULL && (*((uint32*)object) == CAM_IDENT))
        return true;

    return false;
}
