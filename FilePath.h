#pragma once

class CFilePath
{
public:
    CFilePath();
    CFilePath(const CFilePath &parent);
    CFilePath(const CString &);

    CFilePath &operator = (const CFilePath &rhs);
    CFilePath &operator += (const CString &name);

    CString Display() const;
    CString UNC() const;
    bool NullSuffix() const;
    CString Shadow() const;
    bool HasShadow() const;
    void Created();

private:
    CString m_prefix;
    CString m_UNCPrefix;
    CString m_shadowPrefix;
    CString m_suffix;
};

