
#include <sys/select.h>
#include <string.h>

#include "bevtselect.h"
#include "bwrapevt.h"
#include "bmemory.h"

#include "qbarrel.h"

struct _selData
{
    fd_set m_rdfds;
    fd_set m_wtfds;

    fd_set _rdfds;
    fd_set _wtfds;
};

typedef struct _selData selData;

//Inside functions, do not check parameters
static Int _selCreate(wrpEvtLoop *evtLoop)
{
    selData *data;

    data = (selData*)qmalloc(sizeof(selData));
    if(data == NULL)
        return QBARREL_ERR_NOMEM;

    FD_ZERO(&data->m_rdfds);
    FD_ZERO(&data->m_wtfds);
    evtLoop->m_dataopt = data;

    return QBARREL_SUCCESS;
}

//return the number of the triggered events
static Int _selPoll(wrpEvtLoop *evtLoop, btime *tv)
{
    selData *data;
    Int res, index, mask, evtnums;
    wrpFileEvt *fevt;

    data = evtLoop->m_dataopt;

    memcpy(&data->_rdfds, &data->m_rdfds, sizeof(fd_set));
    memcpy(&data->_wtfds, &data->m_wtfds, sizeof(fd_set));

    evtnums = 0;
    res = select(evtLoop->m_maxfd+1, &data->_rdfds, &data->_wtfds, NULL, tv);
    if(res >= 0)//event has been triggered
    {
        for(index = 0; index < evtLoop->m_maxfd+1; index++)
        {
            mask = 0;
            fevt = &evtLoop->m_filevts[index];
            if(fevt->m_mask == EVENT_NONE)
                continue;//check next event
            if(fevt->m_mask & EVENT_READABLE && FD_ISSET(index, &data->_rdfds))
                mask |= EVENT_READABLE;
            if(fevt->m_mask & EVENT_WRITABLE && FD_ISSET(index, &data->_rdfds))
	            mask |= EVENT_WRITABLE;

            evtLoop->m_atvevts[evtnums].m_fd = index;
            evtLoop->m_atvevts[evtnums].m_mask = mask;
            evtnums++;
        }
    }

    return evtnums;
}

static Int _selAdd(wrpEvtLoop *evtLoop, Int fd, Int mask)
{
    selData *data;

    data = evtLoop->m_dataopt;
    if(mask & EVENT_READABLE)
        FD_SET(fd, &data->m_rdfds);
    if(mask & EVENT_WRITABLE)
        FD_SET(fd, &data->m_wtfds);

    return QBARREL_SUCCESS;
}

static void _selDel(wrpEvtLoop *evtLoop, Int fd, Int mask)
{
    selData *data;

    if(mask & EVENT_READABLE)
        FD_CLR(fd, &data->m_rdfds);
    if(mask & EVENT_WRITABLE)
        FD_CLR(fd, &data->m_wtfds);
}

static void _selDestroy(wrpEvtLoop *evtLoop)
{
    (void)evtLoop;//nothing done here, _selectOpt donot need to free
}


static wrpEvtOpt _selectOpt = 
{
    "select",
    _selCreate,
    _selPoll,
    _selAdd,
    _selDel,
};

QB_PUBLIC wrpEvtOpt* Get_SelectOpt()
{
    return &_selectOpt;
}

