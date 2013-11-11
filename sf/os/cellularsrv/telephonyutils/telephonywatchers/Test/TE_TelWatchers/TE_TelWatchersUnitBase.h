// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Telephony Watchers Unit Test base header file.
// 
//

/**
 @file
*/

#ifndef __TE_TELWATCHERSUNITBASE_H__
#define __TE_TELWATCHERSUNITBASE_H__

#include <e32property.h>

#include <sacls.h>

#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>


//
// Test macros
//
#define TESTCHECKL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }

#define TESTCHECKCONDITIONL(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECKSTRL(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        User::Leave(EFail); \
        } \
    }
    
#define TESTCHECK(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    INFO_PRINTF3(_L("Failed: Got %d while expecting %d."),a,b); \
        SetTestStepResult(EFail); \
        } \
    }

#define TESTCHECKCONDITION(a) \
    { \
    if (!(a)) \
	    { \
	    INFO_PRINTF1(_L("Failed: Condition was false.")); \
        SetTestStepResult(EFail); \
        } \
    }
    
#define TESTCHECKSTR(a, b) \
    { \
    if((a)!=(b)) \
	    { \
	    TBuf<255> temp = a ; \
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &temp,&(b)); \
        SetTestStepResult(EFail); \
        } \
    }

class CWatcherLog;
class CTelWatchersUnitTestStep : public CTestStep
{
public:
	CTelWatchersUnitTestStep();
	~CTelWatchersUnitTestStep();
	void SetSimTsyTestNumberL(TInt aTestNumber);
	void CheckSimTsyTestNumberL(TInt aTestNumber);
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void PauseToRunActiveSchedulerL(TInt aSeconds);
	
protected:
	CActiveScheduler*  iScheduler;
	RFs iFs;
	CWatcherLog*  iWatcherLog;
};

/**
 *  Used for waiting for phone watchers event without freezing current thread
 */
class CWatcherObserverPS : public CActive
	{
public:

    CWatcherObserverPS();
    ~CWatcherObserverPS();
	
    void    WaitForPropertyChangeL();
    void    BindProperty(RProperty *aProp);
    TInt    GetPropertyValue(TInt &aVal);


private:

	void RunL()     { CActiveScheduler::Stop(); }
	void DoCancel();

private:

    RProperty   *iProperty;   //< the property which we will subscribe to and wait for value change
	};


class CTimedStopScheduler : public CTimer
	{
public:
	CTimedStopScheduler();
	~CTimedStopScheduler();

	void ConstructL();
	void AfterSeconds(TInt aTime);

private:
	void RunL();
	};


#endif // __TE_TELWATCHERSUNITBASE_H__
