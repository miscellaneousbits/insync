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
#include "InsyncDlg.h"

// CInsyncDlg dialog

IMPLEMENT_DYNAMIC(CInsyncDlg, CDialogEx)

CInsyncDlg::CInsyncDlg(DWORD idd, DWORD helpId, CWnd *pParent /*=NULL*/)
    : CDialogEx(idd, pParent),
      m_pToolTip(NULL) {
    m_helpId = helpId;
    m_idd = idd;
}

CInsyncDlg::~CInsyncDlg() {
    delete m_pToolTip;
}

BEGIN_MESSAGE_MAP(CInsyncDlg, CDialogEx)
    ON_COMMAND(IDHELP, &OnIdhelp)
    ON_COMMAND(ID_HELP, &OnIdhelp)
END_MESSAGE_MAP()


// CInsyncDlg message handlers


void CInsyncDlg::OnIdhelp() {
    InSyncApp.HtmlHelp(m_helpId);
}


BOOL CInsyncDlg::PreTranslateMessage(MSG *pMsg) {
    m_pToolTip->RelayEvent(pMsg);
    return CDialog::PreTranslateMessage(pMsg);
}


BOOL CInsyncDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    m_pToolTip = new CInsyncToolTipCtrl(this);
    return TRUE;
}

bool CInsyncDlg::BrowseForFolder(CString &folder, CWnd *dlg, LPCTSTR title) {
    return B2b(InSyncApp.GetShellManager()->BrowseForFolder(folder,
               dlg, folder, CString(_T("Select the ")) + title,
               BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE));
}

HICON CInsyncDlg::LoadResourceIcon(const UINT id, const WORD x, const WORD y) const {
    WORD xs = (DWORD(x) * InSyncApp.m_scale) / 100;
    WORD ys = (DWORD(y) * InSyncApp.m_scale) / 100;
    return (HICON)::LoadImage(InSyncApp.m_hInstance, MAKEINTRESOURCE(id), IMAGE_ICON, xs, ys, LR_DEFAULTCOLOR | LR_SHARED);
}

const DLGTEMPLATE *CDlgOccManager::PreCreateDialog(_AFX_OCC_DIALOG_INFO *pOccDialogInfo, const DLGTEMPLATE *pOrigTemplate) {
    CDialogTemplate revisedTemplate(pOrigTemplate);
    // here replace font for the template
    revisedTemplate.SetFont(_T("Segoe UI"), 9);
    return COccManager::PreCreateDialog(pOccDialogInfo, (DLGTEMPLATE *)revisedTemplate.Detach());
}