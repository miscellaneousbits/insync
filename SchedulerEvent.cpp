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

#include "stdafx.h"

// CScheduleEventDialog dialog

CScheduleEventDialog::CScheduleEventDialog(CWnd *pParent/*=NULL*/) :
    CInsyncDlg(IDD, HtmlSchedule, pParent) {}

void CScheduleEventDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EVENT_NAME_EDIT, m_eventNameCtl);
    DDX_Control(pDX, IDC_START_DATE_DATETIMEPICKER, m_startDateCtl);
    DDX_Control(pDX, IDC_END_DATE_DATETIMEPICKER, m_endDateCtl);
    DDX_Control(pDX, IDC_USER_NAME_STATIC, m_userNameStaticCtl);
    DDX_Control(pDX, IDC_USER_NAME_EDIT, m_userNameEditCtl);
    DDX_Control(pDX, IDC_USER_PASSWORD_EDIT, m_userPasswordEditCtl);
    DDX_Control(pDX, IDC_MANDALA_STATIC, m_mandalaStaticCtl);
    DDX_Control(pDX, IDC_COMMENT_EDIT, m_commentEditCtl);
    DDX_Control(pDX, IDC_USER_PASSWORD_STATIC, m_passwordStaticCtl);
    DDX_Control(pDX, IDC_START_TIME_DATETIMEPICKER, m_startTimeCtl);
    DDX_Control(pDX, IDC_EVENT_JOB_LIST, m_jobList);
    DDX_Control(pDX, IDC_ONCE_RADIO, m_onceRadioBtnCtl);
    DDX_Control(pDX, IDC_DAYLY_RADIO, m_daylyRadioBtnCtl);
    DDX_Control(pDX, IDC_WEEKLY_RADIO, m_weeklyRadioBtnCtl);
    DDX_Control(pDX, IDC_MONTHLY_RADIO, m_monthlyRadioBtnCtl);
    DDX_Control(pDX, IDC_AUTO_DELETE_CHECK, m_autoDeleteCheckCtl);
    DDX_Control(pDX, IDC_END_DATE_STATIC, m_endDateStaticCtl);
    DDX_Control(pDX, IDC_STARTUP_RADIO, m_startupRadioBtnCtl);
    DDX_Control(pDX, IDC_PARAMETERS_EDIT, m_parametersEditCtl);
    DDX_Control(pDX, IDHELP, m_helpBtnCtl);
    DDX_Control(pDX, IDOK, m_saveBtnCtl);
    DDX_Control(pDX, IDC_HAS_END_DATE_CHECK, m_hasEndDateCheckCtl);
}

BEGIN_MESSAGE_MAP(CScheduleEventDialog, CInsyncDlg)
    ON_BN_CLICKED(IDC_ONCE_RADIO, OnBnClickedOnceRadio)
    ON_BN_CLICKED(IDC_DAYLY_RADIO, OnBnClickedDaylyRadio)
    ON_BN_CLICKED(IDC_WEEKLY_RADIO, OnBnClickedWeeklyRadio)
    ON_BN_CLICKED(IDC_MONTHLY_RADIO, OnBnClickedMonthlyRadio)
    ON_BN_CLICKED(IDC_STARTUP_RADIO, OnBnClickedStartupRadio)
    ON_BN_CLICKED(IDC_HAS_END_DATE_CHECK, OnBnClickedHasEndDateCheck)
    ON_WM_HELPINFO()
END_MESSAGE_MAP()

void CScheduleEventDialog::EnableControls() {
    m_hasEndDateCheckCtl.EnableWindow(!ISCHECKED(m_onceRadioBtnCtl));
    bool enableEndDate = (!ISCHECKED(m_onceRadioBtnCtl) && ISCHECKED(m_hasEndDateCheckCtl));
    m_endDateCtl.EnableWindow(enableEndDate);
    m_endDateStaticCtl.EnableWindow(enableEndDate);
}

static CString GetUserAccount() {
    HANDLE hToken;
    TCHAR user[MAX_PATH];
    TCHAR domain[MAX_PATH];
    if (!::OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &hToken)) {
        if (::GetLastError() != ERROR_NO_TOKEN) {
            return CString(kNullString);
        }
        if (!::OpenProcessToken(InSyncApp.m_currentProcess, TOKEN_QUERY, &hToken)) {
            return CString(kNullString);
        }
    }
    DWORD uilen = 0;
    if (!::GetTokenInformation(hToken, TokenUser, NULL, 0, &uilen)) {
        if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            return CString(kNullString);
        }
    }
    PTOKEN_USER ui = (PTOKEN_USER)new char[uilen];
    if (!::GetTokenInformation(hToken, TokenUser, ui, uilen, &uilen)) {
        delete [] ui;
        return CString(kNullString);
    }
    SID_NAME_USE use;
    if (ui) {
        if (!::LookupAccountSid(NULL, ui->User.Sid, user, &uilen, domain, &uilen, &use)) {
            delete[] ui;
            return CString(kNullString);
        }
        delete[] ui;
        CString r;
        r = domain;
        r += _T("\\");
        r += user;
        return r;
    }
    return _T("unknownuser");
}

BOOL CScheduleEventDialog::OnInitDialog() {
    CInsyncDlg::OnInitDialog();
    m_helpBtnCtl.Init(IDI_HELP, m_pToolTip,
                      _T("Open the help viewer"));
    m_saveBtnCtl.Init(IDI_SAVE, m_pToolTip,
                      _T("Save this scheduled event."));

    m_startTimeCtl.ModifyStyle(0, DTS_TIMEFORMAT);
    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_INSYNC_ICON, 48, 48));
    JobNodeListIter jobNode;
    for (jobNode = CMainDlg::Singleton().m_jobList.begin(); jobNode != CMainDlg::Singleton().m_jobList.end(); ++jobNode) {
        m_jobList.AddString((*jobNode)->m_name);
    }
    CString param = m_event->m_sParameters;
    CString p, name;
    int b = 0;
    int l = param.GetLength();
    while (b < l) {
        while ((b < l) && (param[b] != _T('"'))) {
            b++;
        }
        b++;
        if (b >= l) {
            break;
        }
        int e = b;
        while ((e < l) && (param[e] != _T('"'))) {
            e++;
        }
        if (e >= l) {
            break;
        }
        p = param.Mid(b, e - b);
        for (int indx = 0; indx < m_jobList.GetCount(); indx++) {
            m_jobList.GetText(indx, name);
            if (name == p) {
                m_jobList.SetSel(indx, TRUE);
                break;
            }
        }
        b = e + 1;
    }
    name = m_event->m_name;
    m_eventNameCtl.SetWindowText(name);
    if (name.GetLength()) {
        CTime t;
        m_event->GetStartDateTime(t);
        m_startDateCtl.SetTime(&t);
        m_startTimeCtl.SetTime(&t);
        m_event->GetEndDate(t);
        m_endDateCtl.SetTime(&t);
    }
    CString account = m_event->m_sAccount;
    if (account.GetLength() == 0) {
        account = GetUserAccount();
    }
    m_userNameEditCtl.SetWindowText(account);
    m_userPasswordEditCtl.SetWindowText(kNullString);
    m_commentEditCtl.SetWindowText(m_event->m_sComment);
    m_autoDeleteCheckCtl.SetCheck(SETCHECK(m_event->m_autoDelete));
    m_hasEndDateCheckCtl.SetCheck(SETCHECK(m_event->m_hasEndDate));
    m_parametersEditCtl.SetWindowText(m_event->m_parameters);
    switch (m_event->m_eFreq) {
    case CScheduledTask::freqOnce:
        m_onceRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    case CScheduledTask::freqWeekly:
        m_weeklyRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    case CScheduledTask::freqDaily:
        m_daylyRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    case CScheduledTask::freqMonthly:
        m_monthlyRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    case CScheduledTask::freqStartup:
        m_startupRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    default:
        m_onceRadioBtnCtl.SetCheck(BST_CHECKED);
        break;
    }
    EnableControls();
    return TRUE;
}

void CScheduleEventDialog::OnOK() {
    CString eventName;
    m_eventNameCtl.GetWindowText(eventName);
    if (eventName.GetLength() == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true, _T("Name must be specified for event"), MB_ICONERROR | MB_OK);
        return;
    }
    CString userName;
    m_userNameEditCtl.GetWindowText(userName);
    if (userName.GetLength() == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true, _T("Windows account must be specified for event"), MB_ICONERROR | MB_OK);
        return;
    }
    CString userPassword;
    m_userPasswordEditCtl.GetWindowText(userPassword);
    if (userPassword.GetLength() == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true,
                                               _T("Windows password must be specified for event. Windows")
                                               _T(" does not support the scheduling of tasks using an account with no password."),
                                               MB_ICONERROR | MB_OK);
        return;
    }
    if (m_jobList.GetSelCount() == 0) {
        CMainDlg::Singleton().InSyncMessageBox(true, _T("At least one job must be selected"), MB_ICONERROR | MB_OK);
        return;
    }
    bool notFirst = false;
    CString param;
    CString additional;
    m_parametersEditCtl.GetWindowText(additional);
    m_event->m_parameters = additional;
    if (additional.GetLength()) {
        param = additional;
        notFirst = true;
    }
    for (int indx = 0; indx < m_jobList.GetCount(); indx++) {
        if (m_jobList.GetSel(indx)) {
            CString jobName;
            m_jobList.GetText(indx, jobName);
            if (notFirst) {
                param += _T(" ");
            }
            notFirst = true;
            param += _T("\"") + jobName + _T("\"");
        }
    }
    m_event->m_sParameters = param;
    CTime t, d;
    m_startDateCtl.GetTime(d);
    m_startTimeCtl.GetTime(t);
    m_event->SetStartDateTime(CTime(d.GetYear(), d.GetMonth(), d.GetDay(), t.GetHour(), t.GetMinute(), 0));
    m_endDateCtl.GetTime(d);
    m_event->SetEndDate(CTime(d.GetYear(), d.GetMonth(), d.GetDay(), 0, 0, 0));
    m_event->m_sAccount = userName;
    m_event->m_sPassword = userPassword;
    CString comment;
    m_commentEditCtl.GetWindowText(comment);
    m_event->m_sComment = comment;
    if (ISCHECKED(m_onceRadioBtnCtl)) {
        m_event->m_eFreq = CScheduledTask::freqOnce;
    }
    else if (ISCHECKED(m_daylyRadioBtnCtl)) {
        m_event->m_eFreq = CScheduledTask::freqDaily;
    }
    else if (ISCHECKED(m_monthlyRadioBtnCtl)) {
        m_event->m_eFreq = CScheduledTask::freqMonthly;
    }
    else if (ISCHECKED(m_weeklyRadioBtnCtl)) {
        m_event->m_eFreq = CScheduledTask::freqWeekly;
    }
    else if (ISCHECKED(m_startupRadioBtnCtl)) {
        m_event->m_eFreq = CScheduledTask::freqStartup;
    }
    m_event->m_name = eventName;
    TCHAR programName[512];
    if (::GetModuleFileName(NULL, programName, _countof(programName))) {
        m_event->m_sProgramPath = programName;
    }
    else {
        m_event->m_sProgramPath = _T("Unknown");
    }
    m_event->m_autoDelete = ISCHECKED(m_autoDeleteCheckCtl);
    m_event->m_hasEndDate = ISCHECKED(m_hasEndDateCheckCtl);
    CInsyncDlg::OnOK();
}

void CScheduleEventDialog::OnCancel() {
    CInsyncDlg::OnCancel();
}

void CScheduleEventDialog::OnBnClickedOnceRadio() {
    EnableControls();
}

void CScheduleEventDialog::OnBnClickedDaylyRadio() {
    EnableControls();
}

void CScheduleEventDialog::OnBnClickedWeeklyRadio() {
    EnableControls();
}

void CScheduleEventDialog::OnBnClickedMonthlyRadio() {
    EnableControls();
}

void CScheduleEventDialog::OnBnClickedStartupRadio() {
    EnableControls();
}

void CScheduleEventDialog::OnBnClickedHasEndDateCheck() {
    EnableControls();
}
