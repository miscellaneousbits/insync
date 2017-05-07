#include "stdafx.h"
#include "MainDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

static const UINT iconPhases[18] = {
    IDI_ICON000, IDI_ICON020, IDI_ICON040, IDI_ICON060,
    IDI_ICON080, IDI_ICON100, IDI_ICON120, IDI_ICON140,
    IDI_ICON160, IDI_ICON180, IDI_ICON200, IDI_ICON220,
    IDI_ICON240, IDI_ICON260, IDI_ICON280, IDI_ICON300,
    IDI_ICON320, IDI_ICON340
};

CMainDlg *CMainDlg::m_this;

CMainDlg::CMainDlg(CWnd *pParent) :
    CInsyncDlg(IDD, HtmlInsync, pParent),
    m_ptl(NULL),
    m_shutdownBlockReasonCreate(NULL),
    m_shutdownBlockReasonDestroy(NULL),
    m_globalAbort(false),
    m_endSession(false),
    m_maxThreads(0),
    m_oldpos(0)
{
    m_this = this;
}

void CMainDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ABORT_JOBS_BUTTON, m_abortJobBtnCtl);
    DDX_Control(pDX, IDC_ADD_JOB_BUTTON, m_addJobBtnCtl);
    DDX_Control(pDX, IDC_CLONE_JOB_BUTTON, m_cloneJobBtnCtl);
    DDX_Control(pDX, IDC_CONFIG_STATIC, m_configStaticCtl);
    DDX_Control(pDX, IDC_TIPS_OFF_CHECK, m_tipsOffCheckCtl);
    DDX_Control(pDX, IDC_COPYRIGHT_STATIC, m_copyrightStaticCtl);
    DDX_Control(pDX, IDC_CRITERIA_STATIC, m_criteriaStaticCtl);
    DDX_Control(pDX, IDC_EDIT_JOB_BUTTON, m_editJobBtnCtl);
    DDX_Control(pDX, IDC_INSYNC_STATIC, m_insyncStaticCtl);
    DDX_Control(pDX, IDC_JOB_ERROR_POPUP_CHECK, m_errorPopupCheckCtl);
    DDX_Control(pDX, IDC_JOB_LIST, m_jobListCtl);
    DDX_Control(pDX, IDC_JOB_LOG_APPEND_CHECK, m_logAppendCheckCtl);
    DDX_Control(pDX, IDC_JOB_LOG_SUMMARY_CHECK, m_logSummaryCheckCtl);
    DDX_Control(pDX, IDC_JOB_LOG_SKIP_CHECK, m_logSkippedCheckCtl);
    DDX_Control(pDX, IDC_JOB_LOG_STATIC, m_jobLogStaticCtl);
    DDX_Control(pDX, IDC_JOB_LOG_SYNC_DIR_CHECK, m_logSyncDirCheckCtl);
    DDX_Control(pDX, IDC_JOB_MODE_COMBO, m_jobModeComboCtl);
    DDX_Control(pDX, IDC_JOB_VERIFY_CHECK, m_jobVerifyCheckCtl);
    DDX_Control(pDX, IDC_LED_ICON_STATIC, m_ledIconStaticCtl);
    DDX_Control(pDX, IDC_LED_STATIC, m_ledStaticCtl);
    DDX_Control(pDX, IDC_MANDALA_STATIC, m_mandalaStaticCtl);
    DDX_Control(pDX, IDC_MAX_THREADS_COMBO, m_maxThreadsComboCtl);
    DDX_Control(pDX, IDC_MAX_THREADS_STATIC, m_maxThreadsStaticCtl);
    DDX_Control(pDX, IDC_MIRROR_JOB_BUTTON, m_mirrorJobBtnCtl);
    DDX_Control(pDX, IDC_REMOVE_JOB_BUTTON, m_removeJobBtnCtl);
    DDX_Control(pDX, IDC_RUN_JOBS_BUTTON, m_runJobBtnCtl);
    DDX_Control(pDX, IDC_SIMULATE_JOBS_BUTTON, m_simulateJobBtnCtl);
    DDX_Control(pDX, IDC_STATIC_FOLDER_GROUP, m_folderGroupCtl);
    DDX_Control(pDX, IDC_STATIC_PROGRESS_GROUP, m_progressGroupCtl);
    DDX_Control(pDX, IDC_VIEW_LOG_BUTTON, m_viewLogBtnCtl);
    DDX_Control(pDX, IDC_JOB_GROUP_CTRL, m_jobGroupCtrl);
    DDX_Control(pDX, IDC_RUNNING_THREADS_PROGRESS, m_runningThreadsCtl);
    DDX_Control(pDX, IDC_JOB_PROMPT_BEFORE_RUNNING_CHECK, m_promptBeforeRunningCtl);
    DDX_Control(pDX, IDC_JOB_LOG_EDIT, m_jobLogEditCtl);
    DDX_Control(pDX, IDC_VERBOSE_LOG_CHECK, m_verboseLogCheck);
    DDX_Control(pDX, IDC_END_OF_RUN_STATIC, m_endOfRunStaticCtl);
    DDX_Control(pDX, IDC_END_OF_RUN_COMBO, m_endOfRunComboCtl);
}

BEGIN_MESSAGE_MAP(CMainDlg, CInsyncDlg)
    ON_BN_CLICKED(IDC_ABORT_JOBS_BUTTON, OnAbortJobsButton)
    ON_BN_CLICKED(IDC_ADD_JOB_BUTTON, OnAddJobButton)
    ON_BN_CLICKED(IDC_CLONE_JOB_BUTTON, OnCloneJobButton)
    ON_BN_CLICKED(IDC_TIPS_OFF_CHECK, OnBnClickedTipsOffCheck)
    ON_BN_CLICKED(IDC_EDIT_JOB_BUTTON, OnEditJobButton)
    ON_BN_CLICKED(IDC_MIRROR_JOB_BUTTON, OnMirrorJobButton)
    ON_BN_CLICKED(IDC_REMOVE_JOB_BUTTON, OnRemoveJobButton)
    ON_BN_CLICKED(IDC_RUN_JOBS_BUTTON, OnRunJobsButton)
    ON_BN_CLICKED(IDC_SIMULATE_JOBS_BUTTON, OnSimulateJobsButton)
    ON_BN_CLICKED(IDC_VIEW_LOG_BUTTON, OnBnClickedViewLogButton)
    ON_CBN_SELCHANGE(IDC_MAX_THREADS_COMBO, OnCbnSelchangeMaxThreadsCombo)
    ON_LBN_SELCHANGE(IDC_JOB_LIST, OnSelchangeJobList)
    ON_MESSAGE(WM_USER_JOBS_DONE, OnJobsDone)
    ON_MESSAGE(WM_USER_SET_LED, OnUserSetLed)
    ON_WM_CLOSE()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_SYSCOMMAND()
    ON_WM_TIMER()
    ON_WM_HELPINFO()
    ON_WM_ENDSESSION()
    ON_WM_QUERYENDSESSION()
    ON_WM_CTLCOLOR()
    ON_WM_ENDSESSION()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainDlg message handlers

int CMainDlg::InSyncMessageBox(bool useWnd, LPCTSTR lpszText, UINT nType, int iDefault,
                               LPCTSTR msgId) const
{

    int rc;

    if (!m_autoRun) {
        if (msgId) {
            HWND hWnd = useWnd ? this->m_hWnd : NULL;
            rc = ::SHMessageBoxCheck(hWnd, lpszText, _T("InSync"), nType, iDefault,
                                     CString(kInSyncUID) + msgId);
        } else
            rc = AfxMessageBox(lpszText, nType);
    } else {

        switch (nType & MB_TYPEMASK) {
        case MB_YESNO:
            rc = IDYES;
            break;

        default:
        case MB_OK:
        case MB_OKCANCEL:
            rc = IDOK;
            break;
        }
    }

    return rc;
}

void CMainDlg::ParseJobName(LPCTSTR s)
{
    JobNodeListIter jobNode;

    for (jobNode = m_jobList.begin(); jobNode != m_jobList.end(); ++jobNode) {
        if ((*jobNode)->m_name.CompareNoCase(s) == 0) {
            int listIndx = m_jobListCtl.FindStringExact(-1, (*jobNode)->m_name);

            if (listIndx != LB_ERR)
                m_jobListCtl.SetSel(listIndx);

            return;
        }
    }

    InSyncApp.AppRC(kINSYNC_JOB_ERROR);
}

void CMainDlg::ParseFileOrJobName(LPCTSTR s)
{
    size_t l, le;
    HANDLE hndl;
    LPTSTR cp, cp1, cp2;

    l = _tcslen(s);
    le = _tcslen(kInSyncFileType) + 1;

    if (l >= le) {
        LPCTSTR tcp = s + (l - le);

        if ((*tcp == _T('.')) && (_tcsicmp(kInSyncFileType, tcp + 1) == 0)) {
            if ((hndl = ::CreateFile(s, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL)) != INVALID_HANDLE_VALUE) {
                DWORD sl = ::GetFileSize(hndl, NULL);

                if (sl != INVALID_FILE_SIZE) {
                    cp = new TCHAR[sl];
                    DWORD tl;

                    if (::ReadFile(hndl, cp, sl, &tl, FALSE)) {
                        cp1 = cp;
                        // skip unicode prefix
                        PUCHAR cp3 = (PUCHAR)cp;

                        if ((*cp3 != 0xff) || (*(cp3 + 1) != 0xfe)) {
                            delete [] cp;
                            ::CloseHandle(hndl);
                            InSyncApp.AppRC(kINSYNC_JOB_ERROR);
                            return;
                        }

                        cp1++;
                        sl -= 2;
                        cp2 = cp1 + (sl / sizeof(TCHAR));

                        while (cp1 < cp2) {
                            tl = (DWORD)_tcslen(cp1);

                            if (tl == 0)
                                break;

                            ParseJobName(cp1);
                            cp1 += (tl + 1);
                        }
                    }

                    delete [] cp;
                }

                ::CloseHandle(hndl);
            }

            return;
        }
    }

    ParseJobName(s);
}


void CMainDlg::SetDialogIcon(WORD ix)
{
    if (!IsIconic())
        m_mandalaStaticCtl.SetIcon(m_mandala[ix]);

    SetIcon(m_mandala[ix], TRUE);            // Set big icon
    SetIcon(m_mandala_small[ix], FALSE);      // Set small icon
}

void CMainDlg::RegisterTaskbar()
{
    HRESULT(*sHGetPropertyStoreForWindow)(HWND hwnd, REFIID riid, void **ppv);

    PROPVARIANT pv;

    if (InitPropVariantFromString(kInSyncUID, &pv) != S_OK)
        return;

    IPropertyStore *pps;
    HMODULE hMod = ::LoadLibrary(_T("Shell32.dll"));

    if (hMod == NULL)
        return;

    do {
        sHGetPropertyStoreForWindow = (HRESULT(*)(HWND, REFIID, void **))
                                      ::GetProcAddress(hMod, "SHGetPropertyStoreForWindow");

        if (sHGetPropertyStoreForWindow == NULL)
            break;

        if (sHGetPropertyStoreForWindow(m_hWnd, IID_PPV_ARGS(&pps)) != S_OK)
            break;

        if (pps->SetValue(PKEY_AppUserModel_ID, pv) != S_OK)
            break;

        if (pps->Commit() != S_OK)
            break;

        if (CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3,
                             (void **)&m_ptl) != S_OK) {
            m_ptl = NULL;
            break;
        }
    } while (false);

    ::FreeLibrary(hMod);
}


BOOL CMainDlg::OnInitDialog()
{

    CInsyncDlg::OnInitDialog();

    // Get native resolution
    {
        int horizontalDPI = GetDeviceCaps(GetDC()->m_hDC, LOGPIXELSX);
        // Find the best fit
        const int nDpi = 4;
        int dpi[nDpi] = {
            96, 120, 144, 192
        };
        int scale[nDpi] = {
            100, 125, 150, 200
        };
        int bestIndex = 0;
        int bestvalue = INT_MAX;

        for (int d = 0; d < nDpi; d++) {
            int a = abs(dpi[d] - horizontalDPI);

            if (a < bestvalue) {
                bestvalue = a;
                bestIndex = d;
            }
        }

        InSyncApp.m_scale = (USHORT)scale[bestIndex];
    }
    m_endOfRunComboCtl.InsertString(NoneAction, L"None");
    m_endOfRunComboCtl.InsertString(SleepAction, L"Sleep");
    m_endOfRunComboCtl.InsertString(HibernateAction, L"Hibernate");
    m_endOfRunComboCtl.InsertString(ShutdownAction, L"Shutdown");
    m_endOfRunComboCtl.SetCurSel(InSyncApp.m_endJobAction);

    if (InSyncApp.m_adminPriviledge) {
        m_endOfRunComboCtl.EnableWindow();
        m_endOfRunStaticCtl.EnableWindow();
    }

    RegisterTaskbar();
    CenterWindow();
    BringWindowToTop();

    m_helpButtonCtl.SubclassDlgItem(IDHELP, this);
    m_mirrorJobBtnCtl.Init(IDI_MIRROR, m_pToolTip,
                           _T("Create a copy of the selected job with source and destination folders exchanged."));
    m_cloneJobBtnCtl.Init(IDI_CLONE, m_pToolTip,
                          _T("Create a copy of the selected job."));
    m_addJobBtnCtl.Init(IDI_ADD, m_pToolTip,
                        _T("Create a new job definition."));
    m_editJobBtnCtl.Init(IDI_EDIT, m_pToolTip,
                         _T("Edit the selected job definition."));
    m_removeJobBtnCtl.Init(IDI_REMOVE, m_pToolTip,
                           _T("Delete the selected job definition."));
    m_runJobBtnCtl.Init(IDI_RUN, m_pToolTip,
                        _T("Run the selected jobs."));
    m_simulateJobBtnCtl.Init(IDI_SIMULATE, m_pToolTip,
                             _T("Simulate the selected jobs. A job log can be created but actual copies will not take place. No files will be modified."));
    m_abortJobBtnCtl.Init(IDI_ABORT, m_pToolTip,
                          _T("Stop all currently running jobs, and wait for currently executing file actions to complete."));
    m_helpButtonCtl.Init(IDI_HELP, m_pToolTip,
                         _T("Open help viewer."));
    m_viewLogBtnCtl.Init(IDI_VIEW, m_pToolTip,
                         _T("View latest log file in default text editor."));
    m_logSyncDirCheckCtl.Init(m_pToolTip,
                              _T("Set to log processed folders."));
    m_logSkippedCheckCtl.Init(m_pToolTip,
                              _T("Set to log skipped files and folders."));
    m_logAppendCheckCtl.Init(m_pToolTip,
                             _T("Set to append log to existing file instead of creating new log file."));
    m_verboseLogCheck.Init(m_pToolTip,
                           _T("Set to log extensive debug data (don't use unless needed for support)."));

    m_logSummaryCheckCtl.Init(m_pToolTip,
                              _T("Set to create summary log (log errors only, and final summary)."));
    m_errorPopupCheckCtl.Init(m_pToolTip,
                              _T("Set to pop up dialog when error occurs."));
    m_promptBeforeRunningCtl.Init(m_pToolTip,
                                  _T("Set to pop up prompt dialog before running this job."));
    m_jobVerifyCheckCtl.Init(m_pToolTip,
                             _T("Set to verify all copies by reading destination and comparing with source."));
    m_tipsOffCheckCtl.Init(m_pToolTip,
                           _T("Don't show these pop-up control tips."));

    if (m_pToolTip->isOK()) {
        m_pToolTip->AddTool(&m_jobModeComboCtl,
                            _T("Select synchronization mode."));
        m_pToolTip->AddTool(&m_jobLogEditCtl,
                            _T("Specify job log file name."));
        m_pToolTip->AddTool(&m_maxThreadsComboCtl,
                            _T("Set maximum number of parallel running processes (default is the system's physical number of processor threads)."));
    }

    m_runningThreadsCtl.SetRange(0, kUpperRange);

    GetSystemInfo(&m_systemInfo);
    {
        CString msg;
        msg.Format(_T("Auto (%u)"), CMainDlg::Singleton().m_systemInfo.dwNumberOfProcessors);
        m_maxThreadsComboCtl.AddString(msg);
    }
    CString t;

    for (int i = 1; i < 17; i++) {
        t.Format(_T("%d"), i);
        m_maxThreadsComboCtl.AddString(t);
    }

    CFont *f = m_folderGroupCtl.GetFont();
    LOGFONT lf;
    f->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD;

    if (m_boldfont.CreateFontIndirect(&lf)) {
        m_folderGroupCtl.SetFont(&m_boldfont);
        m_progressGroupCtl.SetFont(&m_boldfont);
        m_jobGroupCtrl.SetFont(&m_boldfont);
    }

    HMODULE hMod = LoadLibrary(_T("User32.dll"));

    if (hMod) {
        m_shutdownBlockReasonCreate = (ShutdownBlockReasonCreateFunction)
                                      ::GetProcAddress(hMod, "ShutdownBlockReasonCreate");
        m_shutdownBlockReasonDestroy = (ShutdownBlockReasonDestroyFunction)
                                       ::GetProcAddress(hMod, "ShutdownBlockReasonDestroy");
    }

    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_ICON000, 48, 48));
    m_insyncStaticCtl.SetIcon(LoadResourceIcon(IDI_SIG_ICON, 104, 44));

    m_ledIcons[LED_GREEN] = LoadResourceIcon(IDI_LED_GREEN, 16, 16);
    m_ledIcons[LED_YELLOW] = LoadResourceIcon(IDI_LED_YELLOW, 16, 16);
    m_ledIcons[LED_RED] = LoadResourceIcon(IDI_LED_RED, 16, 16);
    m_ledIconStaticCtl.SetIcon(m_ledIcons[LED_GREEN]);

    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    WORD ix;

    for (ix = 0; ix < 18; ix++) {
        UINT rId = iconPhases[ix];
        m_mandala[ix] = LoadResourceIcon(rId, 48, 48);
        m_mandala_small[ix] = LoadResourceIcon(rId, 16, 16);
    }

    SetDialogIcon(0);
    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_INSYNC_ICON, 48, 48));

    {
        TCHAR un[UNLEN + 1];
        DWORD unl = _countof(un);

        if (!GetUserName(un, &unl))
            un[0] = 0;

        CString iversion;
        iversion.Format(_T("%s - Version %s - User %s running as %s"),
                        InSyncApp.m_copyright.GetBuffer(), InSyncApp.m_insyncVersion.GetBuffer(),
                        un, InSyncApp.m_adminPriviledge ? _T("administrator") : _T("standard user"));
        SetWindowText(iversion);
    }
    m_copyrightStaticCtl.SetWindowText(InSyncApp.m_copyright);

    int lx = m_jobModeComboCtl.AddString(kRunModeNames[MIRROR_SYNC_MODE]);
    m_jobModeComboCtl.SetItemData(lx, MIRROR_SYNC_MODE);
    lx = m_jobModeComboCtl.AddString(kRunModeNames[TWOWAY_SYNC_MODE]);
    m_jobModeComboCtl.SetItemData(lx, TWOWAY_SYNC_MODE);
    lx = m_jobModeComboCtl.AddString(kRunModeNames[BACKUP_SYNC_MODE]);
    m_jobModeComboCtl.SetItemData(lx, BACKUP_SYNC_MODE);

    m_numberFormat.LeadingZero = 0;
    m_numberFormat.NegativeOrder = 0;

    TCHAR locale[MAX_PATH];

    if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, locale, _countof(locale)) == 0)
        m_numberFormat.Grouping = 3;
    else {
        UINT32 g = 0;
        LPTSTR gp = locale;

        while (*gp) {
            if (*gp != _T(';'))
                g = (g * 10) + *gp - _T('0');

            gp++;
        }

        if ((g % 10) == 0)
            g /= 10;

        m_numberFormat.Grouping = g;
    }

    if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHOUSANDSEP, locale, _countof(locale)) == 0)
        m_numberFormat.lpThousandSep = _tcsdup(_T(","));
    else
        m_numberFormat.lpThousandSep = _tcsdup(locale);

    if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, locale, _countof(locale)) == 0)
        m_numberFormat.lpDecimalSep = _tcsdup(_T("."));
    else
        m_numberFormat.lpDecimalSep = _tcsdup(locale);

    LCTYPE lcType = LOCALE_SLANGUAGE;

    if (IsWindows7OrGreater())
        lcType = LOCALE_SLOCALIZEDLANGUAGENAME;

    if (GetLocaleInfo(LOCALE_USER_DEFAULT, lcType, locale, _countof(locale)) == 0)
        locale[0] = 0;

    m_locale = locale;

    m_jobLogEditCtl.EnableFileBrowseButton();
    CString errCode;
    CString path = InSyncApp.m_optFilename;
    bool loadedOk = false;
    m_configStaticCtl.SetWindowText(path);
    m_xml.SetFilePath(path);

    if (!m_xml.LoadXMLOptions()) {
        ClearJobs();

        if (::PathFileExists(path)) {
            CString msg;
            msg = _T("Error loading file \"");
            msg += path;
            msg += _T("\", ");

            if (InSyncApp.m_optFileNameFromConfig)
                msg += _T("specified using /config parameter, ");

            msg += errCode;
            InSyncMessageBox(false, msg, MB_ICONERROR);
        } else {
            path = InSyncApp.m_optCommonFilename;
            m_configStaticCtl.SetWindowText(path);
            m_xml.SetFilePath(path);

            if (!m_xml.LoadXMLOptions()) {
                ClearJobs();

                if (::PathFileExists(path)) {
                    CString msg;
                    msg.Format(_T("Error loading %s, %s"), path.GetBuffer(), (LPCTSTR)errCode);
                    InSyncMessageBox(false, msg, MB_ICONERROR | MB_OK);
                } else {
                    path = InSyncApp.m_optFilename;
                    m_xml.SetFilePath(path);
                    m_configStaticCtl.SetWindowText(path);
                }
            } else
                loadedOk = true;
        }
    } else
        loadedOk = true;

    if (loadedOk && (m_xml.GetVersion() != InSyncApp.m_insyncVersion)) {
        m_xml.SetVersion(InSyncApp.m_insyncVersion);
        SaveOptions();
    }

    m_maxThreadsComboCtl.SetCurSel(m_maxThreads);
    m_autoRun = false;

    while (!InSyncApp.m_autoNameRoot.empty()) {
        CString name = *InSyncApp.m_autoNameRoot.begin();
        ParseFileOrJobName(name.GetBuffer());
        InSyncApp.m_autoNameRoot.erase(name);
        m_autoRun = true;
    }

    if (InSyncApp.AppRC() != kINSYNC_NO_ERROR) {
        m_autoRun = false;
        InSyncMessageBox(false, _T("Error encountered in command line parameters"), MB_ICONERROR | MB_OK);
        return true;
    }

    m_currentJob = NULL;
    m_abortJobBtnCtl.EnableWindow(FALSE);
    m_tipsOffCheckCtl.SetCheck(m_tipsOff);

    if (InSyncApp.m_adminPriviledge && m_copyLocked) {
        if (IsWow64())
            InSyncMessageBox(false,
                             _T("Volume Shadow support not supported for WoW64. Locked file feature disabled."), MB_ICONWARNING,
                             IDOK, _T("VssWarning"));
    }

    UpdateJobControls();

    if (m_autoRun) {
        PostMessage(WM_COMMAND, MAKELONG(IDC_RUN_JOBS_BUTTON, BN_CLICKED), (LPARAM)
                    m_runJobBtnCtl.m_hWnd);
    }

    return true;
}

CString CMainDlg::AvailableUnnamed() const
{
    if (m_jobListCtl.FindStringExact(-1, _T("Unnamed Job")) == LB_ERR)
        return CString(_T("Unnamed Job"));

    int i;

    for (i = 2; i < 100; i++) {
        CString name;
        name.Format(_T("Unnamed Job %d"), i);

        if (m_jobListCtl.FindStringExact(-1, name) == LB_ERR)
            return name;
    }

    return CString(_T("Unnamed Job 1"));
}

void CMainDlg::OnAddJobButton()
{
    UpdateJobControls();
    CJobDialogPtr job;
    CJobNode *newJobNode = new CJobNode;
    newJobNode->m_name = AvailableUnnamed();
    job.m_ptr->m_jobNode = newJobNode;

    if (job.m_ptr->DoModal() == IDOK) {
        if (m_jobListCtl.FindStringExact(-1, newJobNode->m_name) != LB_ERR) {
            InSyncMessageBox(true, _T("Job name already exists, choose a unique job name"),
                             MB_ICONERROR | MB_OK);
            delete newJobNode;
            return;
        }

        m_jobList.push_back(newJobNode);
        int indx = m_jobListCtl.AddString(newJobNode->m_name);
        m_jobListCtl.SetItemDataPtr(indx, newJobNode);
        m_jobListCtl.SetSel(indx);
        UpdateJobControls();
        SaveOptions();
        return;
    }

    delete newJobNode;
}

void CMainDlg::OnEditJobButton()
{
    UpdateJobControls();
    int indx;
    m_jobListCtl.GetSelItems(1, &indx);
    CJobNode *oldJobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indx);
    CJobNode *newJobNode = new CJobNode(*oldJobNode);
    CJobDialogPtr job;
    job.m_ptr->m_jobNode = newJobNode;

    if ((job.m_ptr->DoModal() == IDOK) && job.m_ptr->m_jobNodeChanged) {
        if (job.m_ptr->m_jobNode && oldJobNode) {
            if (oldJobNode->m_name != job.m_ptr->m_jobNode->m_name) {
                if (m_jobListCtl.FindStringExact(-1, job.m_ptr->m_jobNode->m_name) != LB_ERR) {
                    InSyncMessageBox(true, _T("Job name already exists, choose a job unique name"),
                                     MB_ICONERROR | MB_OK);
                    delete job.m_ptr->m_jobNode;
                    return;
                }

                m_jobListCtl.DeleteString((UINT)indx);
                indx = m_jobListCtl.AddString(job.m_ptr->m_jobNode->m_name);
                m_jobListCtl.SetSel(indx);

                if (job.m_ptr->m_jobNode->m_logName.GetLength() == 0)
                    m_jobLogName = job.m_ptr->m_jobNode->m_name + _T(".log");
            }

            m_jobList.remove(oldJobNode);
            delete oldJobNode;

            if (job.m_ptr->m_jobNode)
                m_jobList.push_back(job.m_ptr->m_jobNode);

            m_currentJob = job.m_ptr->m_jobNode;
            m_jobListCtl.SetItemDataPtr(indx, m_currentJob);
            SaveOptions();
            return;
        }
    }

    delete job.m_ptr->m_jobNode;
}

void CMainDlg::OnRemoveJobButton()
{
    UpdateJobControls();
    int indx;
    m_jobListCtl.GetSelItems(1, &indx);
    CJobNode *jobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indx);

    if (RemoveOK(jobNode->m_name)) {
        m_jobListCtl.DeleteString((UINT)indx);
        m_jobList.remove(jobNode);
        delete jobNode;
        m_currentJob = NULL;
        UpdateJobControls();
        SaveOptions();
    }
}

void CMainDlg::DisplayCounts()
{
    CRun &run = CRun::Singleton();
    DisplayCount(run.m_counts.m_foldersCopiedCount, IDC_DIR_COPIED_STATIC, false);
    DisplayCount(run.m_counts.m_foldersDeletedCount, IDC_DIR_DELETED_STATIC, false);
    DisplayCount(run.m_counts.m_filesSkippedCount, IDC_FILE_BYPASSED_STATIC, false);
    DisplayCount(run.m_counts.m_filesCopiedCount, IDC_FILE_COPIED_STATIC, false);
    DisplayCount(run.m_counts.m_filesUpdatedCount, IDC_FILE_UPDATED_STATIC, false);
    DisplayCount(run.m_counts.m_filesDeletedCount, IDC_FILE_DELETED_STATIC, false);
    DisplayCount(run.m_counts.m_bytesSkippedCount, IDC_BYTES_BYPASSED_STATIC, true);
    DisplayCount(run.m_counts.m_bytesCopiedCount, IDC_BYTES_COPIED_STATIC, true);
    DisplayCount(run.m_counts.m_bytesUpdatedCount, IDC_BYTES_UPDATED_STATIC, true);
    DisplayCount(run.m_counts.m_bytesDeletedCount, IDC_BYTES_DELETED_STATIC, true);
    DisplayCount(run.m_counts.m_fileErrorCount, IDC_FILE_ERRORS_STATIC, false);
    time_t now;
    time(&now);
    CTimeSpan elapsed(CTime(now) - m_startTime);
    SetDlgItemText(IDC_ELAPSED_STATIC, elapsed.Format(_T("%H:%M:%S")));
    __time64_t elapsed64 = elapsed.GetTimeSpan();

    if (elapsed64) {
        UINT64 copied = run.m_counts.m_bytesCopiedCount + run.m_counts.m_bytesUpdatedCount;

        if (copied > m_lastCopied) {
            m_lastCopied = copied;
            UINT64 c;
            c = (copied / elapsed64);
            DisplayCount(c, IDC_XFER_STATIC, true, _T("/s"));
        }
    }
}

void CMainDlg::ZeroDisplay(DWORD id)
{
    CWnd *wnd = GetDlgItem(id);
    wnd->EnableWindow(FALSE);
    SetDlgItemText(id, _T("0"));
}

void CMainDlg::ZeroDisplays()
{
    ZeroDisplay(IDC_DIR_COPIED_STATIC);
    ZeroDisplay(IDC_DIR_DELETED_STATIC);
    ZeroDisplay(IDC_FILE_BYPASSED_STATIC);
    ZeroDisplay(IDC_FILE_COPIED_STATIC);
    ZeroDisplay(IDC_FILE_UPDATED_STATIC);
    ZeroDisplay(IDC_FILE_DELETED_STATIC);
    ZeroDisplay(IDC_BYTES_BYPASSED_STATIC);
    ZeroDisplay(IDC_BYTES_COPIED_STATIC);
    ZeroDisplay(IDC_BYTES_UPDATED_STATIC);
    ZeroDisplay(IDC_BYTES_DELETED_STATIC);
    ZeroDisplay(IDC_FILE_ERRORS_STATIC);
}


void CMainDlg::OnRunJobsButton()
{
    UpdateJobControls();
    int run = IDOK;

    if (!m_autoRun) {
        bool oneWayFound = false;
        int count = m_jobListCtl.GetSelCount();

        if (count) {
            int *indxList = new int[count];

            if (m_jobListCtl.GetSelItems(count, indxList) != LB_ERR) {
                CJobNode *jobNode;

                for (int indx = 0; indx < count; indx++) {
                    jobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indxList[indx]);

                    if (jobNode->m_mode == MIRROR_SYNC_MODE) {
                        oneWayFound = true;
                        break;
                    }
                }
            }

            delete [] indxList;
        }

        if (oneWayFound) {
            run = CMainDlg::Singleton().InSyncMessageBox(true,
                    _T("WARNING. In mirror mode InSync can potentially delete many files from selected target folder. ")
                    _T("Make sure you understand how InSync works, and that your target folders are specified correctly before proceeding.")
                    , MB_ICONWARNING | MB_OKCANCEL, IDOK, _T("MirrorModeWarn"));
        }
    }

    if (run == IDOK) {
        InSyncApp.ClearAppRC();
        SetStatusLedColor(LED_GREEN);
        RunJobs(InSyncApp.m_simulateParm);
        InSyncApp.m_simulateParm = false;
    }
}


void CMainDlg::RunJobs(bool simulate)
{
    InSyncApp.m_simulate = simulate;
    m_mandalaIx = 0;
    m_globalAbort = false;
    time_t now;
    time(&now);
    m_startTime = CTime(now);

    m_verboseLog = ISCHECKED(m_verboseLogCheck);

    CRun::Singleton().m_counts.Clear();
    CRun::Singleton().m_activeThreadCount = 0;

    DisplayCounts();
    ZeroDisplays();
    int count = m_jobListCtl.GetSelCount();

    if (count) {
        m_lastCopied = 0;
        DisplayCounts();
        m_abortJobBtnCtl.EnableWindow(TRUE);
        m_abortJobBtnCtl.SetFocus();
        m_editJobBtnCtl.EnableWindow(FALSE);
        m_cloneJobBtnCtl.EnableWindow(FALSE);
        m_mirrorJobBtnCtl.EnableWindow(FALSE);
        m_removeJobBtnCtl.EnableWindow(FALSE);
        m_runJobBtnCtl.EnableWindow(FALSE);
        m_simulateJobBtnCtl.EnableWindow(FALSE);
        m_addJobBtnCtl.EnableWindow(FALSE);
        m_jobListCtl.EnableWindow(FALSE);
        m_criteriaStaticCtl.EnableWindow(FALSE);
        m_jobModeComboCtl.EnableWindow(FALSE);
        m_jobLogStaticCtl.EnableWindow(FALSE);
        m_jobLogEditCtl.EnableWindow(FALSE);
        m_logSummaryCheckCtl.EnableWindow(FALSE);
        m_logSyncDirCheckCtl.EnableWindow(FALSE);
        m_logSkippedCheckCtl.EnableWindow(FALSE);
        m_logAppendCheckCtl.EnableWindow(FALSE);
        m_verboseLogCheck.EnableWindow(FALSE);
        m_errorPopupCheckCtl.EnableWindow(FALSE);
        m_promptBeforeRunningCtl.EnableWindow(FALSE);
        m_jobVerifyCheckCtl.EnableWindow(FALSE);
        m_viewLogBtnCtl.EnableWindow(FALSE);
        m_maxThreadsComboCtl.EnableWindow(FALSE);
        m_maxThreadsStaticCtl.EnableWindow(FALSE);
        int *indxList = new int[count];

        if (indxList) {
            if (m_jobListCtl.GetSelItems(count, indxList) != LB_ERR) {
                JobNodeListIter node;

                for (node = m_jobList.begin(); node != m_jobList.end(); ++node)
                    (*node)->m_selected = false;

                CJobNode *jobNode;

                for (int indx = 0; indx < count; indx++) {
                    jobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indxList[indx]);
                    jobNode->m_listBoxIndx = indxList[indx];
                    jobNode->m_selected = true;
                }

                if (InSyncApp.m_startupDelay <= 0) {
                    if (!simulate && m_shutdownBlockReasonCreate)
                        m_shutdownBlockReasonCreate(*this, _T("File sync in progress."));

                    CRun::Singleton().Start(simulate);
                } else
                    SetTimer(simulate ? STARTUP_SIM_TIMER : STARTUP_TIMER,
                             InSyncApp.m_startupDelay * 1000, NULL);
            }

            delete [] indxList;
        }
    } else {
        if (m_autoRun)
            PostMessage(WM_USER_JOBS_DONE, b2B(simulate), 0);
    }
}

bool CMainDlg::UpdateJobControls()
{
    bool rc = false;
    int count = m_jobListCtl.GetSelCount();

    if (count > 1) {
        m_runJobBtnCtl.SetWindowText(_T("Run Jobs"));
        m_simulateJobBtnCtl.SetWindowText(_T("Simulate Jobs"));
        m_abortJobBtnCtl.SetWindowText(_T("Abort Jobs"));
    } else {
        m_runJobBtnCtl.SetWindowText(_T("Run Job"));
        m_simulateJobBtnCtl.SetWindowText(_T("Simulate Job"));
        m_abortJobBtnCtl.SetWindowText(_T("Abort Job"));
    }

    m_runJobBtnCtl.EnableWindow(count);

    if (count)
        m_runJobBtnCtl.SetFocus();
    else
        m_addJobBtnCtl.SetFocus();

    BOOL oneCount = (count == 1);
    m_editJobBtnCtl.EnableWindow(oneCount);
    m_removeJobBtnCtl.EnableWindow(oneCount);
    m_cloneJobBtnCtl.EnableWindow(oneCount);
    m_mirrorJobBtnCtl.EnableWindow(oneCount);
    m_jobLogStaticCtl.EnableWindow(oneCount);
    m_errorPopupCheckCtl.EnableWindow(oneCount);
    m_promptBeforeRunningCtl.EnableWindow(oneCount);
    m_logSummaryCheckCtl.EnableWindow(oneCount);
    m_logSyncDirCheckCtl.EnableWindow(oneCount);
    m_logSkippedCheckCtl.EnableWindow(oneCount);
    m_logAppendCheckCtl.EnableWindow(oneCount);
    m_verboseLogCheck.EnableWindow(oneCount);
    m_jobVerifyCheckCtl.EnableWindow(oneCount);
    m_jobModeComboCtl.EnableWindow(oneCount);
    m_criteriaStaticCtl.EnableWindow(oneCount);
    m_jobLogEditCtl.EnableWindow(oneCount);
    m_simulateJobBtnCtl.EnableWindow(count);

    if (m_currentJob) {
        bool jobListChanged = false;
        CString wText;
        m_jobLogEditCtl.GetWindowText(wText);

        if (m_currentJob->m_logName != wText) {
            m_currentJob->m_logName = wText;
            jobListChanged = true;
        }

        if (m_currentJob->m_errPopup ^ ISCHECKED(m_errorPopupCheckCtl)) {
            m_currentJob->m_errPopup ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_promptBeforeRunning ^ ISCHECKED(m_promptBeforeRunningCtl)) {
            m_currentJob->m_promptBeforeRunning ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_logOnlyError ^ ISCHECKED(m_logSummaryCheckCtl)) {
            m_currentJob->m_logOnlyError ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_logFolder ^ ISCHECKED(m_logSyncDirCheckCtl)) {
            m_currentJob->m_logFolder ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_logSkipped ^ ISCHECKED(m_logSkippedCheckCtl)) {
            m_currentJob->m_logSkipped ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_logAppend ^ ISCHECKED(m_logAppendCheckCtl)) {
            m_currentJob->m_logAppend ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_ioVerify ^ ISCHECKED(m_jobVerifyCheckCtl)) {
            m_currentJob->m_ioVerify ^= true;
            jobListChanged = true;
        }

        if (m_currentJob->m_mode != (RunModes)m_jobModeComboCtl.GetSelectedItemData()) {
            m_currentJob->m_mode = (RunModes)m_jobModeComboCtl.GetSelectedItemData();
            jobListChanged = true;
        }

        if (jobListChanged) {
            SaveOptions();
            rc = true;
        }
    }

    if (oneCount) {
        int indx;
        m_jobListCtl.GetSelItems(1, &indx);
        m_currentJob = (CJobNode *)m_jobListCtl.GetItemDataPtr(indx);
        m_jobLogEditCtl.SetWindowText(m_currentJob->m_logName);

        if (m_currentJob->m_logName.GetLength() == 0)
            m_jobLogName = m_currentJob->m_name + _T(".log");
        else
            m_jobLogName = m_currentJob->m_logName;

        m_errorPopupCheckCtl.SetCheck(SETCHECK(m_currentJob->m_errPopup));
        m_promptBeforeRunningCtl.SetCheck(SETCHECK(m_currentJob->m_promptBeforeRunning));
        m_logSummaryCheckCtl.SetCheck(SETCHECK(m_currentJob->m_logOnlyError));
        m_logSyncDirCheckCtl.SetCheck(SETCHECK(m_currentJob->m_logFolder));
        m_logSkippedCheckCtl.SetCheck(SETCHECK(m_currentJob->m_logSkipped));
        m_logAppendCheckCtl.SetCheck(SETCHECK(m_currentJob->m_logAppend));
        m_jobVerifyCheckCtl.SetCheck(SETCHECK(m_currentJob->m_ioVerify));
        m_jobModeComboCtl.SetSelectedFromItemData(m_currentJob->m_mode);
        m_viewLogBtnCtl.EnableWindow(LogFileViewable(m_currentJob));
    } else {
        m_currentJob = NULL;
        m_jobLogEditCtl.SetWindowText(kNullString);
        m_jobModeComboCtl.SetSelectedFromItemData(MIRROR_SYNC_MODE);
        m_viewLogBtnCtl.EnableWindow(FALSE);
        m_errorPopupCheckCtl.SetCheck(SETCHECK(false));
        m_promptBeforeRunningCtl.SetCheck(SETCHECK(false));
        m_logSummaryCheckCtl.SetCheck(SETCHECK(false));
        m_logSyncDirCheckCtl.SetCheck(SETCHECK(false));
        m_logSkippedCheckCtl.SetCheck(SETCHECK(false));
        m_logAppendCheckCtl.SetCheck(SETCHECK(false));
        m_jobVerifyCheckCtl.SetCheck(SETCHECK(false));
        m_jobModeComboCtl.SetSelectedFromItemData(MIRROR_SYNC_MODE);
        m_viewLogBtnCtl.EnableWindow(false);
    }

    return rc;
}

void CMainDlg::OnSelchangeJobList()
{
    UpdateJobControls();
}

void CMainDlg::ClearJobs()
{
    JobNodeListIter job;

    for (job = m_jobList.begin(); job != m_jobList.end(); ++job)
        delete *job;

    m_jobList.clear();
    m_jobListCtl.ResetContent();
}

LRESULT CMainDlg::OnJobsDone(WPARAM, LPARAM)
{
    ::WaitForSingleObject(CRun::Singleton().m_hRun, INFINITE);
    CloseHandle(CRun::Singleton().m_hRun);
    KillTimer(MANDALA_TIMER);
    m_runningThreadsCtl.SetPos(0);
    m_oldpos = 0;

    if (m_shutdownBlockReasonDestroy)
        m_shutdownBlockReasonDestroy(*this);

    DisplayCounts();

    if (m_autoRun || (m_endOfRunComboCtl.GetCurSel() > NoneAction))
        SetTimer(SHUTDOWN_TIMER, 2000, NULL);

    UpdateJobControls();
    SetDialogIcon(0);
    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_INSYNC_ICON, 48, 48));

    if (m_ptl)
        m_ptl->SetProgressState(m_hWnd, TBPF_NOPROGRESS);

    m_abortJobBtnCtl.EnableWindow(FALSE);
    m_addJobBtnCtl.EnableWindow(TRUE);
    m_jobListCtl.EnableWindow(true);
    m_criteriaStaticCtl.EnableWindow(TRUE);
    m_jobModeComboCtl.EnableWindow(TRUE);
    m_maxThreadsComboCtl.EnableWindow(TRUE);
    m_maxThreadsStaticCtl.EnableWindow(TRUE);
    return 0;
}

void CMainDlg::OnAbortJobsButton()
{
    if (InSyncMessageBox(true, _T("Do you really want to abort all running jobs ?"),
                         MB_ICONQUESTION | MB_YESNO) == IDYES)
        m_globalAbort = true;
}


void CMainDlg::OnSimulateJobsButton()
{
    UpdateJobControls();
    SetStatusLedColor(LED_GREEN);
    RunJobs(true);
}

bool CMainDlg::RemoveOK(LPCTSTR name)
{
    CString msg(_T("Do you really want to remove job '"));
    msg += name;
    msg += _T("' ?");
    return InSyncMessageBox(true, msg, MB_ICONQUESTION | MB_YESNO) == IDYES;
}

void CMainDlg::OnCloneJobButton()
{
    UpdateJobControls();
    int indx;
    m_jobListCtl.GetSelItems(1, &indx);
    CJobNode *oldJobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indx);
    CJobDialogPtr job;
    CJobNode *newJobNode = new CJobNode(*oldJobNode);
    job.m_ptr->m_jobNode = newJobNode;
    newJobNode->m_name = AvailableUnnamed();
    bool changed = (job.m_ptr->DoModal() == IDOK);

    if (changed) {
        if (m_jobListCtl.FindStringExact(-1, newJobNode->m_name) != LB_ERR) {
            InSyncMessageBox(true, _T("Job name already exists, choose unique job name"), MB_ICONERROR | MB_OK);
            delete newJobNode;
            return;
        }

        m_jobListCtl.SetSel(indx, FALSE);
        indx = m_jobListCtl.AddString(newJobNode->m_name);
        m_jobListCtl.SetItemDataPtr(indx, newJobNode);
        m_jobListCtl.SetSel(indx);
        m_jobList.push_back(newJobNode);
        m_currentJob = newJobNode;
        SaveOptions();
    } else
        delete newJobNode;
}

LRESULT CMainDlg::OnUserSetLed(WPARAM wParam, LPARAM)
{
    SetStatusLedColor((LedColor)wParam);
    return 0;
}


void CMainDlg::OnMirrorJobButton()
{
    UpdateJobControls();
    int indx;
    m_jobListCtl.GetSelItems(1, &indx);
    CJobNode *oldJobNode = (CJobNode *)m_jobListCtl.GetItemDataPtr(indx);
    bool changed = oldJobNode->m_mode == TWOWAY_SYNC_MODE;
    SrcNodeListIter node;

    for (node = oldJobNode->m_childrenList.begin();
         node != oldJobNode->m_childrenList.end();
         ++node) {
        CSrcNode *srcNode = *node;

        if (srcNode->m_parentJob->m_mode == TWOWAY_SYNC_MODE)
            changed = true;

        int trgcount = 0;
        TrgNodeListIter tnode;

        for (tnode = srcNode->m_childrenList.begin();
             tnode != srcNode->m_childrenList.end();
             ++tnode) {
            CTrgNode *trgNode = *tnode;

            if (trgNode->m_parentJob->m_mode == TWOWAY_SYNC_MODE)
                changed = true;

            trgcount++;
        }

        if (trgcount != 1) {
            CString msg(_T("Source ") + srcNode->m_name +
                        _T(" has more than one target. This job cannot be automatically reversed."));
            InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
            return;
        }
    }

    if (changed) {
        InSyncMessageBox(true, _T("Warning this job uses 2 way synchronization. Please verify that the")
                         _T(" modes used in the reversed job are ok."), MB_ICONWARNING | MB_OK);
    }

    CJobDialogPtr job;
    CJobNode *newJobNode = new CJobNode;
    newJobNode->m_name = _T("Reverse of ") + oldJobNode->m_name;
    job.m_ptr->m_jobNode = newJobNode;
    newJobNode->m_logOnlyError = oldJobNode->m_logOnlyError;
    newJobNode->m_logName = _T("Reverse of ") + oldJobNode->m_logName;
    newJobNode->m_logFolder = oldJobNode->m_logFolder;
    newJobNode->m_ioVerify = oldJobNode->m_ioVerify;

    for (node = oldJobNode->m_childrenList.begin(); node != oldJobNode->m_childrenList.end(); ++node) {
        CSrcNode *srcNode = *node;
        CTrgNode *trgNode = srcNode->m_childrenList.front();
        CSrcNode *newSrcNode = new CSrcNode(newJobNode);
        newSrcNode->m_name = trgNode->m_name;
        newSrcNode->m_exclude = srcNode->m_exclude;
        newSrcNode->m_include = srcNode->m_include;
        newSrcNode->m_subdir = srcNode->m_subdir;
        CTrgNode *newTrgNode = new CTrgNode(newJobNode, newSrcNode);
        newTrgNode->m_name = srcNode->m_name;
        newTrgNode->m_exclude = trgNode->m_exclude;
        newTrgNode->m_include = trgNode->m_include;
        newSrcNode->m_childrenList.push_back(newTrgNode);
        newJobNode->m_childrenList.push_back(newSrcNode);
    }

    changed = (job.m_ptr->DoModal() == IDOK);

    if (changed) {
        if (m_jobListCtl.FindStringExact(-1, newJobNode->m_name) != LB_ERR) {
            InSyncMessageBox(true, _T("Job name already exists, choose unique job name"), MB_ICONERROR | MB_OK);
            delete newJobNode;
            return;
        }

        m_jobListCtl.SetSel(indx, FALSE);
        indx = m_jobListCtl.AddString(newJobNode->m_name);
        m_jobListCtl.SetItemDataPtr(indx, newJobNode);
        m_jobListCtl.SetSel(indx);

        m_jobList.push_back(newJobNode);
        m_currentJob = newJobNode;
        SaveOptions();
        return;
    }

    delete newJobNode;
}


void CMainDlg::SaveOptions()
{
    CString errCode;

    if (!m_xml.SaveXMLOptions()) {
        CString msg;
        msg.Format(_T("Error occured saving program settings to %s, Error code %s"),
                   (LPCTSTR)InSyncApp.m_optFilename,
                   (LPCTSTR)m_xml.GetLastError());
        InSyncMessageBox(true, msg, MB_ICONSTOP | MB_OK);
    }
}


void CMainDlg::SetStatusLedColor(LedColor color)
{
    if (color == LED_GREEN) {
        m_highColor = LED_GREEN;
        m_ledIconStaticCtl.SetIcon(m_ledIcons[LED_GREEN]);
    }

    if (color > m_highColor) {
        m_highColor = color;
        m_ledIconStaticCtl.SetIcon(m_ledIcons[color]);
    }
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
    case MANDALA_TIMER: {
        if (++m_mandalaIx > 17) {
            m_mandalaIx = 0;
            DisplayCounts();
        }

        SetDialogIcon(m_mandalaIx);
        WORD maxThreads = DefaultMaxThreads();

        if (maxThreads) {
            int newpos;
            newpos = (CRun::Singleton().m_activeThreadCount * kUpperRange) / maxThreads;

            if (newpos != m_oldpos)
                m_runningThreadsCtl.SetPos(m_oldpos = newpos);
        }

        break;
    }

    case SHUTDOWN_TIMER:
        KillTimer(SHUTDOWN_TIMER);

        if (m_endOfRunComboCtl.GetCurSel() > NoneAction) {
            switch (m_endOfRunComboCtl.GetCurSel()) {
            case SleepAction:
                // sleep
                SetSuspendState(false, false, false); // stand by
                break;

            case HibernateAction:
                // hibernate
                SetSuspendState(true, false, false); // hibernate                break;
                break;

            case ShutdownAction: {
                // shutdown
                DWORD flags = SHUTDOWN_POWEROFF;

                if (IsWindows8OrGreater())
                    flags |= SHUTDOWN_HYBRID;

                DWORD rc = InitiateShutdown(
                               NULL,
                               L"InSync requested shutdown",
                               10,
                               flags,
                               SHTDN_REASON_MAJOR_OTHER
                           );
                rc++;
                break;
            }

            default:
                break;
            }
        }

        PostMessage(WM_CLOSE, 0, 0);
        break;

    case STARTUP_TIMER:
    case STARTUP_SIM_TIMER:
        KillTimer(nIDEvent);

        if ((nIDEvent == STARTUP_TIMER) && m_shutdownBlockReasonCreate)
            m_shutdownBlockReasonCreate(*this, _T("File sync in progress."));

        CRun::Singleton().Start(nIDEvent == STARTUP_SIM_TIMER);
        break;

    default:
        CInsyncDlg::OnTimer(nIDEvent);
        break;
    }
}

void CMainDlg::OnBnClickedViewLogButton()
{
    if (m_currentJob) {
        if ((m_currentJob->m_lastLogFileName.GetLength() != 0) &&
            ::PathFileExists(m_currentJob->m_lastLogFileName)) {
            ViewLogFile(m_currentJob->m_lastLogFileName);
            return;
        }

        if ((m_currentJob->m_logName.GetLength() != 0) &&
            ::PathFileExists(m_currentJob->m_logName)) {
            ViewLogFile(m_currentJob->m_logName);
            return;
        }
    }
}

void CMainDlg::ViewLogFile(CString logFileName) const
{
    if (ShellExecute(CMainDlg::Singleton(), _T("open"), logFileName, NULL, NULL,
                     SW_SHOWNORMAL) <= (HINSTANCE)32) {
        InSyncMessageBox(true,
                         _T("Unable to open the log file. You may not have an application associated")
                         _T(" to open this file type!"),
                         MB_ICONERROR | MB_OK);
    }
}

bool CMainDlg::LogFileViewable(const CJobNode *job) const
{
    return
        ((job->m_lastLogFileName.GetLength() != 0) && ::PathFileExists(job->m_lastLogFileName)) ||
        ((job->m_logName.GetLength() != 0) && ::PathFileExists(job->m_logName));
}

void CMainDlg::OnBnClickedTipsOffCheck()
{
    if (m_tipsOffCheckCtl) {
        m_tipsOff = ISCHECKED(m_tipsOffCheckCtl);
        SaveOptions();
    }
}

void CMainDlg::OnClose()
{
    if (!m_endSession) {
        if (CRun::Singleton().JobsActive()) {
            if (InSyncMessageBox(true,
                                 _T("There are active jobs. Use abort button instead and wait for current operation to complete in order to avoid data corruption.")
                                 _T(" Choose Ok to continue, or Cancel to force abort (not recommended, use only for diagnostic purpose.)"),
                                 MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL) {
                DisconnectShares();
                (*((volatile DWORD *)0))++;
            }

            return;
        }

        UpdateJobControls();
        WORD ix;

        for (ix = 0; ix < 18; ix++) {
            DestroyIcon(m_mandala[ix]);
            DestroyIcon(m_mandala_small[ix]);
        }

        DestroyIcon(m_ledIcons[LED_GREEN]);
        DestroyIcon(m_ledIcons[LED_YELLOW]);
        DestroyIcon(m_ledIcons[LED_RED]);
        DestroyIcon(m_mandalaStaticCtl.GetIcon());
        ClearJobs();
    }

    // ok disconnect the shares
    DisconnectShares();

    // destroy the modified menu
    GetSystemMenu(TRUE);

    CInsyncDlg::OnClose();
}

void CMainDlg::DisconnectShares()
{
    ConstStrSetIter i = m_shares.begin();

    while (i != m_shares.end()) {
        (void)WNetCancelConnection2(*i, 0, TRUE);
        i++;
    }

    m_shares.clear();
}

void CMainDlg::OnCbnSelchangeMaxThreadsCombo()
{
    int n = m_maxThreadsComboCtl.GetCurSel();

    if (n == CB_ERR)
        n = 0;

    if (n != m_maxThreads) {
        MaxThreads((USHORT)n);
        SaveOptions();
    }
}

BOOL CMainDlg::OnQueryEndSession()
{
    BOOL jobsActive = b2B(CRun::Singleton().JobsActive());

    if (jobsActive) {
        m_globalAbort = true;
        m_endSession = true;
    }

    return !jobsActive;
}

void CMainDlg::OnEndSession(BOOL bEnding)
{
    CInsyncDlg::OnEndSession(bEnding);
}

void CMainDlg::ConnectPath(CFilePath dir)
{

    CString display = dir.Display();

    if ((display[0] != _T('\\')) || (display[1] != _T('\\')))
        return;

    // truncate to the share name
    int slashCount = 0, i;
    CString share;

    for (i = 0; i < display.GetLength(); i++) {
        if (display[i] == _T('\\')) {
            slashCount++;

            if (slashCount >= 4)
                break;
        }
    }

    share = display.Left(i);
    NETRESOURCE nr;

    if (m_shares.find(share) != m_shares.end())
        return;

    memset(&nr, 0, sizeof(NETRESOURCE));
    nr.dwType = RESOURCETYPE_DISK;
    nr.lpLocalName = NULL;
    nr.lpRemoteName = share.GetBuffer();
    nr.lpProvider = NULL;

    if (WNetAddConnection2(&nr, NULL, NULL, CONNECT_INTERACTIVE | CONNECT_TEMPORARY) == NO_ERROR)
        m_shares.insert(share);
}

void CMainDlg::DisplayCount(UINT64 count, DWORD id, bool scale)
{
    DisplayCount(count, id, scale, NULL);
}

void CMainDlg::DisplayCount(UINT64 count, DWORD id, bool scale, LPCTSTR tail)
{
    if (count)
        GetDlgItem((int)id)->EnableWindow(TRUE);

    TCHAR f[48];
    FormatCount(count, scale, false, f, _countof(f), tail);
    SetDlgItemText((int)id, f);
}

WORD CMainDlg::MaxThreads()
{
    return m_maxThreads;
}

WORD CMainDlg::DefaultMaxThreads()
{
    if (m_maxThreads == 0)
        return WORD(m_systemInfo.dwNumberOfProcessors);

    return m_maxThreads;
}

void CMainDlg::MaxThreads(WORD max)
{
    m_maxThreads = max;
}

bool CMainDlg::VerboseLog()
{
    return m_verboseLog;
}
