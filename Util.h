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

#define B2b(x) ((x) ? true : false)
#define b2B(x) ((x) ? TRUE : FALSE)
#define SETCHECK(x) ((x) ? BST_CHECKED : BST_UNCHECKED)
#define ISCHECKED(x) ((x).GetCheck() == BST_CHECKED)

#define HRSUCCEEDED(hr) ((hr) == S_OK)
#define HRFAILED(hr) (!HRSUCCEEDED(hr))

enum UserMessages {
    WM_USER_JOBS_DONE = WM_APP,
    WM_USER_ADD_FOLDER,
    WM_USER_SET_LED
};

void FormatCount(UINT64, bool, bool, LPTSTR, DWORD, LPCTSTR);
bool ReplaceVolumeName(CString &, CString *msg = NULL);
bool ReplaceDriveLetter(CString &);
bool IsUserAdmin(void);
bool IsWow64(void);
void ReconnectNetworkDrives(void);
TOKEN_ELEVATION_TYPE GetElevationType(void);
void CopyFileACL(HANDLE, HANDLE);
bool CopyNamedACLIfDifferent(LPCTSTR, LPCTSTR);
bool SupportsPermissions(LPCTSTR);
tr1::wregex *ParseIncludeExcludeString(LPCTSTR);
QWORD measureIOPS();
