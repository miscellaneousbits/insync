#pragma once

class CWorker
{
public:
    typedef TaskWorkPtr RequestType;

    CWorker();

    BOOL Initialize(void * /*pvParam*/);

    void Terminate(void * /*pvParam*/);

    void Execute(RequestType dw, void * /*pvParam*/, OVERLAPPED * /*pOverlapped*/);

private:

    static CRun *m_run;
    PVOID m_copyBuffer = nullptr;

}; // CWorker
