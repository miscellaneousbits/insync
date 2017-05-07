#pragma once

class CScopedLock
{
public:
    CScopedLock(CCriticalSection *c)
    {
        m_critical = c;
        m_critical->Lock();
    }

    ~CScopedLock()
    {
        m_critical->Unlock();
    }

private:
    CCriticalSection *m_critical;
};

class CTrgScopedLogLock : CScopedLock
{
public:
    CTrgScopedLogLock(CTrgNode *trg, LogFlagsType flags) : CScopedLock(trg->m_parentJob->LogLock())
    {
        trg->m_parentJob->LogFlags(flags);
    }
};

