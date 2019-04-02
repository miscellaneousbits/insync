#pragma once

class CExecutable
{
public:
    CExecutable();
    CExecutable(const CExecutable &e);
    void Start(LPTHREAD_START_ROUTINE proc);

    HANDLE m_winThread;
};

class CJobNode;
class CSrcNode;
class CTrgNode;
class CRun;

class CExecutableWithInclude : public CExecutable
{
public:
    CExecutableWithInclude();
    CExecutableWithInclude(const CExecutableWithInclude &e);

    CString m_include;
    CString m_exclude;
};

typedef enum CopyFileError {
    COPY_NO_ERROR = 0,
    COPY_OPEN_SOURCE_ERROR,
    COPY_OPEN_TARGET_ERROR,
    COPY_CREATE_TARGET_FILE_ERROR,
    COPY_READ_SOURCE_FILE_ERROR,
    COPY_WRITE_TARGET_FILE_ERROR,
    COPY_READ_TARGET_FILE_ERROR,
    COPY_SET_TARGET_FILE_TIME_ERROR,
    COPY_SET_TARGET_FILE_ATTRIBUTES_ERROR,
    COPY_RESTORE_TARGET_FILE_ATTRIBUTES_ERROR,
    COPY_VERIFY_FAILURE,
    COPY_NO_MEMORY,
    COPY_NUM_ERRORS
} CopyFileError;

class CTaskWork;

typedef CTaskWork *TaskWorkPtr;

DWORD TrgThread(LPVOID pParam);

struct CFilePathCompare {
    bool operator()(const CFilePath &x, const CFilePath &y) const
    {
        return x.Display().MakeUpper() < y.Display().MakeUpper();
    }
};

typedef set<CFilePath, CFilePathCompare, std::allocator<int>> FilePathSet;
typedef FilePathSet::iterator FilePathSetIter;

class CTrgNode : public CExecutableWithInclude
{
public:
    CTrgNode(CJobNode *, CSrcNode *);
    CTrgNode(const CTrgNode &);
    bool operator ==(const CTrgNode &) const;
    void MergeFiles(FileDataList &, FileDataList &, const CFilePath &, const CFilePath &, PVOID buffer);
    void MergeDirs(FileDataList &, FileDataList &, const CFilePath &, const CFilePath &, PVOID buffer);
    void BackupOrSync(const CFilePath &, const CFilePath &, PVOID buffer, const bool, const bool);
    void CopyAndLogACLUpdateIfDifferent(const CFilePath &, const CFilePath &);
    UINT CTrgThread();


    CString m_name;
    CSrcNode *m_parentSrc;
    CJobNode *m_parentJob;
    volatile atomic_long m_pendingTasks;
    HANDLE m_doneEvent;

    bool m_useVolume : 1;
    bool m_dontDelete : 1;

private:
    void(__thiscall CTrgNode::*FileInBoth)(const CFileData &, const CFileData &, const CFilePath &,
                                           const CFilePath &,
                                           PVOID buffer);
    void(__thiscall CTrgNode::*DirInBoth)(const CFileData &, const CFileData &, const CFilePath &,
                                          const CFilePath &, PVOID buffer);
    void SetThreadId();
    void FatalError(LPCTSTR);
    bool AbortedByUser();
    void FileError(LPCTSTR, LPCTSTR, const bool);

    int GetReasonCode();
    void SetReasonCode(const int r);
    void TrgWriteToLog(LPCTSTR);
    void FileErrorWithMessage(LPCTSTR, LPCTSTR, const bool, const LPVOID, const LPVOID);
    void VerifyError(LPCTSTR, LPCTSTR, const bool);
    void NonFatalError(const bool, LPCTSTR);
    bool DeleteAFile(LPCTSTR file, LPCTSTR msg);
    bool UnprotectAFile(LPCTSTR file, const DWORD attr, LPCTSTR msg);

    typedef struct fileDesc {
        OVERLAPPED overlapped;
        UINT64 fileSize;
        HANDLE fileLock;
        HANDLE h;
        DWORD bytesXfered;
        bool overlapOn : 1;
        bool ioPending : 1;
    } FILEDESC, *PFILEDESC;

    bool CreateCopyFile(LPCTSTR, PFILEDESC, DWORD, DWORD, bool, int);
    void CloseCopyFile(PFILEDESC desc);
    bool StartRead(PFILEDESC, LPVOID, DWORD, UINT64, int);
    bool StartWrite(PFILEDESC, LPCVOID, DWORD, UINT64);
    bool WaitIO(PFILEDESC desc, int reasonBase);
    void InitDesc(PFILEDESC desc);
    bool RemoveADirectory(LPCTSTR folder, LPCTSTR msg);
    bool IncludedFile(LPCTSTR);
    bool ScanDir(FileDataList &, FileDataList &, const CFilePath &, const bool);
    bool IncludeFolder(const CFileData &, const CFilePath &, const CFilePath &, CFilePath &,
                       CFilePath &);
    bool ExcludedFolder(const CFilePath &path);
    void RenameTargetFileOrDirIfDifferent(const CFilePath &, const CFileData &, const CFileData &);
    void MirrorFileInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                          PVOID buffer);
    void MirrorFileInTrgOnly(const CFileData &, const CFilePath &);
    void FileInSrcOnly(const CFileData &, const CFilePath &, const CFilePath &, PVOID buffer);
    void MirrorDirInTrgOnly(const CFileData &, const CFilePath &);
    void MirrorDirInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                         PVOID buffer);
    void DirInSrcOnly(const CFileData &, const CFilePath &, const CFilePath &, PVOID buffer);
    void BackupFileInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                          PVOID buffer);
    void BackupDirInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                         PVOID buffer);
    void TwoWayFileInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                          PVOID buffer);
    void TwoWayDirInBoth(const CFileData &, const CFileData &, const CFilePath &, const CFilePath &,
                         PVOID buffer);
    CopyFileError CopyAFile(LPCTSTR, LPCTSTR, const CFileData &, const CFileData &, const bool,
                            CJobNode *, PVOID buffer,
                            const bool, const bool);
    CopyFileError VerifyAFile(LPCTSTR, LPCTSTR, const UINT64, PVOID buffer, const bool);
    CString FileLogMsg(LPCTSTR, UINT64, const CFilePath &, const CFilePath &);
    CString FileLogMsg(LPCTSTR, UINT64, const CFilePath &);

    static const DWORD k_perWaitPass = 16;

    LPTSTR m_fatal;
    wregex *m_includes, *m_excludes;
    FilePathSet m_excludedFolders;
    RunModes m_effectiveMode;
    DWORD m_fileErrors;
    int m_reasonCode;
    USHORT m_prefLen;
    TCHAR m_drive;
    CRun *m_run;

    bool m_subDir : 1;
    bool m_simulate : 1;
    bool m_threadVerify : 1;
    bool m_supportsPermissions : 1;
};

typedef list<CTrgNode *, std::allocator<int>> TrgNodeList;
typedef TrgNodeList::iterator TrgNodeListIter;
typedef TrgNodeList::const_iterator ConstTrgNodeListIter;

DWORD SrcThread(LPVOID pParam);

class CSrcNode : public CExecutableWithInclude
{
public:
    CSrcNode(CJobNode *);
    CSrcNode(const CSrcNode &);
    ~CSrcNode();
    bool operator ==(const CSrcNode &) const;
    UINT CSrcThread();

    TrgNodeList m_childrenList;
    StrSet m_excludedSrcDirList;

    CString m_name;
    CJobNode *m_parentJob;
    USHORT m_prefLen;
    TCHAR m_drive;
    bool m_subdir : 1;
    bool m_useVolume : 1;
};

typedef list<CSrcNode *, std::allocator<int>> SrcNodeList;
typedef SrcNodeList::iterator SrcNodeListIter;
typedef SrcNodeList::const_iterator ConstSrcNodeListIter;
typedef list<HANDLE, std::allocator<int>> HandleList;
typedef HandleList::iterator HandleListIter;
typedef HandleList::const_iterator ConstHandleListIter;
typedef list<void *, std::allocator<int>> VoidPtrList;
typedef VoidPtrList::iterator VoidPtrListIter;
typedef VoidPtrList::const_iterator ConstVoidPtrListIter;

DWORD JobThread(LPVOID pParam);

class CJobNode : public CExecutable
{
public:
    CJobNode();
    CJobNode(const CJobNode &node);
    ~CJobNode();
    bool operator ==(const CJobNode &other) const;
    UINT CJobThread();
    void CreateLogFile(const bool append);
    void CloseLogFile();
    void Active(const bool, const CString);
    void WriteToLog(LPCTSTR);
    void WriteToLogVS(LPCTSTR, ...);
    void WriteToLogVSNewLine(LPCTSTR, ...);
    void WriteCountsToLog();
    void TaskError(const bool, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR);
    void FatalTaskError(const bool, LPCTSTR, LPCTSTR, LPCTSTR = NULL, LPCTSTR = NULL);
    void NonFatalTaskError(const bool, LPCTSTR, LPCTSTR, LPCTSTR = NULL, LPCTSTR = NULL);
    void IncrementCount(const enum CounterIndex cx);
    void AddToCount(const enum CounterIndex cx, UINT64 n);
    CCriticalSection *LogLock()
    {
        return &m_logLock;
    }
    void LogFlags(const LogFlagsType flags)
    {
        m_logFlags = flags;
    }

    SrcNodeList m_childrenList;
    StrSet m_runAfterList;
    CString m_name;
    CString m_logName;
    CString m_lastLogFileName;
    int m_listBoxIndx;
    HandleList m_signalAfterList;
    HandleList m_waitAfterList;
    VoidPtrList m_beforeJobs;
    CString m_runState;
    RunModes m_mode;

    bool m_selected : 1;
    bool m_running : 1;
    bool m_errPopup : 1;
    bool m_logAppend : 1;
    bool m_ioVerify : 1;
    bool m_logOnlyError : 1;
    bool m_logFolder : 1;
    bool m_logSkipped : 1;
    bool m_changedFileACLs : 1;
    bool m_unchangedFileACLs : 1;
    bool m_folderACLs : 1;
    bool m_promptBeforeRunning : 1;
    bool m_dependencyVisited : 1;

    volatile bool m_firstAbort;
    CCounts m_counts;

private:
    CCriticalSection m_logLock;
    char *m_utf8Buf;
    HANDLE m_logFile;
    LogFlagsType m_logFlags;
    DWORD m_utf8BufLen;
};

typedef list<CJobNode *, std::allocator<int>> JobNodeList;
typedef JobNodeList::iterator JobNodeListIter;
