#include "stdafx.h"

DWORD TrgThread(LPVOID pParam)
{
    return ((CTrgNode *)pParam)->CTrgThread();
}

CTrgNode::CTrgNode(CJobNode *parentJob, CSrcNode *parentSrc) :
    m_fatal(NULL),
    m_effectiveMode(kDefaultJobRunMode),
    m_subDir(true),
    m_simulate(false),
    m_threadVerify(false),
    m_supportsPermissions(false),
    m_useVolume(false),
    m_dontDelete(true),
    m_includes(NULL),
    m_excludes(NULL),
    m_reasonCode(0)
{
    m_parentJob = parentJob;
    m_parentSrc = parentSrc;
    m_run = &CRun::Singleton();
}

CTrgNode::CTrgNode(const CTrgNode &node) :
    CExecutableWithInclude(node)
{
    if (&node == this) {
        return;
    }

    m_name = node.m_name;
    m_fatal = NULL;
    m_parentJob = node.m_parentJob;
    m_parentSrc = node.m_parentSrc;
    m_subDir = node.m_subDir;
    m_simulate = node.m_simulate;
    m_threadVerify = node.m_threadVerify;
    m_useVolume = node.m_useVolume;
    m_dontDelete = node.m_dontDelete;
    m_supportsPermissions = false;
    m_includes = NULL;
    m_excludes = NULL;
    m_reasonCode = 0;
    m_run = node.m_run;
}

bool CTrgNode::operator ==(const CTrgNode &n) const
{
    if (this == &n) {
        return true;
    }

    return this->m_name == n.m_name;
}

int CTrgNode::GetReasonCode()
{
    int r = m_reasonCode;
    m_reasonCode = 0;
    return r;
}

UINT CTrgNode::CTrgThread()
{
    m_fileErrors = 0;

    m_threadVerify = m_parentJob->m_ioVerify;

    CString incl = m_parentSrc->m_include;

    if (m_include.GetLength()) {
        incl = m_include;
    }

    CString excl = m_parentSrc->m_exclude;

    if (m_exclude.GetLength()) {
        excl = m_exclude;
    }

    bool errPopup = m_parentJob->m_errPopup;
    m_fatal = NULL;
    m_simulate = InSyncApp.m_simulate;
    m_subDir = m_parentSrc->m_subdir;
    m_excludedFolders.clear();
    m_effectiveMode = m_parentJob->m_mode;

    switch (m_effectiveMode) {
    case MIRROR_SYNC_MODE:
        FileInBoth = &CTrgNode::MirrorFileInBoth;
        DirInBoth = &CTrgNode::MirrorDirInBoth;
        break;

    case TWOWAY_SYNC_MODE:
        FileInBoth = &CTrgNode::TwoWayFileInBoth;
        DirInBoth = &CTrgNode::TwoWayDirInBoth;
        break;

    case BACKUP_SYNC_MODE:
        FileInBoth = &CTrgNode::BackupFileInBoth;
        DirInBoth = &CTrgNode::BackupDirInBoth;
        break;
    }

    CString parentName(m_parentSrc->m_name);
    CString name(m_name);
    bool fromFound, toFound;
    CString fromMsg, toMsg;
    fromFound = ReplaceVolumeName(parentName, &fromMsg);
    toFound = ReplaceVolumeName(name, &toMsg);
    CString m;
    m.Format(_T("Starting %s thread \"%s\" -> \"%s\""), (LPCTSTR)kRunModeNames[m_effectiveMode],
             (LPCTSTR)m_parentSrc->m_name, (LPCTSTR)m_name);
    ConstStrSetIter f;
    CString exclMsg;
    bool someExcl = false;

    for (f = m_parentSrc->m_excludedSrcDirList.begin(); f != m_parentSrc->m_excludedSrcDirList.end();
         ++f) {
        CString s(*f);
        someExcl = true;
        exclMsg += _T(" ");
        exclMsg += s;
        ReplaceVolumeName(s);
        m_excludedFolders.insert(s);
    }

    {
        CTrgScopedLogLock lock(this, kLOGALWAYS);
        m_parentJob->WriteToLogVSNewLine(m, NULL);

        if (fromMsg.GetLength()) {
            m_parentJob->WriteToLogVSNewLine(fromMsg, NULL);
        }

        if (toMsg.GetLength()) {
            m_parentJob->WriteToLogVSNewLine(toMsg, NULL);
        }

        if (someExcl) {
            exclMsg = CString(_T("Excluded folders:")) + exclMsg;
            m_parentJob->WriteToLogVSNewLine(exclMsg, NULL);
            exclMsg = kNullString;
        }
    }

    do {
        ::SetLastError(ERROR_NOT_READY);

        if (!fromFound) {
            FatalError(_T("Unable to find source folder volume"));
            break;
        }

        if (!toFound) {
            FatalError(_T("Unable to find target folder volume"));
            break;
        }

        ::SetLastError(ERROR_SUCCESS);
        CFilePath toDir(name), fromDir(parentName);

        if (incl)
            if (_tcslen(incl)) {
                m_includes = ParseIncludeExcludeString((LPCTSTR)incl);

                if (m_includes == NULL) {
                    CString msg;
                    msg.Format(_T("Invalid include specification \"%s\" encountered. Include specification ignored."),
                               (LPCTSTR)incl);
                    CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                }
            }

        if (excl)
            if (_tcslen(excl)) {
                m_excludes = ParseIncludeExcludeString((LPCTSTR)excl);

                if (m_excludes == NULL) {
                    CString msg;
                    msg.Format(_T("Invalid exclude specification \"%s\" encountered. Exclude specification ignored."),
                               (LPCTSTR)excl);
                    CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                }
            }

        bool toExists = true;

        if (!PathIsDirectory(toDir.UNC())) {
            if (m_simulate) {
                toExists = false;
            } else {
                // Try to create target folder
                if (!::CreateDirectory(toDir.UNC(), NULL)) {
                    FatalError(_T("Unable to find or create target folder"));
                    break;
                }
            }
        }

        bool fromExists = true;

        if (!PathIsDirectory(fromDir.UNC())) {
            if (m_simulate) {
                fromExists = false;
            } else {
                FatalError(_T("Unable to find source folder"));
                break;
            }
        }

        m_supportsPermissions = SupportsPermissions(fromDir.UNC()) && SupportsPermissions(toDir.UNC());

        if (!ExcludedFolder(fromDir)) {
            m_doneEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

            if (m_doneEvent) {
                m_pendingTasks = 0;
                CRun::Singleton().ScheduleBackupOrSync(fromDir, toDir, fromExists, toExists, this);
                ::WaitForSingleObject(m_doneEvent, INFINITE);
                CloseHandle(m_doneEvent);
                m_doneEvent = NULL;
            } else {
                FatalError(_T("Unable to create termination event"));
                break;
            }
        }

        m_excludedFolders.clear();

        if (m_includes) {
            delete m_includes;
        }

        if (m_excludes) {
            delete m_excludes;
        }
    } while (false);

    if (errPopup && m_fatal) {
        InSyncApp.AppRC(kINSYNC_JOB_ERROR);
        CMainDlg::Singleton().InSyncMessageBox(false, m_fatal, MB_ICONERROR | MB_OK);
    }

    if (m_fatal) {
        delete [] m_fatal;
        m_fatal = NULL;
    }

    if (m_fileErrors) {
        InSyncApp.AppRC(kINSYNC_FILE_ERROR);

        if (errPopup) {
            m_parentJob->NonFatalTaskError(false, _T("File errors occured"), m_parentJob->m_name, parentName,
                                           name);
        }
    }

    return 0;
}

bool CTrgNode::DeleteAFile(LPCTSTR file, LPCTSTR msg)
{
    if (!m_simulate) {
        if (!::DeleteFile(file)) {
            SetReasonCode(1);
            FileError(_T("deleting"), msg, true);
            return false;
        }
    }

    return true;
}

bool CTrgNode::UnprotectAFile(LPCTSTR file, const DWORD attr, LPCTSTR msg)
{
    if (!m_simulate) {
        if (attr & FILE_ATTRIBUTE_READONLY) {
            DWORD a = attr & (~FILE_ATTRIBUTE_READONLY);

            if (a == 0) {
                a = FILE_ATTRIBUTE_NORMAL;
            }

            if (!::SetFileAttributes(file, a & kFILE_ATTRIBUTE_SETABLE)) {
                SetReasonCode(2);
                FileError(_T("setting attributes"), msg, true);
                return false;
            }
        }
    }

    return true;
}

void CTrgNode::FatalError(LPCTSTR msg)
{
    LPVOID eMsg;

    if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                         ::GetLastError(),
                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0, NULL)) {
        eMsg = 0;
    }

    LPCTSTR eeMsg;

    if (eMsg) {
        eeMsg = (LPTSTR)eMsg;
    } else {
        eeMsg = kNullString;
    }

    size_t l = _tcslen(_T("Job ")) + _tcslen(m_parentJob->m_name) +
               _tcslen(_T(", Source ")) + _tcslen(m_parentSrc->m_name) +
               _tcslen(_T(", Destination ")) + _tcslen(m_name) +
               _tcslen(_T(" - Fatal thread error: ")) + _tcslen(msg) +
               _tcslen(_T(" - OS Return code: ")) + _tcslen(eeMsg) + 1;
    m_fatal = new TCHAR[l];
    {
        CTrgScopedLogLock lock(this, kLOGALWAYS);
        _tcscpy_s(m_fatal, l, _T("Job "));
        _tcscat_s(m_fatal, l, m_parentJob->m_name);
        _tcscat_s(m_fatal, l, _T(", Source "));
        _tcscat_s(m_fatal, l, m_parentSrc->m_name);
        _tcscat_s(m_fatal, l, _T(", Destination "));
        _tcscat_s(m_fatal, l, m_name);
        _tcscat_s(m_fatal, l, _T(" - Fatal error: "));
        _tcscat_s(m_fatal, l, msg);
        _tcscat_s(m_fatal, l, _T(" - OS Return code: "));
        _tcscat_s(m_fatal, l, eeMsg);
        m_parentJob->WriteToLogVSNewLine(m_fatal, NULL);
    }
    delete [] m_fatal;
    m_fatal = NULL;

    if (eMsg) {
        ::LocalFree(eMsg);
    }

    CMainDlg::Singleton().PostMessage(WM_USER_SET_LED, CMainDlg::LED_RED);
}

bool CTrgNode::AbortedByUser()
{
    if (CMainDlg::Singleton().m_globalAbort) {
        if (m_parentJob->m_firstAbort) {
            m_parentJob->m_firstAbort = false;
            size_t l = _tcslen(_T("Job ")) + _tcslen(m_parentJob->m_name) +
                       _tcslen(_T(", Source ")) + _tcslen(m_parentSrc->m_name) +
                       _tcslen(_T(", Destination ")) + _tcslen(m_name) +
                       _tcslen(_T(" - Fatal thread error: Aborted by user")) + 1;
            m_fatal = new TCHAR[l];
            {
                CTrgScopedLogLock lock(this, kLOGALWAYS);
                _tcscpy_s(m_fatal, l, _T("Job "));
                _tcscat_s(m_fatal, l, m_parentJob->m_name);
                _tcscat_s(m_fatal, l, _T(", Source "));
                _tcscat_s(m_fatal, l, m_parentSrc->m_name);
                _tcscat_s(m_fatal, l, _T(", Destination "));
                _tcscat_s(m_fatal, l, m_name);
                _tcscat_s(m_fatal, l, _T(" - Fatal thread error: Aborted by user"));
                m_parentJob->WriteToLogVSNewLine(m_fatal, NULL);
            }
            delete [] m_fatal;
            m_fatal = NULL;
            CMainDlg::Singleton().PostMessage(WM_USER_SET_LED, CMainDlg::LED_RED);
        }

        return true;
    }

    return false;
}

void CTrgNode::FileErrorWithMessage(LPCTSTR msg, LPCTSTR fMsg, bool const fileNotFolder,
                                    const LPVOID eMsg,
                                    const LPVOID reasonCode)
{
    TCHAR fileOrDir[16];
    _tcscpy_s(fileOrDir, fileNotFolder ? _T(" file") : _T(" folder"));
    {
        CTrgScopedLogLock lock(this, kLOGALWAYS);

        if (fMsg) {
            m_parentJob->WriteToLogVSNewLine((LPTSTR)fMsg, NULL);
        }

        m_parentJob->WriteToLogVS(_T("Job "), m_parentJob->m_name.GetBuffer(), _T(" - Error: "), msg,
                                  fileOrDir,
                                  _T(" - OS Return Code: "), eMsg,
                                  _T(" - Reason code: "), reasonCode,
                                  _T("\r\n"), NULL);
    }
    ::LocalFree(eMsg);
    m_parentJob->m_counts.m_fileErrorCount++;
    m_run->m_counts.m_fileErrorCount++;
    m_fileErrors++;
    CMainDlg::Singleton().PostMessage(WM_USER_SET_LED, CMainDlg::LED_YELLOW);
}

void CTrgNode::VerifyError(LPCTSTR msg, LPCTSTR fMsg, const bool fileNotFolder)
{
    LPCTSTR compError = _T("Compare failed");
    LPVOID eMsg = LocalAlloc(LMEM_FIXED, (_tcslen(compError) + 1) * sizeof(TCHAR));

    if (eMsg == NULL) {
        return;
    }

    _tcscpy_s((LPTSTR)eMsg, _tcslen(compError) + 1, compError);
    TCHAR reasonCode[16];
    _itot_s(GetReasonCode(), reasonCode, 10);
    FileErrorWithMessage(msg, fMsg, fileNotFolder, eMsg, reasonCode);
}

void CTrgNode::FileError(LPCTSTR msg, LPCTSTR fMsg, const bool fileNotFolder)
{
    LPTSTR eMsg = NULL;

    if (!::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                         ::GetLastError(),
                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0, NULL)) {
        LPCTSTR uError = _T("Unknown error");
        eMsg = (LPTSTR)LocalAlloc(LMEM_FIXED, (_tcslen(uError) + 1) * sizeof(TCHAR));

        if (eMsg == NULL) {
            return;
        }

        _tcscpy_s((LPTSTR)eMsg, _tcslen(uError) + 1, uError);
    } else {
        eMsg[_tcslen(eMsg) - 2] = 0;
    }

    TCHAR reasonCode[16];
    _itot_s(GetReasonCode(), reasonCode, 10);
    FileErrorWithMessage(msg, fMsg, fileNotFolder, eMsg, reasonCode);
}

void CTrgNode::NonFatalError(const bool dir, LPCTSTR msg)
{
    LogFlagsType flags = kLOGALWAYS;
    flags.LogFlagFolder = dir;
    {
        CTrgScopedLogLock lock(this, flags);
        m_parentJob->WriteToLogVSNewLine(msg, NULL);
    }
    CMainDlg::Singleton().PostMessage(WM_USER_SET_LED, CMainDlg::LED_YELLOW);
}

bool CTrgNode::IncludedFile(LPCTSTR name)
{
    StrListIter current;

    if (m_includes) {
        wcmatch res;

        if (regex_match(name, res, *m_includes)) {
            goto excludes;
        }

        return false;
    }

excludes:

    if (m_excludes) {
        wcmatch res;

        if (regex_match(name, res, *m_excludes)) {
            return false;
        }
    }

    return true;
}

void CTrgNode::RenameTargetFileOrDirIfDifferent(const CFilePath &dir, const CFileData &to,
        const CFileData &from)
{
    if ((!m_simulate) && (_tcscmp(from.m_name, to.m_name))) {
        CString d = dir.UNC();
        CString fName;
        fName.Format(_T("%s\\%s"), (LPCTSTR)d, (LPCTSTR)(to.m_name));
        CString tName;
        tName.Format(_T("%s\\%s"), (LPCTSTR)d, (LPCTSTR)(from.m_name));
        MoveFile(fName, tName);
        //to.m_name = from.m_name;
    }
}

bool CTrgNode::IncludeFolder(const CFileData &from, const CFilePath &fromDir,
                             const CFilePath &toDir, CFilePath &fName,
                             CFilePath &tName)
{
    fName = fromDir;
    fName += from.m_name;
    tName = toDir;
    tName += from.m_name;

    if (ExcludedFolder(fName)) {
        return false;
    }

    CString m;
    m.Format(_T("Synchronizing folder \"%s\" -> \"%s\""), (LPCTSTR)fName.Display(),
             (LPCTSTR)tName.Display());
    {
        CTrgScopedLogLock lock(this, kLOGFOLDER);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }
    return true;
}

static CString InDir(LPTSTR excl, LPCTSTR dir)
{
    size_t l = _tcslen(dir);

    if (_tcslen(excl) <= l) {
        return CString();
    }

    LPCTSTR p;
    TCHAR c;
    c = excl[l];

    if (c != _T('\\')) {
        return CString();
    }

    excl[l] = 0;

    if (_tcsicmp(excl, dir)) {
        excl[l] = c;
        return CString();
    }

    excl[l] = c;
    p = excl + l + 1;

    if (_tcschr(p, _T('\\'))) {
        return CString();
    }

    return CString(p);
}

// comparison, not case sensitive.
static bool CompareNocase(const CFileData &first, const CFileData &second)
{
    return first.m_name.CompareNoCase(second.m_name) < 0;
}

// a binary predicate implemented as a function:
static bool Same(const CFileData &first, const CFileData &second)
{
    return first.m_name.CompareNoCase(second.m_name) == 0;
}


bool CTrgNode::ScanDir(FileDataList &fileList, FileDataList &dirList, const CFilePath &dir,
                       const bool source)
{
    CString scanParm;

    if (AbortedByUser()) {
        return true;
    }

    // Add excluded folders here so we don't delete them if they actually don't exist in source
    if (source) {
        FilePathSetIter excl;

        for (excl = m_excludedFolders.begin(); excl != m_excludedFolders.end(); ++excl) {
            CString p = InDir(excl->Display().GetBuffer(), dir.Display());

            if (p.GetLength()) {
                dirList.push_back(CFileData(p, FILE_ATTRIBUTE_DIRECTORY));
            }
        }
    }

    TCHAR msg[32];
    _tcscpy_s(msg, source ? _T("reading source") : _T("reading target"));
    scanParm = dir.Shadow() + (dir.NullSuffix() ? _T("*") : _T("\\*"));
    WIN32_FIND_DATA findData;
    HANDLE findHndl = FindFirstFile(scanParm, &findData);

    if (findHndl == INVALID_HANDLE_VALUE) {
        ULONG lastError = ::GetLastError();

        if ((lastError != ERROR_NO_MORE_FILES) && (lastError != ERROR_FILE_NOT_FOUND)) {
            FileError(msg, dir.Display(), false);
            return false;
        }

        return true;
    }

    while (true) {
        if (AbortedByUser()) {
            FindClose(findHndl);
            return true;
        }

        if ((_tcsicmp(findData.cFileName, _T(".")) != 0) && (_tcsicmp(findData.cFileName, _T("..")) != 0)) {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!((findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                      IsReparseTagNameSurrogate(findData.dwReserved0))) {
                    dirList.push_back(findData);
                }
            } else {
                fileList.push_back(findData);
            }
        }

        if (FindNextFile(findHndl, &findData) == 0) {
            ULONG lastError = ::GetLastError();

            if ((lastError != ERROR_NO_MORE_FILES) &&
                (lastError != ERROR_FILE_NOT_FOUND)) {
                FindClose(findHndl);
                SetReasonCode(9);
                FileError(msg, NULL, false);
                return false;
            }

            break;
        }
    }

    FindClose(findHndl);
    dirList.sort(CompareNocase);
    fileList.sort(CompareNocase);

    // Need to remove duplicate folders that may have been inserted as excluded
    if (source) {
        dirList.unique(Same);
    }

    if (CMainDlg::Singleton().VerboseLog()) {
        CString logstr;
        logstr.Format(_T("Dir scan result for '%s':"), dir.Display().GetBuffer(0));
        FileDataList::iterator listPtr;

        for (listPtr = dirList.begin(); listPtr != dirList.end(); ++listPtr) {
            logstr += _T(" '");
            logstr += (*listPtr).m_name;
            logstr += _T("'");
        }

        {
            CTrgScopedLogLock lock(this, kLOGALWAYS);
            m_parentJob->WriteToLogVS(logstr, _T("\r\n"), NULL);
        }

        logstr.Format(_T("File scan result for '%s':"), dir.Display().GetBuffer());

        for (listPtr = fileList.begin(); listPtr != fileList.end(); ++listPtr) {
            logstr += _T(" '");
            logstr += (*listPtr).m_name;
            logstr += _T("'");
        }

        {
            CTrgScopedLogLock lock(this, kLOGALWAYS);
            m_parentJob->WriteToLogVS(logstr, _T("\r\n"), NULL);
        }
    }

    return true;
}

void CTrgNode::InitDesc(PFILEDESC desc)
{
    desc->fileLock = NULL;
    desc->h = INVALID_HANDLE_VALUE;
    desc->overlapped.hEvent = NULL;
}

bool CTrgNode::RemoveADirectory(LPCTSTR folder, LPCTSTR msg)
{
    bool rc = ::RemoveDirectory(folder) == TRUE;

    if (!rc) {
        SetReasonCode(3);
        FileError(_T("removing"), msg, false);
    }

    return rc;
};

void CTrgNode::SetReasonCode(const int r)
{
    m_reasonCode = r;
}

bool CTrgNode::ExcludedFolder(const CFilePath &path)
{
    return m_excludedFolders.find(path) != m_excludedFolders.end();
}

CFileData::CFileData(const WIN32_FIND_DATA &f)
{
    m_fileAttributes = f.dwFileAttributes;
    m_creationTime = f.ftCreationTime;
    m_lastAccessTime = f.ftLastAccessTime;
    m_lastWriteTime = f.ftLastWriteTime;
    m_fileSize = ((UINT64)f.nFileSizeHigh << 32) | f.nFileSizeLow;
    m_name = f.cFileName;
}

CFileData::CFileData(LPCTSTR f, const DWORD attr)
{
    m_fileAttributes = attr;
    m_lastWriteTime =
        m_creationTime =
            m_lastAccessTime = { 0, 0 };
    m_fileSize = 0;
    m_name = f;
}

CFileData::CFileData(const CFileData &f)
{
    m_fileAttributes = f.m_fileAttributes;
    m_lastWriteTime = f.m_lastWriteTime;
    m_creationTime = f.m_creationTime;
    m_lastAccessTime = f.m_lastAccessTime;
    m_fileSize = f.m_fileSize;
    m_name = f.m_name;
}

void CCounts::Clear()
{
    m_filesSkippedCount = 0;
    m_filesCopiedCount = 0;
    m_filesUpdatedCount = 0;
    m_filesDeletedCount = 0;
    m_fileErrorCount = 0;
    m_foldersCopiedCount = 0;
    m_foldersDeletedCount = 0;
    m_bytesSkippedCount = 0;
    m_bytesCopiedCount = 0;
    m_bytesUpdatedCount = 0;
    m_bytesDeletedCount = 0;
}
