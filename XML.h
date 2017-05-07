#pragma once

#pragma warning(disable:4192)
#import "msxml3.dll"
using namespace MSXML2;

#define RELEASE_PTR(x)	if (x != NULL) {x.Release(); x = NULL;}

class CXml;
class CXmlNode;
class CXmlNodes;

typedef std::auto_ptr<CXmlNode>  CXmlNodePtr;
typedef std::auto_ptr<CXmlNodes>  CXmlNodesPtr;

class CXmlNode
{
    friend class CXml;
    friend class CXmlNode;
    friend class CXmlNodes;

public:
    CXmlNode();
    CXmlNode(const CXmlNode &refNode);
    CXmlNode(const CXmlNodePtr pNode);
    CString GetName() const;// Get the name of the current node
    CString	GetAttribute(CString strName) const;
    CXmlNodePtr GetChild(CString strName);
    CXmlNodesPtr GetChildren();
    CXmlNodePtr NewChild(CString strName);
    bool IsNull() const; 	// Whether the current element exist
    bool GetValue(bool bDefault) const;
    DWORD GetValue(DWORD dwDefault) const;
    CString	GetValue(LPCTSTR lpszDefault) const;
    bool SetValue(bool bValue);
    bool SetValue(DWORD dwValue);
    bool SetValue(LPCTSTR lpszValue);
    bool SetAttribute(CString strName, LPCTSTR lpszValue);

protected:
    bool _GetAttribute(CString &strName, CString &strValue) const;
    bool _SetAttribute(CString &strName, CString &strValue) const;
    bool _GetValue(CString &strValue) const;
    bool _SetValue(CString &strValue) const;
    CXmlNode(MSXML2::IXMLDOMNodePtr pNode);

    MSXML2::IXMLDOMNodePtr   m_pNode;

private:
    CXmlNodePtr operator = (CXmlNodePtr pNode);
    CXmlNode &operator = (const CXmlNode &refNode);
};

class CXmlNodes
{
    friend class CXml;
    friend class CXmlNode;
    friend class CXmlNodes;

public:
    ~CXmlNodes();
    CXmlNodes();
    LONG GetCount();
    void Release();
    CXmlNodePtr GetItem(LONG nIndex);

protected:
    MSXML2::IXMLDOMNodeListPtr m_pNodeList;

};

class CXml
{
public:
    CXml();
    ~CXml();

    bool Open(LPCTSTR lpszXmlFilePath);
    bool CXml::SaveWithFormatted(LPCTSTR lpszFilePath);
    bool Create(LPCTSTR lpszRootName);
    CXmlNodePtr GetRoot();

private:
    bool CreateInstance();
    CXmlNodePtr CreateNode(LPCTSTR lpszName);
    static bool SaveStreamToFile(IStream *pStream, LPCTSTR lpszFilePath);
    void Close();

    MSXML2::IXMLDOMDocument2Ptr m_pDoc;
};

