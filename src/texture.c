#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "defines.h"
#include "structs_private.h"
#include "dictionary.h"
#include "texture.h"
#include "utils.h"
#include "log_system.h"
#include "file_system.h"
#include "render_system.h"
#include "resource_manager.h"


extern SDictionary* _textures;

/* number of loaded textures to OpenGL */
static uint32 _numOfLoadedFiles = 0;

/** identificator of object as texture */
#define TXR_IDENT 0x5F525854   // 'T', 'X', 'R', '_' = 1599232084 = 0x5F525854

/** return > 0 if file already loaded */
static uint32 TexturesGetCountOfDuplicate(const char* fileName)
{
    if (fileName == NULL)
        return 0;

    if (_textures->size == 0)
        return 0;

    uint32 countOfDuplicates = 0;
    for (SDictElement* element = _textures->first; element; element = element->next)
    {
        if (element->value != NULL)
        {
            if (StrEqual(((STexture*)element->value)->fileName, fileName) == true)
                ++countOfDuplicates;
        }
    }

    return countOfDuplicates;
}

aTexture TextureCreate (const char* name)
{
    if (name == NULL)
        return NULL;

    /* изначально проверим нет ли уже элемента с таким именем */
    STexture* txr = DictionaryGetValueByKey(_textures, name);
    if (txr != NULL)
        return NULL;

    txr = calloc(1, sizeof(STexture));
    if (txr == NULL)
        return NULL;

    txr->ident = TXR_IDENT;

    /* пытаемся добавить новый элемент */
    if (DictionaryAddElement(_textures, name, txr))
        return (aTexture)txr;

    txr->ident = 0;
    free(txr);
    txr = NULL;

    return NULL;
}

bool TextureLoadFromFile (aTexture texture, const char* fileName, bool buildMipMaps)
{
    IS_TEXTURE_VALID(texture);
    if (fileName == NULL)
        return false;

    if (!FileExists(fileName))
    {
        LogWriteError("File '%s' is not exists!\n", fileName);
        return false;
    }

    STexture* txr = (STexture*)texture;

    /* save name of file */
    StrCopy(txr->fileName, fileName, MAX_PATH_LENGTH);

    /* if texture already loaded, get opengl_id */
    if (TexturesGetCountOfDuplicate(fileName) > 1)    // >1 because first is current!
    {
        LogWriteWarning("File '%s' is already loaded!\n", fileName);
        STexture* txrExist = (STexture*)TextureGetByFileName (fileName);
        if (txrExist != NULL)
        {
            txr->width  = txrExist->width;
            txr->height = txrExist->height;
            txr->ogl_id = txrExist->ogl_id;
        }

        txrExist = NULL;

        return true;
    }

    /* */
    _numOfLoadedFiles++;

    /* else load new texture to OpenGL */
    int32 imgWidth = 0, imgHeight = 0;
    txr->ogl_id = RM_LoadGLTexture(fileName, &imgWidth, &imgHeight, buildMipMaps);

    /* save size of image */
    txr->width = (uint16)imgWidth;
    txr->height = (uint16)imgHeight;

    txr = NULL;

    return true;
}

aTexture TextureCreateFromFile(const char* name, const char* fileName, bool buildMipMaps)
{
    if (fileName == NULL || name == NULL)
        return NULL;

    if (!FileExists(fileName))
        return NULL;

    aTexture txr = TextureCreate(name);
    if (txr == NULL)
        return NULL;

    if (!TextureLoadFromFile(txr, fileName, buildMipMaps))
    {
        TextureDestroy(&txr);
        return NULL;
    }

    return txr;
}

void TextureDestroy(aTexture* texture)
{
    if (texture == NULL || !ObjectIsTexture(*texture))
        return;

    /* clear STexture struct */
    STexture* txr = (STexture*)*texture;
    txr->ident = 0;
    uint32 ogl_id = txr->ogl_id;
    if (ogl_id != 0)
    {
        // перед удалением текстуры из OpenGL проверить, что эта текстура не используется ещё кем-то
        bool isAnybodyUses = false;
        for (SDictElement* element = _textures->first; element; element = element->next)
        {
            if (element->value != NULL &&
                element->value != txr)
            {
                if (((STexture*)element->value)->ogl_id == ogl_id)
                {
                    isAnybodyUses = true;
                    break;
                }
            }
        }
        if (!isAnybodyUses)
        {
            glDeleteTextures(1, &ogl_id);
            _numOfLoadedFiles--;
        }
    }

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_textures, txr);

    free(txr);
    *texture = NULL;
    txr = NULL;
}

inline char* TextureGetName(aTexture texture)
{
    IS_TEXTURE_VALID(texture);

    return DictionaryGetKeyByValue(_textures, texture);
}

inline char* TextureGetFileName(aTexture texture)
{
    IS_TEXTURE_VALID(texture);

    return ((STexture*)texture)->fileName;
}

bool TextureGetSize(aTexture texture, uint16* width, uint16* height)
{
    if (!ObjectIsTexture(texture))
    {
        if (width != NULL)
            *width  = 0;
        if (height != NULL)
            *height = 0;

        return false;
    }
    else
    {
        if (width != NULL)
            *width  = TextureGetWidth(texture);
        if (height != NULL)
            *height = TextureGetHeight(texture);
    }

    return true;
}

inline uint16 TextureGetWidth(aTexture texture)
{
    IS_TEXTURE_VALID(texture);

    return ((STexture*)texture)->width;
}

inline uint16 TextureGetHeight(aTexture texture)
{
    IS_TEXTURE_VALID(texture);

    return ((STexture*)texture)->height;
}

inline uint32 TextureGetGLTextureID(aTexture texture)
{
    IS_TEXTURE_VALID(texture);

    return ((STexture*)texture)->ogl_id;
}

aTexture TextureGetByFileName(const char* fileName)
{
    if (fileName == NULL)
        return NULL;

    if (_textures->size == 0)
        return NULL;

    STexture* txr = NULL;
    for (SDictElement* element = _textures->first; element && (element->value); element = element->next)
    {
        txr = (STexture*)(element->value);
        if (StrEqual(txr->fileName, fileName) == true)
            return txr;
    }

    return NULL;
}

inline aTexture TextureGetByName(const char* name)
{
    if (name == NULL || _textures->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_textures, name);
}

bool ObjectIsTexture(void* object)
{
    if (object != NULL && (*((uint32*)object) == TXR_IDENT))
        return true;

    return false;
}

inline bool TextureSetName(aTexture texture, const char* name)
{
    IS_TEXTURE_VALID(texture);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_textures, texture, name);
}

inline uint32 TexturesGetCount()
{
    return _textures->size;
}

inline uint32 TexturesGetCountInOpenGL()
{
    return _numOfLoadedFiles;
}

void TexturesDestroyAll()
{
    if (_textures->size == 0)
        return;

    /* clear STexture structs in dictionary */
    STexture* txr = NULL;
    for (SDictElement* element = _textures->first; element; element = element->next)
    {
        if (element->value)
        {
            txr = (STexture*)element->value;
            txr->ident = 0;
            if (txr->ogl_id != 0)
                glDeleteTextures(1, &(txr->ogl_id));
            free(txr);
        }
    }

    /* delete all elements from dictionary */
    DictionaryClear(_textures);

    _numOfLoadedFiles = 0;
}


/*  TESTS!!! */
//#define _TEXTURE_DEBUG
#ifdef _DEBUG
#include "texture.h"
#include "material.h"
#include "image.h"
#include "font.h"
#include "text.h"
void TextureTest()
{
    aTexture texture = NULL, texture2 = NULL;
    char* name = NULL;
    char* path = NULL;
    uint16 width = 0, height = 0;
    uint32 ogl_id = 0;
    uint32 count = 0;
    bool result = false;

    /* test1 : create and delete */
    printf("--------test1--------\n");
    texture = TextureCreate("texture");
    assert(texture != NULL);
    assert(TexturesGetCount() == 1);
    TextureLoadFromFile(texture, "media/textures/infantry.pcx", false);
    assert(TextureGetGLTextureID(texture) != 0);

    result = ObjectIsTexture(texture);
    assert(result == true);
    result = ObjectIsMaterial(texture);
    assert(result == false);
    result = ObjectIsImage(texture);
    assert(result == false);
    result = ObjectIsText(texture);
    assert(result == false);
    result = ObjectIsFont(texture);
    assert(result == false);

    TextureDestroy(&texture);
    assert(texture == NULL);
    assert(TexturesGetCount() == 0);
    result = ObjectIsTexture(texture);
    assert(result == false);
    TextureDestroy(&texture2);
    result = ObjectIsTexture(texture2);
    assert(result == false);
    printf("passed.\n");

    /* test2 : create from file */
    printf("--------test2--------\n");
    texture = TextureCreateFromFile("media/textures/infantry.pcx", "infantry", false);
    assert(texture != NULL);
    assert(TextureGetGLTextureID(texture) != 0);
    printf("passed.\n");

    /* test3 : get name */
    printf("--------test3--------\n");
    name = TextureGetName(texture);
    assert(name != NULL);
    assert(StrEqual(name, "infantry") == true);
    name = TextureGetName(texture2);
    assert(name == NULL);
    printf("passed.\n");

    /* test4 : get path */
    printf("--------test4--------\n");
    path = TextureGetFileName(texture);
    assert(path != NULL);
    assert(StrEqual(path, "media/textures/infantry.pcx") == true);
    path = TextureGetFileName(texture2);
    assert(path == NULL);
    printf("passed.\n");

    /* test5 : get size */
    printf("--------test5--------\n");
    width = height = 0;
    TextureGetSize(texture, &width, &height);
    assert(width == 276 && height == 194);
    width = height = 0;
    width  = TextureGetWidth(texture);
    height = TextureGetHeight(texture);
    assert(width == 276 && height == 194);

    width = height = 0;
    TextureGetSize(texture2, &width, &height);
    assert(width == 0 && height == 0);

    width = height = 0;
    TextureGetSize(texture2, NULL, NULL);
    assert(width == 0 && height == 0);

    width = height = 0;
    TextureGetSize(texture, NULL, &height);
    assert(width == 0 && height == 194);

    width = height = 0;
    TextureGetSize(texture, &width, NULL);
    assert(width == 276 && height == 0);
    printf("passed.\n");

    /* test6 get ogl_id */
    printf("--------test6--------\n");
    ogl_id = TextureGetGLTextureID(texture);
    assert(ogl_id != 0);
    printf("passed.\n");

    /* test7 get texture */
    printf("--------test7--------\n");
    texture2 = TextureGetByFileName("media/textures/infantry.pcx");
    assert(texture == texture2);
    printf("passed.\n");

    /* test8 get texture */
    printf("--------test8--------\n");
    texture2 = TextureGetByName("infantry");
    assert(texture == texture2);
    printf("passed.\n");

    /* test9 load duplicates */
    printf("--------test9--------\n");
    texture2 = TextureCreateFromFile("media/textures/infantry.pcx", "infantry", false);
    assert(texture2 == NULL);
    texture2 = TextureCreateFromFile("media/textures/infantry.pcx", "infantry2", false);
    assert(TextureGetGLTextureID(texture) == TextureGetGLTextureID(texture2));
    count = TexturesGetCountInOpenGL();
    assert(count == 1);
    printf("passed.\n");

    /* test10 get count of created textures */
    printf("--------test10--------\n");
    count = TexturesGetCount();
    assert(count == 2);
    printf("passed.\n");

    /* test11 delete used texture from OpenGL */
    printf("--------test11--------\n");
    TextureDestroy(&texture);
    count = TexturesGetCount();
    assert(count == 1);
    count = TexturesGetCountInOpenGL();
    assert(count == 1);
    printf("passed.\n");

    /* test12 set new name */
    printf("--------test12--------\n");
    TextureSetName(texture, NULL);
    TextureSetName(texture2, NULL);
    assert(TextureGetName(texture2) != NULL);
    TextureSetName(texture2, "infantry");
    assert(StrEqual(TextureGetName(texture2), "infantry") == true);
    printf("passed.\n");

    /* test13 get count of loaded textures */
    printf("--------test13--------\n");
    count = TexturesGetCount();
    assert(count == 1);
    printf("passed.\n");

    /* test14 delete all textures */
    printf("--------test14--------\n");
    TexturesDestroyAll();
    count = TexturesGetCountInOpenGL();
    assert(count == 0);
    count = TexturesGetCount();
    assert(count == 0);
    printf("passed.\n");

    /* passed */
    printf ("--------result-------\n");
    printf ("all texture's tests are passed!\n");
}
#endif // _DEBUG
