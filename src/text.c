#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "dictionary.h"
#include "structs_private.h"
#include "render_system.h"
#include "vector.h"
#include "text.h"



extern SDictionary* _texts;

/** identificator of object as text */
#define TXT_IDENT 0x5F545854   // 'T', 'X', 'T', '_' = 1599363156 = 0x5F545854

aText TextCreate(const char* name, aFont font, const char* string)
{
    if (name == NULL)
        return NULL;

    /* изначально проверим нет ли уже элемента с таким именем */
    SText* txt = DictionaryGetValueByKey(_texts, name);
    if (txt != NULL)
        return NULL;

    txt = calloc(1, sizeof(SText));
    if (txt == NULL)
        return NULL;

    txt->ident = TXT_IDENT;

    if (ObjectIsFont(font))
        txt->font = font;

    if (string != NULL)
        TextSetString((aText)txt, string);

    /* пытаемся добавить новый элемент */
    if (DictionaryAddElement(_texts, name, txt))
        return (aText)txt;

    txt->ident = 0;
    free(txt);
    txt = NULL;

    return NULL;
}

void TextDestroy(aText* text)
{
    if (text == NULL || !ObjectIsText(*text))
        return;

    /* clear SText struct */
    SText* txt = (SText*)*text;
    txt->ident = 0;
    if (txt->string != NULL)
        free(txt->string);
    if (txt->staticVBO != 0)
        glDeleteBuffers(1, &(txt->staticVBO));

    /* delete from dictionary */
    DictionaryDeleteElementByValue(_texts, txt);

    free(txt);
    *text = NULL;
    txt = NULL;
}

inline char* TextGetName(aText text)
{
    IS_TEXT_VALID(text);

    return DictionaryGetKeyByValue(_texts, text);
}

inline aFont TextGetFont(aText text)
{
    IS_TEXT_VALID(text);

    return ((SText*)text)->font;
}

inline char* TextGetString(aText text)
{
    IS_TEXT_VALID(text);

    return ((SText*)text)->string;
}

bool TextGetPivotf(aText text, float* x, float* y)
{
    IS_TEXT_VALID(text);

    if (x != NULL)
        *x = ((SText*)text)->pivot.x;
    if (y != NULL)
        *y = ((SText*)text)->pivot.y;

    return true;
}

inline SVector2f TextGetPivotv(aText text)
{
    if (!ObjectIsText(text))
        return vZero2f;

    return ((SText*)text)->pivot;
}

inline float TextGetPivotX(aText text)
{
    IS_TEXT_VALID(text);

    return ((SText*)text)->pivot.x;
}

inline float TextGetPivotY(aText text)
{
    IS_TEXT_VALID(text);

    return ((SText*)text)->pivot.y;
}

aText TextGetByName(const char* name)
{
    if (name == NULL || _texts->size == 0)
        return NULL;

    return DictionaryGetValueByKey(_texts, name);
}

bool ObjectIsText(void* object)
{
    if (object != NULL && (*((uint32*)object) == TXT_IDENT))
        return true;

    return false;
}

inline bool TextSetName(aText text, const char* name)
{
    IS_TEXT_VALID(text);
    if (name == NULL)
        return false;

    return DictionarySetKeyByValue(_texts, text, name);
}

inline bool TextSetFont(aText text, aFont font)
{
    IS_TEXT_VALID(text);
    IS_FONT_VALID(font);

    ((SText*)text)->font = font;

    return true;
}

bool TextSetString(aText text, const char* string)
{
    IS_TEXT_VALID(text);
    if (string == NULL)
        return false;

    int32 len = StrLength(string);
    SText* txt = (SText*)text;
    if (txt->staticVBO > 0)
    {
        glDeleteBuffers(1, &(txt->staticVBO));
        txt->staticVBO = 0;
    }
    free(txt->string);
    if (len > 0)
    {
        txt->string = calloc(len + 1, sizeof(char));
        StrCopy(txt->string, string, len + 1);
    }
    txt = NULL;

    return true;
}

bool TextSetStringStatic(aText text, const char* string)
{
    IS_TEXT_VALID(text);
    if (string == NULL)
        return false;

    SText* txt = (SText*)text;
    if (txt->staticVBO > 0)
    {
        glDeleteBuffers(1, &(txt->staticVBO));
        txt->staticVBO = 0;
    }
    free(txt->string);
    txt->string = NULL;

    // TODO : написать код генерации статического текста

    return true;
}

inline bool TextSetPivotf(aText text, float x, float y)
{
    IS_TEXT_VALID(text);

    if (TextSetPivotX(text, x) && TextSetPivotY(text, y))
        return true;

    return false;
}

inline bool TextSetPivotv(aText text, SVector2f pivot)
{
    IS_TEXT_VALID(text);

    return TextSetPivotf(text, pivot.x, pivot.y);
}

inline bool TextSetPivotX(aText text, float x)
{
    IS_TEXT_VALID(text);

    ((SText*)text)->pivot.x = x;

    return true;
}

inline bool TextSetPivotY(aText text, float y)
{
    IS_TEXT_VALID(text);

    ((SText*)text)->pivot.y = y;

    return true;
}


inline uint32 TextsGetCount()
{
    return _texts->size;
}

void TextsDestroyAll()
{
    if (_texts->size == 0)
        return;

    /* clear SText structs in list */
    SText* txt = NULL;
    for (SDictElement* element = _texts->first; element; element = element->next)
    {
        if (element->value)
        {
            txt = (SText*)element->value;
            txt->ident = 0;
            free(txt->string);
            if (txt->staticVBO != 0)
                glDeleteBuffers(1, &(txt->staticVBO));
            free(txt);
        }
    }

    /* delete all elements from list */
    DictionaryClear(_texts);
}

static void TextDrawString(SText* text)
{
    SFont* fnt = text->font;
    if (!ObjectIsFont(fnt))
        return;

    char* string = text->string;

    uint32 txr_id = FontGetGLTextureID(fnt);
    if (txr_id > 0)
    {
        glBindTexture(GL_TEXTURE_2D, txr_id);
        glEnable (GL_TEXTURE_2D);
    }
    //glBlendFunc (GL_ONE, GL_ONE);                 // если фон черный
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);   // если есть альфа-канал
    glEnable (GL_BLEND);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //
    // разбираем строку посимвольно
    //
    int32 horCharsCount = 0;   // количество символов, выведенное по горизонтали.
                               // Для сдвига в левую крайнюю позицию при переносе каретки
    for (uint8 symb = *string; *string != '\0'; string++, symb = *string)
    {

        // сдвигаемся на высоту буквы вниз и на (длинна строки)*(ширина буквы), если наткнулся на ентер
        if (symb == '\n')
        {
            glTranslatef (-horCharsCount*(fnt->charW), -fnt->charH, 0.0f);
            horCharsCount = 0;
            continue;
        }

        // если был символ табуляции, то сдвинуться на 4 символа вправо
        if (symb == '\t')
        {
            glTranslatef (4.0f*(fnt->charW), 0.0f, 0.0f);
            horCharsCount += 4;
            continue;
        }

        ++horCharsCount;
        // вызываем отрисовку буквы из VBO
        if (fnt->charsVBO[symb] != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, fnt->charsVBO[symb]);

            glVertexPointer(2, GL_FLOAT, 0, (void*)0);
            glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(float)*8));    // данные по координатам находятся за вершинами

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        // сдвигаемся на ширину буквы
        glTranslatef ((float)fnt->charW, 0.0f, 0.0f);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

static void TextDrawStatic(SText* text)
{
    if (text == NULL)
        return;

    // заглушка
}

void TextDraw(aText text)
{
    if (!ObjectIsText(text))
        return;


    glPushMatrix();

    SText* txt = (SText*)text;
    glTranslatef(-txt->pivot.x, -txt->pivot.y, 0.0f);

    if (txt->staticVBO != 0)
        TextDrawStatic(txt);
    else
    {
        int32 len = StrLength(txt->string);
        if (len > 0)
            TextDrawString(txt);
    }

    glPopMatrix();
}


/*  TESTS!!! */
//#define _TEXT_DEBUG
#ifdef _DEBUG
#include "texture.h"
#include "material.h"
#include "image.h"
#include "font.h"
#include "text.h"
void TextTest()
{
    aText text = NULL, text2 = NULL;
    char* name = NULL;
    char* string = NULL;
    SVector2f pivot = vZero2f;
    bool result;
    int32 count;

    aTexture texture = TextureCreateFromFile("media/fonts/font.png", "font_texture", false);
    assert(texture != NULL);
    aFont font = FontCreate("font");
    assert(font != NULL);
    result = FontGenerateFromTexture(font, texture, 16, 16, 4, 4);
    assert(result == true);


    /* test1 : create and delete */
    printf("--------test1--------\n");
    text = TextCreate(NULL, NULL, NULL);
    assert(text == NULL);
    text = TextCreate("text", font, NULL);
    assert(text != NULL);
    assert(TextsGetCount() == 1);

    result = ObjectIsTexture(text);
    assert(result == false);
    result = ObjectIsMaterial(text);
    assert(result == false);
    result = ObjectIsImage(text);
    assert(result == false);
    result = ObjectIsText(text);
    assert(result == true);
    result = ObjectIsFont(text);
    assert(result == false);

    TextDestroy(&text);
    assert(text == NULL);
    assert(TextsGetCount() == 0);
    result = ObjectIsText(text);
    assert(result == false);
    TextDestroy(&text2);
    printf("passed.\n");

    text = TextCreate("text", font, "simple string");

    /* test2 : set font */
    printf("--------test2--------\n");
    TextSetFont(text, NULL);
    assert(TextGetFont(text) == NULL);
    TextSetFont(text, font);
    assert(TextGetFont(text) == font);
    printf("passed.\n");

    /* test3 : get name */
    printf("--------test3--------\n");
    name = TextGetName(text);
    assert(name != NULL);
    assert(StrEqual(name, "text") == true);
    name = TextGetName(text2);
    assert(name == NULL);
    printf("passed.\n");

    /* test4 : get/set string */
    printf("--------test4--------\n");
    string = TextGetString(text);
    assert(string != NULL);
    assert(StrEqual(string, "simple string") == true);
    string = TextGetString(text2);
    assert(string == NULL);

    TextSetString(NULL, "new string");
    TextSetString(text2, "new string");
    TextSetString(text, "new string");
    string = TextGetString(text);
    assert(string != NULL);
    assert(StrEqual(string, "new string") == true);

    printf("passed.\n");

    /* test5 get/set pivot of text */
    printf("--------test5--------\n");
    pivot = TextGetPivotv(text);
    assert(pivot.x == 0 && pivot.y == 0);
    TextSetPivotf(text, 10, 10);
    pivot = TextGetPivotv(text);
    assert(pivot.x == 10 && pivot.y == 10);
    pivot.x = pivot.y = 0;
    TextGetPivotf(text, &pivot.x, &pivot.y);
    assert(pivot.x == 10 && pivot.y == 10);
    pivot.x = pivot.y = 0;
    pivot.x = TextGetPivotX(text);
    pivot.y = TextGetPivotY(text);
    assert(pivot.x == 10 && pivot.y == 10);
    printf("passed.\n");

    /* test6 get image */
    printf("--------test6--------\n");
    text2 = TextGetByName("text");
    assert(text == text2);
    printf("passed.\n");

    /* test7 get count of loaded texts */
    printf("--------test7--------\n");
    count = TextsGetCount();
    assert(count == 1);
    printf("passed.\n");

    /* test8 set new name */
    printf("--------test8--------\n");
    TextSetName(text, NULL);
    assert(TextGetName(text) != NULL);
    TextSetName(text2, NULL);
    assert(TextGetName(text2) != NULL);
    TextSetName(text2, "new_name");
    assert(StrEqual(TextGetName(text2), "new_name") == true);
    printf("passed.\n");

    /* test9 delete all texts */
    printf("--------test9--------\n");
    TextsDestroyAll();
    count = TextsGetCount();
    assert(count == 0);
    printf("passed.\n");

    text = NULL;
    text2 = NULL;

    TextureDestroy(&texture);

    /* passed */
    printf ("--------result-------\n");
    printf ("all text's tests are passed!\n");
}
#endif // _DEBUG
