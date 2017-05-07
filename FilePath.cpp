#include "StdAfx.h"

const TCHAR m_bSlashS(_T('\\'));
const CString m_UNC1S(_T("\\\\?\\"));
const CString m_UNC2S(_T("\\\\?\\UNC"));

CFilePath::CFilePath()
{
}

CFilePath::CFilePath(const CFilePath &parent)
{
    m_prefix = parent.m_prefix;
    m_suffix = parent.m_suffix;
    m_UNCPrefix = parent.m_UNCPrefix;
    m_shadowPrefix = parent.m_shadowPrefix;
}

CFilePath::CFilePath(const CString &name)
{
    int nameLen = name.GetLength();

    if (nameLen >= 3) {
        TCHAR at0 = name[0], at1 = name[1];

        if (at1 == _T(':')) {
            m_prefix = name.Left(2);
            m_suffix = name.Mid(3);
            m_UNCPrefix = m_UNC1S + m_prefix;
            CString s;

            if (CRun::Singleton().m_vss->DriveName(_totupper(at0), s)) {
                m_shadowPrefix = s + m_bSlashS;
            }
        } else {
            if ((at0 == _T('\\')) && (at1 == _T('\\'))) {
                int f = name.Mid(2).Find(_T('\\'));

                if (f == -1) {
                    m_prefix = name;
                } else {
                    m_prefix = name.Left(2 + f);
                    m_suffix = name.Mid(f + 3);
                }

                m_UNCPrefix = m_UNC2S + m_prefix.Mid(1);
            }
        }

        m_prefix += m_bSlashS;
        m_UNCPrefix += m_bSlashS;
        int l = m_suffix.GetLength();

        if (l && (m_suffix[l - 1] == _T('\\'))) {
            m_suffix = m_suffix.Left(l - 1);
        }
    }
}

CFilePath &CFilePath::operator += (const CString &name)
{
    if (m_suffix.GetLength()) {
        m_suffix += m_bSlashS + name;
    } else {
        m_suffix = name;
    }

    return *this;
}

CFilePath &CFilePath::operator = (const CFilePath &rhs)
{
    if (this != &rhs) {
        m_prefix = rhs.m_prefix;
        m_suffix = rhs.m_suffix;
        m_UNCPrefix = rhs.m_UNCPrefix;
        m_shadowPrefix = rhs.m_shadowPrefix;
    }

    return *this;
}


CString CFilePath::Display() const
{
    return m_prefix + m_suffix;
}
CString CFilePath::UNC() const
{
    return m_UNCPrefix + m_suffix;
}
bool CFilePath::NullSuffix() const
{
    return m_suffix.IsEmpty();
}
CString CFilePath::Shadow() const
{
    return (m_shadowPrefix.IsEmpty() ? m_UNCPrefix : m_shadowPrefix) + m_suffix;
}
bool CFilePath::HasShadow() const
{
    return m_shadowPrefix.IsEmpty();
}
void CFilePath::Created()
{
    m_shadowPrefix.Empty();
}

