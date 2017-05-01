#pragma once
//****************************************************************************
//
// Copyright (c) 1998-2014 Dillobits Software Inc.
//
// This program is the proprietary software of Dillobits Software and/or its
// licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Dillobits Software (an "Authorized
// License"). Except as set forth in an Authorized License, Dillobits Software
// grants no license (express or implied), right to use, or waiver of any kind
// with respect to the Software, and Dillobits Software expressly reserves all
// rights in and to the Software and all intellectual property rights therein.
// IF YOU HAVE NOT AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
// SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY DILLOBITS SOFTWARE AND
// DISCONTINUE ALL USE OF THE SOFTWARE.
//
//****************************************************************************

class CExcludedDirectoriesDlg : public CInsyncDlg {
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
