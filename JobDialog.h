#pragma once

class CJobDialog : public CInsyncDlg
{
public:
    CJobDialog(CWnd *pParent = NULL);   // standard constructor

    virtual BOOL OnInitDialog();

    CJobNode *m_jobNode;
    bool m_jobNodeChanged;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnAddSourceButton();
    afx_msg void OnRemoveSourceButton();
    afx_msg void OnAddTrgButton();
    afx_msg void OnRemoveTrgButton();
    virtual void OnOK();
    afx_msg void OnSelchangeSrcDirList();
    afx_msg void OnSelchangeTrgDirList();
    afx_msg void OnSubdirsCheck();
    afx_msg void OnChangeJobNameEdit();
    afx_msg void OnChangeSrcButton();
    afx_msg void OnChangeTrgButton();
    afx_msg void OnSrcExcludedDirButton();
    afx_msg LRESULT OnAddFolder(WPARAM, LPARAM);
    afx_msg void OnBnClickedSrcVolCheck();
    afx_msg void OnBnClickedTrgVolCheck();
    afx_msg void OnCbnSelchangeCopyAclsCombo();
    afx_msg void OnClose();
    afx_msg void OnBnClickedDontDeleteCheck();

    DECLARE_MESSAGE_MAP()

private:
    void UpdateSrcControls();
    void UpdatePreTrgControls();
    void UpdatePostTrgControls();
    bool RemoveOK(LPCTSTR, LPCTSTR) const;
    void AddSourceFolder(CString);
    void AddTargetFolder(CString);

    enum DlgId {
        IDD = IDD_JOB_EDIT_DIALOG
    };

    CSrcNode *m_currentSrc;
    CTrgNode *m_currentTrg;
    CInSyncButton m_srcExcludedDirBtnCtl;
    CInSyncButton m_changeTrgBtnCtl;
    CInSyncButton m_changeSrcBtnCtl;
    CInSyncButton m_removeTrgBtnCtl;
    CInSyncButton m_addTrgBtnCtl;
    CInSyncButton m_removeSrcBtnCtl;
    CInSyncButton m_addSrcBtnCtl;
    CInSyncButton m_helpButtonCtl;
    CInSyncButton m_okButtonCtl;
    CStatic m_trgGroupStaticCtl;
    CStatic m_srcInclStaticCtl;
    CStatic m_srcExclStaticCtl;
    CStatic m_trgInclStaticCtl;
    CStatic m_trgExclStaticCtl;
    CEdit m_srcInclEditCtl;
    CEdit m_srcExclEditCtl;
    CEdit m_trgInclEditCtl;
    CEdit m_trgExclEditCtl;
    CDirListBox m_srcDirListCtl;
    CDirListBox m_trgDirListCtl;
    CInSyncCheck m_subDirCheckCtl;
    CEdit m_jobNameEditCtl;
    CStatic m_mandalaStaticCtl;
    CStatic m_insyncStaticCtl;
    CStatic m_srcFilterGroupBoxCtl;
    CStatic m_trgFilterGroupBoxCtl;
    CString m_lastSourceFolder;
    CString m_lastDestFolder;

    int m_currentTrgIx;
    int m_currentSrcIx;
    CInSyncCheck m_srcVolCheckCtl;
    CInSyncCheck m_trgVolCheckCtl;
    CInSyncCheck m_trgDontDeleteCheckCtl;
    CComboBox m_copyAclsComboCtl;
    bool m_jobNameChanged;
    CInsyncListbox m_runAfterListboxCtl;
public:
    afx_msg void OnLbnSelchangeRunAfterList();
};

class CJobDialogPtr
{
public:
    CJobDialogPtr()
    {
        m_ptr = new CJobDialog;
    }
    ~CJobDialogPtr()
    {
        delete m_ptr;
    }
    CJobDialog *m_ptr;
};
