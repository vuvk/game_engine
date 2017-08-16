#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "utils.h"
#include "structs_private.h"
#include "dictionary.h"
#include "texture.h"
#include "material.h"
#include "log_system.h"
#include "render_system.h"



extern SDictionary* _materials;

/** identificator of object as material */
#define MAT_IDENT 0x5F54414D   // 'M', 'A', 'T', '_' = 1599357261 = 0x5F54414D

aMaterial MaterialCreate(const char* name)
{
    if (name == NULL)
        return NULL;

    /* изначально проверим нет ли уже элемента с таким именем */
    SMaterial* mat = DictionaryGetValueByKey(_materials, name);
    if (mat != NULL)
        return NULL;

    mat = calloc(1, sizeof(SMaterial));
    if (mat == NULL)
        return NULL;

    mat->ident = MAT_IDENT;

    /* пытаемся добавить новый элемент */
    if (DictionaryAddElement(_materials, name, mat))
        return (aMaterial)mat;

    mat->ident = 0;
    free(mat);
    mat = NULL;

    return NULL;
}

void MaterialDestroy(aMaterial* material)
{
    if (material == NULL || !ObjectIsMaterial(*material))
        return;

    SMaterial* mat = (SMaterial*)*material;
    mat->ident = 0;

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_materials, mat);

    free(mat);
    *material = NULL;
    mat = NULL;
}


bool MaterialApply(aMaterial material)
{
    IS_MATERIAL_VALID(material);

    SMaterial* mat = (SMaterial*)material;

    // apply culling
    glEnable(GL_CULL_FACE);
    switch (mat->cullFace)
    {
        case CF_NONE  :
        {
            glCullFace (GL_NONE);
            glDisable(GL_CULL_FACE);
            break;
        }

        case CF_BACK  :
        {
            glCullFace(GL_BACK);
            break;
        }

        case CF_FRONT :
        {
            glCullFace(GL_FRONT);
            break;
        }

        case CF_FRONT_AND_BACK :
        {
            glCullFace(GL_FRONT_AND_BACK);
            break;
        }
    }

    // apply type
    glDisable(GL_BLEND);
    switch (mat->type)
    {
        case MT_DEFAULT :
        {
            glShadeModel(GL_SMOOTH);
            break;
        }

        case MT_FLAT_SHADING :
        {
            glShadeModel(GL_FLAT);
            break;
        }

        case MT_ALPHA_BLENDED :
        {
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
            break;
        }

        case MT_POINT_FILTER :
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        }
    }

    // apply texture
    glDisable(GL_TEXTURE_2D);
    if (mat->textures[0] != NULL)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, TextureGetGLTextureID(mat->textures[0]));
    }

    return true;
}


inline const char* MaterialGetName(aMaterial material)
{
    IS_MATERIAL_VALID(material);

    return DictionaryGetKeyByValue(_materials, material);
}

aTexture MaterialGetTextureEx(aMaterial material, uint8 layer)
{
    IS_MATERIAL_VALID(material);

    if (layer > (MAX_MATERIAL_LAYERS - 1))
    {
        LogWriteError("Max textures layer is %d!\n", MAX_MATERIAL_LAYERS - 1);
        return NULL;
    }

    return ((SMaterial*)material)->textures[layer];
}

inline int8 MaterialGetCullFace(aMaterial material)
{
    if (!ObjectIsMaterial(material))
        return -1;

    return ((SMaterial*)material)->cullFace;
}

inline int8 MaterialGetType(aMaterial material)
{
    if (!ObjectIsMaterial(material))
        return -1;

    return ((SMaterial*)material)->type;
}

inline void* MaterialGetGLProgram(aMaterial material)
{
    IS_MATERIAL_VALID(material);

    return ((SMaterial*)material)->glProgram;
}

inline uint32 MaterialGetGLTextureIDEx(aMaterial material, uint8 layer)
{
    IS_MATERIAL_VALID(material);

    if (layer > (MAX_MATERIAL_LAYERS - 1))
    {
        LogWriteError("Max textures layer is %d!\n", MAX_MATERIAL_LAYERS - 1);
        return 0;
    }

    return TextureGetGLTextureID(((SMaterial*)material)->textures[layer]);
}

inline aMaterial MaterialGetByName(const char* name)
{
    if (name == NULL || _materials->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_materials, name);
}

bool ObjectIsMaterial(void* object)
{
    if (object != NULL && (*((uint32*)object) == MAT_IDENT))
        return true;

    return false;
}

inline bool MaterialSetName(aMaterial material, const char* name)
{
    IS_MATERIAL_VALID(material);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_materials, material, name);
}

bool MaterialSetTextureEx(aMaterial material, aTexture texture, uint8 layer)
{
    IS_MATERIAL_VALID(material);

    if (texture != NULL && !ObjectIsTexture(texture))
        return false;

    if (layer > (MAX_MATERIAL_LAYERS - 1))
    {
        LogWriteError("Max textures layer is %d!\n", MAX_MATERIAL_LAYERS - 1);
        return false;
    }

    ((SMaterial*)material)->textures[layer] = texture;
    return true;
}

inline bool MaterialSetCullFace(aMaterial material, int8 cullFace)
{
    IS_MATERIAL_VALID(material);

    ((SMaterial*)material)->cullFace = cullFace;
    return true;
}

inline bool MaterialSetType(aMaterial material, int8 type)
{
    IS_MATERIAL_VALID(material);

    ((SMaterial*)material)->type = type;
    return true;
}

inline bool MaterialSetGLProgram(aMaterial material, void* glProgram)
{
    IS_MATERIAL_VALID(material);

    ((SMaterial*)material)->glProgram = glProgram;
    return true;
}

inline uint32 MaterialsGetCount()
{
    return _materials->size;
}

void MaterialsDestroyAll()
{
    if (_materials->size == 0)
        return;

    /* clear SMaterial structs in dictionary */
    SMaterial* mat = NULL;
    for (SDictElement* element = _materials->first; element; element = element->next)
    {
        if (element->value)
        {
            mat = (SMaterial*)element->value;
            mat->ident = 0;
            free(mat);
        }
    }

    /* delete all elements from dictionary */
    DictionaryClear(_materials);
}




/* TESTS! */
//#define _MATERIAL_DEBUG
#ifdef _DEBUG
#include "texture.h"
#include "material.h"
#include "image.h"
#include "font.h"
#include "text.h"
void MaterialTest()
{
    aMaterial material, material2;
    uint32 glProgramTest = 3;
    bool result = false;

    aTexture texture = TextureCreateFromFile("media/textures/infantry.pcx", "infantry", false);
    assert(texture != NULL);

    /* test1 : create and delete */
    printf("--------test1--------\n");
    material = MaterialCreate("material");
    assert(material != NULL);
    assert(MaterialsGetCount() == 1);

    result = ObjectIsTexture(material);
    assert(result == false);
    result = ObjectIsMaterial(material);
    assert(result == true);
    result = ObjectIsImage(material);
    assert(result == false);
    result = ObjectIsText(material);
    assert(result == false);
    result = ObjectIsFont(material);
    assert(result == false);

    MaterialDestroy(&material);
    assert(material == NULL);
    assert(MaterialsGetCount() == 0);
    result = ObjectIsMaterial(material);
    assert(result == false);
    printf("passed.\n");

    material = MaterialCreate("material");

    /* test2 : set/get new name */
    printf("--------test2--------\n");
    MaterialSetName(material, "material2");
    assert(StrEqual(MaterialGetName(material), "material2") == true);
    MaterialSetName(NULL, "material2");
    MaterialSetName(material, NULL);
    assert(StrEqual(MaterialGetName(material), "material2") == true);
    printf("passed.\n");

    /* test3 : set/get texture */
    printf("--------test3--------\n");
    MaterialSetTexture(material, texture);
    assert(MaterialGetTexture(material) == texture);
    MaterialSetTexture(NULL, texture);
    MaterialSetTexture(material, NULL);
    assert(MaterialGetTexture(material) == NULL);

    MaterialSetTextureEx(material, texture, 0);
    MaterialSetTextureEx(material, texture, 1);
    MaterialSetTextureEx(material, texture, 2);
    MaterialSetTextureEx(material, texture, 3);
    MaterialSetTextureEx(material, texture, 4);
    MaterialSetTextureEx(NULL, NULL, -10);
    assert(MaterialGetTextureEx(material, 0) == texture);
    assert(MaterialGetTextureEx(material, 1) == texture);
    assert(MaterialGetTextureEx(material, 2) == texture);
    assert(MaterialGetTextureEx(material, 3) == texture);
    assert(MaterialGetTextureEx(material, 4) == NULL);

    printf("passed.\n");

    /* test4 : set/get culling face */
    printf("--------test4--------\n");
    MaterialSetCullFace(material, CF_FRONT_AND_BACK);
    assert(MaterialGetCullFace(material) == CF_FRONT_AND_BACK);
    MaterialSetCullFace(NULL, CF_FRONT_AND_BACK);
    printf("passed.\n");

    /* test5 : set/get material type */
    printf("--------test5--------\n");
    MaterialSetType(material, MT_ALPHA_BLENDED);
    assert(MaterialGetType(material) == MT_ALPHA_BLENDED);
    MaterialSetType(NULL, MT_ALPHA_BLENDED);
    printf("passed.\n");

    /* test6 : set/get glProgram (emulation) */
    printf("--------test6--------\n");
    MaterialSetGLProgram(material, &glProgramTest);
    assert((uint32*)MaterialGetGLProgram(material) == &glProgramTest);
    MaterialSetGLProgram(material, NULL);
    assert(MaterialGetGLProgram(material) == NULL);
    printf("passed.\n");

    /* test7 : get texture-identificator for material from OpenGL */
    printf("--------test7--------\n");
    assert(MaterialGetGLTextureID(material) == TextureGetGLTextureID(texture));
    assert(MaterialGetGLTextureIDEx(material, 0) == TextureGetGLTextureID(texture));
    assert(MaterialGetGLTextureIDEx(material, 1) == TextureGetGLTextureID(texture));
    assert(MaterialGetGLTextureIDEx(material, 2) == TextureGetGLTextureID(texture));
    assert(MaterialGetGLTextureIDEx(material, 3) == TextureGetGLTextureID(texture));
    assert(MaterialGetGLTextureIDEx(material, -1) == 0);
    assert(MaterialGetGLTextureIDEx(material, 4) == 0);
    printf("passed.\n");

    /* test8 : get material by name */
    printf("--------test8--------\n");
    material2 = MaterialGetByName("material2");
    assert(material == material2);
    material2 = MaterialGetByName("");
    assert(material2 == NULL);
    material2 = MaterialGetByName(NULL);
    assert(material2 == NULL);
    printf("passed.\n");

    /* test9 clear all materials */
    printf("--------test9--------\n");
    assert(MaterialsGetCount() == 1);
    MaterialsDestroyAll();
    assert(MaterialsGetCount() == 0);
    printf("passed.\n");

    TextureDestroy(&texture);

    /* passed */
    printf ("--------result-------\n");
    printf ("all material's tests are passed!\n");
}
#endif // _DEBUG
