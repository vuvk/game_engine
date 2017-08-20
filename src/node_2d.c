#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "SDL2/SDL.h"

#include "structs_private.h"
#include "matrix.h"
#include "list.h"
#include "utils.h"
#include "log_system.h"
#include "window_system.h"
#include "node_2d.h"


extern SList* _nodes2d;

/** identificator of object as node2d */
#define NODE2D_IDENT 0x44325F4E   // 'N', '_', '2', 'D' = 1144151886 = 0x44325F4E

static void Node2DSetTransform(SNode2D* node)
{
    // определяем позицию с учетом опорных точек
    SVector2f position = Sub(node->position, node->pivot);

    // считаем матрицу трансформаций
    LoadIdentity_M4x4(node->transform);
    Translatef_M4x4(node->transform, position.x, WindowGetHeight() - position.y, 0.0f);
    RotateRzf_M4x4(node->transform, node->rotation);
    Scalef_M4x4(node->transform, node->scale.x, node->scale.y, 1.0f);
}

aNode2D Node2DCreate(const char* name)
{
    SNode2D* node = calloc(1, sizeof(SNode2D));
    if (node == NULL)
        return NULL;

    node->ident = NODE2D_IDENT;
    node->visible = true;
    node->scale.x = node->scale.y = 1.0f;
    Node2DSetTransform(node);
    if (name != NULL)
        StrCopy(node->name, name, MAX_NAME_LENGTH);

    node->images = ListCreate();
    node->texts  = ListCreate();

    if ((node->images != NULL) && (node->texts != NULL))
    {
        if (ListAddElement(_nodes2d, node))
            return (aNode2D)node;
    }

    node->ident = 0;
    ListDestroy(&node->images);
    ListDestroy(&node->texts);

    free(node);
    node = NULL;

    return NULL;
}

void Node2DDestroy(aNode2D* node2D)
{
    if (node2D == NULL || !ObjectIsNode2D(*node2D))
        return;

    SNode2D* node = (SNode2D*)*node2D;
    node->ident = 0;
    ListDestroy(&node->images);
    ListDestroy(&node->texts);

    /* delete from list */
    ListDeleteElementByValue(_nodes2d, node);

    /* clear SNode2D struct */
    free(node);
    *node2D = NULL;
    node = NULL;
}


inline bool Node2DClearAllImageLinks(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    ListClear(((SNode2D*)node2D)->images);

    return true;
}

inline bool Node2DClearAllTextLinks(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    ListClear(((SNode2D*)node2D)->texts);

    return true;
}


inline bool Node2DAddLinkToImage(aNode2D node2D, aImage image)
{
    IS_NODE2D_VALID(node2D);
    IS_IMAGE_VALID(image);

    return ListAddElement(((SNode2D*)node2D)->images, image);
}

inline bool Node2DAddLinkToText(aNode2D node2D, aText text)
{
    IS_NODE2D_VALID(node2D);
    IS_TEXT_VALID(text);

    return ListAddElement(((SNode2D*)node2D)->texts, text);
}

inline bool Node2DDeleteLinkToImage(aNode2D node2D, aImage image)
{
    IS_NODE2D_VALID(node2D);
    IS_IMAGE_VALID(image);

    ListDeleteElementByValue(((SNode2D*)node2D)->images, image);

    return true;
}

inline bool Node2DDeleteLinkToImageByNumber(aNode2D node2D, uint32 numOfImage)
{
    IS_NODE2D_VALID(node2D);

    ListDeleteElementByNumber(((SNode2D*)node2D)->images, numOfImage);

    return true;
}

inline bool Node2DDeleteLinkToText(aNode2D node2D, aText text)
{
    IS_NODE2D_VALID(node2D);
    IS_TEXT_VALID(text);

    ListDeleteElementByValue(((SNode2D*)node2D)->texts, text);

    return true;
}

inline bool Node2DDeleteLinkToTextByNumber(aNode2D node2D, uint32 numOfText)
{
    IS_NODE2D_VALID(node2D);

    ListDeleteElementByNumber(((SNode2D*)node2D)->texts, numOfText);

    return true;
}

inline const char* Node2DGetName(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->name;
}

void Nodes2DGetByName(const char* name, SList* results)
{
    if (_nodes2d->size == 0)
        return;

    if (results == NULL)
        return;

    /* prepare  results */
    ListClear(results);

    /* search all nodes with name */
    SNode2D* node = NULL;
    for (SListElement* element = _nodes2d->first; element; element = element->next)
    {
        node = (SNode2D*)element->value;
        if (ObjectIsNode2D(node))
        {
            if ((node->name == NULL && name == NULL) ||
                (StrEqual(node->name, name) == true))
            {
                ListAddElement(results, node);
            }
        }
    }
}

inline bool Node2DIsVisible(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->visible;
}

bool Node2DGetPivotf(aNode2D node2D, float* x, float* y)
{
    IS_NODE2D_VALID(node2D);

    if (x != NULL)
        *x = ((SNode2D*)node2D)->pivot.x;
    if (y != NULL)
        *y = ((SNode2D*)node2D)->pivot.y;

    return true;
}

SVector2f Node2DGetPivotv(aNode2D node2D)
{
    if (!ObjectIsNode2D(node2D))
        return vZero2f;

    return ((SNode2D*)node2D)->pivot;
}

float Node2DGetPivotX(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->pivot.x;
}

float Node2DGetPivotY(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->pivot.y;
}

bool Node2DGetPosf(aNode2D node2D, float* x, float* y)
{
    IS_NODE2D_VALID(node2D);

    if (x != NULL)
        *x = ((SNode2D*)node2D)->position.x;
    if (y != NULL)
        *y = ((SNode2D*)node2D)->position.y;

    return true;
}

SVector2f Node2DGetPosv(aNode2D node2D)
{
    if (!ObjectIsNode2D(node2D))
        return vZero2f;

    return ((SNode2D*)node2D)->position;
}

float Node2DGetPosX(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->position.x;
}

float Node2DGetPosY(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->position.y;
}

bool Node2DGetScalef(aNode2D node2D, float* x, float* y)
{
    IS_NODE2D_VALID(node2D);

    if (x != NULL)
        *x = ((SNode2D*)node2D)->scale.x;
    if (y != NULL)
        *y = ((SNode2D*)node2D)->scale.y;

    return true;
}

SVector2f Node2DGetScalev(aNode2D node2D)
{
    if (!ObjectIsNode2D(node2D))
        return vZero2f;

    return ((SNode2D*)node2D)->scale;
}

float Node2DGetScaleX(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->scale.x;
}

float Node2DGetScaleY(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->scale.y;
}

float Node2DGetRotation(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->rotation;
}


inline aImage Node2DGetImageByNumber(aNode2D node2D, uint32 numOfImage)
{
    IS_NODE2D_VALID(node2D);

    return ListGetValueByNumber(((SNode2D*)node2D)->images, numOfImage);
}

inline aText Node2DGetTextByNumber(aNode2D node2D, uint32 numOfText)
{
    IS_NODE2D_VALID(node2D);

    return ListGetValueByNumber(((SNode2D*)node2D)->texts, numOfText);
}

inline uint32 Node2DGetNumberOfImage(aNode2D node2D, aImage image)
{
    IS_NODE2D_VALID(node2D);

    return ListGetNumberByValue(((SNode2D*)node2D)->images, image);
}

inline uint32 Node2DGetNumberOfText(aNode2D node2D, aText text)
{
    IS_NODE2D_VALID(node2D);

    return ListGetNumberByValue(((SNode2D*)node2D)->texts, text);
}

inline uint32 Node2DGetCountOfImageLinks(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->images->size;
}

inline uint32 Node2DGetCountOfTextLinks(aNode2D node2D)
{
    IS_NODE2D_VALID(node2D);

    return ((SNode2D*)node2D)->texts->size;
}

bool ObjectIsNode2D(void* object)
{
    if (object != NULL && (*((uint32*)object) == NODE2D_IDENT))
        return true;

    return false;
}


inline bool Node2DSetName(aNode2D node2D, const char* name)
{
    IS_NODE2D_VALID(node2D);

    StrCopy(((SNode2D*)node2D)->name, name, MAX_NAME_LENGTH);

    return true;
}

inline bool Node2DSetVisible(aNode2D node2D, bool isVisible)
{
    IS_NODE2D_VALID(node2D);

    ((SNode2D*)node2D)->visible = isVisible;

    return true;
}

bool Node2DSetPivotf(aNode2D node2D, float x, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->pivot.x = x;
    node->pivot.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPivotv(aNode2D node2D, SVector2f pivot)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->pivot = pivot;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPivotX(aNode2D node2D, float x)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->pivot.x = x;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPivotY(aNode2D node2D, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->pivot.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPosf(aNode2D node2D, float x, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->position.x = x;
    node->position.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPosv(aNode2D node2D, SVector2f position)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->position = position;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPosX(aNode2D node2D, float x)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->position.x = x;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetPosY(aNode2D node2D, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->position.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetScalef(aNode2D node2D, float x, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->scale.x = x;
    node->scale.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetScalev(aNode2D node2D, SVector2f scale)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->scale = scale;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetScaleX(aNode2D node2D, float x)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->scale.x = x;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetScaleY(aNode2D node2D, float y)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    node->scale.y = y;

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetRotationInDeg(aNode2D node2D, float angle)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    if (angle < 0.0f)
        angle += 360.0f;
    if (angle >= 360.0f)
        angle -= 360.0f;
    node->rotation = DegToRad(angle);

    Node2DSetTransform (node);

    return true;
}

bool Node2DSetRotationInRad(aNode2D node2D, float angle)
{
    IS_NODE2D_VALID(node2D);

    SNode2D* node = (SNode2D*)node2D;
    if (angle < 0.0f)
        angle += 2.0f*M_PI;
    if (angle >= 2.0f*M_PI)
        angle -= 2.0f*M_PI;
    node->rotation = angle;

    Node2DSetTransform (node);

    return true;
}



inline uint32 Nodes2DGetCount()
{
    return _nodes2d->size;
}

void Nodes2DDestroyAll()
{
    if (_nodes2d->size == 0)
        return;

    /* clear SFont structs in list */
    SNode2D* node = NULL;
    for (SListElement* element = _nodes2d->first; element; element = element->next)
    {
        if (element->value)
        {
            node = (SNode2D*)element->value;
            node->ident = 0;
            free(node);
        }
    }

    /* delete all elements from list */
    ListClear(_nodes2d);
}
