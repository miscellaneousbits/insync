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

// Constructor with szFilename as parameter
CVersionInfo::CVersionInfo(LPTSTR szFilename) {
    DWORD dwVerInfoSize;
    DWORD dwHnd;
    LPVOID pBuffer;
    VS_FIXEDFILEINFO *pFixedInfo; // pointer to fixed file info structure
    UINT uVersionLen;   // Current length of full version string
    dwVerInfoSize = ::GetFileVersionInfoSize(szFilename, &dwHnd);
    if (dwVerInfoSize) {
        pBuffer = new char[dwVerInfoSize];
        if (pBuffer) {
            ::GetFileVersionInfo(szFilename, NULL, dwVerInfoSize, pBuffer);
            // get the fixed file info (language-independent)
            ::VerQueryValue(pBuffer, _T("\\"), (void **)&pFixedInfo, (UINT *)&uVersionLen);
            if (LOWORD(pFixedInfo->dwProductVersionLS)) {
                m_strFixedProductVersion.Format(_T("%u.%u.%u.%u"), HIWORD(pFixedInfo->dwProductVersionMS),
                                                LOWORD(pFixedInfo->dwProductVersionMS),
                                                HIWORD(pFixedInfo->dwProductVersionLS),
                                                LOWORD(pFixedInfo->dwProductVersionLS));
            }
            else {
                m_strFixedProductVersion.Format(_T("%u.%u.%u"), HIWORD(pFixedInfo->dwProductVersionMS),
                                                LOWORD(pFixedInfo->dwProductVersionMS),
                                                HIWORD(pFixedInfo->dwProductVersionLS));
            }
            delete [] pBuffer;
        }
        else {
            m_strFixedProductVersion = _T("?.?.?.?");
        }

    }
}

