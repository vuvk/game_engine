#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "structs_private.h"
#include "utils.h"
#include "log_system.h"
#include "render_system.h"
#include "font.h"



extern SDictionary* _fonts;

/** identificator of object as font */
#define FNT_IDENT 0x5F544E46   // 'F', 'N', 'T', '_' = 1599360582 = 0x5F544E46

aFont FontCreate(const char* name)
{
    if (name == NULL)
        return NULL;

    /* First check if there is an element with that name */
    SFont* fnt = DictionaryGetValueByKey(_fonts, name);
    if (fnt != NULL)
        return NULL;

    fnt = calloc(1, sizeof(SFont));
    if (fnt == NULL)
        return NULL;

    fnt->ident = FNT_IDENT;

    /* Try add new item to dictionary */
    if (DictionaryAddElement(_fonts, name, fnt))
        return (aFont)fnt;

    fnt->ident = 0;
    free(fnt);
    fnt = NULL;

    return NULL;
}

void FontDestroy(aFont* font)
{
    if (font == NULL || !ObjectIsFont(*font))
        return;

    /* clear SFont struct */
    SFont* fnt = (SFont*)*font;
    glDeleteBuffers(256, fnt->charsVBO);
    fnt->ident = 0;

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_fonts, fnt);

    free(fnt);
    *font = NULL;
    fnt = NULL;
}

bool FontGenerateFromTexture(aFont font, aTexture texture,
                             uint16 xCount,  uint16 yCount,
                             int16  xOffset, int16  yOffset)
{
    IS_FONT_VALID(font);

    SFont* fnt = (SFont*)font;

    if (!ObjectIsTexture(texture))
    {
        glDeleteBuffers(256, fnt->charsVBO);
        memset(fnt->charsVBO, 0, 256*sizeof(uint32));
        return false;
    }

    uint16 charCount = xCount*yCount;
    if (charCount > 256)
    {
        LogWriteError("Maximum letters count is 256!\n");
        return false;
    }

    //STexture* txr = (STexture*)texture;

    // ограничивающий квадрат буквы на текстуре алфавита!
    uint16 charRectW, charRectH;
    // действительный размер буквы с учетом оффсетов
    uint16 charW, charH;

    // определяем размеры буквы
    charRectW = TextureGetWidth(texture) / xCount;
    charRectH = TextureGetHeight(texture) / yCount;
    charW = charRectW - xOffset*2;     // оффсет слева и справа так-то
    charH = charRectH - yOffset*2;     // а ещё он сверху и снизу так-то

    // такой будет шаг при сдвиге интерполяции текстурных координат
    float xTexCoordStep = 1.0f / xCount;
    float yTexCoordStep = 1.0f / yCount;
    // размер оффсета в диапазонее 0.0-1.0 для добавления к сдвигу
    float xTexCoordOffset = (float)xOffset / TextureGetWidth(texture);
    float yTexCoordOffset = (float)yOffset / TextureGetHeight(texture);


    // чистим массив с VBO букв
    // перед генерацией нового. На случай, если шрифт заменяется
    glDeleteBuffers(256, fnt->charsVBO);
    memset(fnt->charsVBO, 0, 256*sizeof(uint32));

    //
    // создаем массив с буквами (генерим VBO для каждой буквы)
    //

    // массив вершин для всех букв одиннаков
    float vertices[8] = {  0.0f,   0.0f,
                          charW,   0.0f,
                          charW, -charH,
                           0.0f, -charH  };
    // мы будем расчитывать текстурные координаты на текстуре с алфавитом
    // для каждого квадратика, в котором находится ОДНА буква
    float texCoords[8] = {};
    float xTexCoord = 0.0f, yTexCoord = 0.0f;
    uint32 w = 0;   // порядковый номер буквы
    for (uint16 j = 0; j < yCount; j++)
    {
        xTexCoord = 0.0f;

        for (uint16 i = 0; i < xCount; i++)
        {
            texCoords[0] = xTexCoord + xTexCoordOffset;                     // 1 coord
            texCoords[1] = yTexCoord + yTexCoordOffset;

            texCoords[2] = xTexCoord - xTexCoordOffset + xTexCoordStep;     // 2 coord
            texCoords[3] = yTexCoord + yTexCoordOffset;

            texCoords[4] = xTexCoord - xTexCoordOffset + xTexCoordStep;     // 3 coord
            texCoords[5] = yTexCoord - yTexCoordOffset + yTexCoordStep;

            texCoords[6] = xTexCoord + xTexCoordOffset;                     // 4 coord
            texCoords[7] = yTexCoord - yTexCoordOffset + yTexCoordStep;

            // запрашиваем указатель на буффер и биндим
            glGenBuffers(1, &(fnt->charsVBO[w]));
            glBindBuffer(GL_ARRAY_BUFFER, fnt->charsVBO[w]);

            // сначала указываем размер данных в буффере, а потом наполняем по двум частям
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), 0, GL_STATIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords), texCoords);

            // открепляем буффер
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            xTexCoord += xTexCoordStep;
            w++;
        }

        yTexCoord += yTexCoordStep;
    }

    fnt->texture = texture;
    fnt->charW = charW;
    fnt->charH = charH;

    fnt = NULL;

    return true;
}

inline const char* FontGetName(aFont font)
{
    IS_FONT_VALID(font);

    return DictionaryGetKeyByValue(_fonts, font);
}

inline aTexture FontGetTexture(aFont font)
{
    IS_FONT_VALID(font);

    return ((SFont*)font)->texture;
}

uint32 FontGetGLTextureID(aFont font)
{
    SFont* fnt = (SFont*)font;

    IS_FONT_VALID(fnt);
    if (!ObjectIsTexture(fnt->texture))
        return 0;

    return TextureGetGLTextureID(fnt->texture);
}

aFont FontGetByName(const char* name)
{
    if (name == NULL || _fonts->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_fonts, name);
}

uint16 FontGetCharWidth(aFont font)
{
    IS_FONT_VALID(font);

    return ((SFont*)font)->charW;
}

uint16 FontGetCharHeight(aFont font)
{
    IS_FONT_VALID(font);

    return ((SFont*)font)->charH;
}

bool ObjectIsFont(void* object)
{
    if (object != NULL && (*((uint32*)object) == FNT_IDENT))
        return true;

    return false;
}

bool FontSetName(aFont font, const char* name)
{
    IS_FONT_VALID(font);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_fonts, font, name);
}

inline uint32 FontsGetCount()
{
    return _fonts->size;
}

void FontsDestroyAll()
{
    if (_fonts->size == 0)
        return;

    /* clear SFont structs in list */
    SFont* fnt = NULL;
    for (SDictElement* element = _fonts->first; element; element = element->next)
    {
        if (element->value)
        {
            fnt = (SFont*)element->value;
            fnt->ident = 0;
            glDeleteBuffers(256, fnt->charsVBO);
            free(fnt);
        }
    }

    /* delete all elements from list */
    DictionaryClear(_fonts);
}



/*  TESTS!!! */
//#define _FONT_DEBUG
#ifdef _DEBUG
#include "texture.h"
#include "material.h"
#include "image.h"
#include "font.h"
#include "text.h"
void FontTest()
{
    aFont font = NULL, font2 = NULL;
    bool result = false;

    aTexture texture = TextureCreateFromFile("media/fonts/font.png", "font", false);
    assert(texture != NULL);

    /* test1 : create and delete */
    printf("--------test1--------\n");
    font = FontCreate("font");
    assert(font != NULL);
    assert(FontsGetCount() == 1);
    result = ObjectIsTexture(font);

    assert(result == false);
    result = ObjectIsMaterial(font);
    assert(result == false);
    result = ObjectIsImage(font);
    assert(result == false);
    result = ObjectIsText(font);
    assert(result == false);
    result = ObjectIsFont(font);
    assert(result == true);

    FontDestroy(&font);
    assert(font == NULL);
    assert(FontsGetCount() == 0);
    result = ObjectIsFont(font);
    assert(result == false);
    result = ObjectIsFont(font2);
    assert(result == false);
    printf("passed.\n");

    /* test2 : generate font from loaded texture */
    printf("--------test2--------\n");
    font = FontCreate("font");
    result = FontGenerateFromTexture(font, NULL, 10, 10, 0, 0);
    assert(result == false);
    result = FontGenerateFromTexture(font, texture, 30, 16, 5, 5);
    assert(result == false);
    result = FontGenerateFromTexture(font, texture, 16, 16, 5, 5);
    assert(result == true);
    printf("passed.\n");


    /* test3 : set/get new name */
    printf("--------test3--------\n");
    FontSetName(font, "new_name");
    assert(StrEqual(FontGetName(font), "new_name") == true);
    FontSetName(NULL, "new_name2");
    FontSetName(font, NULL);
    assert(StrEqual(FontGetName(font), "new_name") == true);
    printf("passed.\n");

    /* test4 : get texture */
    printf("--------test4--------\n");
    assert(FontGetTexture(font) == texture);
    printf("passed.\n");

    /* test5 : get texture-identificator from OpenGL */
    printf("--------test5--------\n");
    assert(FontGetGLTextureID(font) == TextureGetGLTextureID(texture));
    printf("passed.\n");

    /* test6 : get font by name */
    printf("--------test6--------\n");
    font2 = FontGetByName("new_name");
    assert(font == font2);
    font2 = MaterialGetByName("");
    assert(font2 == NULL);
    font2 = MaterialGetByName(NULL);
    assert(font2 == NULL);
    printf("passed.\n");

    /* test7 clear all fonts */
    printf("--------test7--------\n");
    assert(FontsGetCount() == 1);
    FontsDestroyAll();
    assert(FontsGetCount() == 0);
    printf("passed.\n");

    TextureDestroy(&texture);

    /* passed */
    printf ("--------result-------\n");
    printf ("all font's tests are passed!\n");
}
#endif // _DEBUG
