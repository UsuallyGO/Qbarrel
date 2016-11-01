
#include "btype.h"
#include "bwrapevt.h"
#include "bmemory.h"

#include "qbarrel.h"
#include "qevent.h"

#ifdef __linux__
QB_PUBLIC wrpEvtOpt* Get_SelectOpt();
#endif

QB_PUBLIC wrpEvtLoop* evtloop_create(int evtnum)
{
    Int in;
    wrpEvtLoop *evtLoop;

    evtLoop = (wrpEvtLoop*)qmalloc(sizeof(wrpEvtLoop));
    if(evtLoop == NULL)
        goto evtLoop_Err;

    if(evtnum <= 0)
        evtnum = EVENT_INITIALIZE_NUM;
    evtLoop->m_evtnum = evtnum;
    evtLoop->m_filevts = (wrpFileEvt*)qmalloc(sizeof(wrpFileEvt)*evtnum);
    if(evtLoop->m_filevts == NULL)
        goto filevts_Err;
    evtLoop->m_atvevts = (wrpAtvEvt*)qmalloc(sizeof(wrpAtvEvt)*evtnum);
    if(evtLoop->m_atvevts == NULL)
        goto atvevts_Err;

    evtLoop->m_state   = EVENT_RUNNING;
    evtLoop->m_timevts = NULL;
    evtLoop->m_maxfd   = -1;

    for(in = 0; in < evtnum; in++)
        evtLoop->m_filevts[in].m_mask = EVENT_NONE;

    evtLoop->m_evtopt = Get_SelectOpt();//can't be null, doesn't need to check
    in = evtLoop->m_evtopt->createOpt(evtLoop);//evtLoop->m_dataopt will be set inside
    if(in != QBARREL_SUCCESS)
        goto evtcreate_Err;
   
    return evtLoop;

evtcreate_Err:
    qfree(evtLoop->m_atvevts);
atvevts_Err:
    qfree(evtLoop->m_filevts);
filevts_Err:
    qfree(evtLoop);
evtLoop_Err:
    return NULL;
}

QB_PUBLIC void evtloop_destroy(wrpEvtLoop *evtLoop)
{
    evtLoop->m_evtopt->destroy(evtLoop);
    qfree(evtLoop->m_dataopt);
    qfree(evtLoop->m_filevts);
    qfree(evtLoop->m_atvevts);
    qfree(evtLoop);
}

QB_PUBLIC void evtloop_stop(wrpEvtLoop *evtLoop)
{
    evtLoop->m_state = EVENT_STOP;
}

QB_PUBLIC int evtloop_addfilevt(wrpEvtLoop *evtLoop, int fd, int mask,
                                evtFileHandler func, void *arg)
{
    Int res;
    wrpEvtOpt *opt;
    wrpFileEvt *fevt;

    if(evtLoop == NULL || func == NULL || fd >= evtLoop->m_evtnum)
        return QBARREL_ERR_INVAL;

    fevt = &evtLoop->m_filevts[fd];
    opt = evtLoop->m_evtopt;
    res = opt->addOpt(evtLoop, fd, mask);
    if(res != QBARREL_SUCCESS)
        return res;

    fevt->m_mask = 0;
    fevt->m_mask |= mask;
    if(mask & EVENT_READABLE)
        fevt->readHandler = func;
    if(mask & EVENT_WRITABLE)
        fevt->writeHandler = func;
    fevt->m_arg = arg;
    if(fd > evtLoop->m_maxfd)
        evtLoop->m_maxfd = fd;

    return QBARREL_SUCCESS;
}

QB_PUBLIC void evtLoop_delfilevt(wrpEvtLoop *evtLoop, int fd, int mask)
{
    Int index;
    wrpEvtOpt  *opt;
    wrpFileEvt *fevt;

    if(evtLoop == NULL || fd >= evtLoop->m_evtnum)
        return;

    fevt = &evtLoop->m_filevts[fd];
    opt = evtLoop->m_evtopt;
    opt->delOpt(evtLoop, fd, mask);
    fevt->m_mask = fevt->m_mask & (~mask);//clear the mask bits
    if(fd == evtLoop->m_maxfd && fevt->m_mask == EVENT_NONE)
    {//update the maxfd
        for(index = evtLoop->m_maxfd-1; index >=0; index--)
            if(evtLoop->m_filevts[index].m_mask != EVENT_NONE)
                break;//decrease from maxfd-1, so this is the current maxfd

        evtLoop->m_maxfd = index;
    }
}

static int _pollingEvents(wrpEvtLoop *evtLoop, Int flags)
{
    Int evtnums, index, mask, fd;
    Int hdlnums, retrig;
    wrpEvtOpt *opt;
    wrpFileEvt *fevt;

    if(!(flags & FILE_EVENTS) && !(flags & TIME_EVENTS))
        return QBARREL_SUCCESS;

    hdlnums = 0;
    if(evtLoop->m_maxfd > -1)
    {
        opt = evtLoop->m_evtopt;
        evtnums = opt->pollOpt(evtLoop, NULL);
        for(index = 0; index < evtnums; index++)
        {
            fd   = evtLoop->m_atvevts[index].m_fd;
            mask = evtLoop->m_atvevts[index].m_mask;
            fevt = &evtLoop->m_filevts[fd];
            retrig = 0;

            /*Should comment for: fevt->m_mask & mask & ...*/
            if(fevt->m_mask & mask & EVENT_READABLE)
            {
                retrig = 1;
                fevt->readHandler(evtLoop, fd, fevt->m_mask, fevt->m_arg);
            }
            if(fevt->m_mask & mask & EVENT_WRITABLE)
            {
                if(!retrig || fevt->readHandler != fevt->writeHandler)
                    fevt->writeHandler(evtLoop, fd, fevt->m_mask, fevt->m_arg);
            }
            hdlnums++;
        }
    }

    return hdlnums;
}

QB_PUBLIC void evtlooping(wrpEvtLoop *evtLoop)
{
    evtLoop->m_state = EVENT_RUNNING;
    while(evtLoop->m_state == EVENT_RUNNING)
    {
        _pollingEvents(evtLoop, ALL_EVENTS);
    }
}
