#pragma once

class CXmlOptions
{
public:
    bool LoadXMLOptions();
    bool SaveXMLOptions();
    CString GetLastError();
    CString GetVersion();
    void SetFilePath(const CString path);
    void SetVersion(const CString version);

private:
    bool CreatInstance();
    CString m_filePath;
    CString m_errorMessage;
    CString m_version;


};
