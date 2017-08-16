#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL2/SDL.h"

#include "utils.h"
#include "defines.h"
#include "file_system.h"


/* math */
inline float DegToRad(float deg)
{
    const float DEG_TO_RAD_COEFF = M_PI / 180.0f;
	return deg * DEG_TO_RAD_COEFF;
}
inline float RadToDeg(float rad)
{
    const float RAD_TO_DEG_COEFF = 180.0f / M_PI;
	return rad * RAD_TO_DEG_COEFF;
}

/* limit number in range */
void LimitFloat(float* number, float min, float max)
{
    if (*number < min) *number = min;
    if (*number > max) *number = max;
}
void LimitShort(int16* number, int16 min, int16 max)
{
    if (*number < min) *number = min;
    if (*number > max) *number = max;
}
void LimitInt(int32* number, int32 min, int32 max)
{
    if (*number < min) *number = min;
    if (*number > max) *number = max;
}
void LimitLong(int64* number, int64 min, int64 max)
{
    if (*number < min) *number = min;
    if (*number > max) *number = max;
}

/* swap anything */
void SwapPtr (void** a, void** b)
{
    if (a == NULL || b == NULL)
        return;

    void* t = *b;
    *b = *a;
    *a = t;

    /* ПОЧЕМУ ЭТО-ТО НЕ РАБОТАЕТ???
    void* t = *a;
    *a = *b;
    *b = t;*/
}

void SwapShort (int16* a, int16* b)
{
    if (a == NULL || b == NULL)
        return;

    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void SwapInt (int32* a, int32* b)
{
    if (a == NULL || b == NULL)
        return;

    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void SwapLong(int64* a, int64* b)
{
    if (a == NULL || b == NULL)
        return;

    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}

void SwapFloat (float* a, float* b)
{
    if (a == NULL || b == NULL)
        return;

    float t = *a;
    *a = *b;
    *b = t;
}

/* strings */
int8 StrEqual (const char* str1, const char* str2)
{
    // not correct!
    if (str1 == NULL || str2 == NULL)
        return -1;

    // strings equal
    if (strcmp (str1, str2) == 0)
        return true;

    // strings not equal
    return false;
}

void StrCopy(char* dst, const char* src, uint32 maxLength)
{
    uint32 len = StrLength(src);
    if (len > maxLength - 1)
        len = maxLength - 1;
    strncpy(dst, src, len*sizeof(char));
    dst[len] = '\0';
}

int32 StrLength(const char* string)
{
    if (string == NULL)
        return -1;

    int32 len = 0;
    char* chr = (char*)string;
    while ((*chr != '\0') && (len <= MAX_STRING_LENGTH))
    {
        chr++;
        len++;
    }

    return len;
}



/* TESTS! */
//#define _UTILS_DEBUG
#ifdef _DEBUG
void UtilsTest()
{
    int8    result;
    int32   len;
    int32   ai = 5,    bi = 3;
    int16   as = 5,    bs = 3;
    float   af = 5.0f, bf = 3.0f;

    /* test1 : test extension extracting */
    printf ("--------test1--------\n");
    char* ext = "";
    FileExtractExt(&ext, "File1.ext");
    assert(strcmp(ext, "EXT") == 0);
    printf ("%s\n", ext);
    printf ("passed\n");

    /* test2 : string equals test */
    printf ("--------test2--------\n");
    result = StrEqual(NULL, NULL);
    assert (result == -1);
    result = StrEqual("string1", NULL);
    assert (result == -1);
    result = StrEqual(NULL, "string1");
    assert (result == -1);
    result = StrEqual("string1", "string1");
    assert (result == true);
    result = StrEqual("string2", "string1");
    assert (result == false);
    printf ("passed\n");

    /* test3 : string length test */
    printf ("--------test3--------\n");
    len = StrLength("string1");
    assert (len == 7);
    char errString[10] = "abcdefghij";
    len = StrLength(errString);
    assert(len < MAX_STRING_LENGTH);
    printf ("len == %d\n", len);
    printf ("passed\n");

    /* test4 : string copy test */
    printf ("--------test4--------\n");
    char dstString[10];
    // последний 10-й символ должен стать нулл-терминатором
    StrCopy(dstString, errString, 10);
    printf ("string == '%s'\n", dstString);
    assert(StrEqual(dstString, "abcdefghi") == true);
    printf ("passed\n");

    /* test5 : string copy test 2 */
    printf ("--------test5--------\n");
    StrCopy(dstString, errString, 5);
    printf ("string == '%s'\n", dstString);
    assert(StrEqual(dstString, "abcd") == true);
    printf ("passed\n");

    /* test6 : swapping */
    printf ("--------test6--------\n");
    Swap(&ai, &bi);
    assert(ai == 3 && bi == 5);
    Swap(&as, &bs);
    assert(as == 3 && bs == 5);
    Swap(&af, &bf);
    assert(af == 3.0f && bf == 5.0f);
    printf("af == %f\tbf == %f\n", af, bf);
    Swap((void*)&af, (void*)&bf);
    printf("af == %f\tbf == %f\n", af, bf);
    assert(af == 5.0f && bf == 3.0f);
    printf ("passed\n");

    /* test7 : limitting */
    printf ("--------test7--------\n");
    as = -10;
    Limit(&as, 0, 10);
    assert(as == 0);
    as = 100;
    Limit(&as, 0, 10);
    assert(as == 10);

    ai = -10;
    Limit(&ai, 0, 10);
    assert(ai == 0);
    ai = 100;
    Limit(&ai, 0, 10);
    assert(ai == 10);

    af = -10.0f;
    Limit(&af, 0.0f, 10.0f);
    assert(af == 0.0f);
    af = 100.0f;
    Limit(&af, 0.0f, 10.0f);
    assert(af == 10.0f);

    printf ("passed\n");

    /* passed */
    printf ("--------result-------\n");
    printf ("all util's tests are passed!\n");
}
#endif //_DEBUG
