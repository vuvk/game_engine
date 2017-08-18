#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "SDL2/SDL.h"

#include "structs_private.h"
#include "matrix.h"
#include "list.h"
#include "utils.h"
#include "anorms.h"
#include "log_system.h"
#include "window_system.h"
#include "render_system.h"
#include "engine.h"
#include "node_md2.h"


extern SList* _nodesMd2;
extern TMatrix4x4 _world;   // from render_system.h

#define DEFAULT_MD2_ANIMSPEED 8.0f

/** identificator of object as md2-node */
#define NODEMD2_IDENT 0x32444D4E   // 'N', 'M', 'D', '2' = 843337038 = 0x32444D4E

static SVector3f verts[MD2_MAX_VERTICES] = {};
static SVector3f norms[MD2_MAX_VERTICES] = {};


/*
static void NodeMd2SetTransform(SNodeMd2* node)
{
    // определяем позицию с учетом опорных точек
    SVector3f position = Sub(node->position, node->pivot);

    // считаем матрицу трансформаций
    LoadIdentity_M4x4(node->transform);
    Translatev_M4x4(node->transform, position);
    RotateRxf_M4x4(node->transform, node->pitch);
    RotateRyf_M4x4(node->transform, node->yaw);
    RotateRzf_M4x4(node->transform, node->roll);
    Scalev_M4x4(node->transform, node->scale);
}
*/

static void NodeMd2RecalculateParam(SParamMd2* param)
{
    if (param == NULL)
        return;

    SModelMd2*  mdl = param->mesh->model;

    // calculate buffers
    if (param->mesh == NULL || mdl == NULL)
        return;

    SVector3f   v_prev, v_next;
    SVector3f   n_prev, n_next;

    SFrameMd2   frame1, frame2;
    SVertexMd2  vert1,  vert2;
    uint32      arrL = 0;   // длина буффера

    bool isAnimated = ((param->minFrame != param->maxFrame) &&
                       (param->animSpeed != 0.0f) &&
                       (mdl->header.numOfFrames > 1));

    for (int32 i = 0; i < mdl->header.numOfTris; ++i)
    {
        for (int32 j = 0; j < 3; ++j)
        {
            frame1 = mdl->frames[param->numOfFramePrev];
            vert1  = frame1.verts[mdl->triangles[i].vertex[j]];

            // учитываем, что оси иные и x=x, y=z, z=-y
            // prepare vertices for frames (previous and next)
            v_prev.x =   frame1.scale.x * vert1.v[0] + frame1.translate.x;
            v_prev.y =   frame1.scale.z * vert1.v[2] + frame1.translate.z;
            v_prev.z = -(frame1.scale.y * vert1.v[1] + frame1.translate.y);

            // учитываем, что оси иные и x=x, y=z, z=-y
            // prepare normals for frames (previous and next)
            n_prev.x =  anorms[vert1.normalIndex].x;
            n_prev.y =  anorms[vert1.normalIndex].z;
            n_prev.z = -anorms[vert1.normalIndex].y;

            // формируем буфферы
            // prepare buffers
            param->vbuf_prev[arrL] = v_prev;
            param->nbuf_prev[arrL] = n_prev;

            if (isAnimated)
            {
                frame2 = mdl->frames[param->numOfFrameNext];
                vert2  = frame2.verts[mdl->triangles[i].vertex[j]];

                // учитываем, что оси иные и x=x, y=z, z=-y
                // prepare vertices for frames (previous and next)
                v_next.x =   frame2.scale.x * vert2.v[0] + frame2.translate.x;
                v_next.y =   frame2.scale.z * vert2.v[2] + frame2.translate.z;
                v_next.z = -(frame2.scale.y * vert2.v[1] + frame2.translate.y);

                // учитываем, что оси иные и x=x, y=z, z=-y
                // prepare normals for frames (previous and next)
                n_next.x =  anorms[vert2.normalIndex].x;
                n_next.y =  anorms[vert2.normalIndex].z;
                n_next.z = -anorms[vert2.normalIndex].y;

                // формируем буфферы
                // prepare buffers
                param->vbuf_next[arrL] = v_next;
                param->nbuf_next[arrL] = n_next;
            }

            ++arrL;
        }
    }
}

aNodeMd2 NodeMd2Create(const char* name)
{
    SNodeMd2* node = calloc(1, sizeof(SNodeMd2));
    if (node == NULL)
        return NULL;

    node->ident = NODEMD2_IDENT;
    node->visible = true;
    node->scale.x = node->scale.y = node->scale.z = 1.0f;
    //NodeMd2SetTransform(node);
    if (name != NULL)
        StrCopy(node->name, name, MAX_NAME_LENGTH);

    node->params = ListCreate();

    if (node->params != NULL)
    {
        if (ListAddElement(_nodesMd2, node))
            return (aNodeMd2)node;
    }

    node->ident = 0;
    ListDestroy(&node->params);

    free(node);
    node = NULL;

    return NULL;
}

void NodeMd2Destroy(aNodeMd2* nodeMd2)
{
    if (nodeMd2 == NULL || !ObjectIsNodeMd2(*nodeMd2))
        return;

    SNodeMd2* node = (SNodeMd2*)*nodeMd2;
    SParamMd2* param = NULL;
    node->ident = 0;
    /* delete all params */
    if (node->params->size != 0)
    {
        for (SListElement* element = node->params->first; element; element = element->next)
            if (element->value != NULL)
            {
                param = (SParamMd2*)element->value;
                free(param->vbuf_next);
                free(param->nbuf_next);
                free(param->vbuf_prev);
                free(param->nbuf_prev);
                free(param);

                element->value = NULL;
            }
    }
    ListDestroy(&node->params);

    /* delete from list */
    ListDeleteElementByValue(_nodesMd2, node);

    /* clear SNodeMd2 struct */
    free(node);
    *nodeMd2 = NULL;
    node = NULL;
}

bool NodeMd2ClearAllLinks(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2*  node  = (SNodeMd2*)nodeMd2;
    SParamMd2* param = NULL;
    if (node->params->size != 0)
    {
        for (SListElement* element = node->params->first; element; element = element->next)
            if (element->value != NULL)
            {
                param = (SParamMd2*)element->value;
                free(param->vbuf_next);
                free(param->nbuf_next);
                free(param->vbuf_prev);
                free(param->nbuf_prev);
                free(param);

                element->value = NULL;
            }

        ListClear(node->params);
    }

    return true;
}

bool NodeMd2AddLinkToMeshMd2(aNodeMd2 nodeMd2, aMeshMd2 meshMd2)
{
    IS_NODEMD2_VALID(nodeMd2);
    IS_MESHMD2_VALID(meshMd2);

    SParamMd2* param = calloc(1, sizeof(SParamMd2));
    SMeshMd2*  mesh  = (SMeshMd2*)meshMd2;
    param->animSpeed = DEFAULT_MD2_ANIMSPEED;
    param->mesh = mesh;

    if (mesh->model != NULL)
    {
        int32  sizeOfVerts = 3*mesh->model->header.numOfTris*sizeof(SVector3f);

        param->vbuf_next = calloc(1, sizeOfVerts);
        param->vbuf_prev = calloc(1, sizeOfVerts);
        param->nbuf_next = calloc(1, sizeOfVerts);
        param->nbuf_prev = calloc(1, sizeOfVerts);

        param->loop = true;
        param->visible = true;
        param->maxFrame = mesh->model->header.numOfFrames - 1;
        if (param->maxFrame > 0)
            param->numOfFrameNext = 1;
    }

    bool result = ListAddElement(((SNodeMd2*)nodeMd2)->params, param);
    if (!result)
        free(param);

    return result;
}

bool NodeMd2DeleteLinkToMeshMd2(aMeshMd2 nodeMd2, aMeshMd2 meshMd2)
{
    IS_NODEMD2_VALID(nodeMd2);
    IS_NODEMD2_VALID(meshMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    SListElement* element = ListGetElementByValue(node->params, meshMd2);
    if (element == NULL)
        return false;
    ListDeleteElement(node->params, element);

    element = NULL;
    node = NULL;

    return true;
}

bool NodeMd2DeleteLinkToMeshMd2ByNumber(aMeshMd2 nodeMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    SListElement* element = ListGetElementByNumber(node->params, numOfMesh);
    if (element == NULL)
        return false;
    ListDeleteElement(node->params, element);

    element = NULL;
    node = NULL;

    return true;
}


const char* NodeMd2GetName(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->name;
}

void NodesMd2GetByName(const char* name, SList* results)
{
    if (_nodesMd2->size == 0)
        return;

    if (results == NULL)
        return;

    /* prepare  results */
    ListClear(results);

    /* search all nodes with name */
    SNodeMd2* node = NULL;
    for (SListElement* element = _nodes2d->first; element; element = element->next)
    {
        node = (SNodeMd2*)element->value;
        if (ObjectIsNodeMd2(node))
        {
            if ((node->name == NULL && name == NULL) ||
                (StrEqual(node->name, name) == true))
            {
                ListAddElement(results, node);
            }
        }
    }
}


bool NodeMd2GetPivot(aNodeMd2 nodeMd2, float* x, float* y, float* z)
{
    IS_NODEMD2_VALID(nodeMd2);

    if (x != NULL)
        *x = ((SNodeMd2*)nodeMd2)->pivot.x;
    if (y != NULL)
        *y = ((SNodeMd2*)nodeMd2)->pivot.y;
    if (z != NULL)
        *z = ((SNodeMd2*)nodeMd2)->pivot.z;

    return true;
}

inline SVector3f NodeMd2GetPivotv(aNodeMd2 nodeMd2)
{
    if (!ObjectIsNodeMd2(nodeMd2))
        return vZero3f;

    return ((SNodeMd2*)nodeMd2)->pivot;
}

inline float NodeMd2GetPivotX(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->pivot.x;
}

inline float NodeMd2GetPivotY(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->pivot.y;
}

inline float NodeMd2GetPivotZ(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->pivot.z;
}

bool NodeMd2GetPos(aNodeMd2 nodeMd2, float* x, float* y, float* z)
{
    IS_NODEMD2_VALID(nodeMd2);

    if (x != NULL)
        *x = ((SNodeMd2*)nodeMd2)->position.x;
    if (y != NULL)
        *y = ((SNodeMd2*)nodeMd2)->position.y;
    if (z != NULL)
        *z = ((SNodeMd2*)nodeMd2)->position.z;

    return true;
}

inline SVector3f NodeMd2GetPosv(aNodeMd2 nodeMd2)
{
    if (!ObjectIsNodeMd2(nodeMd2))
        return vZero3f;

    return ((SNodeMd2*)nodeMd2)->position;
}

inline float NodeMd2GetPosX(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->position.x;
}

inline float NodeMd2GetPosY(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->position.y;
}

inline float NodeMd2GetPosZ(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->position.z;
}

bool NodeMd2GetScale(aNodeMd2 nodeMd2, float* x, float* y, float* z)
{
    IS_NODEMD2_VALID(nodeMd2);

    if (x != NULL)
        *x = ((SNodeMd2*)nodeMd2)->scale.x;
    if (y != NULL)
        *y = ((SNodeMd2*)nodeMd2)->scale.y;
    if (z != NULL)
        *z = ((SNodeMd2*)nodeMd2)->scale.z;

    return true;
}

inline SVector3f NodeMd2GetScalev(aNodeMd2 nodeMd2)
{
    if (!ObjectIsNodeMd2(nodeMd2))
        return vZero3f;

    return ((SNodeMd2*)nodeMd2)->scale;
}

inline float NodeMd2GetScaleX(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->scale.x;
}

inline float NodeMd2GetScaleY(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->scale.y;
}

inline float NodeMd2GetScaleZ(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->scale.z;
}

bool NodeMd2GetRotation(aNodeMd2 nodeMd2, float* pitch, float* yaw, float* roll)
{
    IS_NODEMD2_VALID(nodeMd2);

    if (pitch != NULL)
        *pitch = ((SNodeMd2*)nodeMd2)->pitch;
    if (yaw != NULL)
        *yaw = ((SNodeMd2*)nodeMd2)->yaw;
    if (roll != NULL)
        *roll = ((SNodeMd2*)nodeMd2)->roll;

    return true;
}

inline float NodeMd2GetPitch(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->pitch;
}

inline float NodeMd2GetYaw(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->yaw;
}

inline float NodeMd2GetRoll(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->roll;
}

bool NodeMd2GetIntervalEx(aNodeMd2 nodeMd2, int32* startFrame, int32* endFrame, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    if (startFrame == NULL || endFrame == NULL)
        return false;

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    SParamMd2* param = (SParamMd2*)element->value;
    *startFrame = param->minFrame;
    *endFrame   = param->maxFrame;

    return true;
}

bool NodeMd2GetAnimLoopEx(aNodeMd2 nodeMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    return ((SParamMd2*)element->value)->loop;
}

inline int32 NodeMd2GetCountOfLinks(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->params->size;
}

aMeshMd2 NodeMd2GetMeshMd2Ex(aNodeMd2 nodeMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return NULL;

    return ((SParamMd2*)element->value)->mesh;
}

aMaterial NodeMd2GetMaterialEx(aNodeMd2 nodeMd2, uint32 numOfMaterial)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    SListElement* element = ListGetElementByNumber(node->params, numOfMaterial);
    if (element == NULL || element->value == NULL)
        return NULL;

    return ((SParamMd2*)element->value)->material;
}

float NodeMd2GetAnimSpeedEx(aNodeMd2 nodeMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return 0.0f;

    return ((SParamMd2*)element->value)->animSpeed;
}

int32 NodeMd2GetCurrentFrameEx(aNodeMd2 nodeMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    return ((SParamMd2*)element->value)->numOfFramePrev;
}

bool NodeMd2IsVisible(aNodeMd2 nodeMd2)
{
    IS_NODEMD2_VALID(nodeMd2);

    return ((SNodeMd2*)nodeMd2)->visible;
}

/** return true if pointer is nodeMd2 */
bool ObjectIsNodeMd2(void* object)
{
    if (object != NULL && (*((uint32*)object) == NODEMD2_IDENT))
        return true;

    return false;
}


inline bool NodeMd2SetName(aNodeMd2 nodeMd2, const char* name)
{
    IS_NODEMD2_VALID(nodeMd2);

    StrCopy(((SNodeMd2*)nodeMd2)->name, name, MAX_NAME_LENGTH);

    return true;
}


bool NodeMd2SetPivot(aNodeMd2 nodeMd2, float x, float y, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->pivot.x = x;
    node->pivot.y = y;
    node->pivot.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPivotv(aNodeMd2 nodeMd2, SVector3f pivot)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->pivot = pivot;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPivotX(aNodeMd2 nodeMd2, float x)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->pivot.x = x;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPivotY(aNodeMd2 nodeMd2, float y)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->pivot.y = y;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPivotZ(aNodeMd2 nodeMd2, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->pivot.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetPos(aNodeMd2 nodeMd2, float x, float y, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->position.x = x;
    node->position.y = y;
    node->position.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPosv(aNodeMd2 nodeMd2, SVector3f position)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->position = position;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPosX(aNodeMd2 nodeMd2, float x)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->position.x = x;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPosY(aNodeMd2 nodeMd2, float y)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->position.y = y;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetPosZ(aNodeMd2 nodeMd2, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->position.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetScale(aNodeMd2 nodeMd2, float x, float y, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->scale.x = x;
    node->scale.y = y;
    node->scale.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetScalev(aNodeMd2 nodeMd2, SVector3f scale)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->scale = scale;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetScaleX(aNodeMd2 nodeMd2, float x)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->scale.x = x;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetScaleY(aNodeMd2 nodeMd2, float y)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->scale.y = y;

    //NodeMd2SetTransform(node);

    return true;
}

inline bool NodeMd2SetScaleZ(aNodeMd2 nodeMd2, float z)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    node->scale.z = z;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetRotationInDeg(aNodeMd2 nodeMd2, float pitch, float yaw, float roll)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (pitch < 0.0f) pitch += 360.0f;
    if (yaw   < 0.0f) yaw   += 360.0f;
    if (roll  < 0.0f) roll  += 360.0f;

    if (pitch >= 360.0f) pitch -= 360.0f;
    if (yaw   >= 360.0f) yaw   -= 360.0f;
    if (roll  >= 360.0f) roll  -= 360.0f;

    node->pitch = DegToRad(pitch);
    node->yaw   = DegToRad(yaw);
    node->roll  = DegToRad(roll);

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetPitchInDeg(aNodeMd2 nodeMd2, float pitch)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (pitch < 0.0f)    pitch += 360.0f;
    if (pitch >= 360.0f) pitch -= 360.0f;

    node->pitch = DegToRad(pitch);

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetYawInDeg(aNodeMd2 nodeMd2, float yaw)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (yaw < 0.0f)    yaw += 360.0f;
    if (yaw >= 360.0f) yaw -= 360.0f;

    node->yaw = DegToRad(yaw);

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetRollInDeg(aNodeMd2 nodeMd2, float roll)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (roll < 0.0f)    roll += 360.0f;
    if (roll >= 360.0f) roll -= 360.0f;

    node->roll = DegToRad(roll);

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetRotationInRad(aNodeMd2 nodeMd2, float pitch, float yaw, float roll)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (pitch < 0.0f) pitch += PI_2;
    if (yaw   < 0.0f) yaw   += PI_2;
    if (roll  < 0.0f) roll  += PI_2;

    if (pitch >= PI_2) pitch -= PI_2;
    if (yaw   >= PI_2) yaw   -= PI_2;
    if (roll  >= PI_2) roll  -= PI_2;

    node->pitch = pitch;
    node->yaw   = yaw;
    node->roll  = roll;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetPitchInRad(aNodeMd2 nodeMd2, float pitch)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (pitch <  0.0f) pitch += PI_2;
    if (pitch >= PI_2) pitch -= PI_2;

    node->pitch = pitch;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetYawInRad(aNodeMd2 nodeMd2, float yaw)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (yaw <  0.0f) yaw += PI_2;
    if (yaw >= PI_2) yaw -= PI_2;

    node->yaw = yaw;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetRollInRad(aNodeMd2 nodeMd2, float roll)
{
    IS_NODEMD2_VALID(nodeMd2);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;

    if (roll <  0.0f) roll += PI_2;
    if (roll >= PI_2) roll -= PI_2;

    node->roll = roll;

    //NodeMd2SetTransform(node);

    return true;
}

bool NodeMd2SetMeshMd2Ex(aNodeMd2 nodeMd2, aMeshMd2 meshMd2, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);
    IS_MESHMD2_VALID(meshMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    SParamMd2* param = (SParamMd2*)element->value;
    param->mesh = meshMd2;
    free(param->vbuf_next);
    free(param->vbuf_prev);
    free(param->nbuf_next);
    free(param->nbuf_prev);

    int32  sizeOfVerts = 3*param->mesh->model->header.numOfTris*sizeof(SVector3f);

    param->vbuf_next = calloc(1, sizeOfVerts);
    param->vbuf_prev = calloc(1, sizeOfVerts);
    param->nbuf_next = calloc(1, sizeOfVerts);
    param->nbuf_prev = calloc(1, sizeOfVerts);

    return true;
}

bool NodeMd2SetMaterialEx(aNodeMd2 nodeMd2, aMaterial material, uint32 numOfMaterial)
{
    IS_NODEMD2_VALID(nodeMd2);
    IS_MATERIAL_VALID(material);

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    SListElement* element = ListGetElementByNumber(node->params, numOfMaterial);
    if (element == NULL || element->value == NULL)
        return false;

    ((SParamMd2*)element->value)->material = material;

    return true;
}

bool NodeMd2SetAnimSpeedEx(aNodeMd2 nodeMd2, float animSpeed, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    ((SParamMd2*)element->value)->animSpeed = animSpeed;

    return true;
}

bool NodeMd2SetCurrentFrameEx(aNodeMd2 nodeMd2, int32 numOfFrame, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    SParamMd2* param = (SParamMd2*)element->value;
    if (param->mesh == NULL || param->mesh->model == NULL)
        return false;

    Limit(&numOfFrame, param->minFrame, param->maxFrame);

    param->numOfFramePrev = numOfFrame;
    param->numOfFrameNext = numOfFrame++;

    return true;
}

bool NodeMd2SetIntervalEx(aNodeMd2 nodeMd2, int32 startFrame, int32 endFrame, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    SParamMd2* param = (SParamMd2*)element->value;
    if (param->mesh == NULL || param->mesh->model == NULL)
        return false;

    Limit(&startFrame, 0, param->mesh->model->header.numOfFrames - 1);
    Limit(&endFrame,   0, param->mesh->model->header.numOfFrames - 1);
    if (startFrame > endFrame)
        Swap(&startFrame, &endFrame);

    param->minFrame = startFrame;
    param->maxFrame = endFrame;

    param->numOfFramePrev = startFrame;
    param->numOfFrameNext = startFrame++;

    NodeMd2RecalculateParam(param);

    return true;
}

bool NodeMd2SetIntervalByNameEx(aNodeMd2 nodeMd2, const char* animName, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    SParamMd2* param = (SParamMd2*)element->value;
    if (param->mesh == NULL || param->mesh->model == NULL)
        return false;

    int32 startFrame = -1,
          endFrame   = -1;
    char  frameName[MD2_MAX_FRAME_NAME]     = {0},
          prevFrameName[MD2_MAX_FRAME_NAME] = {0};

    int32 len = StrLength(animName) + 1;
    if (len > MD2_MAX_FRAME_NAME)
        len = MD2_MAX_FRAME_NAME;

    for (int32 i = 0; /*(startFrame == -1) &&
                      (endFrame == -1) &&*/
                      (i < param->mesh->model->header.numOfFrames); ++i)
    {
        StrCopy(frameName, param->mesh->model->frames[i].name, len);

        // set start frame
        if (startFrame == -1)
        {
            if (StrEqual(frameName, animName) == true)
                startFrame = i;
        }

        // set end frame
        if (endFrame == -1)
        {
            if ((StrEqual(frameName, animName) != true) &&
                (StrEqual(prevFrameName, animName) == true))
            {
                endFrame = i;
            }
        }

        // save previous name
        StrCopy(prevFrameName, frameName, len);
    }

    if (startFrame == -1 && endFrame == -1)
        return false;

    return NodeMd2SetIntervalEx(nodeMd2, startFrame, --endFrame, numOfMesh);
}

bool NodeMd2SetAnimLoopEx(aNodeMd2 nodeMd2, bool loop, uint32 numOfMesh)
{
    IS_NODEMD2_VALID(nodeMd2);

    SListElement* element = ListGetElementByNumber(((SNodeMd2*)nodeMd2)->params, numOfMesh);
    if (element == NULL || element->value == NULL)
        return false;

    ((SParamMd2*)element->value)->loop = loop;

    return true;
}


inline uint32 NodesMdGetCount()
{
    return _nodesMd2->size;
}

void NodesMd2DestroyAll()
{
    if (_nodesMd2->size == 0)
        return;

    /* clear SNodeMd2 structs in dictionary */
    SNodeMd2* node = NULL;
    for (SListElement* element = _nodesMd2->first; element; element = element->next)
    {
        if (element->value)
        {
            node = (SNodeMd2*)element->value;
            node->ident = 0;
            /* delete all params */
            if (node->params->size != 0)
            {
                for (SListElement* element = node->params->first; element; element = element->next)
                    if (element->value != NULL)
                    {
                        free(element->value);
                        element->value = NULL;
                    }
            }
            ListDestroy(&node->params);
            free(node);
        }
    }

    /* delete all elements from dictionary */
    ListClear(_nodesMd2);
}


void NodeMd2Draw(aNodeMd2 nodeMd2)
{
    if (!ObjectIsNodeMd2(nodeMd2))
        return;

    SNodeMd2* node = (SNodeMd2*)nodeMd2;
    if (node->params->size == 0)
        return;

    if (!node->visible)
        return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    SParamMd2*  param = NULL;
    SModelMd2*  mdl = NULL;
    SVector3f   v, v_prev, v_next;
    SVector3f   n, n_prev, n_next;
    float  interp = 0.0f;
    int32  numOfVerts = 0;

    bool isAnimated;

    // transformations
    SVector3f  translate;
    SVector3f  scale;
    float      pitch, yaw, roll;
    TMatrix4x4 transform;

    for (SListElement* element = node->params->first; element; element = element->next)
    {
        if (element->value == NULL)
            continue;

        param = (SParamMd2*)element->value;
        if (!param->visible)
            continue;

        mdl = param->mesh->model;
        numOfVerts = 3*mdl->header.numOfTris;

        // apply material
        if (param->material != NULL)
            MaterialApply(param->material);

        // calculate transformations
        // translation
        translate = Sub(node->position, node->pivot);
        translate = Sub(translate, param->mesh->pivot);
        translate = Add(translate, param->mesh->position);

        // scale
        scale = node->scale;
        scale.x *= param->mesh->scale.x;
        scale.y *= param->mesh->scale.y;
        scale.z *= param->mesh->scale.z;

        // rotations
        pitch = node->pitch + param->mesh->pitch;
        if (pitch <  0.0f) pitch += PI_2;
        if (pitch >= PI_2) pitch -= PI_2;

        yaw = node->yaw + param->mesh->yaw;
        if (yaw <  0.0f) yaw += PI_2;
        if (yaw >= PI_2) yaw -= PI_2;

        roll = node->roll + param->mesh->roll;
        if (roll <  0.0f) roll += PI_2;
        if (roll >= PI_2) roll -= PI_2;

        // total matrix
        LoadIdentity_M4x4(transform);
        Translatev_M4x4(transform, translate);
        RotateRxf_M4x4(transform, pitch);
        RotateRyf_M4x4(transform, yaw);
        RotateRzf_M4x4(transform, roll);
        Scalev_M4x4(transform, scale);

        Multiplyf_M4x4(transform, _world, transform);

        glLoadMatrixf(transform);

        // статика или динамика?
        isAnimated = (
                      (param->minFrame != param->maxFrame) &&
                      (param->animSpeed != 0.0f) &&
                      (mdl->header.numOfFrames > 1) &&
                      ((param->loop) ||
                       // если loop не установлен и достигнут последний кадр, то объект считается static
                       (!param->loop && ((param->animSpeed <  0.0f && param->numOfFrameNext > param->minFrame) ||
                                         (param->animSpeed >= 0.0f && param->numOfFrameNext < param->maxFrame)))
                      )
                     );

        // animate
        if (isAnimated)
        {
            if (param->interp <= 1.0f)
                param->interp += SDL_fabs(param->animSpeed)*EngineGetDeltaTime();
            else
            {
                // change frame
                param->interp -= 1.0f;
                param->numOfFramePrev = param->numOfFrameNext;

                if (param->animSpeed > 0.0f)
                {
                    ++param->numOfFrameNext;

                    if ((param->numOfFrameNext > param->maxFrame) && (param->loop))
                        param->numOfFrameNext = param->minFrame;
                }
                if (param->animSpeed < 0.0f)
                {
                    --param->numOfFrameNext;

                    if ((param->numOfFrameNext < param->minFrame) && (param->loop))
                        param->numOfFrameNext = param->maxFrame;
                }

                // calculate buffers
                NodeMd2RecalculateParam (param);
            }

            // now draw
            // interpolate normals and vertices
            interp = param->interp;
            for (int32 i = 0; i < numOfVerts; ++i)
            {
                v_prev = param->vbuf_prev[i];
                v_next = param->vbuf_next[i];

                // v = v_prev + interp*(v_next - v_prev);
                v = Add(v_prev, Mult(Sub(v_next, v_prev), interp));

                n_prev = param->nbuf_prev[i];
                n_next = param->nbuf_next[i];

                // n = n_prev + interp*(n_next - n_prev);
                n = Add(n_prev, Mult(Sub(n_next, n_prev), interp));

                // формируем буфферы
                verts[i] = v;
                norms[i] = n;
            }

            // рисуем буфферы
            glVertexPointer(3, GL_FLOAT, 0, &verts[0]);
            glNormalPointer(   GL_FLOAT, 0, &norms[0]);
        }
        // static
        else
        {
            glVertexPointer(3, GL_FLOAT, 0, param->vbuf_prev);
            glNormalPointer(   GL_FLOAT, 0, param->nbuf_prev);
        }

        glTexCoordPointer(2, GL_FLOAT, 0, mdl->texCoords);
        glDrawArrays(GL_TRIANGLES, 0, numOfVerts);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
