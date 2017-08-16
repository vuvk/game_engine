#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils.h"
#include "structs_private.h"
#include "matrix.h"
#include "window_system.h"
#include "render_system.h"
#include "gui_manager.h"
#include "font.h"
#include "image.h"

extern bool _is3dMode;

static TMatrix4x4 _proj, _modl, _ortho;

void GuiBegin()
{
    /*
    Эта функция принимает координаты квадрата для нашего 2д экрана.
    Ниже нам нужно включить матрицу проекции. Чтобы это сделать,
    вызываем glMatrixMode(GL_PROJECTION), чтобы указать, какой нам нужен режим.
    Далее мы загружаем новую матрицу, чтобы всё инициализировалось до того, как
    мы перешли в режим ortho. Перейдя в 2д режим вызовом glOrtho(), мы изменяем
    матрицу на GL_MODELVIEW, которая подготавливает нас к рендеру мира, используя
    матрицу моделей. Также мы инициализируем новую матрицу моделей для 2д рендера,
    прежде чем что-то рисовать.
    */

    // запомним матрицы, которые использовались в 3D режиме
    glGetFloatv(GL_PROJECTION_MATRIX, (float*)_proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_modl);

    /* Переключаемся на матрицу проекции, чтобы перейти в режем ortho, не perspective */
    glMatrixMode(GL_PROJECTION);

    /*
    Передадим OpenGL экранные координаты. Лево, право, низ, верх.
    Последние 2 параметра - ближняя и дальняя плоскости.
    */
    Orthof_M4x4(_ortho, 0.0f, WindowGetWidth(), 0.0f, WindowGetHeight(), 0.0f, 1.0f);
    glLoadMatrixf(_ortho);

    /* Переключимся в modelview, чтобы рендерить общую картинку */
    glMatrixMode(GL_MODELVIEW);

    /* Обнулим текущую матрицу */
    glLoadIdentity();

    /* Тест глубины ДОЛЖЕН быть отключен */
    glDisable(GL_DEPTH_TEST);

    //glBlendFunc(GL_DST_COLOR,GL_ZERO);
    //glBlendFunc (GL_ONE, GL_ONE);
    glDisable(GL_BLEND);

    glCullFace (GL_NONE);
    glDisable (GL_CULL_FACE);
    glDisable (GL_LIGHTING);

    _is3dMode = false;
}

void GuiEnd()
{
    /*
    На самом деле, эта ф-я не переключается в режим перспективы, мы не
    используем gluPerspective(), но если вы вспомните OrthoMode(), мы
    работаем в новой матрице. Всё, что нам надо - выйти из этой матрицы,
    и вернутся и изначальную матрицу перспективы. Итак, перед выходом из
    матрицы, нам нужно сказать OpenGL, какую матрицу мы хотим использовать
    - изменением текущей матрицы на перспективу. Потом мы можем вернутся
    в матрицу modelview и рендерить всё как обычно.
    */

    /* Входим в режим матрицы проекции */
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)_proj);

    /* Возвращаемся в матрицу моделей */
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)_modl);

    /* Теперь мы должны быть в нормальном 3д режиме с перспективой */
    glEnable (GL_DEPTH_TEST);

    _is3dMode = true;
}

void GuiDrawImage(aImage image, float x, float y)
{
    if (!ObjectIsImage(image))
        return;

    glPushMatrix();
    y = WindowGetHeight() - y;
    glTranslatef(x, y, 0.0f);

    ImageDraw(image);

    glPopMatrix();
}

void GuiDrawText(aFont font, float x, float y, const char* text)
{
    if (!ObjectIsFont(font) || StrLength(text) < 1)
        return;

    SFont* fnt = (SFont*)font;

    uint32 txr_id = FontGetGLTextureID(font);
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
    glCullFace (GL_NONE);
    glDisable (GL_CULL_FACE);
    glDisable (GL_LIGHTING);

    glPushMatrix();
    // сдвигаемся на точку, где будет текст
    // -wordH, т.к. Y перевернут
    y = WindowGetHeight() - y;
    glTranslatef (x, y, 0.0f);

    //
    // разбираем строку посимвольно
    //
    int32 horCharsCount = 0;   // количество символов, выведенное по горизонтали.
                               // Для сдвига в левую крайнюю позицию при переносе каретки
    for (uint8 symb = *text; *text != '\0'; ++text, symb = *text)
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
        glTranslatef (fnt->charW, 0.0f, 0.0f);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopMatrix();

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
