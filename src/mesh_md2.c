#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "structs_private.h"
#include "utils.h"
#include "types.h"
#include "vector.h"
#include "log_system.h"
#include "file_system.h"
#include "render_system.h"
#include "resource_manager.h"
#include "engine.h"
#include "mesh_md2.h"

extern SDictionary* _meshesMd2;

/** identificator of object as md2-mesh */
#define MD2_IDENT 0x5F32444D   // 'M', 'D', '2', '_' = 1597129805 = 0x5F32444D



/** return > 0 if file already loaded */
static uint32 MeshesMd2GetCountOfDuplicate(const char* fileName)
{
    if (fileName == NULL)
        return 0;

    if (_meshesMd2->size == 0)
        return 0;

    uint32 countOfDuplicates = 0;
    for (SDictElement* element = _meshesMd2->first; element; element = element->next)
    {
        if (element->value != NULL)
        {
            if (StrEqual(((SMeshMd2*)element->value)->fileName, fileName) == true)
                ++countOfDuplicates;
        }
    }

    return countOfDuplicates;
}

/*
static void MeshMd2SetTransform(SMeshMd2* mesh)
{
    // определяем позицию с учетом опорных точек
    SVector3f position = Sub(mesh->position, mesh->pivot);

    // считаем матрицу трансформаций
    LoadIdentity_M4x4(mesh->transform);
    Translatev_M4x4(mesh->transform, position);
    RotateRxf_M4x4(mesh->transform, mesh->pitch);
    RotateRyf_M4x4(mesh->transform, mesh->yaw);
    RotateRzf_M4x4(mesh->transform, mesh->roll);
    Scalev_M4x4(mesh->transform, mesh->scale);
}
*/

aMeshMd2 MeshMd2Create(const char* name)
{
    if (name == NULL)
        return NULL;

    /* изначально проверим нет ли уже элемента с таким именем */
    SMeshMd2* mesh = DictionaryGetValueByKey(_meshesMd2, name);
    if (mesh != NULL)
        return NULL;

    mesh = calloc(1, sizeof(SMeshMd2));
    if (mesh == NULL)
        return NULL;

    mesh->ident = MD2_IDENT;
    mesh->scale = NewVector3f(1.0f, 1.0f, 1.0f);

    //MeshMd2SetTransform(mesh);

    /* пытаемся добавить новый элемент */
    if (DictionaryAddElement(_meshesMd2, name, mesh))
        return (aMeshMd2)mesh;

    mesh->ident = 0;
    free(mesh);
    mesh = NULL;

    return NULL;
}

bool MeshMd2LoadFromFile (aMeshMd2 meshMd2, const char* fileName)
{
    IS_MESHMD2_VALID(meshMd2);
    if (fileName == NULL)
        return false;

    /* не загружен ли уже такой? */
    if (MeshesMd2GetCountOfDuplicate(fileName) > 0)
    {
        LogWriteError("File '%s' is already loaded!\n", fileName);
        return false;
    }

    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' doesn't exist!\n", fileName);
        return false;
    }

    /* save name of file */
    StrCopy(((SMeshMd2*)meshMd2)->fileName, fileName, MAX_PATH_LENGTH);

    return RM_LoadMeshMd2(fileName, &meshMd2);
}

aMeshMd2 MeshMd2CreateFromFile(const char* name, const char* fileName)
{
    if (fileName == NULL || name == NULL)
        return NULL;

    if (!FileExists(fileName))
        return NULL;

    aMeshMd2 mesh = MeshMd2Create(name);
    if (mesh == NULL)
        return NULL;

    if (!MeshMd2LoadFromFile(mesh, fileName))
    {
        MeshMd2Destroy(&mesh);
        return NULL;
    }

    return mesh;
}

void MeshMd2Destroy(aMeshMd2* meshMd2)
{
    if (meshMd2 == NULL || !ObjectIsMeshMd2(*meshMd2))
        return;

    /* clear SMeshMd2 struct */
    SMeshMd2* mesh = (SMeshMd2*)*meshMd2;
    mesh->ident = 0;
    free(mesh->model->skins);
    free(mesh->model->texCoords);
    free(mesh->model->triangles);
    //free(mesh->model->glcmds);
    for (int32 i = 0; i < mesh->model->header.numOfFrames; ++i)
        free(mesh->model->frames[i].verts);
    free(mesh->model->frames);
    free(mesh->model);

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_meshesMd2, mesh);

    free(mesh);
    *meshMd2 = NULL;
    mesh = NULL;
}

inline char* MeshMd2GetName(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return DictionaryGetKeyByValue(_meshesMd2, meshMd2);
}

inline char* MeshMd2GetFileName(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->fileName;
}

aMeshMd2 MeshMd2GetByFileName(const char* fileName)
{
    if (fileName == NULL)
        return NULL;

    if (_meshesMd2->size == 0)
        return NULL;

    SMeshMd2* mesh = NULL;
    for (SDictElement* element = _meshesMd2->first; element && (element->value); element = element->next)
    {
        mesh = (SMeshMd2*)(element->value);
        if (StrEqual(mesh->fileName, fileName) == true)
            return mesh;
    }

    return NULL;
}

inline aMeshMd2 MeshMd2GetByName(const char* name)
{
    if (name == NULL || _meshesMd2->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_meshesMd2, name);
}

bool MeshMd2GetPivotf(aMeshMd2 meshMd2, float* x, float* y, float* z)
{
    IS_MESHMD2_VALID(meshMd2);

    if (x != NULL)
        *x = ((SMeshMd2*)meshMd2)->pivot.x;
    if (y != NULL)
        *y = ((SMeshMd2*)meshMd2)->pivot.y;
    if (z != NULL)
        *z = ((SMeshMd2*)meshMd2)->pivot.z;

    return true;
}

inline SVector3f MeshMd2GetPivotv(aMeshMd2 meshMd2)
{
    if (!ObjectIsMeshMd2(meshMd2))
        return vZero3f;

    return ((SMeshMd2*)meshMd2)->pivot;
}

inline float MeshMd2GetPivotX(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->pivot.x;
}

inline float MeshMd2GetPivotY(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->pivot.y;
}

inline float MeshMd2GetPivotZ(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->pivot.z;
}

bool MeshMd2GetPosf(aMeshMd2 meshMd2, float* x, float* y, float* z)
{
    IS_MESHMD2_VALID(meshMd2);

    if (x != NULL)
        *x = ((SMeshMd2*)meshMd2)->position.x;
    if (y != NULL)
        *y = ((SMeshMd2*)meshMd2)->position.y;
    if (z != NULL)
        *z = ((SMeshMd2*)meshMd2)->position.z;

    return true;
}

inline SVector3f MeshMd2GetPosv(aMeshMd2 meshMd2)
{
    if (!ObjectIsMeshMd2(meshMd2))
        return vZero3f;

    return ((SMeshMd2*)meshMd2)->position;
}

inline float MeshMd2GetPosX(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->position.x;
}

inline float MeshMd2GetPosY(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->position.y;
}

inline float MeshMd2GetPosZ(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->position.z;
}

bool MeshMd2GetScalef(aMeshMd2 meshMd2, float* x, float* y, float* z)
{
    IS_MESHMD2_VALID(meshMd2);

    if (x != NULL)
        *x = ((SMeshMd2*)meshMd2)->scale.x;
    if (y != NULL)
        *y = ((SMeshMd2*)meshMd2)->scale.y;
    if (z != NULL)
        *z = ((SMeshMd2*)meshMd2)->scale.z;

    return true;
}

inline SVector3f MeshMd2GetScalev(aMeshMd2 meshMd2)
{
    if (!ObjectIsMeshMd2(meshMd2))
        return vZero3f;

    return ((SMeshMd2*)meshMd2)->scale;
}

inline float MeshMd2GetScaleX(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->scale.x;
}

inline float MeshMd2GetScaleY(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->scale.y;
}

inline float MeshMd2GetScaleZ(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->scale.z;
}

bool MeshMd2GetRotation(aMeshMd2 meshMd2, float* pitch, float* yaw, float* roll)
{
    IS_MESHMD2_VALID(meshMd2);

    if (pitch != NULL)
        *pitch = ((SMeshMd2*)meshMd2)->pitch;
    if (yaw != NULL)
        *yaw = ((SMeshMd2*)meshMd2)->yaw;
    if (roll != NULL)
        *roll = ((SMeshMd2*)meshMd2)->roll;

    return true;
}

inline float MeshMd2GetPitch(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->pitch;
}

inline float MeshMd2GetYaw(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->yaw;
}

inline float MeshMd2GetRoll(aMeshMd2 meshMd2)
{
    IS_MESHMD2_VALID(meshMd2);

    return ((SMeshMd2*)meshMd2)->roll;
}



inline bool ObjectIsMeshMd2(void* object)
{
    if (object != NULL && (*((uint32*)object) == MD2_IDENT))
        return true;

    return false;
}


inline bool MeshMd2SetName(aMeshMd2 meshMd2, const char* name)
{
    IS_MESHMD2_VALID(meshMd2);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_meshesMd2, meshMd2, name);
}


bool MeshMd2SetPivot(aMeshMd2 meshMd2, float x, float y, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->pivot.x = x;
    mesh->pivot.y = y;
    mesh->pivot.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPivotv(aMeshMd2 meshMd2, SVector3f pivot)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->pivot = pivot;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPivotX(aMeshMd2 meshMd2, float x)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->pivot.x = x;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPivotY(aMeshMd2 meshMd2, float y)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->pivot.y = y;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPivotZ(aMeshMd2 meshMd2, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->pivot.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetPos(aMeshMd2 meshMd2, float x, float y, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->position.x = x;
    mesh->position.y = y;
    mesh->position.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPosv(aMeshMd2 meshMd2, SVector3f position)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->position = position;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPosX(aMeshMd2 meshMd2, float x)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->position.x = x;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPosY(aMeshMd2 meshMd2, float y)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->position.y = y;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetPosZ(aMeshMd2 meshMd2, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->position.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetScale(aMeshMd2 meshMd2, float x, float y, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->scale.x = x;
    mesh->scale.y = y;
    mesh->scale.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetScalev(aMeshMd2 meshMd2, SVector3f scale)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->scale = scale;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetScaleX(aMeshMd2 meshMd2, float x)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->scale.x = x;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetScaleY(aMeshMd2 meshMd2, float y)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->scale.y = y;

    //MeshMd2SetTransform(mesh);

    return true;
}

inline bool MeshMd2SetScaleZ(aMeshMd2 meshMd2, float z)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;
    mesh->scale.z = z;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetRotationInDeg(aMeshMd2 meshMd2, float pitch, float yaw, float roll)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (pitch < 0.0f) pitch += 360.0f;
    if (yaw   < 0.0f) yaw   += 360.0f;
    if (roll  < 0.0f) roll  += 360.0f;

    if (pitch >= 360.0f) pitch -= 360.0f;
    if (yaw   >= 360.0f) yaw   -= 360.0f;
    if (roll  >= 360.0f) roll  -= 360.0f;

    mesh->pitch = DegToRad(pitch);
    mesh->yaw   = DegToRad(yaw);
    mesh->roll  = DegToRad(roll);

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetPitchInDeg(aMeshMd2 meshMd2, float pitch)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (pitch < 0.0f)    pitch += 360.0f;
    if (pitch >= 360.0f) pitch -= 360.0f;

    mesh->pitch = DegToRad(pitch);

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetYawInDeg(aMeshMd2 meshMd2, float yaw)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (yaw < 0.0f)    yaw += 360.0f;
    if (yaw >= 360.0f) yaw -= 360.0f;

    mesh->yaw = DegToRad(yaw);

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetRollInDeg(aMeshMd2 meshMd2, float roll)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (roll < 0.0f)    roll += 360.0f;
    if (roll >= 360.0f) roll -= 360.0f;

    mesh->roll = DegToRad(roll);

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetRotationInRad(aMeshMd2 meshMd2, float pitch, float yaw, float roll)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (pitch < 0.0f) pitch += PI_2;
    if (yaw   < 0.0f) yaw   += PI_2;
    if (roll  < 0.0f) roll  += PI_2;

    if (pitch >= PI_2) pitch -= PI_2;
    if (yaw   >= PI_2) yaw   -= PI_2;
    if (roll  >= PI_2) roll  -= PI_2;

    mesh->pitch = pitch;
    mesh->yaw   = yaw;
    mesh->roll  = roll;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetPitchInRad(aMeshMd2 meshMd2, float pitch)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (pitch <  0.0f) pitch += PI_2;
    if (pitch >= PI_2) pitch -= PI_2;

    mesh->pitch = pitch;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetYawInRad(aMeshMd2 meshMd2, float yaw)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (yaw <  0.0f) yaw += PI_2;
    if (yaw >= PI_2) yaw -= PI_2;

    mesh->yaw = yaw;

    //MeshMd2SetTransform(mesh);

    return true;
}

bool MeshMd2SetRollInRad(aMeshMd2 meshMd2, float roll)
{
    IS_MESHMD2_VALID(meshMd2);

    SMeshMd2* mesh = (SMeshMd2*)meshMd2;

    if (roll <  0.0f) roll += PI_2;
    if (roll >= PI_2) roll -= PI_2;

    mesh->roll = roll;

    //MeshMd2SetTransform(mesh);

    return true;
}


inline uint32 MeshesMdGetCount()
{
    return _meshesMd2->size;
}

void MeshesMd2DestroyAll()
{
    if (_meshesMd2->size == 0)
        return;

    /* clear SMeshMd2 structs in dictionary */
    SMeshMd2* mesh = NULL;
    for (SDictElement* element = _meshesMd2->first; element; element = element->next)
    {
        if (element->value == NULL)
            continue;

        mesh = (SMeshMd2*)element->value;
        mesh->ident = 0;
        if (mesh->model != NULL)
        {
            free(mesh->model->skins);
            free(mesh->model->texCoords);
            free(mesh->model->triangles);
            //free(mesh->model->glcmds);
            for (int32 i = 0; i < mesh->model->header.numOfFrames; ++i)
                free(mesh->model->frames[i].verts);
            free(mesh->model->frames);
            free(mesh->model);
        }
        free(mesh);
    }

    /* delete all elements from dictionary */
    DictionaryClear(_meshesMd2);
}
