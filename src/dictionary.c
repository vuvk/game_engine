#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils.h"
#include "dictionary.h"

//#define _MULTITHREADS                       /* use multithreading? */
#ifdef _MULTITHREADS
    #define MAX_VALUES_FOR_ONE_THRD 50      /* max length of list|dictionary for do search in one thread */
    //#define _USE_SDL_THREADS                /* use SDL2-threads or C11-threads */
    #ifdef _USE_SDL_THREADS
        #include "SDL2/SDL.h"
    #else  // NOT _USE_SDL_THREADS
        #include "tinycthread.h"
    #endif // _USE_SDL_THREADS
#endif // _MULTITHREADS

SDictionary* DictionaryCreate ()
{
    SDictionary* dictionary = calloc(1, sizeof(SDictionary));
    if (dictionary == NULL)
        return NULL;

    #ifdef _DICTIONARY_PRINT_DEBUG
    printf("dictionary created...\n");
    #endif // _DICTIONARY_PRINT_DEBUG

    return dictionary;
}

void DictionaryClear (SDictionary* dictionary)
{
    if (dictionary == NULL || dictionary->size == 0 || dictionary->first == NULL)
        return;

    /* проходимся от первого до последнего */
    SDictElement* element = dictionary->first;
    SDictElement* nextElement;
    while (element != NULL)
    {
        nextElement = element->next;

        /* удаляем значение, если таковое имеется */
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf("\n\t\ttry to delete value...");
        #endif // _DICTIONARY_PRINT_DEBUG

        //free (element->value);
        element->value = NULL;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("done\n");
        #endif // _DICTIONARY_PRINT_DEBUG

        /* удаляем элемент, если есть */
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf("\n\t\ttry to delete element...");
        #endif // _DICTIONARY_PRINT_DEBUG

        free (element);
        //element = NULL;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("done\n");
        #endif // _DICTIONARY_PRINT_DEBUG

        element = nextElement;
    }

    /* чистим указатели на первый и последний элементы */
    if (dictionary->first)
        dictionary->first = NULL;
    if (dictionary->last)
        dictionary->last = NULL;

    dictionary->size = 0;
}

void DictionaryDestroy (SDictionary** dictionary)
{
    if (dictionary == NULL || *dictionary == NULL)
        return;

    DictionaryClear(*dictionary);

    free(*dictionary);
    *dictionary = NULL;
}

bool DictionaryAddElement (SDictionary* dictionary, const char* key, void* value)
{
    if (dictionary == NULL || key == NULL || value == NULL)
        return false;

    // not add value, if key already exists
    if (DictionaryGetValueByKey(dictionary, key) != NULL)
        return false;

    SDictElement* newElement = malloc(sizeof(SDictElement));

    StrCopy(newElement->key, key, MAX_KEY_LENGTH);
    newElement->value = value;
    newElement->next = NULL;
    ++dictionary->size;

    // if it is first element
    if (dictionary->first == NULL)
    {
        newElement->prev = NULL;
        dictionary->first = newElement;
        dictionary->last = newElement;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("add as first\n");
        #endif // _DICTIONARY_PRINT_DEBUG
    }
    else
    {
        newElement->prev = dictionary->last;
        dictionary->last->next = newElement;

        dictionary->last = newElement;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("add to end\n");
        #endif // _DICTIONARY_PRINT_DEBUG
    }

    return true;
}

bool DictionaryDeleteElement (SDictionary* dictionary, SDictElement* element)
{
    if (dictionary == NULL || element == NULL)
        return false;

    if (dictionary->first == NULL)
        return false;

    #ifdef _DICTIONARY_PRINT_DEBUG
    printf ("try to delete value...");
    #endif // _DICTIONARY_PRINT_DEBUG

    //free (element->value);
    element->value = NULL;

    #ifdef _DICTIONARY_PRINT_DEBUG
    printf ("\n\t\tdone\n");
    #endif // _DICTIONARY_PRINT_DEBUG

    --dictionary->size;

    // элемент где-то между головой и хвостом
    if (element->prev != NULL && element->next != NULL)
    {
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("deleting anything...");
        #endif // _DICTIONARY_PRINT_DEBUG

        element->prev->next = element->next;
        element->next->prev = element->prev;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _DICTIONARY_PRINT_DEBUG

        goto element_delete;
    }

    // элемент головной
    if (element->prev == NULL && element->next != NULL)
    {
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("deleting head...");
        #endif // _DICTIONARY_PRINT_DEBUG

        dictionary->first = element->next;
        element->next->prev = NULL;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _DICTIONARY_PRINT_DEBUG

        goto element_delete;
    }

    // элемент хвостовой
    if (element->prev != NULL && element->next == NULL)
    {
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("deleting tail...");
        #endif // _DICTIONARY_PRINT_DEBUG

        dictionary->last = element->prev;
        element->prev->next = NULL;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _DICTIONARY_PRINT_DEBUG

        goto element_delete;
    }

    // элемент последний
    if ((dictionary->size <= 0) || (element->prev == NULL && element->next == NULL))
    {
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("deleting last...");
        #endif // _DICTIONARY_PRINT_DEBUG

        // уже всё удалено. Гудбай
        dictionary->first = NULL;
        dictionary->last = NULL;
        //element = NULL;

        dictionary->size = 0;

        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("all deleted! dictionary is empty now!\n");
        #endif // _DICTIONARY_PRINT_DEBUG
    }


    element_delete:
        #ifdef _DICTIONARY_PRINT_DEBUG
        printf ("try to delete element...");
        #endif // _DICTIONARY_PRINT_DEBUG
        free (element);
        element = NULL;

    #ifdef _DICTIONARY_PRINT_DEBUG
    printf ("\n\t\tdone\n");
    #endif // _DICTIONARY_PRINT_DEBUG

    return true;
}

#ifdef _MULTITHREADS
/* parameters for quick search element by value in dictionary */
typedef struct
{
    SDictElement*  startElement;    /* head or tail */
    SDictElement** resultElement;
    void*   value;                  /* value for search */
    int32   maxNumber;              /* maxNumber - when it's stop */
    int8    direction;              /* direction for finding - 1 (head-to-tail) or -1(tail-to-head) */
} SThrdParameter;

/* func for search element by value in thread */
static int DictionaryGetElementByValueThread(void* arg)
{
    if (arg == NULL)
    #ifdef _USE_SDL_THREADS
        return SDL_FALSE;
    #else
        return thrd_error;
    #endif // _USE_SDL_THREADS

    SThrdParameter* param   = (SThrdParameter*)arg;
    SDictElement*   element = param->startElement;
    int32 maxPos = param->maxNumber;
    SDictElement** resultElement = param->resultElement;


    // from head to tail
    if (param->direction > 0)
    {
        register uint32 i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (element->value != param->value) &&
               (i < maxPos))
        {
            element = element->next;
            ++i;
        }
    }
    else
    // from tail to head
    {
        register uint32 i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (element->value != param->value) &&
               (i < maxPos))
        {
            element = element->prev;
            ++i;
        }
    }

    if (*resultElement != NULL)
    #ifdef _USE_SDL_THREADS
        return SDL_TRUE;
    #else
        return thrd_success;
    #endif // _USE_SDL_THREADS

    if (element != NULL && element->value == param->value)
        *(resultElement) = element;
    //else
    //    *(resultElement) = NULL;

    #ifdef _USE_SDL_THREADS
    return SDL_TRUE;
    #else
    return thrd_success;
    #endif // _USE_SDL_THREADS
}

static SDictElement* DictionaryGetElementByValueInThreads (SDictionary* dictionary, const void* value)
{
    if (dictionary->first == NULL || dictionary->last == NULL)
        return NULL;

    SDictElement* result = NULL;
    /* center of dictionary */
    int32 center = dictionary->size / 2;

    SThrdParameter param1 = {dictionary->first, &result, (void*)value, center,               1};
    SThrdParameter param2 = {dictionary->last,  &result, (void*)value, dictionary->size - center, -1};

    #ifdef _USE_SDL_THREADS
    SDL_Thread* threadFromHead = SDL_CreateThread(DictionaryGetElementByValueThread, "fromHead", &param1);
    if (threadFromHead == NULL)
        return NULL;
    SDL_Thread* threadFromTail = SDL_CreateThread(DictionaryGetElementByValueThread, "fromTail", &param2);
    if (threadFromTail == NULL)
        return NULL;

    SDL_WaitThread(threadFromHead, NULL);
    SDL_WaitThread(threadFromTail, NULL);

    #else // NOT _USE_SDL_THREADS
    thrd_t threadFromHead = 0;
    thrd_t threadFromTail = 0;

    if (thrd_create(&threadFromHead, DictionaryGetElementByValueThread, &param1) != thrd_success)
        return NULL;

    if (thrd_create(&threadFromTail, DictionaryGetElementByValueThread, &param2) != thrd_success)
        return NULL;

    /* waits until threads are completed */
    if (thrd_join(threadFromHead, NULL) != thrd_success)
        return NULL;
    if (thrd_join(threadFromTail, NULL) != thrd_success)
        return NULL;
    #endif // _USE_SDL_THREADS

    return result;
}

/* func for search element by key in thread */
static int DictionaryGetElementByKeyThread(void* arg)
{
    if (arg == NULL)
    #ifdef _USE_SDL_THREADS
        return SDL_FALSE;
    #else
        return thrd_error;
    #endif // _USE_SDL_THREADS

    SThrdParameter* param   = (SThrdParameter*)arg;
    SDictElement*   element = param->startElement;
    int32 maxPos = param->maxNumber;
    SDictElement** resultElement = param->resultElement;
    char* key = (char*)param->value;

    // from head to tail
    if (param->direction > 0)
    {
        register uint32 i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (StrEqual(element->key, key) != true) &&
               (i < maxPos))
        {
            element = element->next;
            ++i;
        }
    }
    else
    // from tail to head
    {
        register uint32 i = 0;
        while ((element != NULL) &&
               (*resultElement == NULL) &&
               (StrEqual(element->key, key) != true) &&
               (i < maxPos))
        {
            element = element->prev;
            ++i;
        }
    }

    if (*resultElement != NULL)
    #ifdef _USE_SDL_THREADS
        return SDL_TRUE;
    #else
        return thrd_success;
    #endif // _USE_SDL_THREADS

    if (element != NULL && (StrEqual(element->key, key) == true))
        *(resultElement) = element;
    //else
    //    *(resultElement) = NULL;

    #ifdef _USE_SDL_THREADS
    return SDL_TRUE;
    #else
    return thrd_success;
    #endif // _USE_SDL_THREADS
}

static SDictElement* DictionaryGetElementByKeyInThreads (SDictionary* dictionary, const char* key)
{
    if (dictionary->first == NULL || dictionary->last == NULL)
        return NULL;

    SDictElement* result = NULL;
    /* center of dictionary */
    int32 center = dictionary->size / 2;

    SThrdParameter param1 = {dictionary->first, &result, (void*)key, center,                     1};
    SThrdParameter param2 = {dictionary->last,  &result, (void*)key, dictionary->size - center, -1};

    #ifdef _USE_SDL_THREADS
    SDL_Thread* threadFromHead = SDL_CreateThread(DictionaryGetElementByKeyThread, "fromHead", &param1);
    if (threadFromHead == NULL)
        return NULL;
    SDL_Thread* threadFromTail = SDL_CreateThread(DictionaryGetElementByKeyThread, "fromTail", &param2);
    if (threadFromTail == NULL)
        return NULL;

    SDL_WaitThread(threadFromHead, NULL);
    SDL_WaitThread(threadFromTail, NULL);

    #else // NOT _USE_SDL_THREADS
    thrd_t threadFromHead = 0;
    thrd_t threadFromTail = 0;

    if (thrd_create(&threadFromHead, DictionaryGetElementByKeyThread, &param1) != thrd_success)
        return NULL;

    if (thrd_create(&threadFromTail, DictionaryGetElementByKeyThread, &param2) != thrd_success)
        return NULL;

    /* waits until threads are completed */
    if (thrd_join(threadFromHead, NULL) != thrd_success)
        return NULL;
    if (thrd_join(threadFromTail, NULL) != thrd_success)
        return NULL;
    #endif // _USE_SDL_THREADS

    return result;
}
#endif // _MULTITHREADS

SDictElement* DictionaryGetElementByValue (SDictionary* dictionary, const void* value)
{
    if (dictionary == NULL || value == NULL)
        return NULL;

    if (dictionary->first == NULL || dictionary->last == NULL)
        return NULL;

    #ifdef _MULTITHREADS
    SDictElement* element = NULL;
    if (dictionary->size <= MAX_VALUES_FOR_ONE_THRD)
    {
        element = dictionary->first;
        while ((element != NULL) && (element->value != value))
            element = element->next;
    }
    else
        element = DictionaryGetElementByValueInThreads(dictionary, value);
    #else // NOT _MULTITHREADS
    SDictElement* element = dictionary->first;
    while ((element != NULL) && (element->value != value))
            element = element->next;
    #endif // _MULTITHREADS

    return element;
}

SDictElement* DictionaryGetElementByKey (SDictionary* dictionary, const char* key)
{
    #ifdef _MULTITHREADS
    SDictElement* element = NULL;
    if (dictionary->size <= MAX_VALUES_FOR_ONE_THRD)
    {
        element = dictionary->first;
        while ((element != NULL) && (StrEqual(key, element->key) != true))
            element = element->next;
    }
    else
        element = DictionaryGetElementByKeyInThreads(dictionary, key);
    #else // NOT _MULTITHREADS
    SDictElement* element = dictionary->first;
    while ((element != NULL) && (StrEqual(key, element->key) != true))
        element = element->next;
    #endif // _MULTITHREADS

    return element;
}

SDictElement* DictionaryGetElementByNumber (SDictionary* dictionary, uint32 numOfElement)
{
    if (dictionary == NULL)
        return NULL;

    if (dictionary->size == 0 || numOfElement >= dictionary->size)
        return NULL;

    SDictElement* element = NULL;
    /* сheck which way is faster - from the head or tail */
    if ((dictionary->size - numOfElement) >= numOfElement)
    {
        /* go from head */
        element = dictionary->first;
        for (uint32 i = 0; element && (i < numOfElement); ++i)
            element = element->next;
    }
    else
    {
        /* go from tail */
        element = dictionary->last;
        for (uint32 i = dictionary->size - 1; element && (i > numOfElement); --i)
            element = element->prev;
    }

    return element;
}

void DictionaryDeleteElementByValue (SDictionary* dictionary, const void* value)
{
    if (dictionary == NULL || value == NULL)
        return;

    if (dictionary->first == NULL)
        return;

    SDictElement* element = DictionaryGetElementByValue(dictionary, value);
    if (element != NULL)
        DictionaryDeleteElement(dictionary, element);
}

void DictionaryDeleteElementByKey (SDictionary* dictionary, const char* key)
{
    if (dictionary == NULL || key == NULL)
        return;

    if (dictionary->first == NULL)
        return;

    SDictElement* element = DictionaryGetElementByKey(dictionary, key);
    if (element != NULL)
        DictionaryDeleteElement(dictionary, element);
}

void DictionaryDeleteElementByNumber (SDictionary* dictionary, uint32 numOfElement)
{
    if (dictionary == NULL || (numOfElement >= dictionary->size))
        return;

    if (dictionary->first == NULL)
        return;

    SDictElement* element = DictionaryGetElementByNumber(dictionary, numOfElement);
    if (element != NULL)
        DictionaryDeleteElement(dictionary, element);
}

void* DictionaryGetValueByKey (SDictionary* dictionary, const char* key)
{
    if (dictionary == NULL || key == NULL)
        return NULL;

    if (dictionary->first == NULL)
        return NULL;

    SDictElement* element = DictionaryGetElementByKey(dictionary, key);
    if (element == NULL)
        return NULL;

    return element->value;
}

void* DictionaryGetValueByNumber (SDictionary* dictionary, uint32 numOfElement)
{
    if (dictionary == NULL || (numOfElement >= dictionary->size))
        return NULL;

    if (dictionary->first == NULL)
        return NULL;

    SDictElement* element = DictionaryGetElementByNumber(dictionary, numOfElement);
    if (element == NULL)
        return NULL;

    return element->value;
}

char* DictionaryGetKeyByNumber (SDictionary* dictionary, uint32 numOfElement)
{
    if (dictionary == NULL || (numOfElement >= dictionary->size))
        return NULL;

    if (dictionary->first == NULL)
        return NULL;

    SDictElement* element = DictionaryGetElementByNumber(dictionary, numOfElement);
    if (element == NULL)
        return NULL;

    return element->key;
}

char* DictionaryGetKeyByValue (SDictionary* dictionary, const void* value)
{
    if (dictionary == NULL || value == NULL)
        return NULL;

    if (dictionary->first == NULL)
        return NULL;

    SDictElement* element = DictionaryGetElementByValue(dictionary, value);
    if (element == NULL)
        return NULL;

    return element->key;
}

int32 DictionaryGetNumberByKey (SDictionary* dictionary, const char* key)
{
    if (dictionary == NULL || key == NULL)
        return -1;

    if (dictionary->first == NULL)
        return -1;

    int32 num = 0;
    SDictElement* element = dictionary->first;
    while ((element != NULL) && (StrEqual(key, element->key) != true))
    {
        element = element->next;
        ++num;
    }

    if (element != NULL)
        return num;
    else
        return -1;
}
int32 DictionaryGetNumberOfValue (SDictionary* dictionary, const void* value)
{
    if (dictionary == NULL || value == NULL)
        return -1;

    if (dictionary->first == NULL)
        return -1;

    int32 num = 0;
    SDictElement* element = dictionary->first;
    while ((element != NULL) && (element->value != value))
    {
        element = element->next;
        ++num;
    }

    if (element != NULL)
        return num;
    else
        return -1;
}

uint32 DictionaryGetSize (SDictionary* dictionary)
{
    if (dictionary == NULL)
        return 0;

    return dictionary->size;
}

void DictionarySetValueByKey(SDictionary* dictionary, const char* key, void* value)
{
    if (dictionary == NULL || value == NULL  || key == NULL)
        return;

    if (dictionary->size == 0)
        return;

    SDictElement* element = DictionaryGetElementByKey(dictionary, key);
    if (element == NULL)
        return;

    //free(element->value);
    element->value = value;
}

void DictionarySetValueByNumber(SDictionary* dictionary, uint32 numOfElement, void* value)
{
    if (dictionary == NULL || value == NULL || numOfElement >= dictionary->size)
        return;

    if (dictionary->size == 0)
        return;

    SDictElement* element = DictionaryGetElementByNumber(dictionary, numOfElement);
    if (element == NULL)
        return;

    //free(element->value);
    element->value = value;
}

bool DictionarySetKeyByNumber(SDictionary* dictionary, uint32 numOfElement, const char* key)
{
    if (dictionary == NULL || key == NULL)
        return false;

    if (dictionary->size == 0)
        return false;

    // сначала проверим - есть ли уже элемент с таким же ключом??
    SDictElement* element = DictionaryGetElementByKey(dictionary, key);
    if (element != NULL)
        return false;

    element = DictionaryGetElementByNumber(dictionary, numOfElement);
    if (element != NULL)
    {
        StrCopy(element->key, key, MAX_KEY_LENGTH);
        return true;
    }

    return false;
}

bool DictionarySetKeyByValue(SDictionary* dictionary, const void* value, const char* key)
{
    if (dictionary == NULL || key == NULL)
        return false;

    if (dictionary->size == 0)
        return false;

    // сначала проверим - есть ли уже элемент с таким же ключом??
    SDictElement* element = DictionaryGetElementByKey(dictionary, key);
    if (element != NULL)
        return false;

    element = DictionaryGetElementByValue(dictionary, value);
    if (element == NULL)
        return false;

    StrCopy(element->key, key, MAX_KEY_LENGTH);

    return true;
}

bool DictionaryChangeKey(SDictionary* dictionary, const char* oldKey, const char* newKey)
{
    if (StrEqual(oldKey, newKey) == true)
        return false;

    if (dictionary == NULL || oldKey == NULL || newKey == NULL)
        return false;

    if (dictionary->size == 0)
        return false;

    // if new key already exists do nothing
    SDictElement* element = DictionaryGetElementByKey(dictionary, newKey);
    if (element != NULL)
        return false;

    // if element with old key not exists do nothing
    element = DictionaryGetElementByKey(dictionary, oldKey);
    if (element == NULL)
        return false;

    StrCopy(element->key, newKey, MAX_KEY_LENGTH);

    return true;
}

bool DictionaryChangeValue(SDictionary* dictionary, const void* oldValue, void* newValue)
{
    if (dictionary == NULL || oldValue == NULL || newValue == NULL)
        return false;

    if (dictionary->size == 0)
        return false;

    SDictElement* element = DictionaryGetElementByValue(dictionary, oldValue);
    if (element == NULL)
        return false;

    //free(element->value);
    element->value = newValue;

    return true;
}

void DictionarySortByKey(SDictionary** dictionary)
{
    if (dictionary == NULL || (*dictionary) == NULL || (*dictionary)->size == 0)
        return;

    uint32 dictionarySize = (*dictionary)->size;

    char keys[dictionarySize][MAX_KEY_LENGTH]; // массив ключей. Его и будем сортировать
    uint32 i = 0;

    /* сначала сформируем массив ключей */
    for (SDictElement* element = (*dictionary)->first; element; element = element->next)
    {
        StrCopy(keys[i], element->key, MAX_KEY_LENGTH);
        ++i;
    }

    #ifdef _DEBUG
    printf("DICTIONARY BEFORE\n");
    for (SDictElement* element = (*dictionary)->first; element && (element->key); element = element->next)
    {
        printf("%s\tvalue - %d\n", element->key, *((int*)element->value));
    }
    #endif // _DEBUG

    /* отсортируем его по возрастанию */
    qsort((void*)keys, dictionarySize, MAX_NAME_LENGTH*sizeof(char), strcmp);

    /* теперь нужно создать новый словарь и назначить его старому */
    SDictionary* newDictionary = DictionaryCreate();
    void* value;
    for (i = 0; i < dictionarySize; ++i)
    {
        value = DictionaryGetValueByKey(*dictionary, keys[i]);
        DictionaryAddElement(newDictionary, keys[i], value);
    }

    DictionaryClear(*dictionary);
    free(*dictionary);
    *dictionary = newDictionary;

    #ifdef _DEBUG
    printf("DICTIONARY AFTER\n");
    for (SDictElement* element = (*dictionary)->first; element && (element->key); element = element->next)
    {
        printf("%s\tvalue - %d\n", element->key, *((int*)element->value));
    }
    #endif // _DEBUG
}


/*  TESTS!!! */
#ifdef _DEBUG
#define MAX_DICTIONARY_SIZE 6000
void DictionaryTest()
{
    int* value1 = malloc(sizeof(int));
    int* value2 = malloc(sizeof(int));
    int* value3 = malloc(sizeof(int));
    int* value4 = malloc(sizeof(int));
    int* value5 = malloc(sizeof(int));
    int* value6 = malloc(sizeof(int));
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;
    *value6 = 6;

    void* ptr = NULL;
    char* key = NULL;
    int32 num = 0;
    bool result;

    SDictionary* dictionary = DictionaryCreate();

    /* test1 : duplicates and NULL value */
    printf ("--------test1--------\n");
    DictionaryAddElement(dictionary, "key1", value1);
    DictionaryAddElement(dictionary, "key2", value2);
    result = DictionaryAddElement(dictionary, "key2", value2);
    assert(result == false);
    DictionaryAddElement(dictionary, "key3", value3);
    DictionaryAddElement(dictionary, "key4", value4);
    result = DictionaryAddElement(dictionary, "key5", NULL);
    assert(result == false);
    result = DictionaryAddElement(dictionary,   NULL, value5);
    assert(result == false);
    assert (DictionaryGetSize(dictionary) == 4);
    printf ("size of dictionary : %d\n", DictionaryGetSize(dictionary));

    /* test2 : Get value by number */
    printf ("--------test2--------\n");
    ptr = DictionaryGetValueByNumber(dictionary, 1);
    // from the head
    assert (ptr != NULL);
    assert ((int*)ptr == value2);
    printf ("get value by number 1 - %d\n", *(int*)ptr);
    // from the tail
    ptr = DictionaryGetValueByNumber(dictionary, 3);
    assert (ptr != NULL);
    assert ((int*)ptr == value4);
    printf ("get value by number 3 - %d\n", *(int*)ptr);
    // number equal size
    ptr = DictionaryGetValueByNumber(dictionary, 4);
    assert (ptr == NULL);

    /* test3 : Get element by number over size of dictionary */
    printf ("--------test3--------\n");
    ptr = DictionaryGetValueByNumber(dictionary, 5);
    assert (ptr == NULL);
    printf ("number over size of dictionary!!\n");

    /* test4 : Get value by key */
    printf ("--------test4--------\n");
    ptr = DictionaryGetValueByKey(dictionary, "key2");
    assert (ptr != NULL);
    printf ("get value by key 'key2' - %d\n", *(int*)ptr);

    /* test5 : Get value by not exists key */
    printf ("--------test5--------\n");
    ptr = DictionaryGetValueByKey(dictionary, "another key");
    assert (ptr == NULL);
    printf ("key 'another key' doesn't exists!\n");

    /* test6 : Get key of value */
    printf ("--------test6--------\n");
    key = DictionaryGetKeyByValue(dictionary, value4);
    assert (key != NULL);
    printf ("key by value is '%s'\n", key);

    /* test7 : Get key of not exists value */
    printf ("--------test7--------\n");
    key = NULL;
    key = DictionaryGetKeyByValue(dictionary, value5);
    assert (key == NULL);
    printf ("value doesn't exists!\n");

    /* test8 : Get key by number */
    printf ("--------test8--------\n");
    key = DictionaryGetKeyByNumber(dictionary, 3);
    assert (key != NULL);
    printf ("key by number is '%s'\n", key);

    /* test9 : Get key by number over size of dictionary */
    printf ("--------test9--------\n");
    key = DictionaryGetKeyByNumber(dictionary, 10);
    assert (key == NULL);
    printf ("number over size of dictionary!!\n");

    /* test10 : Get number of value */
    printf ("--------test10--------\n");
    num = DictionaryGetNumberOfValue(dictionary, value4);
    assert (num != -1);
    printf ("number of value is %d\n", num);

    /* test11 : Get number of not exists value */
    printf ("--------test11--------\n");
    num = DictionaryGetNumberOfValue(dictionary, value5);
    assert (num == -1);
    printf ("value doesn't exists!\n");

    /* test12 : Get number of element by key */
    printf ("--------test12--------\n");
    num = DictionaryGetNumberByKey(dictionary, "key1");
    assert (num != -1);
    printf ("number of key is %d\n", num);

    /* test13 : Get number of element by not exists key */
    printf ("--------test13--------\n");
    num = DictionaryGetNumberByKey(dictionary, "another key");
    assert (num == -1);
    printf ("key 'another key' doesn't exists!\n");

    /* test14 : delete element by key */
    printf ("--------test14--------\n");
    DictionaryDeleteElementByKey(dictionary, "key1");
    assert (DictionaryGetSize(dictionary) == 3);
    printf ("size of dictionary : %d\n", DictionaryGetSize(dictionary));

    /* test15 : delete element by not exists key */
    printf ("--------test15--------\n");
    DictionaryDeleteElementByKey(dictionary, "another key");
    assert (DictionaryGetSize(dictionary) == 3);
    printf ("key 'another key' doesn't exists!\n");

    /* test16 : delete element by number */
    printf ("--------test16--------\n");
    DictionaryDeleteElementByNumber(dictionary, 2);
    assert (DictionaryGetSize(dictionary) == 2);
    printf ("size of dictionary now : %d\n", DictionaryGetSize(dictionary));

    /* test17 : delete element by not exists number */
    printf ("--------test17--------\n");
    DictionaryDeleteElementByNumber(dictionary, 20);
    assert (DictionaryGetSize(dictionary) == 2);
    printf ("...and size of dictionary now : %d\n", DictionaryGetSize(dictionary));

    /* test18 : delete dictionary */
    printf ("--------test18--------\n");
    DictionaryDestroy(&dictionary);
    assert (DictionaryGetSize(dictionary) == 0);
    assert (dictionary == NULL);
    printf ("...and now : %d\n", DictionaryGetSize(dictionary));

    /* test19 : clear deleted dictionary */
    printf ("--------test19--------\n");
    DictionaryClear(dictionary);
    assert (DictionaryGetSize(dictionary) == 0);
    printf ("dictionary already deleted\n");

    /* test20 : delete dictionary and try to get value */
    printf ("--------test20--------\n");
    ptr = DictionaryGetValueByNumber(dictionary, 1);
    assert (ptr == NULL);
    printf ("nothing to do this!\n");

    /* prepare for new test */
    value1 = malloc(sizeof(int));
    value2 = malloc(sizeof(int));
    value3 = malloc(sizeof(int));
    value4 = malloc(sizeof(int));
    value5 = malloc(sizeof(int));
    value6 = malloc(sizeof(int));
    int* value7 = malloc(sizeof(int));
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;
    *value6 = 6;
    *value7 = 7;
    dictionary = DictionaryCreate();
    DictionaryAddElement(dictionary, "key2", value2);
    DictionaryAddElement(dictionary, "key3", value3);
    DictionaryAddElement(dictionary, "key1", value1);
    DictionaryAddElement(dictionary, "key4", value4);

    /* test21 : set new value by key */
    printf ("--------test21--------\n");
    DictionarySetValueByKey(dictionary, "key2", value5);
    assert(DictionaryGetValueByKey(dictionary, "key2") == value5);
    printf ("passed!\n");

    /* test22 : set new value by number */
    printf ("--------test22--------\n");
    DictionarySetValueByNumber(dictionary, 0, value6);
    assert(DictionaryGetValueByNumber(dictionary, 0) == value6);
    printf ("passed!\n");

    /* test23 : set new key by number */
    printf ("--------test23--------\n");
    result = DictionarySetKeyByNumber(dictionary, 0, "newKey");
    assert(result == true);
    key = DictionaryGetKeyByNumber(dictionary, 0);
    assert(key != NULL);
    assert(StrEqual(key, "newKey"));
    printf("new key is - '%s'\n", key);

    /* test24 : set new key by value */
    printf ("--------test24--------\n");
    result = DictionarySetKeyByValue(dictionary, value6, "KEY");
    assert(result == true);
    key = DictionaryGetKeyByValue(dictionary, value6);
    assert(key != NULL);
    assert(StrEqual(key, "KEY"));
    printf("now key is - '%s'\n", key);

    /* test25 : change exists key */
    printf ("--------test25--------\n");
    result = DictionaryChangeKey(dictionary, "KEY", "key2");
    assert(result == true);
    key = DictionaryGetKeyByNumber(dictionary, 0);
    assert(key != NULL);
    assert(StrEqual(key, "key2"));
    printf("now key is - '%s'\n", key);

    /* test26 : change exists value */
    printf ("--------test26--------\n");
    result = DictionaryChangeValue(dictionary, value6, value7);
    assert(result == true);
    printf ("passed!\n");

    /* test27 : try set duplicate key */
    printf ("--------test27--------\n");
    result = DictionaryChangeKey(dictionary, "key2", "key3");
    assert(result == false);
    printf ("passed!\n");

    /* test28 : try set duplicate old and new key */
    printf ("--------test28--------\n");
    result = DictionaryChangeKey(dictionary, "key2", "key2");
    assert(result == false);
    printf ("passed!\n");

    /* test 29 : sorting dictionary*/
    printf ("--------test29--------\n");
    DictionarySortByKey(&dictionary);

    /* prepare for new test */
    DictionaryClear(dictionary);
    value1 = malloc(sizeof(int));
    value2 = malloc(sizeof(int));
    value3 = malloc(sizeof(int));
    value4 = malloc(sizeof(int));
    value5 = malloc(sizeof(int));
    value6 = malloc(sizeof(int));
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;
    *value6 = 6;
    DictionaryAddElement(dictionary, "key2", value2);
    DictionaryAddElement(dictionary, "key3", value3);
    DictionaryAddElement(dictionary, "key1", value1);
    DictionaryAddElement(dictionary, "key4", value4);
    DictionaryAddElement(dictionary, "key6", value6);
    DictionaryAddElement(dictionary, "key5", value5);

    // prepare long dictionary...
    DictionaryClear(dictionary);
    int32* array = (int32*)malloc(MAX_DICTIONARY_SIZE*sizeof(int32));
    char key_name[10] = {0};
    for (uint32 i = 0; i < MAX_DICTIONARY_SIZE; ++i)
    {
        array[i] = i;
        memset(key_name, 0, 10*sizeof(char));
        snprintf(key_name, 10, "key%d", i);
        DictionaryAddElement(dictionary, key_name, &array[i]);
    }

    /* test 30 : get element by value */
    printf ("--------test30--------\n");
    SDictElement* element = NULL;
    uint32 startT = SDL_GetTicks();
    for (uint32 i = 0; i < MAX_DICTIONARY_SIZE; ++i)
    {
        //printf("value%d = ", i);
        element = DictionaryGetElementByValue(dictionary, &array[i]);
        assert(element != NULL);
        assert(element->value == &array[i]);
        //printf("%d\n", i, *(int*)element->value);
    }
    uint32 endT = SDL_GetTicks();
    printf ("time elapsed - %d\n", endT - startT);
    getchar();
    printf ("passed!\n");

    /* test 31 : get element by key */
    printf ("--------test31--------\n");
    startT = SDL_GetTicks();
    for (uint32 i = 0; i < MAX_DICTIONARY_SIZE; ++i)
    {
        memset(key_name, 0, 10*sizeof(char));
        snprintf(key_name, 10, "key%d", i);
        //printf("value%d = ", i);
        element = DictionaryGetElementByKey(dictionary, key_name);
        //printf("%s\n", key_name);
        assert(element != NULL);
        //assert(StrEqual(element->key, key_name) == true);
        //printf("%d\n", i, *(int*)element->value);
    }
    endT = SDL_GetTicks();
    printf ("time elapsed - %d\n", endT - startT);
    getchar();
    printf ("passed!\n");


    DictionaryDestroy(&dictionary);
    free(value1);
    free(value2);
    free(value3);
    free(value4);
    free(value5);
    free(value6);
    free(key);
    free(ptr);

    /* passed */
    printf ("--------result-------\n");
    printf ("all dictionary's tests are passed!\n");
}
#endif // _DEBUG
