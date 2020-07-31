#include "stdafx.h"

const LPCTSTR kErrorString[COPY_NUM_ERRORS] = {
    _T("No error"),
    _T("opening source"),
    _T("opening target"),
    _T("creating target"),
    _T("reading source"),
    _T("writing target"),
    _T("reading target"),
    _T("setting target time stamp"),
    _T("setting target attributes"),
    _T("restoring target attributes"),
    _T("verifying target"),
    _T("getting buffer memory")
};

static LPCTSTR CopyFileErrorString(CopyFileError err)
{
    if (err < COPY_NUM_ERRORS) {
        return kErrorString[err];
    }

    return _T("Unknow operation");
}

//========================================================
// Functions common to 1-way, 2-way, and backup sync modes

void CTrgNode::BackupOrSync(const CFilePath &fromDir, const CFilePath &toDir, PVOID buffer,
                            const bool fromExists,
                            const bool toExists)
{
    if (AbortedByUser()) {
        return;
    }

    FileDataList fromFileList;
    FileDataList toFileList;
    FileDataList fromDirList;
    FileDataList toDirList;

    do {
        if (fromExists) {
            if (!ScanDir(fromFileList, fromDirList, fromDir, (m_effectiveMode == MIRROR_SYNC_MODE))) {
                NonFatalError(false, _T("Source folder skipped"));
                break;
            }
        }

        if (toExists) {
            if (!ScanDir(toFileList, toDirList, toDir, false)) {
                NonFatalError(false, _T("Target folder skipped"));
                break;
            }
        }

        if (m_subDir && (fromDirList.size() || toDirList.size())) {
            m_run->ScheduleDirMerge(fromDirList, toDirList, fromDir, toDir, this);
        }

        if (fromFileList.size() || toFileList.size()) {
            MergeFiles(fromFileList, toFileList, fromDir, toDir, buffer);
        }
    } while (false);
}

void CTrgNode::MergeFiles(FileDataList &fromList, FileDataList &toList, const CFilePath &fromDir,
                          const CFilePath &toDir,
                          PVOID buffer)
{
    FileDataListIter from;
    FileDataListIter to;

    if (m_effectiveMode == MIRROR_SYNC_MODE) {
        from = fromList.begin();
        to = toList.begin();

        while ((from != fromList.end()) && (to != toList.end())) {
            if (AbortedByUser()) {
                goto abort;
            }

            int r = from->m_name.CompareNoCase(to->m_name);

            if (r < 0) {
                ++from;
            } else if (r == 0) {
                if (!IncludedFile(from->m_name)) {
                    if (!m_dontDelete) {
                        MirrorFileInTrgOnly(*to, toDir);
                    }
                }

                ++from;
                ++to;
            } else {
                MirrorFileInTrgOnly(*to, toDir);
                ++to;
            }
        }

        if (to != toList.end()) {
            while (to != toList.end()) {
                if (AbortedByUser()) {
                    goto abort;
                }

                MirrorFileInTrgOnly(*to, toDir);
                ++to;
            }
        }
    }

    from = fromList.begin();
    to = toList.begin();

    while ((from != fromList.end()) && (to != toList.end())) {
        if (AbortedByUser()) {
            goto abort;
        }

        int r = from->m_name.CompareNoCase(to->m_name);

        if (r < 0) {
            if (IncludedFile(from->m_name)) {
                FileInSrcOnly(*from, fromDir, toDir, buffer);
            }

            ++from;
        } else if (r == 0) {
            if (IncludedFile(from->m_name)) {
                (this->*FileInBoth)(*from, *to, fromDir, toDir, buffer);
            }

            ++from;
            ++to;
        } else {
            if (m_effectiveMode == TWOWAY_SYNC_MODE) {
                if (IncludedFile(to->m_name)) {
                    FileInSrcOnly(*to, toDir, fromDir, buffer);
                }
            }

            ++to;
        }
    }

    if (from != fromList.end()) {
        while (from != fromList.end()) {
            if (AbortedByUser()) {
                goto abort;
            }

            if (IncludedFile(from->m_name)) {
                FileInSrcOnly(*from, fromDir, toDir, buffer);
            }

            ++from;
        }
    } else if ((to != toList.end()) && (m_effectiveMode == TWOWAY_SYNC_MODE)) {
        while (to != toList.end()) {
            if (AbortedByUser()) {
                goto abort;
            }

            if (IncludedFile(to->m_name)) {
                FileInSrcOnly(*to, toDir, fromDir, buffer);
            }

            ++to;
        }
    }

abort:
    ;
}

void CTrgNode::CopyAndLogACLUpdateIfDifferent(const CFilePath &from, const CFilePath &to)
{
    if (CopyNamedACLIfDifferent(from.Shadow(), to.UNC())) {
        CString m;
        m.Format(_T("Permission \"%s\" -> \"%s\""), from.Display().GetBuffer(), to.Display().GetBuffer());
        {
            CTrgScopedLogLock lock(this, kLOGFILE);
            m_parentJob->WriteToLogVSNewLine(m, NULL);
        }
    }
}

void CTrgNode::MergeDirs(FileDataList &fromList, FileDataList &toList, const CFilePath &fromDir,
                         const CFilePath &toDir,
                         PVOID buffer)
{
    FileDataListIter from;
    FileDataListIter to;

    if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_folderACLs
        && !m_simulate) {
        if (m_effectiveMode != TWOWAY_SYNC_MODE) {
            CopyAndLogACLUpdateIfDifferent(fromDir, toDir);
        }
    }

    if (m_effectiveMode == MIRROR_SYNC_MODE) {
        from = fromList.begin();
        to = toList.begin();

        while ((from != fromList.end()) && (to != toList.end())) {
            if (AbortedByUser()) {
                goto abort;
            }

            int r = from->m_name.CompareNoCase(to->m_name);

            if (r < 0) {
                ++from;
            } else if (r == 0) {
                ++from;
                ++to;
            } else {
                MirrorDirInTrgOnly(*to, toDir);
                ++to;
            }
        }

        if (to != toList.end()) {
            while (to != toList.end()) {
                if (AbortedByUser()) {
                    goto abort;
                }

                MirrorDirInTrgOnly(*to, toDir);
                ++to;
            }
        }
    }

    from = fromList.begin();
    to = toList.begin();

    while ((from != fromList.end()) && (to != toList.end())) {
        if (AbortedByUser()) {
            goto abort;
        }

        int r = from->m_name.CompareNoCase(to->m_name);

        if (r < 0) {
            DirInSrcOnly(*from, fromDir, toDir, buffer);
            ++from;
        } else if (r == 0) {
            (this->*DirInBoth)(*from, *to, fromDir, toDir, buffer);
            ++from;
            ++to;
        } else {
            if (m_effectiveMode == TWOWAY_SYNC_MODE) {
                DirInSrcOnly(*to, toDir, fromDir, buffer);
            }

            ++to;
        }
    }

    if (from != fromList.end()) {
        while (from != fromList.end()) {
            if (AbortedByUser()) {
                goto abort;
            }

            DirInSrcOnly(*from, fromDir, toDir, buffer);
            ++from;
        }
    } else if ((to != toList.end()) && (m_effectiveMode == TWOWAY_SYNC_MODE)) {
        while (to != toList.end()) {
            if (AbortedByUser()) {
                goto abort;
            }

            DirInSrcOnly(*to, toDir, fromDir, buffer);
            ++to;
        }
    }

abort:
    ;
}

void CTrgNode::FileInSrcOnly(const CFileData &from, const CFilePath &fromDir,
                             const CFilePath &toDir, PVOID buffer)
{
    CFilePath fName(fromDir);
    fName += from.m_name;
    CFilePath tName(toDir);
    tName += from.m_name;
    CString m = FileLogMsg(_T("Copying  "), from.m_fileSize, fName, tName);
    {
        CTrgScopedLogLock lock(this, kLOGFILE);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }
    CopyFileError fileError = COPY_NO_ERROR;

    if (!m_simulate) {
        fileError = CopyAFile(fName.Shadow(), tName.UNC(), from, from, false, m_parentJob, buffer,
                              fName.HasShadow(), false);

        if ((m_threadVerify) && (fileError == COPY_NO_ERROR)) {
            fileError = VerifyAFile(fName.Shadow(), tName.UNC(), from.m_fileSize, buffer, fName.HasShadow());
        }

        if (fileError != COPY_NO_ERROR) {
            ULONG lastError = ::GetLastError();

            if ((lastError == ERROR_SUCCESS) && (fileError == COPY_VERIFY_FAILURE)) {
                VerifyError(CopyFileErrorString(fileError), m, true);
            } else {
                FileError(CopyFileErrorString(fileError), m, true);
            }

            return;
        }
    } else {
        m_parentJob->m_counts.m_bytesCopiedCount += from.m_fileSize;
        m_run->m_counts.m_bytesCopiedCount += from.m_fileSize;
    }

    m_parentJob->m_counts.m_filesCopiedCount++;
    m_run->m_counts.m_filesCopiedCount++;
}

void CTrgNode::DirInSrcOnly(const CFileData &from, const CFilePath &fromDir, const CFilePath &toDir,
                            PVOID buffer)
{
    CFilePath fName;
    CFilePath tName;

    if (IncludeFolder(from, fromDir, toDir, fName, tName)) {
        CString m;
        m.Format(_T("Creating \"%s\""), tName.Display().GetBuffer());
        {
            CTrgScopedLogLock lock(this, kLOGFOLDER);
            m_parentJob->WriteToLogVSNewLine(m, NULL);
        }

        if (!m_simulate) {
            CString unc(tName.UNC());

            if (!::CreateDirectory(unc, NULL)) {
                // Folder should not exist!!! Don't know how the following error could happen!
                // But it seems to occur for some user devices!!!
                if (GetLastError() != ERROR_FILE_EXISTS) {
                    SetReasonCode(4);
                    FileError(_T("creating"), NULL, false);
                    return;
                }
            }

            HANDLE h = ::CreateFile(unc, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,
                                    NULL);

            if (h != INVALID_HANDLE_VALUE) {
                ::SetFileTime(h, &from.m_creationTime, NULL, NULL);
                ::CloseHandle(h);
            }

            if (from.m_fileAttributes & (~FILE_ATTRIBUTE_NORMAL)) {
                ::SetFileAttributes(unc, from.m_fileAttributes & kFILE_ATTRIBUTE_SETABLE);
            }

            if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_folderACLs) {
                CopyAndLogACLUpdateIfDifferent(fName, tName);
            }
        }

        tName.Created();
        BackupOrSync(fName, tName, buffer, true, !m_simulate);
        m_parentJob->m_counts.m_foldersCopiedCount++;
        m_run->m_counts.m_foldersCopiedCount++;
    }
}

//==============================
// Functions for 1-way sync mode

CString CTrgNode::FileLogMsg(LPCTSTR mode, UINT64 fileSize, const CFilePath &fName,
                             const CFilePath &tName)
{
    TCHAR temps[32];
    CString m;
    FormatCount(fileSize, true, true, temps, _countof(temps), _T("bytes"));
    m.Format(_T("%s\"%s\" -> \"%s\" (%s)"), mode, fName.Display().GetBuffer(),
             tName.Display().GetBuffer(), temps);
    return m;
}

CString CTrgNode::FileLogMsg(LPCTSTR mode, UINT64 fileSize, const CFilePath &tName)
{
    TCHAR temps[32];
    CString m;
    FormatCount(fileSize, true, true, temps, _countof(temps), _T("bytes"));
    m.Format(_T("%s\"%s\" (%s)"), mode, tName.Display().GetBuffer(), temps);
    return m;
}

#define FILE_ATTRIBUTE_ALL (FILE_ATTRIBUTE_READONLY | /*FILE_ATTRIBUTE_HIDDEN |*/ FILE_ATTRIBUTE_SYSTEM)

void CTrgNode::MirrorFileInBoth(const CFileData &from, const CFileData &to,
                                const CFilePath &fromDir,
                                const CFilePath &toDir, PVOID buffer)
{
    CString m;
    CFilePath fName(fromDir), tName(toDir);
    LARGE_INTEGER fromTime;
    fromTime.HighPart = from.m_lastWriteTime.dwHighDateTime;
    fromTime.LowPart = from.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER toTime;
    toTime.HighPart = to.m_lastWriteTime.dwHighDateTime;
    toTime.LowPart = to.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER delta;
    delta.QuadPart = fromTime.QuadPart - toTime.QuadPart;
    fName += from.m_name;
    tName += from.m_name;

    do {
        RenameTargetFileOrDirIfDifferent(toDir, to, from);

        if (from.m_fileSize != to.m_fileSize) {
            if (CMainDlg::Singleton().VerboseLog()) {
                m.Format(_T("Size mismatch \"%s\" from %llu to %llu"), fName.Display().GetBuffer(), from.m_fileSize,
                         to.m_fileSize);
                {
                    CTrgScopedLogLock lock(this, kLOGALWAYS);
                    m_parentJob->WriteToLogVSNewLine(m, NULL);
                }
            }

            break;
        }

        if ((from.m_fileAttributes & FILE_ATTRIBUTE_ALL) != (to.m_fileAttributes & FILE_ATTRIBUTE_ALL)) {
            if (CMainDlg::Singleton().VerboseLog()) {
                m.Format(_T("Attr mismatch \"%s\" diff %x"),
                         fName.Display().GetBuffer(), (from.m_fileAttributes ^ to.m_fileAttributes) & FILE_ATTRIBUTE_ALL);
                {
                    CTrgScopedLogLock lock(this, kLOGALWAYS);
                    m_parentJob->WriteToLogVSNewLine(m, NULL);
                }
            }

            break;
        }

        if ((delta.QuadPart < -20000000) || (delta.QuadPart > 20000000)) {
            if (CMainDlg::Singleton().VerboseLog()) {
                m.Format(_T("Time mismatch \"%s\" delta %lld"), fName.Display().GetBuffer(), delta.QuadPart);
                {
                    CTrgScopedLogLock lock(this, kLOGALWAYS);
                    m_parentJob->WriteToLogVSNewLine(m, NULL);
                }
            }

            break;
        }

        if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_unchangedFileACLs
            && !m_simulate) {
            CopyAndLogACLUpdateIfDifferent(fName, tName);
        }

        m_parentJob->m_counts.m_filesSkippedCount++;
        m_parentJob->m_counts.m_bytesSkippedCount += from.m_fileSize;
        m_run->m_counts.m_filesSkippedCount++;
        m_run->m_counts.m_bytesSkippedCount += from.m_fileSize;
        m.Format(_T("Skipping \"%s\" = \"%s\""), fName.Display().GetBuffer(), tName.Display().GetBuffer());
        {
            CTrgScopedLogLock lock(this, kLOGSKIP);
            m_parentJob->WriteToLogVSNewLine(m, NULL);
        }
        return;
    } while (false);

    m = FileLogMsg(_T("Updating "), from.m_fileSize, fName, tName);
    {
        CTrgScopedLogLock lock(this, kLOGFILE);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }
    bool unprotect = true;
    CopyFileError fileError = COPY_NO_ERROR;

    if (!m_simulate) {
        fileError = CopyAFile(fName.Shadow(), tName.UNC(), from, to, unprotect, m_parentJob, buffer,
                              fName.HasShadow(), true);

        if (m_threadVerify && (fileError == COPY_NO_ERROR)) {
            fileError = VerifyAFile(fName.Shadow(), tName.UNC(), from.m_fileSize, buffer, fName.HasShadow());
        }

        if (fileError != COPY_NO_ERROR) {
            ULONG lastError = ::GetLastError();

            if ((lastError == ERROR_SUCCESS) && (fileError == COPY_VERIFY_FAILURE)) {
                VerifyError(CopyFileErrorString(fileError), m, true);
            } else {
                FileError(CopyFileErrorString(fileError), m, true);
            }

            return;
        }
    } else {
        m_parentJob->m_counts.m_bytesUpdatedCount += from.m_fileSize;
        m_run->m_counts.m_bytesUpdatedCount += from.m_fileSize;
    }

    m_parentJob->m_counts.m_filesUpdatedCount++;
    m_run->m_counts.m_filesUpdatedCount++;
}

void CTrgNode::MirrorFileInTrgOnly(const CFileData &to, const CFilePath &toDir)
{
    CFilePath tName(toDir);
    tName += to.m_name;
    CString m = FileLogMsg(_T("Deleting "), to.m_fileSize, tName);
    {
        CTrgScopedLogLock lock(this, kLOGFILE);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }

    if (UnprotectAFile(tName.UNC(), to.m_fileAttributes, m)) {
        if (DeleteAFile(tName.UNC(), m)) {
            m_parentJob->m_counts.m_filesDeletedCount++;
            m_parentJob->m_counts.m_bytesDeletedCount += to.m_fileSize;
            m_run->m_counts.m_filesDeletedCount++;
            m_run->m_counts.m_bytesDeletedCount += to.m_fileSize;
        }
    }
}

void CTrgNode::MirrorDirInBoth(const CFileData &from, const CFileData &to, const CFilePath &fromDir,
                               const CFilePath &toDir, PVOID buffer)
{
    CFilePath fName;
    CFilePath tName;

    if (IncludeFolder(from, fromDir, toDir, fName, tName)) {
        if (!m_simulate) {
            RenameTargetFileOrDirIfDifferent(toDir, to, from);

            if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_folderACLs) {
                CopyAndLogACLUpdateIfDifferent(fName, tName);
            }

            if (!UnprotectAFile(tName.UNC(), to.m_fileAttributes, NULL)) {
                FileError(_T("unprotecting"), NULL, false);
            }
        }

        BackupOrSync(fName, tName, buffer, true, true);
    } else {
        if (!m_dontDelete) {
            MirrorDirInTrgOnly(to, toDir);
        }
    }
}

void CTrgNode::MirrorDirInTrgOnly(const CFileData &to, const CFilePath &toDir)
{
    CFilePath tName(toDir);
    tName += to.m_name;
    FileDataList emptyList;
    FileDataList toFileList;
    FileDataList toDirList;

    if (ScanDir(toFileList, toDirList, tName, false)) {
        if (m_subDir && toDirList.size()) {
            FileDataList newList = toDirList;
            MergeDirs(emptyList, newList, tName, tName, NULL);
        }

        if (toFileList.size()) {
            MergeFiles(emptyList, toFileList, tName, tName, NULL);
        }

        CString m = FileLogMsg(_T("Deleting "), 0, tName);
        bool incr = true;

        if (!m_simulate) {
            if (UnprotectAFile(tName.UNC(), to.m_fileAttributes, m)) {
                if (!RemoveADirectory(tName.UNC(), m)) {
                    incr = false;
                }
            }
        }

        if (incr) {
            {
                CTrgScopedLogLock lock(this, kLOGFOLDER);
                m_parentJob->WriteToLogVSNewLine(m, NULL);
            }
            m_parentJob->m_counts.m_foldersDeletedCount++;
            m_run->m_counts.m_foldersDeletedCount++;
        }
    } else {
        NonFatalError(false, _T("Target folder skipped"));
    }
}

//==============================
// Functions for 2-way sync mode

void CTrgNode::TwoWayDirInBoth(const CFileData &from, const CFileData &to, const CFilePath &fromDir,
                               const CFilePath &toDir, PVOID buffer)
{
    CFilePath fName;
    CFilePath tName;

    if (IncludeFolder(from, fromDir, toDir, fName, tName)) {
        if (!m_simulate) {
            if (!UnprotectAFile(tName.UNC(), to.m_fileAttributes, NULL)) {
                FileError(_T("unprotecting"), NULL, false);
            }
        }

        BackupOrSync(fName, tName, buffer, true, true);
    }
}

void CTrgNode::TwoWayFileInBoth(const CFileData &from, const CFileData &to,
                                const CFilePath &fromDir,
                                const CFilePath &toDir, PVOID buffer)
{
    CString m;
    LARGE_INTEGER fromTime;
    fromTime.HighPart = from.m_lastWriteTime.dwHighDateTime;
    fromTime.LowPart = from.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER toTime;
    toTime.HighPart = to.m_lastWriteTime.dwHighDateTime;
    toTime.LowPart = to.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER delta;
    delta.QuadPart = fromTime.QuadPart - toTime.QuadPart;
    CFilePath fNam(fromDir), tNam(toDir);
    fNam += from.m_name;
    tNam += from.m_name;

    if ((delta.QuadPart <= 20000000) && (delta.QuadPart >= -20000000)) {
        if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_unchangedFileACLs
            && !m_simulate) {
            CopyAndLogACLUpdateIfDifferent(fNam, tNam);
        }

        if (m_parentJob) {
            m_parentJob->m_counts.m_filesSkippedCount++;
            m_parentJob->m_counts.m_bytesSkippedCount += from.m_fileSize;
            m_run->m_counts.m_filesSkippedCount++;
            m_run->m_counts.m_bytesSkippedCount += from.m_fileSize;
        }

        RenameTargetFileOrDirIfDifferent(toDir, to, from);
        m.Format(_T("Skipping \"%s\" = \"%s\""), fNam.Display().GetBuffer(), tNam.Display().GetBuffer());
        {
            CTrgScopedLogLock lock(this, kLOGSKIP);
            m_parentJob->WriteToLogVSNewLine(m, NULL);
        }
        return;
    }

    CopyFileError fileError = COPY_NO_ERROR;
    const CFileData *f, *t;
    CFilePath *fn, *tn;

    if (delta.QuadPart < 0) {
        f = &to;
        t = &from;
        fn = &tNam;
        tn = &fNam;
    } else {
        f = &from;
        t = &to;
        fn = &fNam;
        tn = &tNam;
    }

    m = FileLogMsg(_T("Updating "), f->m_fileSize, fn->Display(), tn->Display());
    {
        CTrgScopedLogLock lock(this, kLOGFILE);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }

    if (!m_simulate) {
        bool unprotect = true;
        fileError =
            CopyAFile(fn->Shadow(), tn->UNC(), *f, *t, unprotect, m_parentJob, buffer, fn->HasShadow(), true);

        if (m_threadVerify && (fileError == COPY_NO_ERROR)) {
            fileError = VerifyAFile(fn->Shadow(), tn->UNC(), f->m_fileSize, buffer, fn->HasShadow());
        }

        if (fileError != COPY_NO_ERROR) {
            ULONG lastError = ::GetLastError();

            if ((lastError == ERROR_SUCCESS) && (fileError == COPY_VERIFY_FAILURE)) {
                VerifyError(CopyFileErrorString(fileError), m, true);
            } else {
                FileError(CopyFileErrorString(fileError), m, true);
            }

            return;
        }
    }

    m_parentJob->m_counts.m_filesUpdatedCount++;
    m_run->m_counts.m_filesUpdatedCount++;

    if (m_simulate) {
        m_parentJob->m_counts.m_bytesUpdatedCount += f->m_fileSize;
        m_run->m_counts.m_bytesUpdatedCount += f->m_fileSize;
    }
}

//===============================
// Functions for backup sync mode

void CTrgNode::BackupDirInBoth(const CFileData &from, const CFileData &to, const CFilePath &fromDir,
                               const CFilePath &toDir, PVOID buffer)
{
    CFilePath fName;
    CFilePath tName;

    if (IncludeFolder(from, fromDir, toDir, fName, tName)) {
        RenameTargetFileOrDirIfDifferent(toDir, to, from);

        if (!m_simulate) {
            if (!UnprotectAFile(tName.UNC(), to.m_fileAttributes, NULL)) {
                FileError(_T("unprotecting"), NULL, false);
            }

            if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_folderACLs) {
                CopyAndLogACLUpdateIfDifferent(fName, tName);
            }
        }

        BackupOrSync(fName, tName, buffer, true, true);

        if (!m_simulate) {
            DWORD a = from.m_fileAttributes & kFILE_ATTRIBUTE_SETABLE;

            if (a) {
                if (!::SetFileAttributes(tName.UNC(), from.m_fileAttributes & kFILE_ATTRIBUTE_SETABLE)) {
                    SetReasonCode(8);
                    FileError(_T("setting attributes"), NULL, false);
                }
            }
        }
    }
}

void CTrgNode::BackupFileInBoth(const CFileData &from, const CFileData &to,
                                const CFilePath &fromDir,
                                const CFilePath &toDir, PVOID buffer)
{
    CString m;
    LARGE_INTEGER fromTime;
    fromTime.HighPart = from.m_lastWriteTime.dwHighDateTime;
    fromTime.LowPart = from.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER toTime;
    toTime.HighPart = to.m_lastWriteTime.dwHighDateTime;
    toTime.LowPart = to.m_lastWriteTime.dwLowDateTime;
    LARGE_INTEGER delta;
    delta.QuadPart = fromTime.QuadPart - toTime.QuadPart;
    CFilePath fName(fromDir), tName(toDir);
    fName += from.m_name;
    tName += from.m_name;

    if (delta.QuadPart <= 20000000) {
        if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_unchangedFileACLs
            && !m_simulate) {
            CopyAndLogACLUpdateIfDifferent(fName, tName);
        }

        if (m_parentJob) {
            m_parentJob->m_counts.m_filesSkippedCount++;
            m_parentJob->m_counts.m_bytesSkippedCount += from.m_fileSize;
            m_run->m_counts.m_filesSkippedCount++;
            m_run->m_counts.m_bytesSkippedCount += from.m_fileSize;
        }

        RenameTargetFileOrDirIfDifferent(toDir, to, from);
        m.Format(_T("Skipping \"%s\" = \"%s\""), fName.Display().GetBuffer(), tName.Display().GetBuffer());
        {
            CTrgScopedLogLock lock(this, kLOGSKIP);
            m_parentJob->WriteToLogVSNewLine(m, NULL);
        }
        return;
    }

    m = FileLogMsg(_T("Updating "), from.m_fileSize, fName, tName);
    {
        CTrgScopedLogLock lock(this, kLOGFILE);
        m_parentJob->WriteToLogVSNewLine(m, NULL);
    }
    CopyFileError fileError = COPY_NO_ERROR;

    if (!m_simulate) {
        bool unprotect = true;
        fileError = CopyAFile(fName.Shadow(), tName.UNC(), from, to, unprotect, m_parentJob, buffer,
                              fName.HasShadow(), true);

        if (m_threadVerify && (fileError == COPY_NO_ERROR)) {
            fileError = VerifyAFile(fName.Shadow(), tName.UNC(), from.m_fileSize, buffer, fName.HasShadow());
        }

        if (fileError != COPY_NO_ERROR) {
            ULONG lastError = ::GetLastError();

            if ((lastError == ERROR_SUCCESS) && (fileError == COPY_VERIFY_FAILURE)) {
                VerifyError(CopyFileErrorString(fileError), m, true);
            } else {
                FileError(CopyFileErrorString(fileError), m, true);
            }

            return;
        }
    } else {
        m_parentJob->m_counts.m_bytesUpdatedCount += from.m_fileSize;
        m_run->m_counts.m_bytesUpdatedCount += from.m_fileSize;
    }

    m_parentJob->m_counts.m_filesUpdatedCount++;
    m_run->m_counts.m_filesUpdatedCount++;
}
