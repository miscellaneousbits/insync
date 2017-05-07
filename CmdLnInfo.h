#pragma once

class CInSyncCmdLineInfo : public CCommandLineInfo
{
public:
    CInSyncCmdLineInfo();
    void ParseParam(LPCTSTR, BOOL, BOOL);

    bool m_autoRun : 1;
    bool m_nextIsConfig : 1;
    bool m_nextIsDelay : 1;
};

