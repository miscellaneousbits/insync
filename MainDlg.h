#pragma once
#include "afxwin.h"
#include "afxcmn.h"
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

class CMainDlg : public CInsyncDlg {
public:
    CMainDlg(CWnd *pParent = NULL);   // standard constructor

    static CMainDlg &Singleton() {
        return *m_this;
    }

    enum TimerType {
        MANDALA_TIMER = 1,
        SHUTDOWN_TIMER,
        ABOUT_TIMER,
        STARTUP_TIMER,
        STARTUP_SIM_TIMER
    };

    typedef enum LedColor {
        LED_GREEN = 0,
        LED_YELLOW,
        LED_RED,
        LED_COLORS
    } LedColor;

    int InSyncMessageBox(bool, LPCTSTR, UINT, int iDefault = IDOK, LPCTSTR msgId = NULL) const;
    void SetStatusLedColor(LedColor);
    void ViewLogFile(CString) const;
    bool LogFileViewable(const CJobNode *) const;
    WORD MaxThreads();
    WORD DefaultMaxThreads();
    VOID MaxThreads(WORD max);
    bool VerboseLog();
    void ConnectPath(CFilePath dir);

    virtual BOOL OnInitDialog();

    JobNodeList m_jobList;
    CInsyncListbox m_jobListCtl;
    LPTSTR m_lpCmdline;
    CString m_locale;
    SYSTEM_INFO m_systemInfo;
    ITaskbarList3 *m_ptl;
    CFont m_boldfont;
    NUMBERFMT m_numberFormat;

    bool m_copyLocked : 1;
    bool m_tipsOff : 1;
    bool m_snapshotUsed : 1;
    bool m_autoRun : 1;
    bool m_verboseLog : 1;


    volatile bool m_globalAbort;
    volatile bool m_endSession;

protected:
    afx_msg void OnAddJobButton();
    afx_msg void OnEditJobButton();
    afx_msg void OnRemoveJobButton();
    afx_msg void OnRunJobsButton();
    afx_msg void OnSelchangeJobList();
    afx_msg LRESULT OnJobsDone(WPARAM, LPARAM);
    afx_msg LRESULT OnUserSetLed(WPARAM, LPARAM);
    afx_msg void OnAbortJobsButton();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSimulateJobsButton();
    afx_msg void OnSaveJobsButton();
    afx_msg void OnCloneJobButton();
    afx_msg void OnMirrorJobButton();
    afx_msg void OnBnClickedScheduleJobsButton();
    afx_msg void OnBnClickedViewLogButton();
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnBnClickedTipsOffCheck();
    afx_msg void OnClose();
    afx_msg void OnCbnSelchangeMaxThreadsCombo();
    afx_msg BOOL OnQueryEndSession();
    afx_msg void OnEndSession(BOOL bEnding);

    DECLARE_MESSAGE_MAP()

private:
    static CMainDlg *m_this;
    typedef BOOL (WINAPI *ShutdownBlockReasonCreateFunction)(HWND, LPCWSTR);
    typedef BOOL (WINAPI *ShutdownBlockReasonDestroyFunction)(HWND);
    void RegisterTaskbar();
    void ParseJobName(LPCTSTR s);
    void ParseFileOrJobName(LPCTSTR s);
    bool UpdateJobControls();
    void DisplayCounts();
    void DisplayCount(UINT64 count, DWORD id, bool scale);
    void DisplayCount(UINT64 count, DWORD id, bool scale, LPCTSTR tail);
    void ZeroDisplays();
    void ZeroDisplay(DWORD);
    bool RemoveOK(LPCTSTR name);
    void RunJobs(bool);
    void ClearJobs();
    void SaveOptions();
    void SetDialogIcon(WORD);
    CString AvailableUnnamed() const;
    void DisconnectShares();

    enum DlgId {
        IDD = IDD_INSYNC_DIALOG
    };

    CJobNode *m_currentJob;
    CInSyncButton m_mirrorJobBtnCtl;
    CInSyncButton m_viewLogBtnCtl;
    CInSyncButton m_abortJobBtnCtl;
    CInSyncButton m_runJobBtnCtl;
    CInSyncButton m_removeJobBtnCtl;
    CInSyncButton m_editJobBtnCtl;
    CInSyncButton m_addJobBtnCtl;
    CInSyncButton m_scheduleJobBtnCtl;
    CInSyncButton m_cloneJobBtnCtl;
    CInSyncButton m_saveJobsButtonCtl;
    CInSyncButton m_simulateJobBtnCtl;
    CInSyncButton m_helpButtonCtl;
    CInSyncCheck m_logSyncDirCheckCtl;
    CInSyncCheck m_logSkippedCheckCtl;
    CInSyncCheck m_logAppendCheckCtl;
    CInSyncCheck m_logSummaryCheckCtl;
    CInSyncCheck m_errorPopupCheckCtl;
    CInSyncCheck m_promptBeforeRunningCtl;
    CInSyncCheck m_jobVerifyCheckCtl;
    CStatic m_criteriaStaticCtl;
    CStatic m_mandalaStaticCtl;
    CStatic m_jobLogStaticCtl;
    CStatic m_ledStaticCtl;
    CStatic m_ledIconStaticCtl;
    CStatic m_insyncStaticCtl;
    CStatic m_copyrightStaticCtl;
    CStatic m_folderGroupCtl;
    CStatic m_progressGroupCtl;
    CStatic m_jobGroupCtrl;
    CStatic m_configStaticCtl;
    CProgressCtrl m_runningThreadsCtl;
    static const int kUpperRange = 64;
    CInsyncComboBox m_jobModeComboCtl;
    CInSyncCheck m_tipsOffCheckCtl;
    CInSyncCheck m_verboseLogCheck;
    CInsyncComboBox m_maxThreadsComboCtl;
    CStatic m_maxThreadsStaticCtl;
    CMFCEditBrowseCtrl m_jobLogEditCtl;
    CStatic m_endOfRunStaticCtl;
    CComboBox m_endOfRunComboCtl;
    CString m_jobLogName;
    WORD m_mandalaIx;
    HICON m_mandala[18];
    HICON m_mandala_small[18];
    CTime m_startTime;
    LedColor m_highColor;
    HICON m_ledIcons[LED_COLORS];
    CString m_loadJobsRegKey;
    CString m_saveJobsRegKey;
    CString m_loadCritsRegKey;
    CString m_saveCritsRegKey;
    UINT64 m_lastCopied;
    CRun m_run;
    ShutdownBlockReasonCreateFunction m_shutdownBlockReasonCreate;
    ShutdownBlockReasonDestroyFunction m_shutdownBlockReasonDestroy;
    WORD m_maxThreads;
    int m_oldpos;
    StrSet m_shares;
    CXmlOptions m_xml;
};
