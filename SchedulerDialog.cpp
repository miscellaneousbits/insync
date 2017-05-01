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

CSchedulerDialog::CSchedulerDialog(CWnd *pParent/*=NULL*/) :
    CInsyncDlg(IDD, HtmlSchedule, pParent) {}

void CSchedulerDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ADD_EVENT_BUTTON, m_addBtnCtl);
    DDX_Control(pDX, IDC_EDIT_EVENT_BUTTON, m_editBtnCtl);
    DDX_Control(pDX, IDC_EVENT_LIST, m_eventListCtl);
    DDX_Control(pDX, IDHELP, m_helpBtnCtl);
    DDX_Control(pDX, IDC_MANDALA_STATIC, m_mandalaStaticCtl);
    DDX_Control(pDX, IDC_REMOVE_EVENT_BUTTON, m_removeBtnCtl);
}


BEGIN_MESSAGE_MAP(CSchedulerDialog, CInsyncDlg)
    ON_BN_CLICKED(IDC_ADD_EVENT_BUTTON, OnBnClickedAddEventButton)
    ON_BN_CLICKED(IDC_EDIT_EVENT_BUTTON, OnBnClickedEditEventButton)
    ON_BN_CLICKED(IDC_REMOVE_EVENT_BUTTON, OnBnClickedRemoveEventButton)
    ON_LBN_SELCHANGE(IDC_EVENT_LIST, OnLbnSelchangeEventList)
    ON_WM_DESTROY()
    ON_WM_HELPINFO()
END_MESSAGE_MAP()

void CSchedulerDialog::EnableControls() {
    bool enable = m_eventListCtl.GetCurSel() != LB_ERR;
    m_editBtnCtl.EnableWindow(enable);
    m_removeBtnCtl.EnableWindow(enable);
}

// CSchedulerDialog message handlers

BOOL CSchedulerDialog::OnInitDialog() {
    CInsyncDlg::OnInitDialog();
	(void)CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    m_editBtnCtl.Init(IDI_EDIT, m_pToolTip,
                      _T("Change the selected scheduled event."));
    m_removeBtnCtl.Init(IDI_REMOVE, m_pToolTip,
                        _T("Delete the selected scheduled event."));
    m_addBtnCtl.Init(IDI_ADD, m_pToolTip,
                     _T("Create a new scheduled event."));
    m_helpBtnCtl.Init(IDI_HELP, m_pToolTip,
                      _T("Open the help viewer"));
    m_mandalaStaticCtl.SetIcon(LoadResourceIcon(IDI_INSYNC_ICON, 48, 48));
    WORD eventCount;
    CStringArray names;
    bool stored;
    HRESULT hr = CScheduledTask::EnumerateTasks(eventCount, names);
    if (HRSUCCEEDED(hr)) {
        for (int i = 0; i < eventCount; i++) {
            stored = false;
            CString name(names.GetAt(i));
            CScheduledTask *schedule = new CScheduledTask;
            if (schedule->LoadTask(name) == S_OK) {
                if (schedule->IsInsyncTask()) {
                    name = name.Left(name.GetLength() - 4);
                    schedule->m_name = name;
                    int listIx = m_eventListCtl.AddString(name);
                    if (listIx != LB_ERR) {
                        m_eventListCtl.SetItemDataPtr(listIx, schedule);
                        stored = true;
                    }
                }
            }
            if (!stored) {
                delete schedule;
            }
        }
    }
    else {
        LPTSTR eMsg;
        if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL,
                                    SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0, NULL)) {
            CMainDlg::Singleton().InSyncMessageBox(true, CString(_T("Unable to retrieve existing schedule - ")) + eMsg,
                                                   MB_ICONSTOP | MB_OK);
            ::LocalFree(eMsg);
        }
        else {
            CMainDlg::Singleton().InSyncMessageBox(true, CString(_T("Unable to retrieve existing schedule - Unknown error")),
                                                   MB_ICONSTOP | MB_OK);
        }
    }
    EnableControls();
    return TRUE;  // return TRUE unless you set the focus to a control
}

void CSchedulerDialog::OnBnClickedAddEventButton() {
    CScheduleEventDialog dlg;
    dlg.m_event = new CScheduledTask;
    if (dlg.DoModal() == IDOK) {
        HRESULT hr;
        hr = dlg.m_event->SaveTask(dlg.m_event->m_name + _T(".job"), TRUE);
        if (HRFAILED(hr)) {
            LPTSTR eMsg;
            if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL,
                                        SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0,
                                NULL)) {
                CMainDlg::Singleton().InSyncMessageBox(true, CString(_T("Error occured saving scheduler event - ")) + eMsg,
                                                       MB_ICONSTOP | MB_OK);
                ::LocalFree(eMsg);
            }
            else {
                CMainDlg::Singleton().InSyncMessageBox(true, _T("Error occured saving scheduler event - Unknown error"),
                                                       MB_ICONSTOP | MB_OK);
            }
            delete dlg.m_event;
        }
        else {
            int indx = m_eventListCtl.AddString(dlg.m_event->m_name);
            m_eventListCtl.SetItemDataPtr(indx, dlg.m_event);
        }
    }
    else {
        delete dlg.m_event;
    }
    EnableControls();
}

void CSchedulerDialog::OnBnClickedEditEventButton() {
    int indx = m_eventListCtl.GetCurSel();
    CScheduleEventDialog dlg;
    dlg.m_event = (CScheduledTask *)m_eventListCtl.GetItemDataPtr(indx);
    if (dlg.DoModal() == IDOK) {
        HRESULT hr = dlg.m_event->SaveTask(dlg.m_event->m_name + _T(".job"), FALSE);
        if (HRFAILED(hr)) {
            LPTSTR eMsg;
            if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL,
                                        SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0,
                                NULL)) {
                CMainDlg::Singleton().InSyncMessageBox(true, CString(_T("Error occured saving scheduler event - ")) + eMsg,
                                                       MB_ICONSTOP | MB_OK);
                ::LocalFree(eMsg);
            }
            else {
                CMainDlg::Singleton().InSyncMessageBox(true, _T("Error occured saving scheduler event - Unknown error"),
                                                       MB_ICONSTOP | MB_OK);
            }
        }
    }
    EnableControls();
}

void CSchedulerDialog::OnBnClickedRemoveEventButton() {
    HRESULT hr;
    int indx = m_eventListCtl.GetCurSel();
    CString name;
    m_eventListCtl.GetText(indx, name);
    hr = CScheduledTask::DeleteTask(name + _T(".job"));
    if (HRFAILED(hr)) {
        LPTSTR eMsg;
        if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                            FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, MAKELANGID(LANG_NEUTRAL,
                                    SUBLANG_DEFAULT), (LPTSTR)&eMsg, 0, NULL)) {
            CMainDlg::Singleton().InSyncMessageBox(true, CString(_T("Error occured deleting scheduler event - ")) + eMsg,
                                                   MB_ICONSTOP | MB_OK);
            ::LocalFree(eMsg);
        }
        else {
            CMainDlg::Singleton().InSyncMessageBox(true, _T("Error occured deleting scheduler event - Unknown error"),
                                                   MB_ICONSTOP | MB_OK);
        }
    }
    else {
        CScheduledTask *schedule = (CScheduledTask *)m_eventListCtl.GetItemDataPtr(indx);
        delete schedule;
        m_eventListCtl.DeleteString(indx);
    }
    EnableControls();
}

void CSchedulerDialog::OnLbnSelchangeEventList() {
    EnableControls();
}

void CSchedulerDialog::OnDestroy() {
    CScheduledTask *schedule;
    for (int i = 0; i < m_eventListCtl.GetCount(); i++) {
        schedule = (CScheduledTask *)m_eventListCtl.GetItemDataPtr(i);
        delete schedule;
    }
    CInsyncDlg::OnDestroy();
}
