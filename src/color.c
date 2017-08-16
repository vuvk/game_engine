//#include <math.h>

#include "SDL2/SDL.h"

#include "color.h"
#include "utils.h"


/* "constructors" */
inline RGBColor NewRGBColor(uint8 r, uint8 g, uint8 b)
{
    RGBColor c = {r, g, b};
    return c;
}

inline RGBAColor NewRGBAColor(uint8 r, uint8 g, uint8 b, uint8 a)
{
    RGBAColor c = {r, g, b, a};
    return c;
}

RGBColorf NewRGBColorf(float r, float g, float b)
{
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);

    RGBColorf c = {r, g, b};
    return c;
}

RGBAColorf NewRGBAColorf(float r, float g, float b, float a)
{
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);
    Limit(&a, 0.0f, 1.0f);

    RGBAColorf c = {r, g, b, a};
    return c;
}

/* equals */
inline bool IsEqualRGBColor(RGBColor col1, RGBColor col2)
{
    return (col1.r == col2.r &&
            col1.g == col2.g &&
            col1.b == col2.b);
}

inline bool IsEqualRGBColorf(RGBColorf col1, RGBColorf col2)
{
    return (col1.r == col2.r &&
            col1.g == col2.g &&
            col1.b == col2.b);
}

inline bool IsEqualRGBAColor(RGBAColor col1, RGBAColor col2)
{
    return (col1.r == col2.r &&
            col1.g == col2.g &&
            col1.b == col2.b &&
            col1.a == col2.a);
}

inline bool IsEqualRGBAColorf(RGBAColorf col1, RGBAColorf col2)
{
    return (col1.r == col2.r &&
            col1.g == col2.g &&
            col1.b == col2.b &&
            col1.a == col2.a);
}

/* math operations */
/* addition */
RGBColor AddRGBColor(RGBColor col1, RGBColor col2)
{
    int16 r = col1.r + col2.r;
    int16 g = col1.g + col2.g;
    int16 b = col1.b + col2.b;
    LimitShort(&r, 0, 255);
    LimitShort(&g, 0, 255);
    LimitShort(&b, 0, 255);

    return NewRGBColor((uint8)r,(uint8)g,(uint8)b);
}

RGBColorf AddRGBColorf(RGBColorf col1, RGBColorf col2)
{
    float r = col1.r + col2.r;
    float g = col1.g + col2.g;
    float b = col1.b + col2.b;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);

    return NewRGBColorf(r, g, b);
}

RGBAColor AddRGBAColor(RGBAColor col1, RGBAColor col2)
{
    int16 r = col1.r + col2.r;
    int16 g = col1.g + col2.g;
    int16 b = col1.b + col2.b;
    int16 a = col1.a + col2.a;
    Limit(&r, 0, 255);
    Limit(&g, 0, 255);
    Limit(&b, 0, 255);
    Limit(&a, 0, 255);

    return NewRGBAColor((uint8)r,
                        (uint8)g,
                        (uint8)b,
                        (uint8)a);
}

RGBAColorf AddRGBAColorf(RGBAColorf col1, RGBAColorf col2)
{
    float r = col1.r + col2.r;
    float g = col1.g + col2.g;
    float b = col1.b + col2.b;
    float a = col1.a + col2.a;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);
    Limit(&a, 0.0f, 1.0f);

    return NewRGBAColorf(r, g, b, a);
}

/* subtract */
RGBColor SubRGBColor(RGBColor col1, RGBColor col2)
{
    int16 r = col1.r - col2.r;
    int16 g = col1.g - col2.g;
    int16 b = col1.b - col2.b;
    Limit(&r, 0, 255);
    Limit(&g, 0, 255);
    Limit(&b, 0, 255);

    return NewRGBColor((uint8)r,
                       (uint8)g,
                       (uint8)b);
}

RGBColorf SubRGBColorf(RGBColorf col1, RGBColorf col2)
{
    float r = col1.r - col2.r;
    float g = col1.g - col2.g;
    float b = col1.b - col2.b;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);

    return NewRGBColorf(r, g, b);
}

RGBAColor SubRGBAColor(RGBAColor col1, RGBAColor col2)
{
    int16 r = col1.r - col2.r;
    int16 g = col1.g - col2.g;
    int16 b = col1.b - col2.b;
    int16 a = col1.a - col2.a;
    Limit(&r, 0, 255);
    Limit(&g, 0, 255);
    Limit(&b, 0, 255);
    Limit(&a, 0, 255);

    return NewRGBAColor((uint8)r,
                        (uint8)g,
                        (uint8)b,
                        (uint8)a);
}

RGBAColorf SubRGBAColorf(RGBAColorf col1, RGBAColorf col2)
{
    float r = col1.r - col2.r;
    float g = col1.g - col2.g;
    float b = col1.b - col2.b;
    float a = col1.a - col2.a;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);
    Limit(&a, 0.0f, 1.0f);

    return NewRGBAColorf(r, g, b, a);
}

/* multiplication by scalar */
RGBColor MultRGBColor(RGBColor col, float scalar)
{
    int16 r =(int16) SDL_floor(col.r * scalar);
    int16 g =(int16) SDL_floor(col.g * scalar);
    int16 b =(int16) SDL_floor(col.b * scalar);
    Limit(&r, 0, 255);
    Limit(&g, 0, 255);
    Limit(&b, 0, 255);

    return NewRGBColor((uint8)r,(uint8)g,(uint8)b);
}

RGBColorf MultRGBColorf(RGBColorf col, float scalar)
{
    float r = col.r * scalar;
    float g = col.g * scalar;
    float b = col.b * scalar;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);

    return NewRGBColorf(r, g, b);
}

RGBAColor MultRGBAColor(RGBAColor col, float scalar)
{
    int16 r =(int16) SDL_floor(col.r * scalar);
    int16 g =(int16) SDL_floor(col.g * scalar);
    int16 b =(int16) SDL_floor(col.b * scalar);
    int16 a =(int16) SDL_floor(col.a * scalar);
    Limit(&r, 0, 255);
    Limit(&g, 0, 255);
    Limit(&b, 0, 255);
    Limit(&a, 0, 255);

    return NewRGBAColor((uint8)r,(uint8)g,(uint8)b,(uint8)a);
}

RGBAColorf MultRGBAColorf(RGBAColorf col, float scalar)
{
    float r = col.r * scalar;
    float g = col.g * scalar;
    float b = col.b * scalar;
    float a = col.a * scalar;
    Limit(&r, 0.0f, 1.0f);
    Limit(&g, 0.0f, 1.0f);
    Limit(&b, 0.0f, 1.0f);
    Limit(&a, 0.0f, 1.0f);

    return NewRGBAColorf(r, g, b, a);
}

/* division by scalar */
RGBColor DivRGBColor(RGBColor col, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    return MultRGBColor(col, scalar);
}

RGBColorf DivRGBColorf(RGBColorf col, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    return MultRGBColorf(col, scalar);
}

RGBAColor DivRGBAColor(RGBAColor col, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    return MultRGBAColor(col, scalar);
}

RGBAColorf DivRGBAColorf(RGBAColorf col, float scalar)
{
    if (scalar != 0.0f)
        scalar = 1.0f/scalar;
    return MultRGBAColorf(col, scalar);
}
