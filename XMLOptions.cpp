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

#include "StdAfx.h"

LPCTSTR ALLJOBSKEY = _T("Jobs");
LPCTSTR COMPAREFOLDERSECURITYKEY = _T("CompareFolderSecurity");
LPCTSTR COMPARENEWSECURITYKEY = _T("CompareNewSecurity");
LPCTSTR COMPARESECURITYKEY = _T("CompareSecurity");
LPCTSTR DONTDELETEKEY = _T("DontDelete");
LPCTSTR ERRORPOPUPKEY = _T("ErrorPopup");
LPCTSTR EXCLUDEDIRKEY = _T("ExcludeFolder");
LPCTSTR EXCLUDEKEY = _T("Exclude");
LPCTSTR INCLUDEKEY = _T("Include");
LPCTSTR INSYNCVERSIONATTR = _T("Version");
LPCTSTR JOBKEY = _T("Job");
LPCTSTR LOGAPPENDKEY = _T("LogAppend");
LPCTSTR LOGERRORSKEY = _T("LogErrors");
LPCTSTR LOGFILEKEY = _T("LogFile");
LPCTSTR LOGSKIPPEDKEY = _T("LogSkipped");
LPCTSTR MAXTHREADSKEY = _T("MaxThreads");
LPCTSTR MAXTHREADKEY = _T("MaxThread");
LPCTSTR NOLOGSYNCDIRKEY = _T("NoLogSyncDir");
LPCTSTR PROMPTBEFORERUNNINGKEY = _T("PromptBeforeRunning");
LPCTSTR RUNAFTERKEY = _T("RunAfter");
LPCTSTR RUNAFTERLISTKEY = _T("RunAfterList");
LPCTSTR RUNMODEKEY = _T("Mode");
LPCTSTR SRCKEY = _T("Source");
LPCTSTR SUBDIRSKEY = _T("SubDirs");
LPCTSTR TIPSOFFKEY = _T("TipsOff");
LPCTSTR TRGKEY = _T("Target");
LPCTSTR USEVOLKEY = _T("UseVolumeNames");
LPCTSTR VERIFYKEY = _T("Verify");


bool CXmlOptions::LoadXMLOptions() {
    CXml xml;
    if (!xml.Open(m_filePath)) {
        m_errorMessage = "Error  opening InSync XML file";
        return false;
    }
    CXmlNode all = xml.GetRoot();
    if (all.GetName() != "InSync") {
        m_errorMessage = "XML file not for InSync";
        return false;
    }
    m_version = all.GetAttribute(INSYNCVERSIONATTR);
    if (m_version.GetLength() == 0) {
        m_version = _T("0.0.0");
    }

    CXmlNode jobs = all.GetChild(ALLJOBSKEY);
    if (jobs.IsNull()) {
        m_errorMessage = "XML file not contain jobs definitions";
        return false;
    }
    CMainDlg::Singleton().m_tipsOff = jobs.GetChild(TIPSOFFKEY)->GetValue(false);
    CXmlNode max = jobs.GetChild(MAXTHREADSKEY);
    WORD maxThreads;
    if (!max.IsNull()) {
        maxThreads = (WORD)jobs.GetChild(MAXTHREADSKEY)->GetValue((DWORD)0);
        if (maxThreads) {
            maxThreads = (1 << (maxThreads - 1));
        }
        if (maxThreads < 1) {
            maxThreads = 1;
        }
    }
    else {
        maxThreads = (WORD)jobs.GetChild(MAXTHREADKEY)->GetValue((DWORD)0);
    }
    CMainDlg::Singleton().MaxThreads(maxThreads);
    CXmlNodesPtr allJobs = jobs.GetChildren();
    for (long j = 0; j < allJobs->GetCount(); j++) {
        CXmlNode job = *allJobs->GetItem(j);
        if (job.GetName() == JOBKEY) {
            CString jobName = job.GetAttribute(_T("Name"));
            if (jobName.GetLength()) {
                int listIndx = CMainDlg::Singleton().m_jobListCtl.AddString(jobName);
                CJobNode *jobNode = new CJobNode;
                CMainDlg::Singleton().m_jobList.push_back(jobNode);
                jobNode->m_name = jobName;
                if (listIndx != LB_ERR) {
                    CMainDlg::Singleton().m_jobListCtl.SetItemDataPtr(listIndx, jobNode);
                }
                jobNode->m_logName = (CString)job.GetChild(LOGFILEKEY)->GetValue(_T(""));
                jobNode->m_errPopup = !job.GetChild(ERRORPOPUPKEY)->GetValue(false);
                jobNode->m_logOnlyError = job.GetChild(LOGERRORSKEY)->GetValue(false);
                jobNode->m_unchangedFileACLs = job.GetChild(COMPARESECURITYKEY)->GetValue(false);
                jobNode->m_folderACLs = job.GetChild(COMPAREFOLDERSECURITYKEY)->GetValue(false);
                jobNode->m_promptBeforeRunning = job.GetChild(PROMPTBEFORERUNNINGKEY)->GetValue(false);
                jobNode->m_changedFileACLs = job.GetChild(COMPARENEWSECURITYKEY)->GetValue(true);
                jobNode->m_logFolder = !job.GetChild(NOLOGSYNCDIRKEY)->GetValue(false);
                jobNode->m_logSkipped = job.GetChild(LOGSKIPPEDKEY)->GetValue(false);
                jobNode->m_logAppend = job.GetChild(LOGAPPENDKEY)->GetValue(false);
                jobNode->m_ioVerify = job.GetChild(VERIFYKEY)->GetValue(false);
                jobNode->m_mode = (RunModes)(job.GetChild(RUNMODEKEY)->GetValue((DWORD)0) + 1);

                CXmlNode afterList = job.GetChild(RUNAFTERLISTKEY);
                CXmlNodesPtr allRunAfter = afterList.GetChildren();
                for (long r = 0; r < allRunAfter->GetCount(); r++) {
                    CXmlNode runAfter = allRunAfter->GetItem(r);
                    if (runAfter.GetName() == RUNAFTERKEY) {
                        CString name = runAfter.GetAttribute(_T("Name"));
                        jobNode->m_runAfterList.insert(name);
                    }
                }
                CXmlNodesPtr allSrcs = job.GetChildren();
                for (long s = 0; s < allSrcs->GetCount(); s++) {
                    CXmlNode src = allSrcs->GetItem(s);
                    if (src.GetName() == SRCKEY) {
                        CString srcName = src.GetAttribute(_T("Name"));
                        if (srcName.GetLength()) {
                            CSrcNode *srcNode = new CSrcNode(jobNode);
                            jobNode->m_childrenList.push_back(srcNode);
                            srcNode->m_name = srcName;
                            srcNode->m_include = src.GetChild(INCLUDEKEY)->GetValue(_T(""));
                            srcNode->m_exclude = src.GetChild(EXCLUDEKEY)->GetValue(_T(""));
                            srcNode->m_subdir = src.GetChild(SUBDIRSKEY)->GetValue(false);
                            srcNode->m_useVolume = src.GetChild(USEVOLKEY)->GetValue(false);
                            CString n(srcNode->m_name);
                            if (srcNode->m_useVolume) {
                                ReplaceDriveLetter(n);
                            }
                            else {
                                ReplaceVolumeName(n);
                            }
                            srcNode->m_name = n;
                            CXmlNodesPtr srcChildren = src.GetChildren();
                            for (long e = 0; e < srcChildren->GetCount(); e++) {
                                CXmlNode node = srcChildren->GetItem(e);
                                CString nodeName = node.GetName();
                                if (nodeName == EXCLUDEDIRKEY) {
                                    CString exclName = node.GetAttribute(_T("Name"));
                                    if (exclName.GetLength()) {
                                        srcNode->m_excludedSrcDirList.insert(exclName);
                                    }
                                }
                                else if (nodeName == TRGKEY) {
                                    CString trgName = node.GetAttribute(_T("Name"));
                                    if (trgName.GetLength()) {
                                        CTrgNode *trgNode = new CTrgNode(jobNode, srcNode);
                                        srcNode->m_childrenList.push_back(trgNode);
                                        trgNode->m_name = trgName;
                                        trgNode->m_useVolume = node.GetChild(USEVOLKEY)->GetValue(false);
                                        trgNode->m_dontDelete = node.GetChild(DONTDELETEKEY)->GetValue(true);
                                        trgNode->m_include = node.GetChild(INCLUDEKEY)->GetValue(_T(""));
                                        trgNode->m_exclude = node.GetChild(EXCLUDEKEY)->GetValue(_T(""));
                                        CString name(trgNode->m_name);
                                        if (trgNode->m_useVolume) {
                                            ReplaceDriveLetter(name);
                                        }
                                        else {
                                            ReplaceVolumeName(name);
                                        }
                                        trgNode->m_name = name;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}



bool CXmlOptions::SaveXMLOptions() {
    CXml xml;

    // To create the xml file by CXml::Create
    if (!xml.Create(_T("InSync"))) {
        m_errorMessage = "can't create XML instance";
        return false;
    }
    CXmlNode root = xml.GetRoot();
    if (root.GetName() != "InSync") {
        m_errorMessage = "can't create XML instance";
        return false;
    }

    root.SetAttribute(INSYNCVERSIONATTR, InSyncApp.m_insyncVersion);

    CXmlNode jobs = root.NewChild(ALLJOBSKEY);

#if !defined(_DEBUG)
    TCHAR fn[MAX_PATH];
    ::GetModuleFileName(NULL, fn, _countof(fn));
#endif

    if (CMainDlg::Singleton().m_tipsOff) {
        jobs.NewChild(TIPSOFFKEY)->SetValue(CMainDlg::Singleton().m_tipsOff);
    }
    if (CMainDlg::Singleton().MaxThreads()) {
        jobs.NewChild(MAXTHREADKEY)->SetValue((DWORD)CMainDlg::Singleton().MaxThreads());
    }

    for (JobNodeListIter jobNode = CMainDlg::Singleton().m_jobList.begin(); jobNode != CMainDlg::Singleton().m_jobList.end();
         ++jobNode) {
        CXmlNode job = jobs.NewChild(JOBKEY);
        job.SetAttribute(_T("Name"), (*jobNode)->m_name);
        if ((*jobNode)->m_logName.GetLength()) {
            job.NewChild(LOGFILEKEY)->SetValue((*jobNode)->m_logName);
        }
        if (!(*jobNode)->m_errPopup) {
            job.NewChild(ERRORPOPUPKEY)->SetValue(!(*jobNode)->m_errPopup);
        }
        if ((*jobNode)->m_logOnlyError) {
            job.NewChild(LOGERRORSKEY)->SetValue((*jobNode)->m_logOnlyError);
        }
        if ((*jobNode)->m_unchangedFileACLs) {
            job.NewChild(COMPARESECURITYKEY)->SetValue((*jobNode)->m_unchangedFileACLs);
        }
        if (!(*jobNode)->m_changedFileACLs) {
            job.NewChild(COMPARENEWSECURITYKEY)->SetValue((*jobNode)->m_changedFileACLs);
        }
        if ((*jobNode)->m_folderACLs) {
            job.NewChild(COMPAREFOLDERSECURITYKEY)->SetValue((*jobNode)->m_folderACLs);
        }
        if ((*jobNode)->m_promptBeforeRunning) {
            job.NewChild(PROMPTBEFORERUNNINGKEY)->SetValue((*jobNode)->m_promptBeforeRunning);
        }
        if (!(*jobNode)->m_logFolder) {
            job.NewChild(NOLOGSYNCDIRKEY)->SetValue(!(*jobNode)->m_logFolder);
        }
        if ((*jobNode)->m_logSkipped) {
            job.NewChild(LOGSKIPPEDKEY)->SetValue((*jobNode)->m_logSkipped);
        }
        if ((*jobNode)->m_logAppend) {
            job.NewChild(LOGAPPENDKEY)->SetValue((*jobNode)->m_logAppend);
        }
        if ((*jobNode)->m_ioVerify) {
            job.NewChild(VERIFYKEY)->SetValue((*jobNode)->m_ioVerify);
        }
        if ((*jobNode)->m_mode > 1) {
            job.NewChild(RUNMODEKEY)->SetValue((DWORD)((*jobNode)->m_mode - 1));
        }
        if ((*jobNode)->m_runAfterList.size()) {
            CXmlNode runList = job.NewChild(RUNAFTERLISTKEY);
            for (ConstStrSetIter runIter = (*jobNode)->m_runAfterList.begin(); runIter != (*jobNode)->m_runAfterList.end(); ++runIter) {
                CXmlNode runAfter = runList.NewChild(RUNAFTERKEY);
                runAfter.SetAttribute(_T("Name"), *runIter);
            }
        }

        for (SrcNodeListIter srcNode = (*jobNode)->m_childrenList.begin(); srcNode != (*jobNode)->m_childrenList.end(); ++srcNode) {
            CXmlNode src = job.NewChild(SRCKEY);
            src.SetAttribute(_T("Name"), (*srcNode)->m_name);
            if (((*srcNode)->m_include).GetLength()) {
                src.NewChild(INCLUDEKEY)->SetValue((*srcNode)->m_include);
            }
            if (((*srcNode)->m_exclude).GetLength()) {
                src.NewChild(EXCLUDEKEY)->SetValue((*srcNode)->m_exclude);
            }
            if ((*srcNode)->m_subdir) {
                src.NewChild(SUBDIRSKEY)->SetValue((*srcNode)->m_subdir);
            }
            if ((*srcNode)->m_useVolume) {
                src.NewChild(USEVOLKEY)->SetValue((*srcNode)->m_useVolume);
            }

            for (ConstStrSetIter exclNode = (*srcNode)->m_excludedSrcDirList.begin();
                 exclNode != (*srcNode)->m_excludedSrcDirList.end();
                 ++exclNode) {
                CXmlNode excl = src.NewChild(EXCLUDEDIRKEY);
                excl.SetAttribute(_T("Name"), *exclNode);
            }
            for (TrgNodeListIter trgNode = (*srcNode)->m_childrenList.begin();
                 trgNode != (*srcNode)->m_childrenList.end(); ++trgNode) {
                CXmlNode trg = src.NewChild(TRGKEY);
                trg.SetAttribute(_T("Name"), (*trgNode)->m_name);
                if ((*trgNode)->m_include.GetLength()) {
                    trg.NewChild(INCLUDEKEY)->SetValue(((*trgNode)->m_include));
                }
                if ((*trgNode)->m_exclude.GetLength()) {
                    trg.NewChild(EXCLUDEKEY)->SetValue(((*trgNode)->m_exclude));
                }
                if ((*trgNode)->m_useVolume) {
                    trg.NewChild(USEVOLKEY)->SetValue((*trgNode)->m_useVolume);
                }
                if (!(*trgNode)->m_dontDelete) {
                    trg.NewChild(DONTDELETEKEY)->SetValue((*trgNode)->m_dontDelete);
                }
            }
        }
    }

    if (!xml.SaveWithFormatted(m_filePath)) {
        m_errorMessage = "can't save XML";
        return false;
    }

    return true;
}

CString CXmlOptions::GetLastError() {
    return m_errorMessage;
}

CString CXmlOptions::GetVersion() {
    return m_version;
}

void CXmlOptions::SetFilePath(const CString path) {
    m_filePath = path;
}

void CXmlOptions::SetVersion(const CString version) {
    m_version = version;
}

