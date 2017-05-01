//****************************************************************************
//
// Copyright (c) 1998-2014 Dillobits Software Inc.
//
// This program is the proprietary software of Dillobits Software and/or its
// licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Dillobits Software (an "Authorized
// License"). Except as set forth in an Authorized License, Dillobits Software
// grants no license (express or implied), right to use, or waiver of any kind
// with respect to the Software, and Dillobits Software expressly reserves all
// rights in and to the Software and all intellectual property rights therein.
// IF YOU HAVE NOT AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
// SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY DILLOBITS SOFTWARE AND
// DISCONTINUE ALL USE OF THE SOFTWARE.
//
//****************************************************************************

#include "stdafx.h"

CRun *CWorker::m_run = NULL;

CWorker::CWorker() {
    if (m_run == NULL) {
        m_run = &CRun::Singleton();
    }
}

BOOL CWorker::Initialize(void * /*pvParam*/) {
    CRun &run = CRun::Singleton();
    CScopedLock lock(&run.m_threadBufferLock);
    m_copyBuffer = run.m_threadBuffer;
    run.m_threadBuffer += run.kMaxThreadBufferSize * 2;
    return TRUE;
}

void CWorker::Terminate(void * /*pvParam*/) {
}

void CWorker::Execute(RequestType dw, void * /*pvParam*/, OVERLAPPED * /*pOverlapped*/) {
    m_run->IncrementRunningThreads();
    if (dw->m_workType == CTaskWork::mergeType) {
        CMergeTaskWork *merge = (CMergeTaskWork *)dw;
        merge->m_trg->MergeDirs(merge->m_fromList, merge->m_toList, merge->m_from, merge->m_to, m_copyBuffer);
    }
    else {
        CBackupOrSyncTaskWork *backup = (CBackupOrSyncTaskWork *)dw;
        backup->m_trg->BackupOrSync(backup->m_from, backup->m_to, m_copyBuffer, backup->m_fromExists, backup->m_toExists);
    }
    m_run->DecrementRunningThreads();
    if (--(dw->m_trg->m_pendingTasks) <= 0) {
        ::SetEvent(dw->m_trg->m_doneEvent);
    }
    delete dw;
}
