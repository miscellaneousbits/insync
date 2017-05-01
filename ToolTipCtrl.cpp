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

IMPLEMENT_DYNAMIC(CInsyncToolTipCtrl, CToolTipCtrl)

CInsyncToolTipCtrl::CInsyncToolTipCtrl(CInsyncDlg *dlg) {
    m_isOK = false;
    if (Create(dlg)) {
        m_isOK = true;
        SetDelayTime(1500);
    }
}

BEGIN_MESSAGE_MAP(CInsyncToolTipCtrl, CToolTipCtrl)
END_MESSAGE_MAP()

void CInsyncToolTipCtrl::RelayEvent(LPMSG lpMsg) {
    if (isOK() && !CMainDlg::Singleton().m_tipsOff) {
        CToolTipCtrl::RelayEvent(lpMsg);
    }
}

bool CInsyncToolTipCtrl::isOK() const {
    return m_isOK;
}
