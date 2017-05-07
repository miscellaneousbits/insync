#include "stdafx.h"

CRun *CWorker::m_run = NULL;

CWorker::CWorker()
{
    if (m_run == NULL) {
        m_run = &CRun::Singleton();
    }
}

BOOL CWorker::Initialize(void * /*pvParam*/)
{
    CRun &run = CRun::Singleton();
    CScopedLock lock(&run.m_threadBufferLock);
    m_copyBuffer = run.m_threadBuffer;
    run.m_threadBuffer += run.kMaxThreadBufferSize * 2;
    return TRUE;
}

void CWorker::Terminate(void * /*pvParam*/)
{
}

void CWorker::Execute(RequestType dw, void * /*pvParam*/, OVERLAPPED * /*pOverlapped*/)
{
    m_run->IncrementRunningThreads();

    if (dw->m_workType == CTaskWork::mergeType) {
        CMergeTaskWork *merge = (CMergeTaskWork *)dw;
        merge->m_trg->MergeDirs(merge->m_fromList, merge->m_toList, merge->m_from, merge->m_to,
                                m_copyBuffer);
    } else {
        CBackupOrSyncTaskWork *backup = (CBackupOrSyncTaskWork *)dw;
        backup->m_trg->BackupOrSync(backup->m_from, backup->m_to, m_copyBuffer, backup->m_fromExists,
                                    backup->m_toExists);
    }

    m_run->DecrementRunningThreads();

    if (--(dw->m_trg->m_pendingTasks) <= 0) {
        ::SetEvent(dw->m_trg->m_doneEvent);
    }

    delete dw;
}
