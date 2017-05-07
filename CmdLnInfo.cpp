#include "stdafx.h"

void CInSyncCmdLineInfo::ParseParam(LPCTSTR pszParam, BOOL bFlag, BOOL /*bLast*/)
{
    if (bFlag) {
        m_nextIsConfig = false;
        m_nextIsDelay = false;

        if (_tcsicmp(pszParam, _T("simulate")) == 0) {
            InSyncApp.m_simulateParm = true;
            return;
        }

        if (_tcsicmp(pszParam, _T("global")) == 0) {
            InSyncApp.m_globalAppData = true;
            return;
        }

        if (_tcsicmp(pszParam, _T("shutdown")) == 0) {
            InSyncApp.m_endJobAction = ShutdownAction;
            return;
        }

        if (_tcsicmp(pszParam, _T("sleep")) == 0) {
            InSyncApp.m_endJobAction = SleepAction;
            return;
        }

        if (_tcsicmp(pszParam, _T("hibernate")) == 0) {
            InSyncApp.m_endJobAction = HibernateAction;
            return;
        }

        CString s(pszParam);

        if (_tcsicmp(s.Left(5), _T("delay")) == 0) {
            if (_tcslen(s) > 5)
                if (s[5] == _T(':')) {
                    InSyncApp.m_startupDelay = _tstoi(s.Right(s.GetLength() - 6));
                } else {
                    m_nextIsDelay = true;
                }

            return;
        }

        if (_tcsicmp(pszParam, _T("config")) == 0) {
            m_nextIsConfig = true;
            return;
        }
    } else {
        if (m_nextIsConfig) {
            InSyncApp.m_optFilename = pszParam;
            InSyncApp.m_optFileNameFromConfig = true;
        } else if (m_nextIsDelay) {
            InSyncApp.m_startupDelay = _tstoi(pszParam);
        } else {
            InSyncApp.m_autoNameRoot.insert(pszParam);
            m_autoRun = true;
        }

        m_nextIsConfig = false;
        m_nextIsDelay = false;
    }
}

CInSyncCmdLineInfo::CInSyncCmdLineInfo() :
    m_autoRun(false),
    m_nextIsConfig(false),
    m_nextIsDelay(false) {};
