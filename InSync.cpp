#include "stdafx.h"

LPCTSTR kInSyncFileType = _T("isy");
LPCTSTR kInSyncName = _T("InSync");
LPCTSTR kCopyrightPrefix = _T("InSync \u00a9 Dillobits Software ");
LPCTSTR kInSyncUID = _T("BFDB0CDE-583B-4258-8865-381B20087600-");
LPCTSTR kInSyncRegistryHive = _T("Software\\Dillobits\\");
LPCTSTR kNullString = _T("");

/////////////////////////////////////////////////////////////////////////////
// CInSyncApp

BEGIN_MESSAGE_MAP(CInSyncApp, CWinApp)
END_MESSAGE_MAP()

CInSyncApp InSyncApp;

/////////////////////////////////////////////////////////////////////////////
// CInSyncApp construction

CInSyncApp::CInSyncApp()
    : m_startupDelay(0),
      m_appRc(0),
      m_globalAppData(false),
      m_optFileNameFromConfig(false),
      m_simulateParm(false),
      m_endJobAction(NoneAction)
{
    EnableHtmlHelp();
}

static int dpiY = 0;

/////////////////////////////////////////////////////////////////////////////
// CInSyncApp initialization

static bool SetPrivilege(HANDLE hToken, LPCTSTR name)
{
    TOKEN_PRIVILEGES tkp;

    // get the LUID for shutdown privilege.
    if (!::LookupPrivilegeValue(NULL, name, &tkp.Privileges[0].Luid))
        return false;

    tkp.PrivilegeCount = 1;  // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    // get shutdown privilege for this process.
    return B2b(::AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0));
}

BOOL CInSyncApp::InitInstance()
{
    m_currentProcess = ::GetCurrentProcess();

#if 0

    // Not needed till mfc becomes per monotor DPI aware
    if (IsWindows8Point1OrGreater()) {
        HMODULE module = ::LoadLibrary(_T("Shcore.dll"));

        if (module) {
            typedef HRESULT(WINAPI * SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS);
            SetProcessDpiAwareness setProcessDpiAwareness = (SetProcessDpiAwareness)
                    ::GetProcAddress(module, "SetProcessDpiAwareness");

            if (setProcessDpiAwareness)
                setProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

            ::FreeLibrary(module);
        }
    }

#endif

    // Does not exist in Vista before SP1
    // Enable DEP
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    {
        HMODULE module = ::LoadLibrary(_T("Kernel32.dll"));

        if (module) {
            typedef BOOL(WINAPI * SetProcessDEPPolicyFunction)(DWORD);
            SetProcessDEPPolicyFunction setProcessDEPPolicy = (SetProcessDEPPolicyFunction)
                    ::GetProcAddress(module, "SetProcessDEPPolicy");

            if (setProcessDEPPolicy)
                setProcessDEPPolicy(PROCESS_DEP_ENABLE | PROCESS_DEP_DISABLE_ATL_THUNK_EMULATION);

            ::FreeLibrary(module);
        }
    }

    // Deal with UAC stuff
    (void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    ClearAppRC();

    m_copyright = CString(kCopyrightPrefix) + CString(_T(__DATE__)).Right(4);

    CInSyncCmdLineInfo cmdLineInfo;
    ParseCommandLine(cmdLineInfo);

    TCHAR moduleName[MAX_PATH + MAX_PATH];
    ::GetModuleFileName(NULL, moduleName, _countof(moduleName));
    CVersionInfo version(moduleName);
    m_insyncVersion = version.m_strFixedProductVersion;

    CString installFolderName;
    installFolderName = moduleName;
    installFolderName = installFolderName.Left(installFolderName.GetLength() - 3);

    CString helpFilePath;
    helpFilePath = installFolderName + _T("chm");
    m_pszHelpFilePath = _tcsdup(helpFilePath);

    DWORD lName = _countof(moduleName);

    if (!::GetComputerName(moduleName, &lName))
        moduleName[0] = 0;

    m_computerName = moduleName;

    m_adminPriviledge = IsUserAdmin() && (GetElevationType() != TokenElevationTypeLimited);
    m_objectPriviledge = false;
    HANDLE hToken = NULL;

    if (m_adminPriviledge) {
        // get the current process token handle
        if (::OpenProcessToken(InSyncApp.m_currentProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                               &hToken)) {
            m_objectPriviledge = SetPrivilege(hToken, SE_SECURITY_NAME) &&
                                 SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME) &&
                                 SetPrivilege(hToken, SE_BACKUP_NAME) &&
                                 SetPrivilege(hToken, SE_RESTORE_NAME) &&
                                 SetPrivilege(hToken, SE_SHUTDOWN_NAME);
        }

        // if we are running elevated we need to reconnect network drives under Vista or later
        ReconnectNetworkDrives();
    }

    CMainDlg *dlg = new CMainDlg;
    dlg->m_autoRun = cmdLineInfo.m_autoRun;
    m_pMainWnd = dlg;

    // load the run time configuration specs
    if (!m_optFileNameFromConfig) {
        TCHAR szAppData[MAX_PATH + MAX_PATH];

        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, szAppData)))
            m_optCommonFilename = CString(szAppData) + _T("\\Dillobits Software\\InSync\\InSync.xml");

        if (SUCCEEDED(SHGetFolderPath(NULL, m_globalAppData ? CSIDL_COMMON_APPDATA : CSIDL_APPDATA, NULL, 0,
                                      szAppData))) {
            do {
                CString appdata = CString(szAppData) + _T("\\Dillobits Software");

                if ((!PathIsDirectory(appdata)) && (!::CreateDirectory(appdata, NULL)))
                    break;

                appdata += _T("\\InSync");

                if ((!PathIsDirectory(appdata)) && (!::CreateDirectory(appdata, NULL)))
                    break;

                m_optFilename = appdata + _T("\\InSync.xml");
            } while (false);
        }
    }

    // extract the command line
    dlg->m_lpCmdline = m_lpCmdLine;

    DWORD major, minor;
    LPBYTE pinfoRawData;

    if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData)) {
        WKSTA_INFO_100 *pworkstationInfo = (WKSTA_INFO_100 *)pinfoRawData;
        major = pworkstationInfo->wki100_ver_major;
        minor = pworkstationInfo->wki100_ver_minor;
        ::NetApiBufferFree(pinfoRawData);
    } else {
        major = 0;
        minor = 0;
    }

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
    AfxEnableControlContainer(&m_occManager);

    // Ok, we are ready to go
    dlg->DoModal();
    delete dlg;

    if (hToken)
        ::CloseHandle(hToken);

    return false;
}

int CInSyncApp::ExitInstance()
{
    CWinApp::ExitInstance();
    return (int)m_appRc;
}


void CInSyncApp::AppRC(const DWORD rc)
{
    if (rc > m_appRc)
        m_appRc = rc;
}
DWORD CInSyncApp::AppRC() const
{
    return m_appRc;
}
void CInSyncApp::ClearAppRC()
{
    m_appRc = kINSYNC_NO_ERROR;
}

