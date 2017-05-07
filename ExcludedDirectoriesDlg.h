#pragma once

class CExcludedDirectoriesDlg : public CInsyncDlg
{
public:
    CExcludedDirectoriesDlg(CWnd *pParent = NULL);   // standard constructor

    virtual BOOL OnInitDialog();

    StrSet m_excludedDirListRoot;
    CString m_folderName;
    bool m_listChanged : 1;
    bool m_useVolume : 1;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnAddButton();
    afx_msg void OnRemoveButton();
    virtual void OnOK();
    afx_msg void OnSelchangeExcludedDirectoryList();
    afx_msg LRESULT OnAddFolder(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()

private:
    void UpdateControlStates();
    void AddExcludedFolder(CString);

    enum DlgId {
        IDD = IDD_EXCLUDED_DIRECTORIES_DIALOG
    };

    CDirListBox m_excludedDirectoryListboxCtl;
    CInSyncButton m_removeBtnCtl;
    CInSyncButton m_addButtonCtl;
    CInSyncButton m_saveButtonCtl;
};
