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

class CScheduleEventDialog : public CInsyncDlg {
public:
    CScheduleEventDialog(CWnd *pParent = NULL);   // standard constructor

    CScheduledTask *m_event;

    enum DlgId {
        IDD = IDD_EVENT_DIALOG
    };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnBnClickedOnceRadio();
    afx_msg void OnBnClickedDaylyRadio();
    afx_msg void OnBnClickedWeeklyRadio();
    afx_msg void OnBnClickedMonthlyRadio();
    afx_msg void OnBnClickedStartupRadio();
    afx_msg void OnBnClickedHasEndDateCheck();

    DECLARE_MESSAGE_MAP()

private:
    void EnableControls();

    CDateTimeCtrl m_endDateCtl;
    CDateTimeCtrl m_endTimeCtl;
    CDateTimeCtrl m_startDateCtl;
    CDateTimeCtrl m_startTimeCtl;
    CEdit m_commentEditCtl;
    CEdit m_eventNameCtl;
    CEdit m_parametersEditCtl;
    CEdit m_userNameEditCtl;
    CEdit m_userPasswordEditCtl;
    CListBox m_jobList;
    CStatic m_endDateStaticCtl;
    CStatic m_mandalaStaticCtl;
    CStatic m_passwordStaticCtl;
    CStatic m_userNameStaticCtl;
    CInSyncCheck m_autoDeleteCheckCtl;
    CInSyncCheck m_daylyRadioBtnCtl;
    CInSyncCheck m_hasEndDateCheckCtl;
    CInSyncCheck m_hourlyRadioBtnCtl;
    CInSyncCheck m_monthlyRadioBtnCtl;
    CInSyncCheck m_onceRadioBtnCtl;
    CInSyncCheck m_startupRadioBtnCtl;
    CInSyncCheck m_weeklyRadioBtnCtl;
    CInSyncButton m_saveBtnCtl;
    CInSyncButton m_helpBtnCtl;
};
