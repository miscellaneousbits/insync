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

#define kFILE_ATTRIBUTE_UNPROTECT \
    (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)

static void ReleaseAndCloseMutex(HANDLE &h) {
    if (h) {
        ::ReleaseMutex(h);
        ::CloseHandle(h);
        h = NULL;
    }
}

bool CTrgNode::CreateCopyFile(LPCTSTR fileName, PFILEDESC desc, DWORD desiredAccess, DWORD flagsAndAttributes,
                              bool lock, int reasonBase) {
    if (lock) {
        TCHAR hash[MAX_PATH];
        TCHAR hashout[16];
        unsigned long lhash = 2166136261;
        _tcscpy_s(hash, _T("Local\\"));
        _tcscat_s(hash, kInSyncUID);
        LPCTSTR cp = fileName;
        TCHAR c = *cp++;
        while (c) {
            lhash = (lhash * 16777619) ^ c; /* hash * FNV Prime XOR c */
            c = *cp++;
        }
        _ultow_s(lhash, hashout, _countof(hashout), 16);
        _tcscat_s(hash, hashout);
        hash[MAX_PATH - 1] = 0;
        desc->fileLock = ::CreateMutex(NULL, FALSE, hash);
        if (desc->fileLock) {
            ::WaitForSingleObject(desc->fileLock, INFINITE);
        }
    }
    DWORD shareMode, creationDisposition;
    bool write = false;
    if (desiredAccess == GENERIC_READ) {
        shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
        creationDisposition = OPEN_EXISTING;
    }
    else {
        shareMode = 0;
        desiredAccess |= GENERIC_READ;
        creationDisposition = CREATE_ALWAYS;
        if (InSyncApp.m_adminPriviledge) {
            desiredAccess |= READ_CONTROL | WRITE_DAC | WRITE_OWNER;
        }
        write = true;
    }
    if (desc->overlapOn) {
        desc->h = ::CreateFile(fileName, desiredAccess, shareMode, NULL, creationDisposition,
                               flagsAndAttributes | FILE_FLAG_OVERLAPPED, NULL);
    }
    if (desc->h == INVALID_HANDLE_VALUE) {
        desc->overlapOn = FALSE;
        desc->h = ::CreateFile(fileName, desiredAccess, shareMode, NULL, creationDisposition, flagsAndAttributes,
                               NULL);
    }
    if (desc->h == INVALID_HANDLE_VALUE) {
        ReleaseAndCloseMutex(desc->fileLock);
        SetReasonCode(reasonBase + 0);
        return false;
    }
    // Preallocate space to avoid fragmentation
    if (write && desc->fileSize) {
        DWORD l = desc->fileSize & 0xffffffff;
        DWORD h = desc->fileSize >> 32;
        if (::SetFilePointer(desc->h, l, (PLONG)&h, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
            ReleaseAndCloseMutex(desc->fileLock);
            SetReasonCode(reasonBase + 1);
            return false;
        }
        if (!::SetEndOfFile(desc->h)) {
            ReleaseAndCloseMutex(desc->fileLock);
            SetReasonCode(reasonBase + 2);
            return false;
        }
        ::SetFilePointer(desc->h, 0, NULL, FILE_BEGIN);
    }
    if (lock) {
        if (!::LockFile(desc->h, 0, 0, (DWORD)(desc->fileSize & 0xffffffff), (DWORD)(desc->fileSize >> 32))) {
            ::CloseHandle(desc->h);
            desc->h = INVALID_HANDLE_VALUE;
            ReleaseAndCloseMutex(desc->fileLock);
            SetReasonCode(reasonBase + 3);
            return false;
        }
    }
    if (desc->overlapOn) {
        if ((desc->overlapped.hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
            ::CloseHandle(desc->h);
            desc->h = INVALID_HANDLE_VALUE;
            ReleaseAndCloseMutex(desc->fileLock);
            SetReasonCode(reasonBase + 4);
            return false;
        }
    }
    return true;
}

void CTrgNode::CloseCopyFile(PFILEDESC desc) {
    if (desc->h != INVALID_HANDLE_VALUE) {
        ::CloseHandle(desc->h);
        desc->h = INVALID_HANDLE_VALUE;
    }
    ReleaseAndCloseMutex(desc->fileLock);
    if (desc->overlapped.hEvent) {
        ::CloseHandle(desc->overlapped.hEvent);
        desc->overlapped.hEvent = NULL;
    }
}

bool CTrgNode::StartRead(PFILEDESC desc, LPVOID buffer, DWORD len, UINT64 offset, int reasonBase) {
    desc->ioPending = FALSE;
    if (desc->overlapOn) {
        desc->overlapped.Offset = (DWORD)offset;
        desc->overlapped.OffsetHigh = (DWORD)(offset >> 32);
        if (!::ReadFile(desc->h, buffer, len, &(desc->bytesXfered), &(desc->overlapped))) {
            DWORD rc = ::GetLastError();
            if (rc == ERROR_HANDLE_EOF) {
                desc->bytesXfered = 0;
                return true;
            }
            if (rc != ERROR_IO_PENDING) {
                SetReasonCode(reasonBase + 0);
                return false;
            }
            desc->ioPending = TRUE;
        }
    }
    else {
        if (!::ReadFile(desc->h, buffer, len, &(desc->bytesXfered), NULL)) {
            SetReasonCode(reasonBase + 1);
            return false;
        }
    }
    return true;
}

bool CTrgNode::StartWrite(PFILEDESC desc, LPCVOID buffer, DWORD bytes, UINT64 offset) {
    desc->ioPending = FALSE;
    if (desc->overlapOn) {
        desc->overlapped.Offset = (DWORD)offset;
        desc->overlapped.OffsetHigh = (DWORD)(offset >> 32);
        if (!::WriteFile(desc->h, buffer, bytes, &(desc->bytesXfered), &(desc->overlapped))) {
            DWORD rc = ::GetLastError();
            if (rc != ERROR_IO_PENDING) {
                SetReasonCode(116);
                return false;
            }
            desc->ioPending = TRUE;
        }
    }
    else {
        if (!::WriteFile(desc->h, buffer, bytes, &(desc->bytesXfered), NULL)) {
            SetReasonCode(117);
            return false;
        }
    }
    return true;
}

bool CTrgNode::WaitIO(PFILEDESC desc, int reasonBase) {
    bool result = true;
    if (desc->ioPending) {
        // Huge (60 second) timeout
        if (WaitForSingleObject(desc->overlapped.hEvent, 60000) == WAIT_TIMEOUT) {
            CancelIo(desc->h);
            SetReasonCode(reasonBase);
            result = false;
        }
        if (!::GetOverlappedResult(desc->h, &(desc->overlapped), &(desc->bytesXfered), TRUE)) {
            SetReasonCode(reasonBase);
            result = false;
        }
    }
    return result;
}

static const UINT64 k4KMinus1 = (4 * 1024) - 1;

CopyFileError CTrgNode::CopyAFile(
    LPCTSTR fromName,
    LPCTSTR toName,
    const CFileData &from,
    const CFileData &to,
    const bool unprotect,
    CJobNode *job,
    PVOID buffer,
    const bool fromIsShadow,
    const bool update) {
    DWORD lastError = NO_ERROR, attr, readSize, wBufSize;
    CopyFileError rc = COPY_NO_ERROR;
    FILEDESC fromDesc, toDesc;
    UINT64 CurrentOffset, CurrentBytesLeft;
    const DWORD kFromFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_READONLY | FILE_FLAG_BACKUP_SEMANTICS;
    const DWORD kToFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS;
    UINT64 bufferSize = CRun::kMaxThreadBufferSize;
    LPVOID buffer1 = buffer;
    LPVOID buffer2;

    if (buffer1 == NULL) {
        SetReasonCode(130);
        return COPY_NO_MEMORY;
    }
    buffer2 = (PCHAR)buffer1 + bufferSize;

    InitDesc(&fromDesc);
    InitDesc(&toDesc);

    fromDesc.fileSize = from.m_fileSize;
    fromDesc.overlapOn = true;
    if (!CreateCopyFile(fromName, &fromDesc, GENERIC_READ, kFromFlags, !fromIsShadow, 10)) {
        rc = COPY_OPEN_SOURCE_ERROR;
        lastError = ::GetLastError();
        goto copydone;
    }
    if (unprotect) {
        if (to.m_fileAttributes & kFILE_ATTRIBUTE_UNPROTECT) {
            attr = to.m_fileAttributes & (~kFILE_ATTRIBUTE_UNPROTECT);
            if (attr == 0) {
                attr = FILE_ATTRIBUTE_NORMAL;
            }
            if (!::SetFileAttributes(toName, attr & kFILE_ATTRIBUTE_SETABLE)) {
                SetReasonCode(60);
                rc = COPY_SET_TARGET_FILE_ATTRIBUTES_ERROR;
                lastError = ::GetLastError();
                goto copydone;
            }
        }
    }
    toDesc.fileSize = fromDesc.fileSize;
    toDesc.overlapOn = true;
    if (!CreateCopyFile(toName, &toDesc, GENERIC_WRITE, kToFlags, false, 20)) {
        rc = COPY_CREATE_TARGET_FILE_ERROR;
        lastError = ::GetLastError();
        goto copyloopdone;
    }
    CurrentOffset = 0;
    CurrentBytesLeft = fromDesc.fileSize;
    readSize = (DWORD)bufferSize;
    if (CurrentBytesLeft < (DWORD)bufferSize) {
        readSize = (DWORD)CurrentBytesLeft;
    }
    if (readSize) {
        if (!StartRead(&fromDesc, buffer1, readSize, CurrentOffset, 70)) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ::GetLastError();
            goto copyloopdone;
        }
        if (!WaitIO(&fromDesc, 110)) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ::GetLastError();
            goto copyloopdone;
        }
        if (readSize != fromDesc.bytesXfered) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ERROR_HANDLE_EOF;
            SetReasonCode(115);
            goto copyloopdone;
        }
        do {
            wBufSize = readSize;
            if (!StartWrite(&toDesc, buffer1, wBufSize, CurrentOffset)) {
                rc = COPY_WRITE_TARGET_FILE_ERROR;
                lastError = ::GetLastError();
                break;
            }
            CurrentOffset += readSize;
            CurrentBytesLeft -= readSize;
            readSize = (DWORD)bufferSize;
            if (CurrentBytesLeft < (DWORD)bufferSize) {
                readSize = (DWORD)CurrentBytesLeft;
            }
            if (readSize) {
                if (!StartRead(&fromDesc, buffer2, readSize, CurrentOffset, 80)) {
                    rc = COPY_READ_SOURCE_FILE_ERROR;
                    lastError = ::GetLastError();
                    break;
                }
            }
            if (!WaitIO(&toDesc, 111)) {
                rc = COPY_WRITE_TARGET_FILE_ERROR;
                lastError = ::GetLastError();
                break;
            }
            if (wBufSize != toDesc.bytesXfered) {
                rc = COPY_WRITE_TARGET_FILE_ERROR;
                lastError = ERROR_HANDLE_EOF;
                SetReasonCode(118);
                break;
            }
            if (update) {
                job->m_counts.m_bytesUpdatedCount += (UINT64)wBufSize;
                CRun::Singleton().m_counts.m_bytesUpdatedCount += (UINT64)wBufSize;
            }
            else {
                job->m_counts.m_bytesCopiedCount += (UINT64)wBufSize;
                CRun::Singleton().m_counts.m_bytesCopiedCount += (UINT64)wBufSize;
            }
            if (readSize == 0) {
                break;
            }
            if (!WaitIO(&fromDesc, 112)) {
                rc = COPY_READ_SOURCE_FILE_ERROR;
                lastError = ::GetLastError();
                break;
            }
            if (readSize != fromDesc.bytesXfered) {
                rc = COPY_READ_SOURCE_FILE_ERROR;
                lastError = ERROR_HANDLE_EOF;
                SetReasonCode(119);
                break;
            }
            // Exchange buffer1 and buffer2
            {
                LPVOID buffert;
                buffert = buffer1;
                buffer1 = buffer2;
                buffer2 = buffert;
            }
        }
        while (true);
    }
    if (rc == COPY_NO_ERROR) {
        if (!::SetFileTime(toDesc.h, &from.m_creationTime, &from.m_lastAccessTime, &from.m_lastWriteTime)) {
            rc = COPY_SET_TARGET_FILE_TIME_ERROR;
            lastError = ::GetLastError();
            SetReasonCode(120);
            goto copyloopdone;
        }
        if (m_supportsPermissions && InSyncApp.m_adminPriviledge && m_parentJob->m_changedFileACLs) {
            CopyFileACL(fromDesc.h, toDesc.h);
        }
    }

copyloopdone:
    CloseCopyFile(&toDesc);
    if (rc == COPY_NO_ERROR) {
        if (from.m_fileAttributes & (~FILE_ATTRIBUTE_NORMAL)) {
            if (!::SetFileAttributes(toName, from.m_fileAttributes & kFILE_ATTRIBUTE_SETABLE)) {
                rc = COPY_RESTORE_TARGET_FILE_ATTRIBUTES_ERROR;
                lastError = ::GetLastError();
                SetReasonCode(121);
                goto copydone;
            }
        }
    }

copydone:
    CloseCopyFile(&fromDesc);
    ::SetLastError(lastError);
    return rc;
}

CopyFileError CTrgNode::VerifyAFile(LPCTSTR fromName, LPCTSTR toName, const UINT64 fileSize, PVOID buffer,
                                    const bool fromIsShadow) {
    DWORD lastError = NO_ERROR, bufSize;
    CopyFileError rc = COPY_NO_ERROR;
    FILEDESC fromDesc, toDesc;
    UINT64 CurrentOffset, CurrentBytesLeft;
    const DWORD kFromFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_READONLY | FILE_FLAG_BACKUP_SEMANTICS;
    const DWORD kToFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_READONLY | FILE_FLAG_BACKUP_SEMANTICS;
    UINT64 bufferSize = CRun::kMaxThreadBufferSize;
    LPVOID buffer1 = buffer;
    LPVOID buffer2 = NULL;

    if (buffer1 == NULL) {
        SetReasonCode(131);
        return COPY_NO_MEMORY;
    }
    buffer2 = (PCHAR)buffer1 + bufferSize;

    InitDesc(&fromDesc);
    InitDesc(&toDesc);

    toDesc.overlapped.hEvent = NULL;
    fromDesc.overlapped.hEvent = NULL;
    fromDesc.fileSize = fileSize;
    fromDesc.overlapOn = false;
    if (!CreateCopyFile(fromName, &fromDesc, GENERIC_READ, kFromFlags, !fromIsShadow, 30)) {
        rc = COPY_OPEN_SOURCE_ERROR;
        lastError = ::GetLastError();
        goto verifydone;
    }
    toDesc.overlapOn = false;
    if (!CreateCopyFile(toName, &toDesc, GENERIC_READ, kToFlags, false, 50)) {
        rc = COPY_OPEN_TARGET_ERROR;
        lastError = ::GetLastError();
        goto verifydone2;
    }
    CurrentOffset = 0;
    CurrentBytesLeft = fileSize;
    bufSize = (DWORD)bufferSize;
    if (CurrentBytesLeft < bufferSize) {
        bufSize = (DWORD)CurrentBytesLeft;
    }
    while (bufSize) {
        if (!StartRead(&fromDesc, buffer1, bufSize, CurrentOffset, 90)) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ::GetLastError();
            break;
        }
        if (!StartRead(&toDesc, buffer2, bufSize, CurrentOffset, 100)) {
            rc = COPY_READ_TARGET_FILE_ERROR;
            lastError = ::GetLastError();
            break;
        }
        if (!WaitIO(&fromDesc, 113)) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ::GetLastError();
            break;
        }
        if (!WaitIO(&toDesc, 114)) {
            rc = COPY_READ_TARGET_FILE_ERROR;
            lastError = ::GetLastError();
            break;
        }
        if (bufSize != fromDesc.bytesXfered) {
            rc = COPY_READ_SOURCE_FILE_ERROR;
            lastError = ERROR_HANDLE_EOF;
            SetReasonCode(132);
            break;
        }
        if (bufSize != toDesc.bytesXfered) {
            rc = COPY_READ_TARGET_FILE_ERROR;
            lastError = ERROR_HANDLE_EOF;
            SetReasonCode(133);
            break;
        }
        if (memcmp(buffer1, buffer2, bufSize)) {
            rc = COPY_VERIFY_FAILURE;
            lastError = ERROR_SUCCESS;
            SetReasonCode(134);
            break;
        }
        CurrentOffset += bufSize;
        CurrentBytesLeft -= bufSize;
        bufSize = (DWORD)bufferSize;
        if (CurrentBytesLeft < bufferSize) {
            bufSize = (DWORD)CurrentBytesLeft;
        }
    }

    CloseCopyFile(&toDesc);

verifydone2:
    CloseCopyFile(&fromDesc);

verifydone:
    ::SetLastError(lastError);
    return rc;
}
