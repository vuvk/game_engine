#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "list.h"

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

SList* ListCreate()
{
    SList* list = calloc(1, sizeof(SList));
    if (list == NULL)
        return NULL;

    return list;
}

void ListClear (SList* list)
{
    if (list == NULL || list->size == 0 || list->first == NULL)
        return;

    /* проходимся от первого до последнего */
    SListElement* element = list->first;
    SListElement* nextElement = NULL;
    while (element != NULL)
    {
        nextElement = element->next;

        /* удаляем значение, если таковое имеется */
        #ifdef _LIST_PRINT_DEBUG
        printf("\n\t\ttry to delete value...");
        #endif // _LIST_PRINT_DEBUG

        //free (element->value);
        element->value = NULL;

        #ifdef _LIST_PRINT_DEBUG
        printf ("done\n");
        #endif // _LIST_PRINT_DEBUG

        /* удаляем элемент, если есть */
        #ifdef _LIST_PRINT_DEBUG
        printf("\n\t\ttry to delete element...");
        #endif // _LIST_PRINT_DEBUG

        free (element);
        //element = NULL;

        #ifdef _LIST_PRINT_DEBUG
        printf ("done\n");
        #endif // _LIST_PRINT_DEBUG

        element = nextElement;
    }

    /* чистим указатели на первый и последний элементы */
    if (list->first)
        list->first = NULL;
    if (list->last)
        list->last = NULL;

    list->size = 0;
}

void ListDestroy (SList** list)
{
    if (list == NULL || *list == NULL)
        return;

    ListClear (*list);

    free (*list);
    *list = NULL;
}

bool ListAddElement (SList* list, void* value)
{
    if (list == NULL || value == NULL)
        return false;

    SListElement* newElement = malloc(sizeof(SListElement));
    newElement->value = value;
    newElement->next = NULL;
    ++list->size;

    // if it is first element
    if (list->first == NULL)
    {
        newElement->prev = NULL;
        list->first = newElement;
        list->last = newElement;

        #ifdef _LIST_PRINT_DEBUG
        printf ("add as first\n");
        #endif // _LIST_PRINT_DEBUG
    }
    else
    {
        newElement->prev = list->last;
        list->last->next = newElement;

        list->last = newElement;

        #ifdef _LIST_PRINT_DEBUG
        printf ("add to end\n");
        #endif // _LIST_PRINT_DEBUG
    }

    return true;
}

bool ListDeleteElement (SList* list, SListElement* element)
{
    if (list == NULL || element == NULL)
        return false;

    // удаляем элемент и сцепляем соседей
    #ifdef _LIST_PRINT_DEBUG
    printf ("try to delete value...");
    #endif // _LIST_PRINT_DEBUG

    //free (element->value);
    element->value = NULL;

    #ifdef _LIST_PRINT_DEBUG
    printf ("\n\t\tdone\n");
    #endif // _LIST_PRINT_DEBUG

    --list->size;

    // элемент где-то между головой и хвостом
    if (element->prev != NULL && element->next != NULL)
    {
        #ifdef _LIST_PRINT_DEBUG
        printf ("deleting anything...");
        #endif // _LIST_PRINT_DEBUG

        element->prev->next = element->next;
        element->next->prev = element->prev;

        #ifdef _LIST_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _LIST_PRINT_DEBUG

        goto element_delete;
    }

    // элемент головной
    if (element->prev == NULL && element->next != NULL)
    {
        #ifdef _LIST_PRINT_DEBUG
        printf ("deleting head...");
        #endif // _LIST_PRINT_DEBUG

        list->first = element->next;
        element->next->prev = NULL;

        #ifdef _LIST_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _LIST_PRINT_DEBUG

        goto element_delete;
    }

    // элемент хвостовой
    if (element->prev != NULL && element->next == NULL)
    {
        #ifdef _LIST_PRINT_DEBUG
        printf ("deleting tail...");
        #endif // _LIST_PRINT_DEBUG

        list->last = element->prev;
        element->prev->next = NULL;

        #ifdef _LIST_PRINT_DEBUG
        printf ("\n\t\tdone\n");
        #endif // _LIST_PRINT_DEBUG

        goto element_delete;
    }

    // элемент последний
    if ((list->size <= 0) || (element->prev == NULL && element->next == NULL))
    {
        #ifdef _LIST_PRINT_DEBUG
        printf ("deleting last...");
        #endif // _LIST_PRINT_DEBUG

        // уже всё удалено. Гудбай
        list->first = NULL;
        list->last = NULL;
        //element = NULL;

        list->size = 0;

        #ifdef _LIST_PRINT_DEBUG
        printf ("all deleted! list is empty now!\n");
        #endif // _LIST_PRINT_DEBUG
    }

    element_delete:
        #ifdef _LIST_PRINT_DEBUG
        printf ("try to delete element...");
        #endif // _LIST_PRINT_DEBUG

        free (element);
        element = NULL;

    #ifdef _LIST_PRINT_DEBUG
    printf ("\n\t\tdone\n");
    #endif // _LIST_PRINT_DEBUG

    return true;
}

#ifdef _MULTITHREADS
/* parameters for quick search element by value in list */
typedef struct
{
    SListElement*  startElement;    /* head or tail */
    SListElement** resultElement;
    void*   value;                  /* value for search */
    int32   maxNumber;              /* maxNumber - when it's stop */
    int8    direction;              /* direction for finding - 1 (head-to-tail) or -1(tail-to-head) */
} SThrdParameter;

/* func for search element by value in thread */
static int ListGetElementByValueThread(void* arg)
{
    if (arg == NULL)
    #ifdef _USE_SDL_THREADS
        return SDL_FALSE;
    #else
        return thrd_error;
    #endif // _USE_SDL_THREADS

    SThrdParameter* param   = (SThrdParameter*)arg;
    SListElement*   element = param->startElement;
    int32 maxPos = param->maxNumber;
    SListElement** resultElement = param->resultElement;


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

static SListElement* ListGetElementByValueInThreads (SList* list, const void* value)
{
    if (list->first == NULL || list->last == NULL)
        return NULL;

    SListElement* result = NULL;
    /* center of list */
    int32 center = list->size / 2;

    SThrdParameter param1 = {list->first, &result, (void*)value, center,               1};
    SThrdParameter param2 = {list->last,  &result, (void*)value, list->size - center, -1};

    #ifdef _USE_SDL_THREADS
    SDL_Thread* threadFromHead = SDL_CreateThread(ListGetElementByValueThread, "fromHead", &param1);
    if (threadFromHead == NULL)
        return NULL;
    SDL_Thread* threadFromTail = SDL_CreateThread(ListGetElementByValueThread, "fromTail", &param2);
    if (threadFromTail == NULL)
        return NULL;

    SDL_WaitThread(threadFromHead, NULL);
    SDL_WaitThread(threadFromTail, NULL);

    #else // NOT _USE_SDL_THREADS
    thrd_t threadFromHead = 0;
    thrd_t threadFromTail = 0;

    if (thrd_create(&threadFromHead, ListGetElementByValueThread, &param1) != thrd_success)
        return NULL;

    if (thrd_create(&threadFromTail, ListGetElementByValueThread, &param2) != thrd_success)
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

SListElement* ListGetElementByValue (SList* list, const void* value)
{
    #ifdef _MULTITHREADS
    SListElement* element = NULL;
    if (list->size <= MAX_VALUES_FOR_ONE_THRD)
    {
        element = list->first;
        while ((element != NULL) && (element->value != value))
            element = element->next;
    }
    else
        element = ListGetElementByValueInThreads(list, value);
    #else // NOT _MULTITHREADS
    SListElement* element = list->first;
    while ((element != NULL) && (element->value != value))
            element = element->next;
    #endif // _MULTITHREADS

    return element;
}

SListElement* ListGetElementByNumber (SList* list, uint32 numOfElement)
{
    SListElement* element = NULL;
    /* сheck which way is faster - from the head or tail */
    if ((list->size - numOfElement) >= numOfElement)
    {
        /* go from head */
        element = list->first;
        for (uint32 i = 0; element && (i < numOfElement); ++i)
            element = element->next;
    }
    else
    {
        /* go from tail */
        element = list->last;
        for (uint32 i = list->size - 1; element && (i > numOfElement); --i)
            element = element->prev;
    }

    return element;
}

void ListDeleteElementByValue (SList* list, void* value)
{
    if (list == NULL || value == NULL)
        return;

    if (list->first == NULL)
        return;

    SListElement* element = ListGetElementByValue(list, value);
    if (element != NULL)
        ListDeleteElement (list, element);
}

void ListDeleteElementByNumber (SList* list, uint32 numOfElement)
{
    if (list == NULL || (numOfElement >= list->size))
        return;

    if (list->size == 0 || list->first == NULL)
        return;

    SListElement* element = ListGetElementByNumber(list, numOfElement);
    if (element != NULL)
        ListDeleteElement (list, element);
}

void* ListGetValueByNumber (SList* list, uint32 numOfElement)
{
    if (list == NULL || (numOfElement >= list->size))
        return NULL;

    if (list->size == 0 || list->first == NULL)
        return NULL;

    SListElement* element = ListGetElementByNumber(list, numOfElement);
    if (element == NULL)
        return NULL;

    return element->value;;
}

int32 ListGetNumberByValue (SList* list, void* value)
{
    if (list == NULL || value == NULL)
        return -1;

    if (list->first == NULL)
        return -1;

    // пробегаемся по всем элементам, пока не наткнемся на значение или на конец...
    int32 num = 0;
    SListElement* element = list->first;
    while ((element != NULL) && (element->value != value))
    {
        element = element->next;
        ++num;
    }

    if (element != NULL && element->value == value)
        return num;
    else
        return -1;
}

uint32 ListGetSize (SList* list)
{
    if (list == NULL)
        return 0;

    return list->size;
}

void ListSetValueByNumber(SList* list, uint32 numOfElement, void* value)
{
    if (list == NULL || value == NULL || numOfElement >= list->size)
        return;

    if (list->size == 0)
        return;

    SListElement* element = ListGetElementByNumber(list, numOfElement);
    if (element == NULL)
        return;

    //free(element->value);
    element->value = value;
}

bool ListChangeValue(SList* list, const void* oldValue, void* newValue)
{
    if (list == NULL || oldValue == NULL || newValue == NULL)
        return false;

    if (list->size == 0)
        return false;

    SListElement* element = ListGetElementByValue(list, oldValue);
    if (element == NULL)
        return false;

    //free(element->value);
    element->value = newValue;

    return true;
}


/*  TESTS!!! */
#ifdef _DEBUG
#define MAX_LIST_SIZE 6000
void ListTest()
{
    printf ("List's tests started!!!\n");

    int* value1 = malloc(sizeof(int));
    int* value2 = malloc(sizeof(int));
    int* value3 = malloc(sizeof(int));
    int* value4 = malloc(sizeof(int));
    int* value5 = malloc(sizeof(int));
    int* value6 = NULL;
    *value1 = 1;
    *value2 = 2;
    *value3 = 3;
    *value4 = 4;
    *value5 = 5;

    void* ptr = NULL;
    int num = -1;
    bool result;

    SList* list = ListCreate();

    /* test1 : check size */
    printf ("--------test1--------\n");
    ListAddElement(list, value1);
    ListAddElement(list, value2);
    ListAddElement(list, value3);
    ListAddElement(list, value4);
    assert (ListGetSize(list) == 4);
    printf ("size of list : %d\n", ListGetSize(list));

    /* test2 : Get element by number */
    printf ("--------test2--------\n");
    // from the head
    ptr = ListGetValueByNumber(list, 1);
    assert (ptr != NULL);
    assert ((int*)ptr == value2);
    printf ("get value by number 1 - %d\n", *(int*)ptr);
    // from the tail
    ptr = ListGetValueByNumber(list, 3);
    assert (ptr != NULL);
    assert ((int*)ptr == value4);
    printf ("get value by number 3 - %d\n", *(int*)ptr);

    /* test3 : Get element by number over size of list */
    printf ("--------test3--------\n");
    ptr = ListGetValueByNumber(list, 5);
    assert (ptr == NULL);
    printf ("number over size of list!!\n");

    /* test4 : delete element by number */
    printf ("--------test4--------\n");
    ListDeleteElementByNumber (list, 0);
    assert (ListGetSize(list) == 3);
    printf ("new size of list : %d\n", ListGetSize(list));

    /* test5 : Get new element by number */
    printf ("--------test5--------\n");
    ptr = ListGetValueByNumber(list, 2);
    assert (ptr != NULL);
    printf ("get new value by number 2 : %d\n", *(int*)ptr);

    /* test6 : Get element by number over new size of list */
    printf ("--------test6--------\n");
    ptr = ListGetValueByNumber(list, 4);
    assert (ptr == NULL);
    printf ("number over size of list!!\n");

    /* test7 : delete element by deleted value early */
    printf ("--------test7--------\n");
    ListDeleteElementByValue(list, value1);
    assert (ListGetSize(list) == 3);
    printf ("size of list : %d\n", ListGetSize(list));

    /* test8 : delete element by exists value */
    printf ("--------test8--------\n");
    ListDeleteElementByValue(list, value2);
    assert (ListGetSize(list) == 2);
    printf ("new size of list : %d\n", ListGetSize(list));

    /* test9 : Get number of value */
    printf ("--------test9--------\n");
    num = ListGetNumberByValue(list, value3);
    assert (num != -1);
    printf ("number of value is %d\n", num);

    /* test10 : Get number of deleted value */
    printf ("--------test10--------\n");
    num = ListGetNumberByValue(list, value2);
    assert (num == -1);
    printf ("value not exists!\n");

    /* test11 : clear list */
    printf ("--------test11--------\n");
    ListClear(list);
    assert (ListGetSize(list) == 0);
    printf ("now size of list : %d\n", ListGetSize(list));

    /* test12 : delete list and try to get value */
    printf ("--------test12--------\n");
    ListDestroy(&list);
    assert (list == NULL);
    ptr = ListGetValueByNumber(list, 1);
    assert (ptr == NULL);
    printf ("nothing to do this!\n");

    /* prepare for new tests */
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
    list = ListCreate();
    ListAddElement(list, value1);
    ListAddElement(list, value2);
    ListAddElement(list, value3);

    /* test13 : set new value by number */
    printf ("--------test13--------\n");
    ListSetValueByNumber(list, 0, value5);
    assert(ListGetValueByNumber(list, 0) == value5);
    printf ("passed!\n");

    /* test14 : change exists value */
    printf ("--------test14--------\n");
    result = ListChangeValue(list, value5, value4);
    assert(result == true);
    assert(ListGetValueByNumber(list, 0) == value4);
    printf ("passed!\n");

    // prepare long list...
    ListClear(list);
    int32* array = malloc(MAX_LIST_SIZE*sizeof(int32));
    for (uint32 i = 0; i < MAX_LIST_SIZE; ++i)
    {
        array[i] = i;
        ListAddElement(list, &array[i]);
    }

    /* test15 : get element by value */
    printf ("--------test15--------\n");
    SListElement* element = NULL;
    uint32 startT = SDL_GetTicks();
    for (uint32 i = 0; i < MAX_LIST_SIZE; ++i)
    {
        //printf("value%d = ", i);
        element = ListGetElementByValue(list, &array[i]);
        assert(element != NULL);
        assert(element->value == &array[i]);
        //printf("%d\n", i, *(int*)element->value);
    }
    uint32 endT = SDL_GetTicks();
    printf ("time elapsed - %d\n", endT - startT);
    getchar();
    printf ("passed!\n");

    ListDestroy(&list);
    free(array);
    free(value1);
    free(value2);
    free(value3);
    free(value4);
    free(value5);
    free(value6);
    free(ptr);

    /* passed */
    printf ("--------result-------\n");
    printf ("all list's tests are passed!\n");
}
#endif // _DEBUG
