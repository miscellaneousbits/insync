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

CSrcNode::CSrcNode(CJobNode *parentJob) :
    m_subdir(true),
    m_useVolume(false) {
	m_parentJob = parentJob;
}

CSrcNode::CSrcNode(const CSrcNode &node) :
    CExecutableWithInclude(node) {
    if (&node == this) {
        return;
    }
    m_subdir = node.m_subdir;
    m_name = node.m_name;
    m_exclude = node.m_exclude;
    m_include = node.m_include;
    m_useVolume = node.m_useVolume;
    m_excludedSrcDirList = node.m_excludedSrcDirList;
    m_parentJob = node.m_parentJob;
    ConstTrgNodeListIter iter;
    for (iter = node.m_childrenList.begin(); iter != node.m_childrenList.end(); ++iter) {
        CTrgNode *trg = new CTrgNode(*(*iter));
        trg->m_parentSrc = this;
        trg->m_parentJob = m_parentJob;
        m_childrenList.push_back(trg);
    }
}

bool CSrcNode::operator ==(const CSrcNode &n) const {
    if (this == &n) {
        return true;
    }
    return this->m_name == n.m_name;
}

CSrcNode::~CSrcNode() {
    TrgNodeListIter iter;
    for (iter = m_childrenList.begin(); iter != m_childrenList.end(); ++iter) {
        CTrgNode *t = *iter;
        delete t;
    }
}

DWORD SrcThread(LPVOID pParam) {
    return ((CSrcNode *)pParam)->CSrcThread();
}

UINT CSrcNode::CSrcThread() {
    if (m_childrenList.size()) {
        TrgNodeListIter node;
        for (node = m_childrenList.begin(); node != m_childrenList.end(); ++node) {
            (*node)->m_parentSrc = this;
            (*node)->m_parentJob = this->m_parentJob;
            (*node)->Start(TrgThread);
        }
        for (node = m_childrenList.begin(); node != m_childrenList.end(); ++node) {
            CTrgNode *t = *node;
            if (t->m_winThread) {
                ::WaitForSingleObject(t->m_winThread, INFINITE);
                CloseHandle(t->m_winThread);
                t->m_winThread = NULL;
            }
        }
    }
    return 0;
}
