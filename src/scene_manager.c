#include "SDL2/SDL.h"

#include "color.h"
#include "structs_private.h"
#include "window_system.h"
#include "input_system.h"
#include "log_system.h"
#include "render_system.h"
#include "gui_manager.h"
#include "scene_manager.h"
#include "engine.h"
#include "image.h"
#include "text.h"
#include "node_2d.h"
#include "node_md2.h"


static RGBAColorf _clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

extern bool _is3dMode;

static TMatrix4x4 _modl;

void SceneBegin()
{
    EngineUpdateTime();
    WindowUpdateState();

    // очистка экрана
    glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // выставляем перспективу и вьюпорт по размеру окна
    int32 windowWidth, windowHeight;
    WindowGetSize(&windowWidth, &windowHeight);
    //gluPerspective(45.0f, (float)windowWidth / windowHeight, 0.1f, 1000.0f);
    glLoadMatrixf(_pers);
    glViewport(0, 0, windowWidth, windowHeight);

    // переходим в режим модели
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
}

void SceneEnd()
{
    SDL_GL_SwapWindow((SDL_Window*)WindowGetPointer());

    EngineUpdateFPS();
    InputUpdateState();
}

inline void SceneSetClearColorEx(float r, float g, float b, float a)
{
    _clearColor = NewRGBAColorf(r, g, b, a);
}

void SceneDrawNodes()
{
    SListElement* element = NULL;

    if (_nodesMd2->size > 0)
    {
        for (element = _nodesMd2->first; element; element = element->next)
        {
            if (element->value == NULL)
                continue;

            if (ObjectIsNodeMd2(element->value))
                NodeMd2Draw(element->value);
            else
                element->value = NULL;
        }
    }
}

void SceneDrawNodes2D()
{
    if (_nodes2d->size > 0)
    {
        GuiBegin();

        // запомним матрицы, которые использовались
        glMatrixMode(GL_MODELVIEW);
        glGetFloatv(GL_MODELVIEW_MATRIX, _modl);

        SNode2D*  node;
        aImage    img;
        aText     txt;
        for (SListElement* element = _nodes2d->first; element; element = element->next)
        {
            if (element->value != NULL)
            {
                node = (SNode2D*)element->value;

                if (!node->visible)
                    continue;

                if (ObjectIsNode2D(node))
                {
                    // пропихиваем матрицу
                    glLoadMatrixf(node->transform);

                    // рисуем картинки
                    if (node->images->size > 0)
                    {
                        for (SListElement* imgElement = node->images->first; imgElement; imgElement = imgElement->next)
                        {
                            img = imgElement->value;
                            if (ObjectIsImage(img))
                            {
                                // рисуем элемент
                                ImageDraw(img);
                            }
                            else
                            {
                                LogWriteError("Image-object is not valid!\n");
                                imgElement->value = NULL;
                            }
                        }
                    }

                    // рисуем тексты
                    if (node->texts->size > 0)
                    {
                        for (SListElement* txtElement = node->texts->first; txtElement; txtElement = txtElement->next)
                        {
                            txt = txtElement->value;
                            if (ObjectIsText(txt))
                            {
                                // рисуем элемент
                                TextDraw(txt);
                            }
                            else
                            {
                                LogWriteError("Text-object is not valid!\n");
                                txtElement->value = NULL;
                            }
                        }
                    }

                    // возвращаем матрицу исходную
                    glLoadMatrixf(_modl);

                }
                else
                    element->value = NULL;
            }
        }

        GuiEnd();
    }
}

inline void SceneDrawAllNodes()
{
    /* первый проход для 3D сцены */
    SceneDrawNodes();

    /* второй для 2D */
    SceneDrawNodes2D();
}
