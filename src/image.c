#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "dictionary.h"
#include "window_system.h"
#include "render_system.h"
#include "structs_private.h"
#include "image.h"


extern SDictionary* _images;

/** identificator of object as image */
#define IMG_IDENT 0x5F474D49   // 'I', 'M', 'G', '_' = 1598508361 = 0x5F474D49

aImage ImageCreate(const char* name)
{
    if (name == NULL)
        return NULL;

    /* изначально проверим нет ли уже элемента с таким именем */
    SImage* img = DictionaryGetValueByKey(_images, name);
    if (img != NULL)
        return NULL;

    img = calloc(1, sizeof(SImage));
    if (img == NULL)
        return NULL;

    img->ident = IMG_IDENT;

    /* пытаемся добавить новый элемент */
    if (DictionaryAddElement(_images, name, img))
        return (aImage)img;

    img->ident = 0;
    free(img);
    img = NULL;

    return NULL;
}

void ImageDestroy(aImage* image)
{
    if (image == NULL || !ObjectIsImage(*image))
        return;

    /* clear SImage struct */
    SImage* img = (SImage*)*image;
    img->ident  = 0;
    if (img->indVBO != 0)
        glDeleteBuffers(1, &(img->indVBO));

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_images, img);

    free(img);
    *image = NULL;
    img = NULL;
}

inline char* ImageGetName(aImage image)
{
    IS_IMAGE_VALID(image);

    return DictionaryGetKeyByValue(_images, image);
}

bool ImageGetPivotf(aImage image, float* x, float* y)
{
    IS_IMAGE_VALID(image);

    if (x != NULL)
        *x = ((SImage*)image)->pivot.x;
    if (y != NULL)
        *y = ((SImage*)image)->pivot.y;

    return true;
}

inline SVector2f ImageGetPivotv(aImage image)
{
    if (!ObjectIsImage(image))
        return vZero2f;

    return ((SImage*)image)->pivot;
}

inline float ImageGetPivotX(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->pivot.x;
}

inline float ImageGetPivotY(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->pivot.y;
}

bool ImageGetSize(aImage image, uint16* width, uint16* height)
{
    if (!ObjectIsImage(image))
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
            *width  = ImageGetWidth(image);
        if (height != NULL)
            *height = ImageGetHeight(image);
    }

    return true;
}

inline uint16 ImageGetWidth(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->width;
}

inline uint16 ImageGetHeight(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->height;
}

inline aTexture ImageGetTexture(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->texture;
}

inline aMaterial ImageGetMaterial(aImage image)
{
    IS_IMAGE_VALID(image);

    return ((SImage*)image)->material;
}

inline aImage ImageGetByName(const char* name)
{
    if (name == NULL || _images->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_images, name);
}

inline uint32 ImageGetGLTextureID(aImage image)
{
    IS_IMAGE_VALID(image);

    return TextureGetGLTextureID(((SImage*)image)->texture);
}

bool ObjectIsImage(void* object)
{
    if (object != NULL && (*((uint32*)object) == IMG_IDENT))
        return true;

    return false;
}

inline bool ImageSetName(aImage image, const char* name)
{
    IS_IMAGE_VALID(image);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_images, image, name);
}

inline bool ImageSetPivotf(aImage image, float x, float y)
{
    IS_IMAGE_VALID(image);

    if (ImageSetPivotX(image, x) && ImageSetPivotY(image, y))
        return true;

    return false;
}

inline bool ImageSetPivotv(aImage image, SVector2f pivot)
{
    return ImageSetPivotf(image, pivot.x, pivot.y);
}

inline bool ImageSetPivotX(aImage image, float x)
{
    IS_IMAGE_VALID(image);

    SImage* img = (SImage*)image;

    img->pivot.x = x;

    x = -x;

    // поправляем те вершины, которые изменились (1-я и 4-я)
    if (img->indVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float), &x);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*6, sizeof(float), &x);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ImageSetWidth(image, img->width);

    return true;
}

inline bool ImageSetPivotY(aImage image, float y)
{
    IS_IMAGE_VALID(image);

    SImage* img = (SImage*)image;

    img->pivot.y = y;

    // поправляем те вершины, которые изменились (1-я и 2-я)
    if (img->indVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*1, sizeof(float), &y);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*3, sizeof(float), &y);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ImageSetHeight(image, img->height);

    return true;
}

bool ImageSetWidth(aImage image, uint16 width)
{
    IS_IMAGE_VALID(image);

    SImage* img = (SImage*)image;
    img->width  = width;

    // нам нужно передать флоаты
    float w = (float)width - img->pivot.x;

    // поправляем те вершины, которые изменились (2-я и 3-я)
    if (img->indVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*2, sizeof(float), &w);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*4, sizeof(float), &w);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}

bool ImageSetHeight(aImage image, uint16 height)
{
    IS_IMAGE_VALID(image);

    SImage* img = (SImage*)image;
    img->height  = height;

    // нам нужно передать флоаты
    float h = -((float)height) + img->pivot.y;

    // поправляем те вершины, которые изменились (3-я и 4-я)
    if (img->indVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*5, sizeof(float), &h);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*7, sizeof(float), &h);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}

inline bool ImageSetSize(aImage image, uint16 width, uint16 height)
{
    IS_IMAGE_VALID(image);

    ImageSetWidth(image, width);
    ImageSetHeight(image, height);

    return true;
}

bool ImageSetTexture(aImage image, aTexture texture)
{
    IS_IMAGE_VALID(image);

    if (texture != NULL && !ObjectIsTexture(texture))
        return false;

    SImage* img = (SImage*)image;

    if (texture == NULL)
    {
        img->texture = NULL;
        img->width  = 0;
        img->height = 0;
        return false;
    }

    STexture* txr = (STexture*)texture;
    img->width  = txr->width;
    img->height = txr->height;
    img->texture = texture;

    // сгенерировать VBO
    if (img->indVBO != 0)
        glDeleteBuffers(1, &(img->indVBO));
    glGenBuffers(1, &(img->indVBO));
    glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);

    float vertices[8] = { 0.0f,       0.0f,
                          img->width, 0.0f,
                          img->width, -(img->height),
                          0.0f,       -(img->height) };
    const float imgTexCoords[8] = { 0.0f, 0.0f,
                                    1.0f, 0.0f,
                                    1.0f, 1.0f,
                                    0.0f, 1.0f };

    // сначала указываем размер данных в буффере, а потом наполняем по двум частям
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(imgTexCoords), 0, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(imgTexCoords), imgTexCoords);

    // открепляем буффер
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    txr = NULL;
    img = NULL;

    return true;
}

inline bool ImageSetMaterial(aImage image, aMaterial material)
{
    IS_IMAGE_VALID(image);

    if (material != NULL && !ObjectIsMaterial(material))
        return false;

    ((SImage*)image)->material = material;
    if (material != NULL)
        return ImageSetTexture(image, ((SMaterial*)material)->textures[0]);
    else
        return false;
}

inline uint32 ImagesGetCount()
{
    return _images->size;
}

void ImagesDestroyAll()
{
    if (_images->size == 0)
        return;

    /* clear SImage structs in dictionary */
    SImage* img = NULL;
    for (SDictElement* element = _images->first; element; element = element->next)
    {
        if (element->value)
        {
            img = (SImage*)element->value;
            img->ident = 0;
            if (img->indVBO != 0)
                glDeleteBuffers(1, &(img->indVBO));
            free(img);
        }
    }

    /* delete all elements from dictionary */
    DictionaryClear(_images);
}

void ImageDraw(aImage image)
{
    if (!ObjectIsImage(image))
        return;

    SImage* img = (SImage*)image;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glCullFace (GL_NONE);
    glDisable (GL_CULL_FACE);
    glDisable (GL_LIGHTING);

    // если указана материал, то биндим текстуру из первого слоя
    // а если указана сразу текстура, то биндим её
    if (img->material != NULL)
        MaterialApply(img->material);
    else
        if (img->texture != NULL)
            glBindTexture (GL_TEXTURE_2D, ((STexture*)img->texture)->ogl_id);

    // рисуем квадик
    glEnable (GL_TEXTURE_2D);

    /* рисуем через VBO */
    if (img->indVBO != 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, img->indVBO);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, (void*)0);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(float)*8));    // данные по координатам находятся за вершинами
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glDisable (GL_TEXTURE_2D);
}


/*  TESTS!!! */
//#define _IMAGE_DEBUG
#ifdef _DEBUG
#include "texture.h"
#include "material.h"
#include "image.h"
#include "font.h"
#include "text.h"
void ImageTest()
{
    aImage image = NULL, image2 = NULL;
    char* name = NULL;
    uint16 width = 0, height = 0;
    uint32 ogl_id = 0;
    uint32 count = 0;
    SVector2f pivot = vZero2f;
    bool result = false;

    aTexture texture = TextureCreateFromFile("media/textures/infantry.pcx", "infantry", false);
    assert(texture != NULL);
    aMaterial material = MaterialCreate("material");
    assert(material != NULL);
    MaterialSetTexture(material, texture);
    assert(MaterialGetTexture(material) == texture);

    /* test1 : create and delete */
    printf("--------test1--------\n");
    image = ImageCreate("image");
    assert(image != NULL);
    assert(ImagesGetCount() == 1);

    result = ObjectIsTexture(image);
    assert(result == false);
    result = ObjectIsMaterial(image);
    assert(result == false);
    result = ObjectIsImage(image);
    assert(result == true);
    result = ObjectIsText(image);
    assert(result == false);
    result = ObjectIsFont(image);
    assert(result == false);

    ImageDestroy(&image);
    assert(image == NULL);
    assert(ImagesGetCount() == 0);
    ImageDestroy(&image2);
    result = ObjectIsImage(image);
    assert(result == false);
    result = ObjectIsImage(image2);
    assert(result == false);
    printf("passed.\n");

    image = ImageCreate("image");

    /* test2 : set texture and material */
    printf("--------test2--------\n");
    ImageSetMaterial(image, material);
    printf("1.\n");
    assert(ImageGetMaterial(image) == material);
    printf("1a.\n");
    assert(ImageGetTexture(image) == texture);
    printf("1b.\n");

    ImageSetMaterial(image, NULL);
    printf("2.\n");
    assert(ImageGetMaterial(image) == NULL);
    printf("2a.\n");

    ImageSetTexture(image, NULL);
    assert(ImageGetTexture(image) == NULL);
    printf("3.\n");

    printf("passed.\n");

    /* test3 : get name */
    printf("--------test3--------\n");
    name = ImageGetName(image);
    assert(name != NULL);
    assert(StrEqual(name, "image") == true);
    name = ImageGetName(image2);
    assert(name == NULL);
    printf("passed.\n");

    /* test4 : get size */
    printf("--------test4--------\n");
    ImageSetMaterial(image, material);
    width = height = 0;
    ImageGetSize(image, &width, &height);
    assert(width == 276 && height == 194);
    width = height = 0;
    width  = ImageGetWidth(image);
    height = ImageGetHeight(image);
    assert(width == 276 && height == 194);

    width = height = 0;
    ImageGetSize(image2, &width, &height);
    assert(width == 0 && height == 0);

    width = height = 0;
    ImageGetSize(image2, NULL, NULL);
    assert(width == 0 && height == 0);

    width = height = 0;
    ImageGetSize(image, NULL, &height);
    assert(width == 0 && height == 194);

    width = height = 0;
    ImageGetSize(image, &width, NULL);
    assert(width == 276 && height == 0);
    printf("passed.\n");

    /* test5 get ogl_id */
    printf("--------test5--------\n");
    ogl_id = ImageGetGLTextureID(image);
    assert(ogl_id != 0);
    printf("passed.\n");

    /* test6 get/set pivot of image */
    printf("--------test6--------\n");
    pivot = ImageGetPivotv(image);
    assert(pivot.x == 0 && pivot.y == 0);
    ImageSetPivotf(image, 10, 10);
    pivot = ImageGetPivotv(image);
    assert(pivot.x == 10 && pivot.y == 10);
    pivot.x = pivot.y = 0;
    ImageGetPivotf(image, &pivot.x, &pivot.y);
    assert(pivot.x == 10 && pivot.y == 10);
    pivot.x = pivot.y = 0;
    pivot.x = ImageGetPivotX(image);
    pivot.y = ImageGetPivotY(image);
    assert(pivot.x == 10 && pivot.y == 10);
    printf("passed.\n");

    /* test7 get image */
    printf("--------test7--------\n");
    image2 = ImageGetByName("image");
    assert(image == image2);
    printf("passed.\n");

    /* test8 get count of loaded images */
    printf("--------test8--------\n");
    count = ImagesGetCount();
    assert(count == 1);
    printf("passed.\n");

    /* test9 set new name */
    printf("--------test9--------\n");
    ImageSetName(image, NULL);
    ImageSetName(image2, NULL);
    assert(ImageGetName(image2) != NULL);
    ImageSetName(image2, "infantry");
    assert(StrEqual(ImageGetName(image2), "infantry") == true);
    printf("passed.\n");

    /* test10 delete all images */
    printf("--------test10--------\n");
    ImagesDestroyAll();
    count = ImagesGetCount();
    assert(count == 0);
    printf("passed.\n");

    image = NULL;
    image2 = NULL;

    /* passed */
    printf ("--------result-------\n");
    printf ("all image's tests are passed!\n");
}
#endif // _DEBUG
