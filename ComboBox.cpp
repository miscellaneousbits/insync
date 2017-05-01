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

void CInsyncComboBox::SetSelectedFromItemData(DWORD_PTR data) {
    int ix, ixe;
    ixe = GetCount();
    for (ix = 0; ix < ixe; ix++) {
        if (data == GetItemData(ix)) {
            SetCurSel(ix);
            return;
        }
    }
    SetCurSel(0);
}

DWORD_PTR CInsyncComboBox::GetSelectedItemData() {
    int ix = GetCurSel();
    if (ix != CB_ERR) {
        return GetItemData(ix);
    }
    return 0;
}
