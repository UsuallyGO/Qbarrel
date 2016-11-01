
#ifndef __QBARREL_EVENT_H__
#define __QBARREL_EVENT_H__

#include "qbarrel.h"

#define EVENT_READABLE 1
#define EVENT_WRITABLE 2

typedef struct wrapEventLoop_t wrpEvtLoop;

typedef void (*evtFileHandler)(wrpEvtLoop *evtLoop, int fd, int mask, void* arg);
typedef void (*evtTimeHandler)(wrpEvtLoop *evtLoop, int id, void* arg);

QB_PUBLIC wrpEvtLoop* evtloop_create(int evtnum);

QB_PUBLIC void evtloop_destroy(wrpEvtLoop *evtLoop);

QB_PUBLIC void evtloop_stop(wrpEvtLoop *evtLoop);

QB_PUBLIC int evtloop_addfilevt(wrpEvtLoop *evtLoop, int fd,
                         int mask, evtFileHandler func, void *arg);

QB_PUBLIC void evtLoop_delfilevt(wrpEvtLoop *evtLoop, int fd, int mask);

QB_PUBLIC void evtlooping(wrpEvtLoop *evtLoop);

#endif //! __QBARREL_EVENT_H__
