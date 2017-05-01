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

CExecutable::CExecutable() : m_winThread(NULL) {}

CExecutable::CExecutable(const CExecutable &e) {
    if (this == &e) {
        return;
    }
    m_winThread = NULL;
}

void CExecutable::Start(LPTHREAD_START_ROUTINE proc) {
    DWORD id;
    m_winThread = CreateThread(NULL, 0, proc, this, CREATE_SUSPENDED, &id);
    if (m_winThread != NULL) {
        ::ResumeThread(m_winThread);
    }
}

CExecutableWithInclude::CExecutableWithInclude() {};

CExecutableWithInclude::CExecutableWithInclude(const CExecutableWithInclude &e)
    : CExecutable(e) {
    if (this == &e) {
        return;
    }
    m_include = e.m_include;
    m_exclude = e.m_exclude;
}

CJobNode::CJobNode() :
    CExecutable(),
    m_name(kNullString),
    m_running(false),
    m_selected(false),
    m_logAppend(false),
    m_errPopup(true),
    m_logOnlyError(false),
    m_logFolder(true),
    m_logSkipped(false),
    m_ioVerify(false),
    m_unchangedFileACLs(false),
    m_changedFileACLs(true),
    m_folderACLs(false),
    m_promptBeforeRunning(false),
    m_logFile(INVALID_HANDLE_VALUE),
    m_utf8Buf(NULL),
    m_utf8BufLen(0),
    m_logFlags(kLOGFILE),
    m_listBoxIndx(-1) {
    m_mode = kDefaultJobRunMode;
    m_counts.Clear();
}

CJobNode::CJobNode(const CJobNode &node)
    : CExecutable(node) {
    if (this == &node) {
        return;
    }
    m_name = node.m_name;
    m_selected = node.m_selected;
    m_logAppend = node.m_logAppend;
    m_mode = node.m_mode;
    m_errPopup = node.m_errPopup;
    m_logOnlyError = node.m_logOnlyError;
    m_logFolder = node.m_logFolder;
    m_logSkipped = node.m_logSkipped;
    m_ioVerify = node.m_ioVerify;
    m_unchangedFileACLs = node.m_unchangedFileACLs;
    m_changedFileACLs = node.m_changedFileACLs;
    m_folderACLs = node.m_folderACLs;
    m_promptBeforeRunning = node.m_promptBeforeRunning;
    m_utf8Buf = NULL;
    m_utf8BufLen = node.m_utf8BufLen;
    {
        ConstSrcNodeListIter iter;
        for (iter = node.m_childrenList.begin(); iter != node.m_childrenList.end(); ++iter) {
            CSrcNode *src = new CSrcNode(*(*iter));
            src->m_parentJob = this;
            m_childrenList.push_back(src);
        }
    }
    {
        ConstStrSetIter iter;
        for (iter = node.m_runAfterList.begin(); iter != node.m_runAfterList.end(); ++iter) {
            CString s(*iter);
            m_runAfterList.insert(s);
        }
    }
    m_logName = node.m_logName;
    m_lastLogFileName = node.m_lastLogFileName;
    m_logFlags = node.m_logFlags;
    m_listBoxIndx = node.m_listBoxIndx;
    m_logFile = INVALID_HANDLE_VALUE;
    m_counts.Clear();
}

void CJobNode::Active(const bool a, const CString s) {
    CMainDlg::Singleton().m_jobListCtl.SetRunning(m_listBoxIndx, a, s);
}

DWORD JobThread(LPVOID pParam) {
    UINT rc;
    CJobNode *job = (CJobNode *)pParam;
    rc = job->CJobThread();
    job->Active(false, kNullString);
    HandleListIter hIter;
    for (hIter = job->m_signalAfterList.begin(); hIter != job->m_signalAfterList.end(); hIter++) {
        ::SetEvent(*hIter);
    }
    return rc;
}

UINT CJobNode::CJobThread() {

    Active(true, _T("Waiting..."));

    // Here's where we wait for runafters
    HandleListIter hIter;
    for (hIter = m_waitAfterList.begin(); hIter != m_waitAfterList.end(); hIter++) {
        ::WaitForSingleObject(*hIter, INFINITE);
    }
    if (CMainDlg::Singleton().m_globalAbort) {
        return 0;
    }

    Active(true, InSyncApp.m_simulate ? _T("Simulating") : _T("Running"));
    m_counts.Clear();
    m_logFile = INVALID_HANDLE_VALUE;
    if (m_logName.GetLength()) {
        CreateLogFile(m_logAppend);
        if (m_logFile == INVALID_HANDLE_VALUE) {
            FatalTaskError(true, _T("cannot open log file"), m_name);
            return 1;
        }
    }
    time_t start;
    time(&start);
    TCHAR sTime[64];
    struct tm t;
    localtime_s(&t, &start);
    _tcsftime(sTime, (size_t)_countof(sTime), _T("%#c"), &t);
    {
        CScopedLock lock(&m_logLock);
        m_logFlags = kLOGALWAYS;
        CString v = InSyncApp.m_insyncVersion;
        WriteToLogVSNewLine(_T("Dillobits Software, InSync version "), v.GetBuffer(), 0);
        WriteToLog(_T("Starting "));
        if (InSyncApp.m_simulate) {
            WriteToLog(_T("simulation"));
        }
        else {
            WriteToLog(_T("execution"));
        }
        CString n = m_name;
        WriteToLogVS(_T(" of Job: "), n.GetBuffer(), NULL);
        if (InSyncApp.m_optFileNameFromConfig) {
            WriteToLogVS(_T(", from config file: "), (LPCTSTR)InSyncApp.m_optFilename, NULL);
        }
        WriteToLogVS(_T(", starting time: "), sTime, _T("\r\n\r\n"), NULL);
    }
    if (m_childrenList.size()) {
        SrcNodeListIter node;
        for (node = m_childrenList.begin(); node != m_childrenList.end(); ++node) {
            (*node)->m_parentJob = this;
            (*node)->Start(SrcThread);
        }
        for (node = m_childrenList.begin(); node != m_childrenList.end(); ++node) {
            CSrcNode *src = *node;
            if (src->m_winThread) {
                ::WaitForSingleObject(src->m_winThread, INFINITE);
                CloseHandle(src->m_winThread);
                src->m_winThread = NULL;
            }
        }
    }

    time_t end;
    time(&end);
    localtime_s(&t, &end);
    _tcsftime(sTime, (size_t)_countof(sTime), _T("%#c"), &t);
    end = end - start;
    TCHAR eTime[64];
    gmtime_s(&t, &end);
    _tcsftime(eTime, (size_t)_countof(eTime), _T("%H:%M:%S"), &t);
    {
        CScopedLock lock(&m_logLock);
        m_logFlags = kLOGALWAYS;
        CString n = m_name;
        WriteToLogVSNewLine(_T("\r\nEnding Job: "), n.GetBuffer(), NULL);
        WriteCountsToLog();
        if (m_counts.m_fileErrorCount) {
            TCHAR errors[64];
            _itot_s((int)m_counts.m_fileErrorCount, errors, 10);
            WriteToLogVSNewLine(_T("Total number of errors: "), errors, NULL);
        }
        WriteToLogVSNewLine(_T("Ending time: "), sTime, NULL);
        WriteToLogVS(_T("Elapsed time: "), eTime, _T("\r\n\r\n"), NULL);
    }
    CloseLogFile();
    return 0;
}

void CJobNode::WriteToLog(LPCTSTR text) {
    DWORD written;

    if ((m_logFile != INVALID_HANDLE_VALUE) &&
        ((!m_logFlags.LogFlagFolder || m_logFolder) &&
         (m_logFlags.LogFlagError || !m_logOnlyError) &&
         (!m_logFlags.LogFlagSkipped || m_logSkipped))) {
        DWORD buf_len;
        do {
            buf_len =
                (DWORD)WideCharToMultiByte(CP_UTF8, 0, text, (int)_tcslen(text), m_utf8Buf, (int)m_utf8BufLen, 0, 0);
            if (buf_len == 0) {
                if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                    delete [] m_utf8Buf;
                    m_utf8BufLen *= 2;
                    m_utf8Buf = new char[m_utf8BufLen];
                }
                else {
                    return;
                }
            }
        }
        while (buf_len == 0);
        ::WriteFile(m_logFile, m_utf8Buf, buf_len, &written, NULL);
    }
}

void CJobNode::WriteToLogVS(LPCTSTR first, ...) {
    LPTSTR s;
    va_list marker;
    WriteToLog(first);
    va_start(marker, first);     /* Initialize variable arguments. */
    s = va_arg(marker, LPTSTR);
    while (s) {
        WriteToLog(s);
        s = va_arg(marker, LPTSTR);
    }
}

void CJobNode::WriteCountsToLog() {
    TCHAR temp[48], templ[48];
    FormatCount(m_counts.m_filesSkippedCount, false, false, temp, _countof(temp), NULL);
    FormatCount(m_counts.m_bytesSkippedCount, true, true, templ, _countof(templ), _T("bytes"));
    WriteToLogVS(_T("Files skipped: "), temp, _T(" ("), templ, _T(")"), NULL);
    FormatCount(m_counts.m_filesCopiedCount, false, false, temp, _countof(temp), NULL);
    FormatCount(m_counts.m_bytesCopiedCount, true, true, templ, _countof(templ), _T("bytes"));
    WriteToLogVS(_T(", created: "), temp, _T(" ("), templ, _T(")"), NULL);
    FormatCount(m_counts.m_filesUpdatedCount, false, false, temp, _countof(temp), NULL);
    FormatCount(m_counts.m_bytesUpdatedCount, true, true, templ, _countof(templ), _T("bytes"));
    WriteToLogVS(_T(", updated: "), temp, _T(" ("), templ, _T(")"), NULL);
    FormatCount(m_counts.m_filesDeletedCount, false, false, temp, _countof(temp), NULL);
    FormatCount(m_counts.m_bytesDeletedCount, true, true, templ, _countof(templ), _T("bytes"));
    WriteToLogVSNewLine(_T(", removed: "), temp, _T(" ("), templ, _T(")"), NULL);
    FormatCount(m_counts.m_foldersCopiedCount, false, false, temp, _countof(temp), NULL);
    WriteToLogVS(_T("Folders added: "), temp, NULL);
    FormatCount(m_counts.m_foldersDeletedCount, false, false, temp, _countof(temp), NULL);
    WriteToLogVSNewLine(_T(", removed: "), temp, NULL);
}

void CJobNode::WriteToLogVSNewLine(LPCTSTR first, ...) {
    LPTSTR s;
    va_list marker;
    WriteToLog(first);
    va_start(marker, first);     /* Initialize variable arguments. */
    s = va_arg(marker, LPTSTR);
    while (s) {
        WriteToLog(s);
        s = va_arg(marker, LPTSTR);
    }
    WriteToLog(_T("\r\n"));
}

static UCHAR BOM[] = { (UCHAR)0xEF, (UCHAR)0xBB, (UCHAR)0xBF };

void CJobNode::CreateLogFile(const bool append) {
    LPCTSTR ip = m_logName;
    CString fn;
    SYSTEMTIME st;
    GetLocalTime(&st);
    while (*ip) {
        if (*ip == _T('%')) {
            switch (*(ip + 1)) {
            case 'd':
            case 'D': {
                TCHAR o[16];
                _stprintf_s(o, _T("%04hu%02hu%02hu"), st.wYear, st.wMonth, st.wDay);
                fn += o;
                ip += 2;
                break;
            }
            case 't':
            case 'T': {
                TCHAR o[16];
                _stprintf_s(o, _T("%02hu%02hu%02hu"), st.wHour, st.wMinute, st.wSecond);
                fn += o;
                ip += 2;
                break;
            }
            case 'x':
            case 'X': {
                if (InSyncApp.m_simulate) {
                    fn += _T(".Simulated");
                }
                ip += 2;
                break;
            }
            default:
                ip++;
                break;
            }
        }
        else if (*ip == _T('<')) {
            LPCTSTR ip2 = ++ip;
            while (*ip2) {
                if (*ip2 == _T('>')) {
                    break;
                }
                ip2++;
            }
            if (*ip2 == _T('>')) {
                CString v;
                while (ip != ip2) {
                    v += *ip++;
                }
                if ((v == _T("d")) || (v == _T("D"))) {
                    TCHAR o[16];
                    _stprintf_s(o, _T("%04hu%02hu%02hu"), st.wYear, st.wMonth, st.wDay);
                    fn += o;
                }
                else if ((v == _T("t")) || (v == _T("T"))) {
                    TCHAR o[16];
                    _stprintf_s(o, _T("%02hu%02hu%02hu"), st.wHour, st.wMinute, st.wSecond);
                    fn += o;
                }
                else if ((v == _T("x")) || (v == _T("X"))) {
                    if (InSyncApp.m_simulate) {
                        fn += _T(".Simulated");
                    }
                }
                else {
                    CString val;
                    if (val.GetEnvironmentVariable(v.MakeUpper())) {
                        fn += val;
                    }
                }

                ip = ip2 + 1;
            }
        }
        else {
            fn += *ip++;
        }
    }
    ReplaceVolumeName(fn);
    bool appending = false;
    m_logFile = ::CreateFile(fn, GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,
                             append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    m_lastLogFileName = kNullString;
    if (m_logFile != INVALID_HANDLE_VALUE) {
        if (append) {
            ::SetFilePointer(m_logFile, 0, NULL, FILE_END);
            DWORD high;
            appending = (::GetFileSize(m_logFile, &high) != 0) || (high != 0);
        }
        m_lastLogFileName = fn;
        m_utf8BufLen = 256;
        m_utf8Buf = new char[m_utf8BufLen];
    }
    DWORD written;
    if (m_logFile != INVALID_HANDLE_VALUE) {
        if (!appending) {
            ::WriteFile(m_logFile, BOM, sizeof(BOM), &written, NULL);
        }
    }
}

void CJobNode::CloseLogFile() {
    if (m_logFile != INVALID_HANDLE_VALUE) {
        ::CloseHandle(m_logFile);
        m_logFile = INVALID_HANDLE_VALUE;
    }
    delete [] m_utf8Buf;
    m_utf8Buf = NULL;
}

void CJobNode::TaskError(const bool showOSRc, LPCTSTR msg, LPCTSTR severity, LPCTSTR job, LPCTSTR src, LPCTSTR trg) {
    LPTSTR eMsg = NULL;
    if (showOSRc)
        if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, ::GetLastError(),
                             MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0, NULL)) {
            eMsg = 0;
        }
    CString fatal(severity);
    fatal += _T(" error. Job ");
    fatal += job;
    if (src) {
        fatal += _T(", Source ");
        fatal += src;
    }
    if (trg) {
        fatal += _T(", Target ");
        fatal += trg;
    }
    fatal += _T(" - Warning: ");
    fatal += msg;
    if (showOSRc) {
        if (eMsg) {
            fatal += _T(" - OS Return code: ");
            fatal += eMsg;
            ::LocalFree(eMsg);
        }
    }
    fatal += _T("\r\n");
    WriteToLog(fatal);
    CMainDlg::Singleton().InSyncMessageBox(true, fatal, MB_ICONEXCLAMATION | MB_OK);
}

void CJobNode::FatalTaskError(const bool showOSRc, LPCTSTR msg, LPCTSTR job, LPCTSTR src, LPCTSTR trg) {
    TaskError(showOSRc, msg, _T("Fatal job"), job, src, trg);
}

void CJobNode::NonFatalTaskError(const bool showOSRc, LPCTSTR msg, LPCTSTR job, LPCTSTR src, LPCTSTR trg) {
    TaskError(showOSRc, msg, _T("Job"), job, src, trg);
}

bool CJobNode::operator ==(const CJobNode &other) const {
    if (this == &other) {
        return true;
    }
    return this->m_name == other.m_name;
}

CJobNode::~CJobNode() {
    SrcNodeListIter iter;
    for (iter = m_childrenList.begin(); iter != m_childrenList.end(); ++iter) {
        CSrcNode *src = *iter;
        delete src;
    }
    m_runAfterList.clear();
    if (m_utf8Buf) {
        delete [] m_utf8Buf;
    }
}

