#include "stdafx.h"

CSrcNode::CSrcNode(CJobNode *parentJob) :
    m_subdir(true),
    m_useVolume(false)
{
    m_parentJob = parentJob;
}

CSrcNode::CSrcNode(const CSrcNode &node) :
    CExecutableWithInclude(node)
{
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

bool CSrcNode::operator ==(const CSrcNode &n) const
{
    if (this == &n) {
        return true;
    }

    return this->m_name == n.m_name;
}

CSrcNode::~CSrcNode()
{
    TrgNodeListIter iter;

    for (iter = m_childrenList.begin(); iter != m_childrenList.end(); ++iter) {
        CTrgNode *t = *iter;
        delete t;
    }
}

DWORD SrcThread(LPVOID pParam)
{
    return ((CSrcNode *)pParam)->CSrcThread();
}

UINT CSrcNode::CSrcThread()
{
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
