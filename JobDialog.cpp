#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CJobDialog dialog

CJobDialog::CJobDialog(CWnd *pParent/*=NULL*/) :
    CInsyncDlg(IDD, HtmlJobEdit, pParent),
    m_currentSrc(NULL),
    m_currentTrg(NULL),
    m_jobNode(NULL),
    m_jobNodeChanged(false),
    m_jobNameChanged(false) {}

void CJobDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ADD_SRC_BUTTON, m_addSrcBtnCtl);
    DDX_Control(pDX, IDC_ADD_TRG_BUTTON, m_addTrgBtnCtl);
    DDX_Control(pDX, IDC_CHANGE_SRC_BUTTON, m_changeSrcBtnCtl);
    DDX_Control(pDX, IDC_CHANGE_TRG_BUTTON, m_changeTrgBtnCtl);
    DDX_Control(pDX, IDC_INSYNC_STATIC, m_insyncStaticCtl);
    DDX_Control(pDX, IDC_MANDALA_STATIC, m_mandalaStaticCtl);
    DDX_Control(pDX, IDC_REMOVE_SRC_BUTTON, m_removeSrcBtnCtl);
    DDX_Control(pDX, IDC_REMOVE_TRG_BUTTON, m_removeTrgBtnCtl);
    DDX_Control(pDX, IDC_SRC_DIR_LIST, m_srcDirListCtl);
    DDX_Control(pDX, IDC_SRC_EXCL_EDIT, m_srcExclEditCtl);
    DDX_Control(pDX, IDC_SRC_EXCL_STATIC, m_srcExclStaticCtl);
    DDX_Control(pDX, IDC_SRC_EXCLUDED_DIR_BUTTON, m_srcExcludedDirBtnCtl);
    DDX_Control(pDX, IDC_SRC_FILTER_GRP, m_srcFilterGroupBoxCtl);
    DDX_Control(pDX, IDC_SRC_INCL_EDIT, m_srcInclEditCtl);
    DDX_Control(pDX, IDC_SRC_INCL_STATIC, m_srcInclStaticCtl);
    DDX_Control(pDX, IDC_SRC_VOL_CHECK, m_srcVolCheckCtl);
    DDX_Control(pDX, IDC_SUBDIRS_CHECK, m_subDirCheckCtl);
    DDX_Control(pDX, IDC_TRG_DIR_LIST, m_trgDirListCtl);
    DDX_Control(pDX, IDC_TRG_EXCL_EDIT, m_trgExclEditCtl);
    DDX_Control(pDX, IDC_TRG_EXCL_STATIC, m_trgExclStaticCtl);
    DDX_Control(pDX, IDC_TRG_FILTER_GRP, m_trgFilterGroupBoxCtl);
    DDX_Control(pDX, IDC_TRG_GROUP_STATIC, m_trgGroupStaticCtl);
    DDX_Control(pDX, IDC_TRG_INCL_EDIT, m_trgInclEditCtl);
    DDX_Control(pDX, IDC_TRG_INCL_STATIC, m_trgInclStaticCtl);
    DDX_Control(pDX, IDC_TRG_VOL_CHECK, m_trgVolCheckCtl);
    DDX_Control(pDX, IDC_DONT_DELETE_CHECK, m_trgDontDeleteCheckCtl);
    DDX_Control(pDX, IDOK, m_okButtonCtl);
    DDX_Control(pDX, IDC_JOB_NAME_EDIT, m_jobNameEditCtl);
    DDX_Control(pDX, IDC_SRC_EXCL_EDIT, m_srcExclEditCtl);
    DDX_Control(pDX, IDC_COPY_ACLS_COMBO, m_copyAclsComboCtl);
    DDX_Control(pDX, IDC_RUN_AFTER_LIST, m_runAfterListboxCtl);
}

BEGIN_MESSAGE_MAP(CJobDialog, CInsyncDlg)
    ON_BN_CLICKED(IDC_ADD_SRC_BUTTON, OnAddSourceButton)
    ON_BN_CLICKED(IDC_ADD_TRG_BUTTON, OnAddTrgButton)
    ON_BN_CLICKED(IDC_CHANGE_SRC_BUTTON, OnChangeSrcButton)
    ON_BN_CLICKED(IDC_CHANGE_TRG_BUTTON, OnChangeTrgButton)
    ON_BN_CLICKED(IDC_REMOVE_SRC_BUTTON, OnRemoveSourceButton)
    ON_BN_CLICKED(IDC_REMOVE_TRG_BUTTON, OnRemoveTrgButton)
    ON_BN_CLICKED(IDC_SRC_EXCLUDED_DIR_BUTTON, OnSrcExcludedDirButton)
    ON_BN_CLICKED(IDC_SRC_VOL_CHECK, OnBnClickedSrcVolCheck)
    ON_BN_CLICKED(IDC_SUBDIRS_CHECK, OnSubdirsCheck)
    ON_BN_CLICKED(IDC_TRG_VOL_CHECK, OnBnClickedTrgVolCheck)
    ON_EN_CHANGE(IDC_JOB_NAME_EDIT, OnChangeJobNameEdit)
    ON_LBN_SELCHANGE(IDC_SRC_DIR_LIST, OnSelchangeSrcDirList)
    ON_LBN_SELCHANGE(IDC_TRG_DIR_LIST, OnSelchangeTrgDirList)
    ON_MESSAGE(WM_USER_ADD_FOLDER, OnAddFolder)
    ON_WM_DESTROY()
    ON_WM_HELPINFO()
    ON_CBN_SELCHANGE(IDC_COPY_ACLS_COMBO, OnCbnSelchangeCopyAclsCombo)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_DONT_DELETE_CHECK, OnBnClickedDontDeleteCheck)
    ON_LBN_SELCHANGE(IDC_RUN_AFTER_LIST, OnLbnSelchangeRunAfterList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJobDialog message handlers

static BYTE aclComboValues[] = {0, 1, 3, 4, 5, 7};

BOOL CJobDialog::OnInitDialog()
{
    CInsyncDlg::OnInitDialog();
    m_jobNameEditCtl.SetWindowText(m_jobNode->m_name);
    m_jobNameChanged = true;
    m_helpButtonCtl.SubclassDlgItem(IDHELP, this);
    m_addTrgBtnCtl.Init(IDI_ADD, m_pToolTip,
                        _T("Add an new target folder to the list of targets for the selected source folder."));
    m_addSrcBtnCtl.Init(IDI_ADD, m_pToolTip,
                        _T("Add an new source folder to the list of source folders."));
    m_changeTrgBtnCtl.Init(IDI_EDIT, m_pToolTip,
                           _T("Change the selected target folder."));
    m_changeSrcBtnCtl.Init(IDI_EDIT, m_pToolTip,
                           _T("Change the selected source folder."));
    m_removeTrgBtnCtl.Init(IDI_REMOVE, m_pToolTip,
                           _T("Delete the selected target folder from the list of targets for the selected source folder."));
    m_removeSrcBtnCtl.Init(IDI_REMOVE, m_pToolTip,
                           _T("Delete the selected source folder from the list of source folders."));
    m_srcExcludedDirBtnCtl.Init(IDI_EXCLUDE, m_pToolTip,
                                _T("Create or edit the list of excluded source folders."));
    m_helpButtonCtl.Init(IDI_HELP, m_pToolTip,
                         _T("Open the help viewer"));
    m_okButtonCtl.Init(IDI_SAVE, m_pToolTip,
                       _T("Save this job definition."));
    m_subDirCheckCtl.Init(m_pToolTip,
                          _T("Set to include all sub-folders."));

    if (m_pToolTip->isOK()) {
        m_pToolTip->AddTool(&m_srcInclEditCtl,
                            _T("Set to specify included files mask."));
        m_pToolTip->AddTool(&m_srcExclEditCtl,
                            _T("Set to specify excluded files mask."));
        m_pToolTip->AddTool(&m_trgInclEditCtl,
                            _T("Set to specify included files mask."));
        m_pToolTip->AddTool(&m_trgExclEditCtl,
                            _T("Set to specify excluded files mask."));
        m_pToolTip->AddTool(&m_jobNameEditCtl,
                            _T("Set to desired job name"));
        m_pToolTip->AddTool(&m_srcVolCheckCtl,
                            _T("Set to use volume names instead of drive lettere"));
        m_pToolTip->AddTool(&m_trgVolCheckCtl,
                            _T("Set to use volume names instead of drive lettere"));
        m_pToolTip->AddTool(&m_trgDontDeleteCheckCtl,
                            _T("Set to prevent deletion of excluded files or folders"));
    }

    m_copyAclsComboCtl.AddString(_T("Don't copy"));						// 0b000
    m_copyAclsComboCtl.AddString(_T("New or updated files (default)"));	// 0b001
    m_copyAclsComboCtl.AddString(_T("All files"));						// 0b011
    m_copyAclsComboCtl.AddString(_T("Folders"));						// 0b100
    m_copyAclsComboCtl.AddString(_T("Folders and updated files"));		// 0b101
    m_copyAclsComboCtl.AddString(_T("Folder and all files"));			// 0b111

    m_insyncStaticCtl.SetIcon(LoadResourceIcon(IDI_SIG_ICON, 104, 44));

    {
        JobNodeListIter node;
        CJobNode *jobNode;

        for (node = CMainDlg::Singleton().m_jobList.begin(); node != CMainDlg::Singleton().m_jobList.end();
             ++node) {
            jobNode = *node;

            if (jobNode->m_name != m_jobNode->m_name) {
                int index = m_runAfterListboxCtl.AddString(jobNode->m_name);
                m_runAfterListboxCtl.SetItemDataPtr(index, NULL);
                ConstStrSetIter iter;

                for (iter = m_jobNode->m_runAfterList.begin(); iter != m_jobNode->m_runAfterList.end(); ++iter) {
                    CString s(*iter);

                    if (s == jobNode->m_name) {
                        m_runAfterListboxCtl.SetSel(index);
                    }
                }
            }
        }
    }

    m_copyAclsComboCtl.EnableWindow(InSyncApp.m_adminPriviledge);

    if (InSyncApp.m_adminPriviledge) {
        BYTE s = 0;

        if (m_jobNode->m_changedFileACLs) {
            s |= 1;
        }

        if (m_jobNode->m_unchangedFileACLs) {
            s |= 2;
        }

        if (m_jobNode->m_folderACLs) {
            s |= 4;
        }

        int i;

        for (i = 0; i < 6; i ++) {
            if (s == aclComboValues[i]) {
                break;
            }
        }

        if (i == 6) {
            i = 1;
        }

        m_copyAclsComboCtl.SetCurSel(i);
    }

    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_INSYNC_ICON, 48, 48));
    SetWindowText(_T("InSync job definition"));
    m_currentSrc = NULL;
    m_currentTrg = NULL;
    SrcNodeListIter snode;

    if (m_jobNode) {
        for (snode = m_jobNode->m_childrenList.begin(); snode != m_jobNode->m_childrenList.end(); ++snode) {
            CSrcNode *srcNode = *snode;
            int indx = m_srcDirListCtl.AddString(srcNode->m_name);
            m_srcDirListCtl.SetItemDataPtr(indx, srcNode);
        }
    }

    UpdateSrcControls();
    return TRUE;
}

void CJobDialog::AddSourceFolder(CString folderName)
{
    if (m_jobNode) {
        CSrcNode *newSrcNode = new CSrcNode(m_jobNode);
        m_jobNode->m_childrenList.push_back(newSrcNode);
        newSrcNode->m_name = folderName;
        newSrcNode->m_subdir = true;
        int indx = m_srcDirListCtl.AddString(newSrcNode->m_name);
        m_srcDirListCtl.SetItemDataPtr(indx, newSrcNode);
        m_srcDirListCtl.SetCurSel(indx);
        m_jobNodeChanged = true;
        UpdateSrcControls();
    }
}

void CJobDialog::OnAddSourceButton()
{
    CString n(m_lastSourceFolder);
    ReplaceVolumeName(n);

    if (BrowseForFolder(n, this, _T("Source folder"))) {
        if (n.GetLength()) {
            if (m_srcDirListCtl.FindStringExact(-1, n) == LB_ERR) {
                m_lastSourceFolder = n;
                AddSourceFolder(n);
            }
        }
    }
}

void CJobDialog::OnRemoveSourceButton()
{
    int indx = m_srcDirListCtl.GetCurSel();
    CSrcNode *srcNode = (CSrcNode *)m_srcDirListCtl.GetItemDataPtr(indx);

    if (RemoveOK(_T("source entry"), srcNode->m_name)) {
        m_srcDirListCtl.DeleteString(indx);

        if (m_jobNode) {
            m_jobNode->m_childrenList.remove(srcNode);
        }

        m_jobNodeChanged = true;
        m_currentSrc = NULL;
        m_currentTrg = NULL;
        UpdateSrcControls();
    }
}

void CJobDialog::OnOK()
{
    UpdateSrcControls();
    CString jobname;
    m_jobNameEditCtl.GetWindowText(jobname);

    if (_tcslen(jobname) == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true, _T("Job name must be specified"),
                                               MB_ICONERROR | MB_OK);
        return;
    }

    m_jobNode->m_name = jobname;

    if (m_jobNode->m_childrenList.size() == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true, _T("No source folders are specified!"),
                                               MB_ICONERROR | MB_OK);
        return;
    }

    WORD count;

    SrcNodeListIter snode;

    for (snode = m_jobNode->m_childrenList.begin(); snode != m_jobNode->m_childrenList.end(); ++snode) {
        CSrcNode *src = *snode;

        if (src->m_childrenList.empty()) {
            CMainDlg::Singleton().InSyncMessageBox(true,
                                                   CString(_T("Source folder ")) + src->m_name + _T(" has no target folder"),
                                                   MB_ICONERROR | MB_OK);
            return;
        }
    }

    for (snode = m_jobNode->m_childrenList.begin(); snode != m_jobNode->m_childrenList.end(); ++snode) {
        CSrcNode *src = *snode;
        src->m_parentJob = m_jobNode;

        if (src->m_childrenList.size() > 1) {
            TrgNodeListIter tnode;
            count = 0;

            for (tnode = src->m_childrenList.begin(); tnode != src->m_childrenList.end(); ++tnode) {
                CTrgNode *trg = *tnode;
                trg->m_parentJob = m_jobNode;
                trg->m_parentSrc = src;

                if (trg->m_parentJob->m_mode == TWOWAY_SYNC_MODE) {
                    count++;
                }
            }

            if (count) {
                CString msg = CString(_T("Source folder '")) + src->m_name +
                              _T("' in job '") + m_jobNode->m_name +
                              _T("' has multiple destination folders of which some have an effective ") +
                              _T("2-way sync mode. This will cause unpredictable results for this ") +
                              _T(" source folder and all of its targets.");
                CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                return;
            }
        }
    }

    tr1::wregex *re;

    for (snode = m_jobNode->m_childrenList.begin(); snode != m_jobNode->m_childrenList.end(); ++snode) {
        CSrcNode *src = *snode;

        if (src->m_include.GetLength()) {
            re = ParseIncludeExcludeString(src->m_include);

            if (re) {
                delete re;
            } else {
                CString msg;
                msg.Format(_T("Source include specification \"%s\" in source \"%s\" is invalid."),
                           (LPCTSTR)src->m_include, (LPCTSTR)src->m_name);
                CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                return;
            }
        }

        if (src->m_exclude.GetLength()) {
            re = ParseIncludeExcludeString(src->m_exclude);

            if (re) {
                delete re;
            } else {
                CString msg;
                msg.Format(_T("Source exclude specification \"%s\" in source \"%s\" is invalid."),
                           (LPCTSTR)src->m_exclude, (LPCTSTR)src->m_name);
                CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                return;
            }
        }

        TrgNodeListIter tnode;

        for (tnode = src->m_childrenList.begin(); tnode != src->m_childrenList.end(); ++tnode) {
            CTrgNode *trg = *tnode;

            if (trg->m_include.GetLength()) {
                re = ParseIncludeExcludeString(trg->m_include);

                if (re) {
                    delete re;
                } else {
                    CString msg;
                    msg.Format(_T("Target include specification \"%s\" in source \"%s\" and target \"%s\" is invalid."),
                               (LPCTSTR)trg->m_include, (LPCTSTR)src->m_name, (LPCTSTR)trg->m_name);
                    CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                    return;
                }
            }

            if (trg->m_exclude.GetLength()) {
                re = ParseIncludeExcludeString(trg->m_exclude);

                if (re) {
                    delete re;
                } else {
                    CString msg;
                    msg.Format(_T("Target exclude specification \"%s\" in source \"%s\" and target \"%s\" is invalid."),
                               (LPCTSTR)trg->m_exclude, (LPCTSTR)src->m_name, (LPCTSTR)trg->m_name);
                    CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONERROR | MB_OK);
                    return;
                }
            }
        }
    }

    CInsyncDlg::OnOK();
}

void CJobDialog::UpdateSrcControls()
{
    UpdatePreTrgControls();
    int indx = m_srcDirListCtl.GetCurSel();
    BOOL oneSelected = (indx != -1);
    m_removeSrcBtnCtl.EnableWindow(oneSelected);
    m_changeSrcBtnCtl.EnableWindow(oneSelected);
    m_subDirCheckCtl.EnableWindow(oneSelected);
    m_srcExcludedDirBtnCtl.EnableWindow(oneSelected);
    m_srcInclEditCtl.EnableWindow(oneSelected);
    m_srcExclEditCtl.EnableWindow(oneSelected);
    m_srcInclStaticCtl.EnableWindow(oneSelected);
    m_srcExclStaticCtl.EnableWindow(oneSelected);
    m_removeTrgBtnCtl.EnableWindow(oneSelected);
    m_changeTrgBtnCtl.EnableWindow(oneSelected);
    m_addTrgBtnCtl.EnableWindow(oneSelected);
    m_srcVolCheckCtl.EnableWindow(oneSelected);
    m_srcFilterGroupBoxCtl.EnableWindow(oneSelected);
    m_trgDirListCtl.EnableWindow(oneSelected);
    m_subDirCheckCtl.EnableWindow(oneSelected);

    if (m_currentSrc) {
        CString name;
        m_srcInclEditCtl.GetWindowText(name);

        if (m_currentSrc->m_include != name) {
            m_currentSrc->m_include = name;
            m_jobNodeChanged = true;
        }

        m_srcExclEditCtl.GetWindowText(name);

        if (m_currentSrc->m_exclude != name) {
            m_currentSrc->m_exclude = name;
            m_jobNodeChanged = true;
        }
    }

    m_trgDirListCtl.ResetContent();
    m_currentTrg = NULL;

    if (oneSelected) {
        m_currentSrc = (CSrcNode *)m_srcDirListCtl.GetItemDataPtr(indx);
        m_currentSrcIx = indx;
        m_srcInclEditCtl.SetWindowText(m_currentSrc->m_include);
        m_srcExclEditCtl.SetWindowText(m_currentSrc->m_exclude);
        TrgNodeListIter node;

        for (node = m_currentSrc->m_childrenList.begin(); node != m_currentSrc->m_childrenList.end();
             ++node) {
            CTrgNode *trg = *node;
            indx = m_trgDirListCtl.AddString(trg->m_name);
            m_trgDirListCtl.SetItemDataPtr(indx, trg);
        }

        m_subDirCheckCtl.SetCheck(SETCHECK(m_currentSrc->m_subdir));
        m_srcVolCheckCtl.SetCheck(SETCHECK(m_currentSrc->m_useVolume));
    } else {
        m_currentSrc = NULL;
        m_srcInclEditCtl.SetWindowText(kNullString);
        m_srcExclEditCtl.SetWindowText(kNullString);
        m_subDirCheckCtl.SetCheck(BST_UNCHECKED);
        m_srcVolCheckCtl.SetCheck(BST_UNCHECKED);
    }

    if (oneSelected) {
        if (m_currentSrc && m_currentSrc->m_excludedSrcDirList.size()) {
            m_srcExcludedDirBtnCtl.SetWindowText(_T("Excluded folders*"));
        } else {
            m_srcExcludedDirBtnCtl.SetWindowText(_T("Excluded folders"));
        }
    } else {
        m_srcExcludedDirBtnCtl.SetWindowText(_T("Excluded folders"));
    }

    UpdatePostTrgControls();
}

void CJobDialog::OnSelchangeSrcDirList()
{
    UpdateSrcControls();
}

void CJobDialog::OnSubdirsCheck()
{
    if (m_currentSrc) {
        if (m_currentSrc->m_subdir ^ ISCHECKED(m_subDirCheckCtl)) {
            m_currentSrc->m_subdir ^= true;
            m_jobNodeChanged = true;
        }
    }
}

void CJobDialog::AddTargetFolder(CString folderName)
{
    UpdatePreTrgControls();
    CTrgNode *newTrgNode = new CTrgNode(m_jobNode, m_currentSrc);
    m_currentSrc->m_childrenList.push_back(newTrgNode);
    newTrgNode->m_name = folderName;
    int indx = m_trgDirListCtl.AddString(newTrgNode->m_name);
    m_trgDirListCtl.SetItemDataPtr(indx, newTrgNode);
    m_trgDirListCtl.SetCurSel(indx);
    m_jobNodeChanged = true;
    UpdatePostTrgControls();
    UpdateSrcControls();
}

void CJobDialog::OnAddTrgButton()
{
    CString n(m_lastDestFolder);
    ReplaceVolumeName(n);

    if (BrowseForFolder(n, this, _T("Target folder"))) {
        if (n.GetLength()) {
            if (m_trgDirListCtl.FindStringExact(-1, n) == LB_ERR) {
                m_lastDestFolder = n;
                AddTargetFolder(n);
            }
        }
    }
}

void CJobDialog::OnRemoveTrgButton()
{
    UpdatePreTrgControls();
    int indx = m_trgDirListCtl.GetCurSel();
    CTrgNode *trgNode = (CTrgNode *)m_trgDirListCtl.GetItemDataPtr(indx);

    if (RemoveOK(_T("target entry"), trgNode->m_name)) {
        m_trgDirListCtl.DeleteString(indx);
        m_currentSrc->m_childrenList.remove(trgNode);
        m_jobNodeChanged = true;
        m_currentTrg = NULL;
        UpdatePostTrgControls();
        UpdateSrcControls();
    }
}

void CJobDialog::OnSelchangeTrgDirList()
{
    UpdatePreTrgControls();
    UpdatePostTrgControls();
}

void CJobDialog::UpdatePreTrgControls()
{
    if (m_currentTrg) {
        CString name;
        m_trgInclEditCtl.GetWindowText(name);

        if (m_currentTrg->m_include != name) {
            m_currentTrg->m_include = name;
            m_jobNodeChanged = true;
        }

        m_trgExclEditCtl.GetWindowText(name);

        if (m_currentTrg->m_exclude != name) {
            m_currentTrg->m_exclude = name;
            m_jobNodeChanged = true;
        }
    }
}

void CJobDialog::UpdatePostTrgControls()
{
    int indx = m_trgDirListCtl.GetCurSel();
    BOOL oneSelected = (indx != -1);
    m_removeTrgBtnCtl.EnableWindow(oneSelected);
    m_changeTrgBtnCtl.EnableWindow(oneSelected);
    m_trgInclEditCtl.EnableWindow(oneSelected);
    m_trgExclEditCtl.EnableWindow(oneSelected);
    m_trgInclStaticCtl.EnableWindow(oneSelected);
    m_trgExclStaticCtl.EnableWindow(oneSelected);
    m_trgFilterGroupBoxCtl.EnableWindow(oneSelected);
    m_trgVolCheckCtl.EnableWindow(oneSelected);

    if (oneSelected) {
        m_currentTrg = (CTrgNode *)m_trgDirListCtl.GetItemDataPtr(indx);
        m_currentTrgIx = indx;
        m_trgDontDeleteCheckCtl.EnableWindow(oneSelected
                                             && (m_currentTrg->m_parentJob->m_mode == MIRROR_SYNC_MODE));
        m_trgInclEditCtl.SetWindowText(m_currentTrg->m_include);
        m_trgExclEditCtl.SetWindowText(m_currentTrg->m_exclude);
        m_trgVolCheckCtl.SetCheck(SETCHECK(m_currentTrg->m_useVolume));
        m_trgDontDeleteCheckCtl.SetCheck(SETCHECK(m_currentTrg->m_dontDelete));
    } else {
        m_currentTrg = NULL;
        m_trgDontDeleteCheckCtl.EnableWindow(false);
        m_trgInclEditCtl.SetWindowText(kNullString);
        m_trgExclEditCtl.SetWindowText(kNullString);
        m_trgVolCheckCtl.SetCheck(BST_UNCHECKED);
        m_trgDontDeleteCheckCtl.SetCheck(BST_UNCHECKED);
    }
}

bool CJobDialog::RemoveOK(LPCTSTR object, LPCTSTR name) const
{
    CString msg(_T("Do you really want to remove "));
    msg += object;
    msg += _T(" '");
    msg += name;
    msg += _T("' ?");
    return CMainDlg::Singleton().InSyncMessageBox(true, msg, MB_ICONQUESTION | MB_YESNO) == IDYES;
}

void CJobDialog::OnChangeJobNameEdit()
{
    m_jobNodeChanged = m_jobNameChanged;
}

void CJobDialog::OnChangeSrcButton()
{
    int indx = m_srcDirListCtl.GetCurSel();
    CSrcNode *srcNode = (CSrcNode *)m_srcDirListCtl.GetItemDataPtr(indx);
    CString n(srcNode->m_name);
    ReplaceVolumeName(n);

    if (BrowseForFolder(n, this, _T("Source folder"))) {
        if (srcNode->m_useVolume) {
            ReplaceDriveLetter(n);
        }

        if (n.GetLength()) {
            srcNode->m_name = n;
            m_srcDirListCtl.DeleteString(indx);
            m_srcDirListCtl.InsertString(indx, n);
            m_srcDirListCtl.SetCurSel(indx);
            m_srcDirListCtl.SetItemDataPtr(indx, srcNode);
            m_jobNodeChanged = true;
            UpdateSrcControls();
        }
    }
}

void CJobDialog::OnChangeTrgButton()
{
    UpdatePreTrgControls();
    int indx = m_trgDirListCtl.GetCurSel();
    CTrgNode *trgNode = (CTrgNode *)m_trgDirListCtl.GetItemDataPtr(indx);
    CString n(trgNode->m_name);
    ReplaceVolumeName(n);

    if (BrowseForFolder(n, this, _T("Target folder"))) {
        if (trgNode->m_useVolume) {
            ReplaceDriveLetter(n);
        }

        if (n.GetLength()) {
            m_trgDirListCtl.DeleteString(indx);
            trgNode->m_name = n;
            m_trgDirListCtl.InsertString(indx, n);
            m_trgDirListCtl.SetCurSel(indx);
            m_trgDirListCtl.SetItemDataPtr(indx, trgNode);
            m_jobNodeChanged = true;
            UpdatePostTrgControls();
        }
    }
}

void CJobDialog::OnSrcExcludedDirButton()
{
    if (m_currentSrc) {
        CExcludedDirectoriesDlg dlg;
        dlg.m_useVolume = m_currentSrc->m_useVolume;
        dlg.m_folderName = m_currentSrc->m_name;
        dlg.m_excludedDirListRoot = m_currentSrc->m_excludedSrcDirList;
        bool changed = dlg.DoModal() == IDOK;
        changed = changed && dlg.m_listChanged;

        if (changed) {
            m_currentSrc->m_excludedSrcDirList.clear();
            m_currentSrc->m_excludedSrcDirList = dlg.m_excludedDirListRoot;
            m_jobNodeChanged = true;
        }
    }
}

LRESULT CJobDialog::OnAddFolder(WPARAM wParam, LPARAM lParam)
{
    CDirListBox *dirList = (CDirListBox *)wParam;
    LPTSTR fName = (LPTSTR)lParam;

    if (dirList == &m_srcDirListCtl) {
        AddSourceFolder(fName);
    } else if (dirList == &m_trgDirListCtl) {
        AddTargetFolder(fName);
    }

    delete [] fName;
    return 0;
}

void CJobDialog::OnBnClickedSrcVolCheck()
{
    if (m_currentSrc) {
        m_currentSrc->m_useVolume = ISCHECKED(m_srcVolCheckCtl);
        m_jobNodeChanged = true;
        CString name(m_currentSrc->m_name);

        if (m_currentSrc->m_useVolume) {
            ReplaceDriveLetter(name);
        } else {
            ReplaceVolumeName(name);
        }

        m_srcDirListCtl.DeleteString(m_currentSrcIx);

        if (m_currentSrc->m_useVolume) {
            ReplaceDriveLetter(name);
        }

        m_currentSrc->m_name = name;
        m_srcDirListCtl.InsertString(m_currentSrcIx, name);
        m_srcDirListCtl.SetCurSel(m_currentSrcIx);
        m_srcDirListCtl.SetItemDataPtr(m_currentSrcIx, m_currentSrc);
        UpdateSrcControls();
    }
}

void CJobDialog::OnBnClickedTrgVolCheck()
{
    if (m_currentTrg) {
        m_currentTrg->m_useVolume = ISCHECKED(m_trgVolCheckCtl);
        m_jobNodeChanged = true;
        CString name(m_currentTrg->m_name);

        if (m_currentTrg->m_useVolume) {
            ReplaceDriveLetter(name);
        } else {
            ReplaceVolumeName(name);
        }

        m_trgDirListCtl.DeleteString(m_currentTrgIx);

        if (m_currentTrg->m_useVolume) {
            ReplaceDriveLetter(name);
        }

        m_currentTrg->m_name = name;
        m_trgDirListCtl.InsertString(m_currentTrgIx, name);
        m_trgDirListCtl.SetCurSel(m_currentTrgIx);
        m_trgDirListCtl.SetItemDataPtr(m_currentTrgIx, m_currentTrg);
        UpdatePostTrgControls();
    }
}

void CJobDialog::OnCbnSelchangeCopyAclsCombo()
{
    int i = m_copyAclsComboCtl.GetCurSel();
    BYTE s = aclComboValues[i];
    m_jobNode->m_changedFileACLs = (s & 1) != 0;
    m_jobNode->m_unchangedFileACLs = (s & 2) != 0;
    m_jobNode->m_folderACLs = (s & 4) != 0;

    if (m_jobNode->m_folderACLs) {
        CMainDlg::Singleton().InSyncMessageBox(true,
                                               _T("Warning: This option is extremely file system intensive and will slow synchronization."),
                                               MB_ICONWARNING | MB_OK);
    } else if (m_jobNode->m_unchangedFileACLs) {
        CMainDlg::Singleton().InSyncMessageBox(true,
                                               _T("Warning: This option is very file system intensive and will slow synchronization."),
                                               MB_ICONWARNING | MB_OK);
    }

    m_jobNodeChanged = true;
}


void CJobDialog::OnClose()
{
    if (m_jobNodeChanged) {
        if (CMainDlg::Singleton().InSyncMessageBox(true,
                _T("You have changed this job. Are you sure you want to discard these changes?"),
                MB_ICONQUESTION | MB_YESNO) != IDYES) {
            return;
        }
    }

    CInsyncDlg::OnClose();
}

void CJobDialog::OnBnClickedDontDeleteCheck()
{
    if (m_currentTrg) {
        m_currentTrg->m_dontDelete = ISCHECKED(m_trgDontDeleteCheckCtl);
        m_jobNodeChanged = true;
        UpdatePostTrgControls();
    }
}

void CJobDialog::OnLbnSelchangeRunAfterList()
{
    m_jobNode->m_runAfterList.clear();
    int numSel = m_runAfterListboxCtl.GetSelCount();

    if (numSel) {
        int *selects = new int[numSel];
        m_runAfterListboxCtl.GetSelItems(numSel, selects);
        int i;

        for (i = 0; i < numSel; i++) {
            CString name;
            m_runAfterListboxCtl.GetText(selects[i], name);
            m_jobNode->m_runAfterList.insert(name);
        }

        delete [] selects;
    }

    m_jobNodeChanged = true;
}
