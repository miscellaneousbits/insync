#pragma once
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

class CFilePath {
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

