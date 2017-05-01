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

class CScheduledTask {
public:
    CScheduledTask();

    enum ETaskFrequency {
        freqOnce = 1,
        freqDaily,
        freqWeekly,
        freqMonthly,
        freqStartup
    };

    bool GetStartDateTime(CTime &) const;       // get the task's starting date and time
    void SetStartDateTime(const CTime &);       // set the task's starting date and time
    bool GetEndDate(CTime &) const;             // get the task's ending date
    void SetEndDate(const CTime &);             // set the task's ending date
    HRESULT SaveTask(LPCTSTR szTaskName, bool bFailIfExists) const;  // save the task to the scheduler
    HRESULT LoadTask(LPCTSTR szTaskName);
    static HRESULT EnumerateTasks(WORD &eventCount, CStringArray &names);
    static HRESULT DeleteTask(LPCTSTR szTaskName);   // delete a task from the scheduler
    bool IsInsyncTask() const;

    ETaskFrequency m_eFreq;
    CString m_sProgramPath;
    CString m_sParameters;
    CString m_sAccount;
    CString m_sPassword;
    CString m_sComment;
    CString m_name;
    CString m_parameters;

    bool m_autoDelete : 1;
    bool m_hasEndDate : 1;

protected:
    SYSTEMTIME m_timeStart;
    SYSTEMTIME m_timeEnd;

private:
    WORD GetDayOfWeekFlag(const CTime &) const;
};
