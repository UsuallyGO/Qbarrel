
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "qbarrel.h"
#include "qdict.h"

void setTest(qDict *dict)
{
    char key[100], val[100];
    int res;

    printf("key:"), fflush(stdout), scanf("%s", key);
    printf("value:"), fflush(stdout), scanf("%s", val);

    res = qdict_set(dict, key, val);
    assert(res == QBARREL_SUCCESS);
}

void getTest(qDict *dict)
{
    char key[100];
    char *val;
    int res;

    printf("key:"), fflush(stdout), scanf("%s", key);
    val = qdict_get(dict, key);
    if(val == NULL)
        printf("key:%s not found.\n", key);
    else
        printf("key:%s value:%s\n", key, val);
}

void addTest(qDict *dict)
{
    char key[100], val[100];
    int res;

    printf("key:"), fflush(stdout), scanf("%s", key);
    printf("val:"), fflush(stdout), scanf("%s", val);

    res = qdict_add(dict, key, val);
    if(res != QBARREL_SUCCESS)
        printf("Add failed.\n");
    else
        printf("Add succeed.\n");
}

void findTest(qDict *dict)
{
    char key[100], val[100];
    int res;

    printf("key:"), fflush(stdout), scanf("%s", key);
    printf("val:"), fflush(stdout), scanf("%s", val);

    res = qdict_find(dict, key, val);
    if(res == QBARREL_SUCCESS)
       printf("Has find! key:%s val:%s\n", key, val);
    else
       printf("Not found! key:%s, val:%s\n", key, val);
}

int main()
{
    int res, sign;
    qDict *dict;
    char psz[100];

    //dict = qdict_new();
    dict = qdict_create(DICT_STR_STR);
    assert(dict != NULL);

    sign = 1;
    while(sign)
    {
        printf("Command:"),fflush(stdout), scanf("%s", psz);
        if(strcmp("get", psz) == 0)
            getTest(dict);
        else if(strcmp("set", psz) == 0)
            setTest(dict);
        else if(strcmp("add", psz) == 0)
            addTest(dict);
        else if(strcmp("find", psz) == 0)
            findTest(dict);
        else
            sign = 0;
    }

    return 0;
}
