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

CRun *CRun::m_this = NULL;

CRun::CRun() :
    m_jobsActive(false) {
    m_this = this;
    m_counts.Clear();
    m_activeThreadCount = 0;
}

DWORD JobsThread(LPVOID pParam) {
    return ((CRun *)pParam)->CJobsThread();
}

void CRun::Start(const bool simulate) {
    InSyncApp.m_simulate = simulate;
    DWORD id;
    m_hRun = ::CreateThread(NULL, 0, JobsThread, this, CREATE_SUSPENDED, &id);
    if (m_hRun) {
        ResumeThread(m_hRun);
    }
}

// Scan the dependency subtree and make sure there are no loops
bool CRun::CheckDependency(void *j) {
    CJobNode *job = (CJobNode *)j;
    job->m_dependencyVisited = true;
    VoidPtrListIter dj;
    for (dj = job->m_beforeJobs.begin(); dj != job->m_beforeJobs.end(); dj++) {
        CJobNode *dJob = (CJobNode *)(*dj);
        if (dJob->m_selected) {
            if (dJob->m_dependencyVisited) {
                return false;
            }
            if (!CheckDependency(dJob)) {
                return false;
            }
        }
    }
    job->m_dependencyVisited = false;
    return true;
}

// Scan the dependency tree and make sure there are no loops
bool CRun::CheckDependencies(void *job) {
    for (JobNodeListIter node = CMainDlg::Singleton().m_jobList.begin(); node != CMainDlg::Singleton().m_jobList.end(); ++node) {
        CJobNode *jobNode = *node;
        if (jobNode->m_selected) {
            jobNode->m_dependencyVisited = false;
        }
    }
    return CheckDependency(job);
}

UINT CRun::CJobsThread() {
    JobNodeListIter node;
    CJobNode *jobNode;
    JobNodeListIter jNode;

    CMainDlg &mainDlg = CMainDlg::Singleton();

    mainDlg.m_snapshotUsed = false;
    DWORD jobCount = 0;
    for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
        jobNode = *node;
        // Just to be safe clear all dependency lists
        if (jobNode->m_selected) {
            jobCount++;
            jobNode->m_waitAfterList.clear();
            jobNode->m_beforeJobs.clear();
            jobNode->m_signalAfterList.clear();
        }
    }
    if (jobCount) {
        if (mainDlg.m_ptl) {
            mainDlg.m_ptl->SetProgressState(mainDlg.m_hWnd, TBPF_INDETERMINATE);
        }
    }
    bool abortFlag = false;

    if (jobCount > 1) {
        for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
            jobNode = *node;
            if (jobNode->m_selected) {
                JobNodeListIter node2;
                CJobNode *jobNode2;
                for (node2 = mainDlg.m_jobList.begin(); node2 != mainDlg.m_jobList.end(); ++node2) {
                    jobNode2 = *node2;
                    if ((jobNode2->m_selected) && (jobNode != jobNode2)) {
                        ConstStrSetIter sIter;
                        for (sIter = jobNode2->m_runAfterList.begin(); sIter != jobNode2->m_runAfterList.end(); sIter++) {
                            if (*sIter == jobNode->m_name) {
                                HANDLE afterEvent = ::CreateEvent(NULL, false, false, NULL);
                                jobNode->m_signalAfterList.push_back(afterEvent);
                                jobNode2->m_waitAfterList.push_back(afterEvent);
                                jobNode->m_beforeJobs.push_back(jobNode2);
                            }
                        }
                    }
                }
            }
        }
        for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
            jobNode = *node;
            if (jobNode->m_selected) {
                if (!CheckDependencies(jobNode)) {
                    mainDlg.InSyncMessageBox(true,
                                             _T("Dependencies contain cycle. Some jobs could never run. Aborting."),
                                             MB_ICONERROR | MB_OK);
                    abortFlag = true;
                    break;
                }
            }
        }
    }
    if (!abortFlag) {
        if ((!InSyncApp.m_adminPriviledge) || IsWow64() || InSyncApp.m_simulate) {
            mainDlg.m_snapshotUsed = false;
        }
        else {
            mainDlg.m_snapshotUsed = jobCount != 0;
        }
        m_vss = new CVssHelper;
        if (mainDlg.m_snapshotUsed) {
            if (!m_vss->Initialize()) {
                mainDlg.InSyncMessageBox(true,
                                         _T("Volume Snapshop support failed to initialize. ")
                                         _T("Volume may not support shadowing or feature has been disabled. ")
                                         _T("Locked files cannot not be copied."),
                                         MB_ICONERROR | MB_OK, IDOK, _T("VSSFailInitWarn"));
            }
            else {
                for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
                    jobNode = *node;
                    if (mainDlg.m_snapshotUsed && jobNode->m_selected) {
                        bool noSnap = true;
                        CSrcNode *srcNode;
                        SrcNodeListIter snode;
                        for (snode = jobNode->m_childrenList.begin(); snode != jobNode->m_childrenList.end(); ++snode) {
                            srcNode = *snode;
                            srcNode->m_parentJob = jobNode;
                            CString name = srcNode->m_name;
                            ReplaceVolumeName(name);
                            if (name[1] == _T(':')) {
                                if (m_vss->AddDriveToSnapshotSet(_totupper(name[0]))) {
                                    if (noSnap) {
                                        noSnap = false;
                                        jobNode->Active(true, _T("Creating snapshot"));
                                    }
                                }
                            }
                            CTrgNode *trgNode;
                            TrgNodeListIter tnode;
                            for (tnode = srcNode->m_childrenList.begin();
                                 tnode != srcNode->m_childrenList.end(); ++tnode) {
                                trgNode = *tnode;
                                trgNode->m_parentSrc = srcNode;
                                trgNode->m_parentJob = jobNode;
                                if (trgNode->m_parentJob->m_mode == TWOWAY_SYNC_MODE) {
                                    name = trgNode->m_name;
                                    ReplaceVolumeName(name);
                                    if (name[1] == _T(':')) {
                                        if (m_vss->AddDriveToSnapshotSet(_totupper(name[0])))
                                            if (noSnap) {
                                                noSnap = false;
                                                CString a(_T("Creating snapshot"));
                                                jobNode->Active(true, a);
                                            }
                                    }
                                }
                            }
                        }
                    }
                }
                if (!m_vss->PrepareForBackup()) {
                    mainDlg.InSyncMessageBox(true,
                                             _T("Volume Snapshop support failed to prepare for backup. ")
                                             _T("Volume may not support shadowing or feature has been disabled. ")
                                             _T("Locked files will not be copied."),
                                             MB_ICONERROR | MB_OK);
                }
            }
        }
    }
    if (jobCount) {
        if (!abortFlag) {
            for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
                jobNode = *node;
                if (jobNode->m_selected) {
                    CSrcNode *srcNode;
                    SrcNodeListIter snode;
                    for (snode = jobNode->m_childrenList.begin(); snode != jobNode->m_childrenList.end(); ++snode) {
                        srcNode = *snode;
                        srcNode->m_parentJob = jobNode;
                        CString name = srcNode->m_name;
                        ReplaceVolumeName(name);
                        mainDlg.ConnectPath(name);
                        CTrgNode *trgNode;
                        TrgNodeListIter tnode;
                        for (tnode = srcNode->m_childrenList.begin();
                             tnode != srcNode->m_childrenList.end(); ++tnode) {
                            trgNode = *tnode;
                            trgNode->m_parentSrc = srcNode;
                            trgNode->m_parentJob = jobNode;
                            name = trgNode->m_name;
                            ReplaceVolumeName(name);
                            mainDlg.ConnectPath(name);
                        }
                    }
                }
            }
            m_jobsActive = true;
            m_poolBuffer = VirtualAlloc(NULL, CMainDlg::Singleton().DefaultMaxThreads() * kMaxThreadBufferSize * 2,
                                        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (m_poolBuffer == NULL) {
                mainDlg.InSyncMessageBox(true,
                                         _T("Could not allocate thread pool buffer. Aborting."),
                                         MB_ICONERROR | MB_OK);
                abortFlag = true;
            }
            else {
                m_threadBuffer = (PCHAR)m_poolBuffer;
                m_threadPool = new CThreadPool<CWorker>;
                if (m_threadPool->Initialize(NULL, CMainDlg::Singleton().DefaultMaxThreads()) != S_OK) {
                    mainDlg.InSyncMessageBox(true,
                                             _T("Could not create thread pool. Aborting."),
                                             MB_ICONERROR | MB_OK);
                    abortFlag = true;
                }
            }
            if (!abortFlag) {
                mainDlg.SetTimer(CMainDlg::MANDALA_TIMER, 150, NULL);
                for (jNode = mainDlg.m_jobList.begin(); jNode != mainDlg.m_jobList.end(); ++jNode) {
                    CJobNode *j = *jNode;
                    if (j->m_selected) {
                        if (j->m_promptBeforeRunning && !mainDlg.m_autoRun && !InSyncApp.m_simulate) {
                            CString msg;
                            msg.Format(_T("Do you really want to run job '%s'"), (LPCTSTR)j->m_name);
                            if (mainDlg.InSyncMessageBox(true, msg, MB_ICONQUESTION | MB_YESNO) == IDNO) {
                                j->m_selected = false;
                                mainDlg.m_jobListCtl.SetSel(j->m_listBoxIndx, false);
                                continue;
                            }
                        }
						j->m_firstAbort = true;
                        j->Start(JobThread);
                    }
				}
				for (jNode = mainDlg.m_jobList.begin(); jNode != mainDlg.m_jobList.end(); ++jNode) {
                    CJobNode *j = *jNode;
                    if (j->m_selected) {
                        ::WaitForSingleObject(j->m_winThread, INFINITE);
                        CloseHandle(j->m_winThread);
                        j->m_winThread = NULL;
                    }
                }
                m_jobsActive = false;
            }
            if (m_poolBuffer) {
                m_threadPool->Shutdown();
                delete m_threadPool;
                VirtualFree(m_poolBuffer, 0, MEM_RELEASE);
            }
        }
        if (jobCount > 1) {
            for (node = mainDlg.m_jobList.begin(); node != mainDlg.m_jobList.end(); ++node) {
                jobNode = *node;
                if (jobNode->m_selected) {
                    HandleListIter hIter;
                    for (hIter = jobNode->m_waitAfterList.begin(); hIter != jobNode->m_waitAfterList.end(); hIter++) {
                        ::CloseHandle(*hIter);
                    }
                    jobNode->m_waitAfterList.clear();
                    jobNode->m_beforeJobs.clear();
                    jobNode->m_signalAfterList.clear();
                }
            }
        }
    }
    m_vss->Finalize();
    delete m_vss;
    mainDlg.PostMessage(WM_USER_JOBS_DONE, InSyncApp.m_simulate, 0);
    return 0;
}

void CRun::ScheduleDirMerge(FileDataList &fromList, FileDataList &toList, const CFilePath &fromDir, const CFilePath &toDir,
                            CTrgNode *trg) {
    if (!CMainDlg::Singleton().m_globalAbort) {
        trg->m_pendingTasks++;
        m_threadPool->QueueRequest(new CMergeTaskWork(fromList, toList, fromDir, toDir, trg));
    }
}

void CRun::ScheduleBackupOrSync(const CFilePath &fromDir, const CFilePath &toDir, const bool fromExists, const bool toExists,
                                CTrgNode *trg) {
    trg->m_pendingTasks++;
    m_threadPool->QueueRequest(new CBackupOrSyncTaskWork(fromDir, toDir, fromExists, toExists, trg));
}

