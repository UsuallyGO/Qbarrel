
#ifndef __BARREL_WRAPEVT_H__
#define __BARREL_WRAPEVT_H__

#include "btype.h"
#include "btime.h"

#include "qevent.h"

#define EVENT_NONE     0

#define FILE_EVENTS 1
#define TIME_EVENTS 2
#define ALL_EVENTS  (FILE_EVENTS|TIME_EVENTS)

#define EVENT_INITIALIZE_NUM 100

#define EVENT_RUNNING 1
#define EVENT_STOP    0

typedef struct wrapEventLoop_t wrpEvtLoop;
typedef struct wrapFileEvent_t wrpFileEvt;
typedef struct wrapTimeEvent_t wrpTimeEvt;
typedef struct wrapAtvEvent_t  wrpAtvEvt;
typedef struct wrapEventOpt_t  wrpEvtOpt;

struct wrapFileEvent_t
{
    Int m_mask; //READABLE or WRITABLE event

    VPtr m_arg;
    evtFileHandler readHandler;
    evtFileHandler writeHandler;
};

struct wrapTimeEvent_t
{
    Int   m_id; //identifier
    Long  m_sec;//seconds
    Long  m_ms; //milliseconds

    VPtr  m_arg;
    evtTimeHandler timeHandler;

    wrpTimeEvt *m_next;
};

struct wrapAtvEvent_t
{
    int m_fd;
    int m_mask;
};

struct wrapEventOpt_t
{
    const Char *m_name;   
    Int  (*createOpt)(wrpEvtLoop *evtLoop);
    Int  (  *pollOpt)(wrpEvtLoop *evtLoop, btime *tv);
    Int  (   *addOpt)(wrpEvtLoop *evtLoop, Int fd, Int mask);
    void (   *delOpt)(wrpEvtLoop *evtLoop, Int fd, Int mask);
    void (  *destroy)(wrpEvtLoop *evtLoop);
};

struct wrapEventLoop_t
{
    VPtr m_dataopt;
    wrpEvtOpt *m_evtopt;

    Int m_evtnum;
    Int m_maxfd;
    Int m_state;
    wrpFileEvt *m_filevts;
    wrpTimeEvt *m_timevts;
    wrpAtvEvt  *m_atvevts;
};

#endif //!__BARREL_WRAPEVT_H__
