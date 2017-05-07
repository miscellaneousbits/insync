#pragma once

extern LPCTSTR kInSyncFileType;
extern LPCTSTR kInSyncName;
extern LPCTSTR kCopyrightPrefix;
extern LPCTSTR kInSyncUID;
extern LPCTSTR kInSyncRegistryHive;
extern LPCTSTR kNullString;

const enum JobErrors {
    kINSYNC_NO_ERROR = 0,
    kINSYNC_FILE_ERROR,
    kINSYNC_JOB_ERROR
};

const enum endJobAction {
    NoneAction = 0,
    SleepAction,
    HibernateAction,
    ShutdownAction
};

const CString kRunModeNames[NUMBER_OF_SYNC_MODES] = { kNullString, _T("Mirror"), _T("2-Way"), _T("Backup") };

class CInSyncApp : public CWinAppEx
{
public:
    CInSyncApp();
    virtual int ExitInstance();
    virtual BOOL InitInstance();

    typedef enum RegType {
        RegTypeStandard = 0,
        RegTypeTrial,
        RegTypeEnterprise
    } RegType;

    void AppRC(const DWORD rc);
    DWORD AppRC() const;
    void ClearAppRC();

    enum endJobAction m_endJobAction;

    StrSet m_autoNameRoot;
    CString m_insyncVersion;
    CString m_copyright;
    CString m_optFilename;
    CString m_optCommonFilename;
    CString m_computerName;
    CString m_userName;
    CString m_userKey;
    int m_startupDelay;
    HANDLE m_currentProcess;
    USHORT m_scale;

    bool m_optFileNameFromConfig : 1;
    bool m_globalAppData : 1;
    bool m_simulateParm : 1;
    bool m_simulate : 1;
    bool m_adminPriviledge : 1;
    bool m_objectPriviledge : 1;

protected:
    DECLARE_MESSAGE_MAP()

private:
    DWORD m_appRc;
    CDlgOccManager m_occManager;
};

extern CInSyncApp InSyncApp;
