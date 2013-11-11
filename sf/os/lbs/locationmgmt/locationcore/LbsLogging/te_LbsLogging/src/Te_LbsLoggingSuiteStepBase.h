/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file Te_LbsLoggingSuiteStepBase.h
*/

#if (!defined __TE_LBSLOGGING_STEP_BASE__)
#define __TE_LBSLOGGING_STEP_BASE__
#include <test/testexecutestepbase.h>

#include "LbsLogger.h"
#include <logcli.h>
#include <logview.h>
#include <logwrap.h>
// Please add your include here if you have 

/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/
class CTe_LbsLoggingSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTe_LbsLoggingSuiteStepBase();
	CTe_LbsLoggingSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();
	static TInt StopScheduler(TAny* aPtr);
	
protected:
	CPeriodic* iTimer;
	CActiveScheduler* iSched;

	};

//**********************************
// CLbsLogTestActive
//**********************************

class CLbsLogTestActive : public CActive
    {
public:
    CLbsLogTestActive(TInt aPriority = EPriorityIdle-100);
    ~CLbsLogTestActive();
    void StartL();
    void StartL(TInt);
protected:
    void DoCancel();
    void RunL();
private:
    TBool iDelayCompletion;
    TRequestStatus iStoredStatus;
    TInt iDelayTime;
    };
    
#endif
