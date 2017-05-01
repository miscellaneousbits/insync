#pragma once
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

typedef enum RunModes {
    FROMPARENT = 0,
    MIRROR_SYNC_MODE,
    TWOWAY_SYNC_MODE,
    BACKUP_SYNC_MODE,
    NUMBER_OF_SYNC_MODES
} RunModes;

const RunModes kDefaultJobRunMode = MIRROR_SYNC_MODE;

const DWORD kFILE_ATTRIBUTE_SETABLE =
    (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL | \
     FILE_ATTRIBUTE_NOT_CONTENT_INDEXED | FILE_ATTRIBUTE_OFFLINE | \
     FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY);

class CFileData {

public:
    CFileData(const WIN32_FIND_DATA &);
    CFileData(LPCTSTR, const DWORD);
    CFileData(const CFileData &);

    FILETIME m_creationTime;
    FILETIME m_lastAccessTime;
    FILETIME m_lastWriteTime;
    UINT64 m_fileSize;
    CString m_name;
    DWORD m_fileAttributes;
};

typedef list<CFileData, std::allocator<int>> FileDataList;
typedef FileDataList::iterator FileDataListIter;

typedef list<CString, std::allocator<int>> StrList;
typedef StrList::iterator StrListIter;
typedef StrList::const_iterator ConstStrListIter;

struct CStrCompare {
    bool operator()(const CString &x, const CString &y) const {
        return CString(x).MakeUpper() < CString(y).MakeUpper();
    }
};

typedef set<CString, CStrCompare, std::allocator<int>> StrSet;
typedef StrSet::iterator StrSetIter;
typedef StrSet::const_iterator ConstStrSetIter;

class CCounts {
public:
    void Clear();

    volatile atomic_ullong m_filesSkippedCount;
    volatile atomic_ullong m_filesCopiedCount;
    volatile atomic_ullong m_filesUpdatedCount;
    volatile atomic_ullong m_filesDeletedCount;
    volatile atomic_ullong m_fileErrorCount;
    volatile atomic_ullong m_foldersCopiedCount;
    volatile atomic_ullong m_foldersDeletedCount;
    volatile atomic_ullong m_bytesSkippedCount;
    volatile atomic_ullong m_bytesCopiedCount;
    volatile atomic_ullong m_bytesUpdatedCount;
    volatile atomic_ullong m_bytesDeletedCount;
};

typedef struct LogFlagsType {
    bool LogFlagSkipped : 1;
    bool LogFlagFolder : 1;
    bool LogFlagError : 1;
} LogFlagsType;

const LogFlagsType kLOGALWAYS = { false, false, true };
const LogFlagsType kLOGFILE = { false, false, false };
const LogFlagsType kLOGSKIP = { true, false, false };
const LogFlagsType kLOGFOLDER = { false, true, false };
