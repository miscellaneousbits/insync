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

class CSchedulerDialog : public CInsyncDlg {
public:
    CSchedulerDialog(CWnd *pParent = NULL);   // standard constructor

    virtual BOOL OnInitDialog();

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnBnClickedAddEventButton();
    afx_msg void OnBnClickedEditEventButton();
    afx_msg void OnBnClickedRemoveEventButton();
    afx_msg void OnLbnSelchangeEventList();
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()

private:
    void EnableControls();

    enum DlgId {
        IDD = IDD_SCHEDULE_DIALOG
    };

    CStatic m_mandalaStaticCtl;
    CListBox m_eventListCtl;
    CInSyncButton m_editBtnCtl;
    CInSyncButton m_removeBtnCtl;
    CInSyncButton m_addBtnCtl;
    CInSyncButton m_helpBtnCtl;
};
