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

#include "StdAfx.h"

CXml::CXml()
    : m_pDoc(NULL) {
}

CXml::~CXml() {
    Close();
}

bool CXml::Open(LPCTSTR lpszXmlFilePath) {
    if (!CreateInstance()) {
        return false;
    }

    VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

    try {
        vbSuccessful = m_pDoc->load(_variant_t(lpszXmlFilePath));
    }
    catch (...) {
        vbSuccessful = false;
    }

    return (vbSuccessful == VARIANT_TRUE);
}

bool CXml::SaveWithFormatted(LPCTSTR lpszFilePath) {
    if (m_pDoc == NULL) {
        return false;
    }

    bool result = false;

    MSXML2::IMXWriterPtr pMXWriter = NULL;
    MSXML2::ISAXXMLReaderPtr pSAXReader = NULL;
    MSXML2::ISAXContentHandlerPtr pISAXContentHandler = NULL;
    MSXML2::ISAXErrorHandlerPtr pISAXErrorHandler = NULL;
    MSXML2::ISAXDTDHandlerPtr pISAXDTDHandler = NULL;

    try {

		do {
			// create
			if (FAILED(pMXWriter.CreateInstance(__uuidof(MSXML2::MXXMLWriter)))) {
				break;
			}

			if (FAILED(pSAXReader.CreateInstance(__uuidof(MSXML2::SAXXMLReader)))) {
				break;
			}

			// save in formatted
			pISAXContentHandler = pMXWriter;
			pISAXErrorHandler = pMXWriter;
			pISAXDTDHandler = pMXWriter;

			if (FAILED(pMXWriter->put_omitXMLDeclaration(VARIANT_FALSE)) ||
				FAILED(pMXWriter->put_standalone(VARIANT_TRUE)) ||
				FAILED(pMXWriter->put_indent(VARIANT_TRUE)) ||
				FAILED(pMXWriter->put_encoding(_bstr_t(_T("UTF-8"))))
				) {
				break;
			}

			if (FAILED(pSAXReader->putContentHandler(pISAXContentHandler)) ||
				FAILED(pSAXReader->putDTDHandler(pISAXDTDHandler)) ||
				FAILED(pSAXReader->putErrorHandler(pISAXErrorHandler)) ||
				FAILED(pSAXReader->putProperty((unsigned short *)L"http://xml.org/sax/properties/lexical-handler",
					_variant_t(pMXWriter.GetInterfacePtr()))) ||
				FAILED(pSAXReader->putProperty((unsigned short *)L"http://xml.org/sax/properties/declaration-handler",
					_variant_t(pMXWriter.GetInterfacePtr())))) {
				break;
			}

			IStream *pOutStream = NULL;

			if (FAILED(CreateStreamOnHGlobal(NULL, true, &pOutStream))) {
				break;
			}

			if (FAILED(pMXWriter->put_output(_variant_t(pOutStream)))) {
				break;
			}

			if (FAILED(pSAXReader->parse(m_pDoc.GetInterfacePtr()))) {
				break;
			}

			if (!CXml::SaveStreamToFile(pOutStream, lpszFilePath)) {
				break;
			}

			result = true;

		} while (false);

    }
    catch (...) {
    }

    RELEASE_PTR(pISAXDTDHandler);
    RELEASE_PTR(pISAXErrorHandler);
    RELEASE_PTR(pISAXContentHandler);
    RELEASE_PTR(pSAXReader);
    RELEASE_PTR(pMXWriter);

    return result;
}

bool CXml::SaveStreamToFile(IStream *pStream, LPCTSTR lpszFilePath) {
    LPVOID pOutput = NULL;
    HGLOBAL hGlobal = NULL;

    if (FAILED(::GetHGlobalFromStream(pStream, &hGlobal))) {
        return false;
    }

    ::GlobalUnlock(hGlobal);
    LARGE_INTEGER llStart = { 0, 0 };
    ULARGE_INTEGER ullSize = { 0, 0 };
    pStream->Seek(llStart, STREAM_SEEK_CUR, &ullSize);
    pOutput = ::GlobalLock(hGlobal);

    HANDLE hFile = ::CreateFile(lpszFilePath
                                , GENERIC_WRITE
                                , FILE_SHARE_WRITE | FILE_SHARE_READ
                                , NULL
                                , CREATE_ALWAYS
                                , 0
                                , NULL
                               );

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD dwWritten = 0;
    ::WriteFile(hFile, pOutput, (UINT)ullSize.QuadPart, &dwWritten, NULL);
    ::FlushFileBuffers(hFile);
    ::CloseHandle(hFile);

    ::GlobalUnlock(hGlobal);

    return true;
}

CXmlNodePtr CXml::CreateNode(LPCTSTR lpszName) {

    CXmlNodePtr pChild(new CXmlNode());

    try {
        MSXML2::IXMLDOMNodePtr pChildNode = NULL;
        pChildNode = m_pDoc->createNode(_variant_t(_T("element")), _bstr_t(lpszName), _bstr_t(""));
        pChild->m_pNode = pChildNode;
        RELEASE_PTR(pChildNode);
    }
    catch (...) {
    }

    return pChild;
}

bool CXml::Create(LPCTSTR lpszRootName) {
    HRESULT hr = S_FALSE;

    if (!CreateInstance()) {
        return false;
    }

    VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

    try {
        MSXML2::IXMLDOMProcessingInstructionPtr pPI = NULL;
        pPI = m_pDoc->createProcessingInstruction(_bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\"")));

        if (pPI == NULL) {
            return false;
        }

        m_pDoc->appendChild(pPI);

        // create the root element
        CXmlNode root = CreateNode(lpszRootName);
        m_pDoc->appendChild(root.m_pNode);

        vbSuccessful = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
    }
    catch (...) {
        vbSuccessful = false;
    }

    return (vbSuccessful == VARIANT_TRUE);
}

bool CXml::CreateInstance() {
    Close();

    if (FAILED(m_pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument)))) {
        return false;
    }

    m_pDoc->setProperty(_bstr_t(_T("SelectionLanguage")), _variant_t(_T("XPath")));	// 3.0 only
    m_pDoc->setProperty(_bstr_t(_T("AllowXsltScript")), _variant_t(true));
    m_pDoc->setProperty(_bstr_t(_T("AllowDocumentFunction")), _variant_t(true));
    m_pDoc->resolveExternals = VARIANT_TRUE;
    m_pDoc->preserveWhiteSpace = VARIANT_FALSE;
    m_pDoc->validateOnParse = VARIANT_FALSE;

    return true;
}

void CXml::Close() {
    RELEASE_PTR(m_pDoc);
}

CXmlNodePtr CXml::GetRoot() {
    CXmlNodePtr pNode(new CXmlNode());

    try {
        MSXML2::IXMLDOMElementPtr pElement = NULL;
        m_pDoc->get_documentElement(&pElement);
        pNode->m_pNode = pElement;
        RELEASE_PTR(pElement);
    }
    catch (...) {
    }

    return pNode;
}

CString CXmlNode::GetName() const {
    CString strRet;

    try {
        BSTR bstr = NULL;
        m_pNode->get_nodeName(&bstr);
        strRet = (LPCTSTR)_bstr_t(bstr, true);

        if (bstr != NULL) {
            SysFreeString(bstr);
            bstr = NULL;
        }
    }
    catch (...) {
    }

    return strRet;
}

bool CXmlNode::_GetAttribute(CString &strName IN, CString &strValue) const {
    if (m_pNode == NULL) {
        return false;
    }

    try {
        MSXML2::IXMLDOMNamedNodeMapPtr pIXMLDOMNamedNodeMap = NULL;

        if (FAILED(m_pNode->get_attributes(&pIXMLDOMNamedNodeMap))) {
            return false;
        }

        MSXML2::IXMLDOMNodePtr pIXMLDOMNode = NULL;
        pIXMLDOMNode = pIXMLDOMNamedNodeMap->getNamedItem(_bstr_t(strName));

        if (pIXMLDOMNode == NULL) {
            strValue.Empty();
        }
        else {
            VARIANT varValue;

            if (FAILED(pIXMLDOMNode->get_nodeValue(&varValue))) {
                return false;
            }

            strValue = (LPCTSTR)(_bstr_t)varValue;
        }

        RELEASE_PTR(pIXMLDOMNode);
        RELEASE_PTR(pIXMLDOMNamedNodeMap);
    }
    catch (...) {
        return false;
    }

    return true;
}

CString CXmlNode::GetAttribute(CString strName) const {
    CString strValue;
    _GetAttribute(strName, strValue);
    return strValue;
}

bool CXmlNode::_SetAttribute(CString &strName, CString &strValue) const {
    if (m_pNode == NULL) {
        return false;
    }

    try {
        MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
        m_pNode->get_ownerDocument(&pDoc);

        MSXML2::IXMLDOMAttributePtr pAttribute = NULL;
        pAttribute = pDoc->createNode(_variant_t(_T("attribute")), _bstr_t(strName), _bstr_t(""));
        pAttribute->Putvalue(_variant_t(strValue));

        MSXML2::IXMLDOMElementPtr pElement = static_cast<MSXML2::IXMLDOMElementPtr>(m_pNode);
        pElement->setAttributeNode(pAttribute);
        RELEASE_PTR(pAttribute);
        RELEASE_PTR(pElement);
        RELEASE_PTR(pDoc);
    }
    catch (...) {
        return false;
    }

    return true;
}

bool CXmlNode::SetAttribute(CString strName, LPCTSTR lpszValue) {
    CString strValue(lpszValue);
    return _SetAttribute(strName, strValue);
}

CXmlNodePtr CXmlNode::GetChild(CString strName) {

    CXmlNodePtr pChild(new CXmlNode());

    try {
        MSXML2::IXMLDOMNodePtr pChildNode = NULL;
        CString strXPath;
        strName.Replace(_T("'"), _T("''"));
        strXPath.Format(_T("*[local-name(.) = '%s']"), strName.GetBuffer());
        pChildNode = m_pNode->selectSingleNode(_bstr_t((LPCTSTR)strXPath));
        pChild->m_pNode = pChildNode;
        RELEASE_PTR(pChildNode);
    }
    catch (...) {
    }

    return pChild;
}

CXmlNodePtr CXmlNode::NewChild(CString strName) {
    CXmlNodePtr pChild(new CXmlNode());

    try {
        MSXML2::IXMLDOMDocumentPtr pDoc = NULL;
        m_pNode->get_ownerDocument(&pDoc);

        MSXML2::IXMLDOMNodePtr pChildNode = NULL;
        pChildNode = pDoc->createElement(_bstr_t(strName));
        RELEASE_PTR(pDoc);

        m_pNode->appendChild(pChildNode);
        pChild->m_pNode = pChildNode;
        RELEASE_PTR(pChildNode);
    }
    catch (...) {
    }

    return pChild;
}

CXmlNodesPtr CXmlNode::GetChildren() {
    CXmlNodesPtr pNodes(new CXmlNodes());

    try {
        MSXML2::IXMLDOMNodeListPtr pNodeList = NULL;
        pNodeList = m_pNode->selectNodes(_bstr_t(_T("child::*")));

        pNodes->m_pNodeList = pNodeList;
        RELEASE_PTR(pNodeList);
    }
    catch (...) {
    }

    return pNodes;
}


bool CXmlNode::IsNull() const {
    return m_pNode == NULL;
}

bool CXmlNode::GetValue(bool bDefault) const {
    CString strValue;
    _GetValue(strValue);

    if (strValue.Compare(_T("1")) == 0) {
        return true;
    }
    else if (strValue.Compare(_T("0")) == 0) {
        return false;
    }
    else {
        strValue = bDefault ? _T("1") : _T("0");
        _SetValue(strValue);
        return bDefault;
    }
}

bool CXmlNode::_GetValue(CString &strValue) const {
    HRESULT hr = S_OK;

    try {
        BSTR bstr = NULL;
        hr = m_pNode->get_text(&bstr);
        strValue = (LPCTSTR)_bstr_t(bstr, true);

        if (bstr != NULL) {
            SysFreeString(bstr);
            bstr = NULL;
        }
    }
    catch (...) {
        return false;
    }

    return SUCCEEDED(hr);
}

bool CXmlNode::_SetValue(CString &strValue IN) const {
    HRESULT hr = S_OK;

    try {
        hr = m_pNode->put_text(_bstr_t(strValue));
    }
    catch (_com_error e) {
        return false;
    }

    return SUCCEEDED(hr);
}

// get DWORD value
DWORD CXmlNode::GetValue(DWORD dwDefault) const {
    CString strValue;
    _GetValue(strValue);

    if (strValue.IsEmpty()) {
        strValue.Format(_T("%lu"), dwDefault);
        _SetValue(strValue);
    }

    return _tcstoul(strValue, NULL, 10);
}

CString CXmlNode::GetValue(LPCTSTR lpszValue) const {
    CString strValue;
    _GetValue(strValue);

    if (strValue.IsEmpty() &&
        lpszValue != NULL) {
        strValue = lpszValue;
        _SetValue(strValue);
    }

    return strValue;
}

bool CXmlNode::SetValue(bool bValue) {
    CString strValue(bValue ? _T("1") : _T("0"));
    return _SetValue(strValue);
}

bool CXmlNode::SetValue(DWORD dwValue) {
    CString strValue;
    strValue.Format(_T("%lu"), dwValue);
    return _SetValue(strValue);
}

bool CXmlNode::SetValue(LPCTSTR lpszValue) {
    CString strValue(lpszValue);
    return _SetValue(strValue);
}

CXmlNode::CXmlNode() {
    m_pNode = NULL;
}

CXmlNode::CXmlNode(const CXmlNode &refNode) {
    m_pNode = refNode.m_pNode;
}

CXmlNode::CXmlNode(const CXmlNodePtr pNode) {
    m_pNode = pNode->m_pNode;
}

CXmlNodes::CXmlNodes()
    : m_pNodeList(NULL) {
}

CXmlNode::CXmlNode(MSXML2::IXMLDOMNodePtr pNode IN) {
    m_pNode = pNode;
}

CXmlNodes::~CXmlNodes() {
    Release();
}

void CXmlNodes::Release() {
    RELEASE_PTR(m_pNodeList);
}

LONG CXmlNodes::GetCount() {
    try {
        return m_pNodeList->length;
    }
    catch (_com_error e) {
        return -1;
    }
}

CXmlNodePtr CXmlNodes::GetItem(LONG nIndex) {
    CXmlNodePtr pNode(new CXmlNode(m_pNodeList->item[nIndex]));
    return pNode;
}
