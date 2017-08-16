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
    ��� ������� ��������� ���������� �������� ��� ������ 2� ������.
    ���� ��� ����� �������� ������� ��������. ����� ��� �������,
    �������� glMatrixMode(GL_PROJECTION), ����� �������, ����� ��� ����� �����.
    ����� �� ��������� ����� �������, ����� �� ������������������ �� ����, ���
    �� ������� � ����� ortho. ������� � 2� ����� ������� glOrtho(), �� ��������
    ������� �� GL_MODELVIEW, ������� �������������� ��� � ������� ����, ���������
    ������� �������. ����� �� �������������� ����� ������� ������� ��� 2� �������,
    ������ ��� ���-�� ��������.
    */

    // �������� �������, ������� �������������� � 3D ������
    glGetFloatv(GL_PROJECTION_MATRIX, (float*)_proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, (float*)_modl);

    /* ������������� �� ������� ��������, ����� ������� � ����� ortho, �� perspective */
    glMatrixMode(GL_PROJECTION);

    /*
    ��������� OpenGL �������� ����������. ����, �����, ���, ����.
    ��������� 2 ��������� - ������� � ������� ���������.
    */
    Orthof_M4x4(_ortho, 0.0f, WindowGetWidth(), 0.0f, WindowGetHeight(), 0.0f, 1.0f);
    glLoadMatrixf(_ortho);

    /* ������������ � modelview, ����� ��������� ����� �������� */
    glMatrixMode(GL_MODELVIEW);

    /* ������� ������� ������� */
    glLoadIdentity();

    /* ���� ������� ������ ���� �������� */
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
    �� ����� ����, ��� �-� �� ������������� � ����� �����������, �� ��
    ���������� gluPerspective(), �� ���� �� ��������� OrthoMode(), ��
    �������� � ����� �������. ��, ��� ��� ���� - ����� �� ���� �������,
    � �������� � ����������� ������� �����������. ����, ����� ������� ��
    �������, ��� ����� ������� OpenGL, ����� ������� �� ����� ������������
    - ���������� ������� ������� �� �����������. ����� �� ����� ��������
    � ������� modelview � ��������� �� ��� ������.
    */

    /* ������ � ����� ������� �������� */
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((float*)_proj);

    /* ������������ � ������� ������� */
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((float*)_modl);

    /* ������ �� ������ ���� � ���������� 3� ������ � ������������ */
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
    //glBlendFunc (GL_ONE, GL_ONE);                 // ���� ��� ������
    glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);   // ���� ���� �����-�����
    glEnable (GL_BLEND);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glCullFace (GL_NONE);
    glDisable (GL_CULL_FACE);
    glDisable (GL_LIGHTING);

    glPushMatrix();
    // ���������� �� �����, ��� ����� �����
    // -wordH, �.�. Y ����������
    y = WindowGetHeight() - y;
    glTranslatef (x, y, 0.0f);

    //
    // ��������� ������ �����������
    //
    int32 horCharsCount = 0;   // ���������� ��������, ���������� �� �����������.
                               // ��� ������ � ����� ������� ������� ��� �������� �������
    for (uint8 symb = *text; *text != '\0'; ++text, symb = *text)
    {
        // ���������� �� ������ ����� ���� � �� (������ ������)*(������ �����), ���� ��������� �� �����
        if (symb == '\n')
        {
            glTranslatef (-horCharsCount*(fnt->charW), -fnt->charH, 0.0f);
            horCharsCount = 0;
            continue;
        }

        // ���� ��� ������ ���������, �� ���������� �� 4 ������� ������
        if (symb == '\t')
        {
            glTranslatef (4.0f*(fnt->charW), 0.0f, 0.0f);
            horCharsCount += 4;
            continue;
        }

        ++horCharsCount;
        // �������� ��������� ����� �� VBO
        if (fnt->charsVBO[symb] != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, fnt->charsVBO[symb]);

            glVertexPointer(2, GL_FLOAT, 0, (void*)0);
            glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(float)*8));    // ������ �� ����������� ��������� �� ���������

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        // ���������� �� ������ �����
        glTranslatef (fnt->charW, 0.0f, 0.0f);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPopMatrix();

    glDisable (GL_TEXTURE_2D);
    glDisable (GL_BLEND);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
