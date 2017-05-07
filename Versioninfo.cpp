#include "stdafx.h"

// Constructor with szFilename as parameter
CVersionInfo::CVersionInfo(LPTSTR szFilename)
{
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
            } else {
                m_strFixedProductVersion.Format(_T("%u.%u.%u"), HIWORD(pFixedInfo->dwProductVersionMS),
                                                LOWORD(pFixedInfo->dwProductVersionMS),
                                                HIWORD(pFixedInfo->dwProductVersionLS));
            }

            delete [] pBuffer;
        } else {
            m_strFixedProductVersion = _T("?.?.?.?");
        }

    }
}

