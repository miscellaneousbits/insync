#include "StdAfx.h"

void FormatCount(UINT64 count, bool scale, bool longScale, LPTSTR f, DWORD lf, LPCTSTR tail)
{
    TCHAR s[32];
    LPCTSTR k = kNullString;
    NUMBERFMT numberFormat = CMainDlg::Singleton().m_numberFormat;
    const UINT64 KB = 1024;
    const UINT64 MB = KB * 1024;
    const UINT64 GB = MB * 1024;
    const UINT64 TB = GB * 1024;
    const UINT64 PB = TB * 1024;
    typedef struct {
        LPCTSTR lName;
        LPCTSTR sName;
        double divisor;
        double limit;
        bool showFrac;
    } scaleEntry;
    static const scaleEntry scales[] = {
        {_T(" peta"), _T("pb"), PB, PB * .75, true},
        {_T(" tera"), _T("tb"), TB, TB * .75, true},
        {_T(" giga"), _T("gb"), GB, GB * .75, true},
        {_T(" mega"), _T("mb"), MB, MB * .75, true},
        {_T(" kilo"), _T("kb"), KB, KB * .75, true},
        {_T(" "), _T("b"), 0, 0, false}
    };

    if (scale) {
        const scaleEntry *p = scales;

        while (true) {
            if (count >= p->limit) {
                double frac = count / double(p->divisor);
                k = longScale ? p->lName : p->sName;

                if (p->showFrac) {
                    _stprintf_s(s, _T("%.2f"), frac);
                    numberFormat.NumDigits = 2;
                } else {
                    numberFormat.NumDigits = 0;
                    _ui64tot_s(count, s, _countof(s), 10);
                }

                break;
            }

            p++;
        }
    } else {
        numberFormat.NumDigits = 0;
        _ui64tot_s(count, s, _countof(s), 10);
    }

    ::GetNumberFormat(LOCALE_USER_DEFAULT, 0, s, &numberFormat, f, (int)lf);

    _tcscat_s(f, lf, k);

    if (tail) {
        _tcscat_s(f, lf, tail);
    }
}

bool ReplaceVolumeName(CString &s, CString *msg)
{
    if (msg) {
        *msg = kNullString;
    }

    if (s[0] != _T('[')) {
        return true;
    }

    int i = s.Find(_T(']'), 1);

    if (i < 2) {
        return false;
    }

    bool rc = false;
    CString volName = s.Mid(1, i - 1);
    DWORD logDrives = ::GetLogicalDrives();
    TCHAR driveLetter;
    TCHAR name[MAX_PATH + 1];
    TCHAR volRoot[4] = { _T('A'), _T(':'), _T('\\'), 0 };
    UINT errorMode = ::GetErrorMode();
    ::SetErrorMode(errorMode | SEM_FAILCRITICALERRORS);

    for (driveLetter = _T('A'); driveLetter <= _T('Z'); driveLetter++) {
        if (logDrives & 1) {
            volRoot[0] = driveLetter;

            if (GetVolumeInformation(volRoot, name, MAX_PATH + 1, NULL, NULL, NULL, NULL, 0)) {
                if (volName.CompareNoCase(name) == 0) {
                    if (msg) {
                        *msg = CString(_T("Volume [")) + volName + _T("] found on drive ") + driveLetter + _T(':');
                    }

                    s = CString(driveLetter) + s.Mid(i + 1);
                    rc = true;
                    break;
                }
            }
        }

        logDrives >>= 1;
    }

    ::SetErrorMode(errorMode);
    return rc;
}

bool ReplaceDriveLetter(CString &s)
{
    if (s[1] != _T(':')) {
        return true;
    }

    TCHAR volRoot[4] = { _totupper(s[0]), _T(':'), _T('\\'), 0 };
    TCHAR name[MAX_PATH + 1];
    UINT errorMode = ::GetErrorMode();
    bool rc = false;
    ::SetErrorMode(errorMode | SEM_FAILCRITICALERRORS);

    if (GetVolumeInformation(volRoot, name, _countof(name), NULL, NULL, NULL, NULL, 0)) {
        if (_tcslen(name)) {
            s = CString(_T("[")) + name + CString(_T("]")) + s.Mid(1);
            rc = true;
        }
    }

    ::SetErrorMode(errorMode);
    return rc;
}

bool IsUserAdmin()
{
    BOOL b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdminGroup;
    b = ::AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                   DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0,
                                   0, 0, &AdminGroup);

    if (b) {
        b = ::CheckTokenMembership(NULL, AdminGroup, &b);
        ::FreeSid(AdminGroup);
    }

    return B2b(b);
}

bool IsWow64()
{
    BOOL bIsWow64;

    if (!IsWow64Process(InSyncApp.m_currentProcess, &bIsWow64)) {
        return false;
    }

    return B2b(bIsWow64);
}

static void ReconnectNetworkDrive(LPNETRESOURCE lpnrLocal)
{
    if (lpnrLocal->dwScope != RESOURCE_REMEMBERED) {
        return;
    }

    if (lpnrLocal->dwType != RESOURCETYPE_DISK) {
        return;
    }

    if (lpnrLocal->dwDisplayType != RESOURCEDISPLAYTYPE_SHARE) {
        return;
    }

    (void)::WNetAddConnection2(lpnrLocal, NULL, NULL, 0);
}

void ReconnectNetworkDrives()
{
    HANDLE hEnum;
    DWORD rc = ::WNetOpenEnum(RESOURCE_REMEMBERED, RESOURCETYPE_DISK, 0, NULL, &hEnum);

    if (rc == NO_ERROR) {
        DWORD cbBuffer = 16384;     // 16K is a good size
        DWORD cEntries = (DWORD)(-1); // enumerate all possible entries
        DWORD i;
        LPNETRESOURCE lpnrLocal = (LPNETRESOURCE)new char[cbBuffer];

        if (lpnrLocal) {
            do {
                memset(lpnrLocal, 0, cbBuffer);
                rc = ::WNetEnumResource(hEnum, &cEntries, lpnrLocal, &cbBuffer);

                if (rc == NO_ERROR)
                    for (i = 0; i < cEntries; i++) {
                        ReconnectNetworkDrive(&lpnrLocal[i]);
                    } else if (rc != ERROR_NO_MORE_ITEMS) {
                    break;
                }
            } while (rc != ERROR_NO_MORE_ITEMS);

            delete[] lpnrLocal;
        }

        (void)::WNetCloseEnum(hEnum);
    }
}

TOKEN_ELEVATION_TYPE GetElevationType(void)
{
    HANDLE hToken;
    TOKEN_ELEVATION_TYPE elevationType = (TOKEN_ELEVATION_TYPE)0;
    DWORD dwSize;

    if (::OpenProcessToken(InSyncApp.m_currentProcess, TOKEN_QUERY, &hToken)) {
        ::GetTokenInformation(hToken, TokenElevationType, &elevationType, sizeof(elevationType), &dwSize);
        ::CloseHandle(hToken);
    }

    return elevationType;
}

#define CERT_CRC32 0x2b8f5c23
//#define DISPLAY_CERT_CRC


bool SupportsPermissions(LPCTSTR dir)
{
    if (!InSyncApp.m_objectPriviledge) {
        return false;
    }

    bool rc = false;
    PSID pIdOwner, pIdGroup;
    PACL pDAcl, pSAcl;
    PSECURITY_DESCRIPTOR pSecurityDescriptor;
    SECURITY_INFORMATION securityInfo = DACL_SECURITY_INFORMATION |
                                        GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION;

    if (::GetNamedSecurityInfo((LPWSTR)dir, SE_FILE_OBJECT,
                               securityInfo, &pIdOwner, &pIdGroup, &pDAcl, &pSAcl, &pSecurityDescriptor) ==
        ERROR_SUCCESS) {
        rc = pDAcl != NULL;
        ::LocalFree(pSecurityDescriptor);
    }

    return rc;
}

static bool StripInherittedACEs(PACL pAcl)
{
    if (pAcl == 0) {
        return false;
    }

    USHORT i = 0;

    while (i < pAcl->AceCount) {
        PACE_HEADER pAce;

        if (::GetAce(pAcl, i, (LPVOID *)&pAce)) {
            if (pAce->AceFlags & INHERITED_ACE) {
                ::DeleteAce(pAcl, i);
                continue;
            }
        }

        i++;
    }

    return true;
}

static bool CompareACEs(PACL from, PACL to)
{
    USHORT i, j;
    PACE_HEADER pAceFrom, pAceTo;

    for (i = 0; i < from->AceCount; i++) {
        ::GetAce(from, i, (LPVOID *)&pAceFrom);

        for (j = 0; j < to->AceCount; j++) {
            ::GetAce(to, j, (LPVOID *)&pAceTo);

            if (pAceFrom->AceType == pAceTo->AceType) {
                switch (pAceFrom->AceType) {
                case ACCESS_ALLOWED_ACE_TYPE:
                case ACCESS_ALLOWED_CALLBACK_ACE_TYPE:
                case ACCESS_DENIED_ACE_TYPE:
                case ACCESS_DENIED_CALLBACK_ACE_TYPE: {
                    ACCESS_ALLOWED_ACE *f = (ACCESS_ALLOWED_ACE *)pAceFrom;
                    ACCESS_ALLOWED_ACE *t = (ACCESS_ALLOWED_ACE *)pAceTo;

                    if (f->Mask != t->Mask) {
                        goto nomatch;
                    }

                    if (!::EqualSid(&f->SidStart, &t->SidStart)) {
                        goto nomatch;
                    }

                    break;
                }

                case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
                case ACCESS_DENIED_OBJECT_ACE_TYPE: {
                    ACCESS_ALLOWED_OBJECT_ACE *f = (ACCESS_ALLOWED_OBJECT_ACE *)pAceFrom;
                    ACCESS_ALLOWED_OBJECT_ACE *t = (ACCESS_ALLOWED_OBJECT_ACE *)pAceTo;

                    if (f->Mask != t->Mask) {
                        goto nomatch;
                    }

                    if (f->Flags != t->Flags) {
                        goto nomatch;
                    }

                    CHAR *sidStartFrom = (CHAR *)&f->ObjectType;
                    CHAR *sidStartTo = (CHAR *)&t->ObjectType;

                    if (f->Flags & ACE_OBJECT_TYPE_PRESENT) {
                        sidStartFrom += sizeof(GUID);
                        sidStartTo += sizeof(GUID);
                    }

                    if (f->Flags & ACE_INHERITED_OBJECT_TYPE_PRESENT) {
                        sidStartFrom += sizeof(GUID);
                        sidStartTo += sizeof(GUID);
                    }

                    if (!::EqualSid(sidStartFrom, sidStartTo)) {
                        goto nomatch;
                    }

                    if (f->Flags & ACE_OBJECT_TYPE_PRESENT) {
                        if (memcmp(&f->ObjectType, &t->ObjectType, sizeof(GUID))) {
                            goto nomatch;
                        }
                    }
                }
                break;
                }

                goto match;
            }

nomatch:
            ;
        }

        return false;
match:
        ;
    }

    return true;
}

void CopyFileACL(HANDLE fromH, HANDLE toH)
{
    PSID pIdOwnerFrom, pIdGroupFrom;
    PACL pDAclFrom;
    PSECURITY_DESCRIPTOR pSecurityDescriptorFrom;
    SECURITY_INFORMATION securityInfo = DACL_SECURITY_INFORMATION |
                                        GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION;

    if (::GetSecurityInfo(fromH, SE_FILE_OBJECT, securityInfo, &pIdOwnerFrom, &pIdGroupFrom, &pDAclFrom,
                          NULL, &pSecurityDescriptorFrom) == ERROR_SUCCESS) {
        if (pDAclFrom) {
            StripInherittedACEs(pDAclFrom);
        }

        securityInfo = 0;

        if (pIdOwnerFrom) {
            securityInfo |= OWNER_SECURITY_INFORMATION;
        }

        if (pIdGroupFrom) {
            securityInfo |= GROUP_SECURITY_INFORMATION;
        }

        if (pDAclFrom) {
            if (pDAclFrom->AceCount) {
                securityInfo |= DACL_SECURITY_INFORMATION;
            }
        }

        if (securityInfo) {
            ::SetSecurityInfo(toH, SE_FILE_OBJECT, securityInfo, pIdOwnerFrom, pIdGroupFrom, pDAclFrom, NULL);
        }

        ::LocalFree(pSecurityDescriptorFrom);
    }
}

bool CopyNamedACLIfDifferent(LPCTSTR fromName, LPCTSTR toName)
{
    PSID pIdOwnerTo, pIdGroupTo;
    PSID pIdOwnerFrom, pIdGroupFrom;
    PACL pDAclTo, pDAclFrom;
    PSECURITY_DESCRIPTOR pSecurityDescriptorTo = NULL;
    PSECURITY_DESCRIPTOR pSecurityDescriptorFrom = NULL;
    SECURITY_INFORMATION securityInfo = DACL_SECURITY_INFORMATION |
                                        GROUP_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION;
    bool updated = false;
    bool protcted = false;

    do {
        if (::GetNamedSecurityInfo(fromName, SE_FILE_OBJECT, securityInfo, &pIdOwnerFrom, &pIdGroupFrom,
                                   &pDAclFrom,
                                   NULL, &pSecurityDescriptorFrom) != ERROR_SUCCESS) {
            break;
        }

        if (::GetNamedSecurityInfo(toName, SE_FILE_OBJECT, securityInfo, &pIdOwnerTo, &pIdGroupTo, &pDAclTo,
                                   NULL, &pSecurityDescriptorTo) != ERROR_SUCCESS) {
            break;
        }

        if ((((SECURITY_DESCRIPTOR *)pSecurityDescriptorFrom)->Control ^
             ((SECURITY_DESCRIPTOR *)pSecurityDescriptorTo)->Control) &
            (SE_DACL_PRESENT | SE_SACL_PRESENT | SE_DACL_PROTECTED)) {
            if ((((SECURITY_DESCRIPTOR *)pSecurityDescriptorFrom)->Control ^
                 ((SECURITY_DESCRIPTOR *)pSecurityDescriptorTo)->Control) &
                SE_DACL_PROTECTED) {
                protcted = (((SECURITY_DESCRIPTOR *)pSecurityDescriptorFrom)->Control & SE_DACL_PROTECTED) != 0;
                updated = true;
            }

            goto copy;
        }

        if (pDAclFrom && pDAclTo) {
            StripInherittedACEs(pDAclFrom);
            StripInherittedACEs(pDAclTo);

            if (pDAclTo->AceCount != pDAclFrom->AceCount) {
                goto copy;
            }

            if (pDAclTo->AceCount != 0) {
                if (!CompareACEs(pDAclTo, pDAclFrom)) {
                    goto copy;
                }
            }
        } else {
            if (!((pDAclFrom == NULL) && (pDAclTo == NULL))) {
                goto copy;
            }
        }

        if (pIdOwnerFrom && pIdOwnerTo) {
            if (!::EqualSid(pIdOwnerFrom, pIdOwnerTo)) {
                goto copy;
            }
        } else {
            if (!((pIdOwnerFrom == NULL) && (pIdOwnerTo == NULL))) {
                goto copy;
            }
        }

        if (pIdGroupFrom && pIdGroupTo) {
            if (!::EqualSid(pIdGroupFrom, pIdGroupTo)) {
                goto copy;
            }
        } else {
            if (!((pIdGroupFrom == NULL) && (pIdGroupTo == NULL))) {
                goto copy;
            }
        }

        break;

copy:
        securityInfo = 0;

        if (pIdOwnerFrom) {
            securityInfo |= OWNER_SECURITY_INFORMATION;
        }

        if (pIdGroupFrom) {
            securityInfo |= GROUP_SECURITY_INFORMATION;
        }

        if (pDAclFrom) {
            securityInfo |= DACL_SECURITY_INFORMATION | (protcted ? PROTECTED_DACL_SECURITY_INFORMATION :
                            UNPROTECTED_DACL_SECURITY_INFORMATION);
        }

        if (securityInfo) {
            ::SetNamedSecurityInfo((LPWSTR)toName, SE_FILE_OBJECT, securityInfo, pIdOwnerFrom, pIdGroupFrom,
                                   pDAclFrom, NULL);
            updated = true;
        }
    } while (false);

    if (pSecurityDescriptorFrom) {
        ::LocalFree(pSecurityDescriptorFrom);
    }

    if (pSecurityDescriptorTo) {
        ::LocalFree(pSecurityDescriptorTo);
    }

    return updated;
}

static CString ConvertToRegex(LPCTSTR wildcard)
{

    CString buffer;

    for (int i = 0; i < (int)_tcslen(wildcard); ++i) {
        if (wildcard[i] == _T('*')) {
            buffer += _T(".*");
        } else if (wildcard[i] == _T('?')) {
            buffer += _T(".");
        } else if (_tcschr(_T("+()^$.{}[]|\\"), wildcard[i])) {
            buffer += _T("\\");
            buffer += wildcard[i];
        } else if (_tcschr(_T("abcdefghijklmnopqrstuv"), _totlower(wildcard[i]))) {
            TCHAR c;
            buffer += _T('[');
            c = _totupper(wildcard[i]);
            buffer += c;
            c = _totlower(wildcard[i]);
            buffer += c;
            buffer += _T(']');
        } else {
            buffer += wildcard[i];
        }
    }

    return buffer;
}

wregex *ParseIncludeExcludeString(LPCTSTR names)
{
    CString regexp;
    LPCTSTR cp1 = names;

    while (*cp1) {
        while (*cp1 == _T(' ')) {
            cp1++;
        }

        if (*cp1 == 0) {
            break;
        }

        if ((*cp1 != _T(';')) && (*cp1 != _T(','))) {
            TCHAR delim1;
            TCHAR delim2;

            if ((*cp1 == _T('\'')) || (*cp1 == _T('"'))) {
                delim1 = delim2 = *cp1++;
            } else {
                delim1 = _T(';');
                delim2 = _T(',');
            }

            LPCTSTR cp2 = cp1;

            while ((*cp2) && (*cp2 != delim1) && (*cp2 != delim2)) {
                cp2++;
            }

            DWORD l = (DWORD)(cp2 - cp1);
            LPTSTR cp3 = new TCHAR[l + 1];
            _tcsncpy_s(cp3, l + 1, cp1, l);
            *(cp3 + l) = 0;

            if (regexp.GetLength()) {
                regexp += _T("|");
            }

            regexp += _T("(");
            regexp += ConvertToRegex(cp3);
            regexp += _T(")");
            delete [] cp3;

            if (*cp2 == 0) {
                break;
            }

            cp1 = cp2 + 1;
        } else {
            cp1++;
        }
    }

    wregex *re;

    try {
        re = new wregex(CString(_T('^')) + regexp + _T('$'),
                             regex_constants::icase | regex_constants::optimize);
    } catch (...) {
        re = NULL;
    }

    return re;
}

