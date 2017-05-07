#pragma once

class CTrgNode;
class CMergeTaskWork;

class CVssHelper;

class CRun
{
public:
    CRun();
    static CRun &Singleton()
    {
        return *m_this;
    }
    void Start(const bool simulate);
    void ScheduleDirMerge(FileDataList &, FileDataList &, const CFilePath &, const CFilePath &,
                          CTrgNode *);
    void ScheduleBackupOrSync(const CFilePath &, const CFilePath &, const bool, const bool, CTrgNode *);
    void IncrementRunningThreads()
    {
        m_activeThreadCount++;
    }
    void DecrementRunningThreads()
    {
        m_activeThreadCount--;
    }
    UINT CJobsThread();
    bool CheckDependencies(void *);
    bool CheckDependency(void *);

    bool JobsActive()
    {
        return m_jobsActive;
    }

    CCounts m_counts;
    volatile atomic_ulong m_activeThreadCount;
    CVssHelper *m_vss;
    HANDLE m_hRun;
    PCHAR m_threadBuffer;
    CCriticalSection m_threadBufferLock;
    static const UINT64 kMaxThreadBufferSize = 64 * 1024;

private:
    static CRun *m_this;
    bool m_jobsActive;
    CThreadPool<CWorker> *m_threadPool;
    PVOID m_poolBuffer;
};

class CTaskWork
{
public:
    virtual ~CTaskWork()
    {
    }

    typedef enum {
        mergeType = true,
        backupOrSyncType = false
    } WorkType;

    void Process();

    bool m_workType;
    CTrgNode *m_trg;
    CFilePath m_from;
    CFilePath m_to;

protected:
    CTaskWork(bool wType, const CFilePath &from, const CFilePath &to, CTrgNode *trg)
    {
        m_workType = wType;
        m_from = from;
        m_to = to;
        m_trg = trg;
    }
};

class CMergeTaskWork : public CTaskWork
{
public:
    CMergeTaskWork(FileDataList &fromList, FileDataList &toList, const CFilePath &from,
                   const CFilePath &to, CTrgNode *trg)
        : CTaskWork(mergeType, from, to, trg)
    {
        m_fromList = fromList;
        m_toList = toList;
    }
    ~CMergeTaskWork()
    {
    }

    FileDataList m_fromList;
    FileDataList m_toList;
};

class CBackupOrSyncTaskWork : public CTaskWork
{
public:
    CBackupOrSyncTaskWork(const CFilePath &from, const CFilePath &to, const bool fromExists,
                          const bool toExists, CTrgNode *trg)
        : CTaskWork(backupOrSyncType, from, to, trg)
    {
        m_fromExists = fromExists;
        m_toExists = toExists;
    }
    ~CBackupOrSyncTaskWork()
    {
    }

    bool m_fromExists;
    bool m_toExists;
};

