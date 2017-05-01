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

class CInsyncDlg : public CDialogEx {
    DECLARE_DYNAMIC(CInsyncDlg)

public:
    CInsyncDlg(DWORD idd, DWORD helpId, CWnd *pParent = NULL);   // standard constructor
    virtual ~CInsyncDlg();
    afx_msg void OnIdhelp();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnInitDialog();
    HICON LoadResourceIcon(const UINT, const WORD, const WORD) const;

protected:
    virtual void DoDataExchange(CDataExchange *pDX) = 0;
    CInsyncToolTipCtrl *m_pToolTip;
    bool BrowseForFolder(CString &, CWnd *, LPCTSTR);

    DECLARE_MESSAGE_MAP()

private:
    DWORD m_helpId;
    int m_idd;
};

class CDlgOccManager : public COccManager {
public:
    const DLGTEMPLATE *CDlgOccManager::PreCreateDialog(_AFX_OCC_DIALOG_INFO *, const DLGTEMPLATE *);
};

