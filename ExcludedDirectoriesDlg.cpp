#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
// CExcludedDirectoriesDlg dialog

CExcludedDirectoriesDlg::CExcludedDirectoriesDlg(CWnd *pParent/*=NULL*/) :
    CInsyncDlg(IDD, HtmlJobEdit, pParent),
    m_listChanged(false) {}

void CExcludedDirectoriesDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ADD_BUTTON, m_addButtonCtl);
    DDX_Control(pDX, IDC_EXCLUDED_DIRECTORY_LIST, m_excludedDirectoryListboxCtl);
    DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeBtnCtl);
    DDX_Control(pDX, IDOK, m_saveButtonCtl);
}

BEGIN_MESSAGE_MAP(CExcludedDirectoriesDlg, CInsyncDlg)
    ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
    ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
    ON_LBN_SELCHANGE(IDC_EXCLUDED_DIRECTORY_LIST, OnSelchangeExcludedDirectoryList)
    ON_MESSAGE(WM_USER_ADD_FOLDER, OnAddFolder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExcludedDirectoriesDlg message handlers

void CExcludedDirectoriesDlg::UpdateControlStates()
{
    m_removeBtnCtl.EnableWindow(m_excludedDirectoryListboxCtl.GetCurSel() != LB_ERR);
}

BOOL CExcludedDirectoriesDlg::OnInitDialog()
{
    CInsyncDlg::OnInitDialog();

    m_addButtonCtl.Init(IDI_ADD, m_pToolTip,
                        _T("Add a folders to exclude from operation."));
    m_removeBtnCtl.Init(IDI_REMOVE, m_pToolTip,
                        _T("Remove selected folder from excluded list of folders."));
    m_saveButtonCtl.Init(IDI_SAVE, m_pToolTip,
                         _T("Save list of excluded folders."));

    StrSetIter node;

    for (node = m_excludedDirListRoot.begin(); node != m_excludedDirListRoot.end(); ++node) {
        CString name(*node);

        if (m_useVolume) {
            ReplaceDriveLetter(name);
        } else {
            ReplaceVolumeName(name);
        }

        m_excludedDirectoryListboxCtl.AddString(name);
    }

    m_listChanged = false;
    UpdateControlStates();
    return TRUE;
}

void CExcludedDirectoriesDlg::AddExcludedFolder(CString folderName)
{
    if (m_excludedDirectoryListboxCtl.FindStringExact(-1, folderName) == LB_ERR) {
        int indx = m_excludedDirectoryListboxCtl.AddString(folderName);
        m_excludedDirectoryListboxCtl.SetCurSel(indx);
        m_folderName = folderName;
        m_listChanged = true;
        UpdateControlStates();
    }
}

void CExcludedDirectoriesDlg::OnAddButton()
{
    CString n(m_folderName);
    ReplaceVolumeName(n);

    if (BrowseForFolder(n, this, _T("Excluded folder"))) {
        if (n.GetLength()) {
            if (m_useVolume) {
                ReplaceDriveLetter(n);
            }

            AddExcludedFolder(n);
        }
    }
}

void CExcludedDirectoriesDlg::OnRemoveButton()
{
    int indx = m_excludedDirectoryListboxCtl.GetCurSel();

    if (indx != LB_ERR) {
        m_excludedDirectoryListboxCtl.DeleteString((unsigned int)indx);
        m_listChanged = true;
    }

    UpdateControlStates();
}

void CExcludedDirectoriesDlg::OnOK()
{
    CInsyncDlg::OnOK();

    if (m_listChanged) {
        m_excludedDirListRoot.clear();
        int items = m_excludedDirectoryListboxCtl.GetCount();

        for (int indx = 0; indx < items; indx++) {
            CString name;
            m_excludedDirectoryListboxCtl.GetText(indx, name);

            if (m_useVolume) {
                ReplaceDriveLetter(name);
            } else {
                ReplaceVolumeName(name);
            }

            m_excludedDirListRoot.insert(name);
        }
    }
}

void CExcludedDirectoriesDlg::OnSelchangeExcludedDirectoryList()
{
    UpdateControlStates();
}

LRESULT CExcludedDirectoriesDlg::OnAddFolder(WPARAM wParam, LPARAM lParam)
{
    ASSERT(wParam == wParam);
    CString name((LPTSTR)lParam);
    free((LPTSTR)lParam);

    if (m_useVolume) {
        ReplaceDriveLetter(name);
    } else {
        ReplaceVolumeName(name);
    }

    AddExcludedFolder(name);
    return 0;
}
