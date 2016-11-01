
#ifndef _QBARREL_DICT_H__
#define _QBARREL_DICT_H__

#define DICT_STR_STR 0

typedef struct qDict_t qDict;

QB_PUBLIC qDict* qdict_create(int opt);

QB_PUBLIC void* qdict_get(qDict *dict, const void *key);

QB_PUBLIC int qdict_find(qDict *dict, const void *key, const void *value);

QB_PUBLIC int qdict_set(qDict *dict, const void *key, const void *value);

QB_PUBLIC int qdict_add(qDict *dict, const void *key, const void *value);


#endif //!_QBARREL_DICT_H__
