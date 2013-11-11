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
// Telephony Watchers Integration Test base header file.
// 
//

/**
 @file
*/

#ifndef _TE_TELWATCHERSINTBASE_H_
#define _TE_TELWATCHERSINTBASE_H_

#include <sacls.h>

#include "indicatorwatcher.h"
#include "signalstrengthwatcher.h"

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


class CTelWatchersIntTestStep : public CTestStep
{
public:
	CTelWatchersIntTestStep();
	~CTelWatchersIntTestStep();
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	void StartWatcher();
	void StopWatcher();
	void SetSimTsyTestNumberL(TInt aTestNumber);
	void CheckSimTsyTestNumberL(TInt aTestNumber);
	void SetUIDToValueL(const TUid aUid, TInt aValue);
	TInt GetValueFromUID(const TUid aUid);

protected:
	CActiveScheduler*  iScheduler;

    RProcess  iWatcherExe;
    TBool  iWatcherExeRunning;
	TRequestStatus  iWatcherExeStatus;
};


#endif // __TE_TELWATCHERSINTBASE_H__
