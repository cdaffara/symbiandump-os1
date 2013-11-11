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
// Dial API Test base header file.
// 
//

/**
 @file
*/

#ifndef _TE_DIALBASE_H_
#define _TE_DIALBASE_H_

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
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &(a),&(b)); \
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
	    INFO_PRINTF3(_L("Failed: Got \"%S\" while expecting \"%S\"."), &(a),&(b)); \
        SetTestStepResult(EFail); \
        } \
    }


class CDialTestStep : public CTestStep
{
public:
	CDialTestStep();
	~CDialTestStep();
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	CActiveScheduler*  iScheduler;
};


#endif // __TE_DIALBASE_H__
