#pragma once

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
wregex *ParseIncludeExcludeString(LPCTSTR);
QWORD measureIOPS();
