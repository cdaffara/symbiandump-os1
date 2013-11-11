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
 @file Te_LbsDebugSuiteStepBase.h
 @internalTechnology
*/

#if (!defined __TE_LBSDEBUG_STEP_BASE__)
#define __TE_LBSDEBUG_STEP_BASE__
#include <test/TestExecuteStepBase.h>
// Please add your include here if you have 
#include "LbsOomListener.h"
#include "LbsOomTest.h"
#include "lbstimer.h"


/****************************************************************************
* The reason to have a new step base is that it is very much possible
* that the all individual test steps have project related common variables 
* and members 
* and this is the place to define these common variable and members.
* 
****************************************************************************/

/*
 * Class which allows tests to use an active scheduler
 */
class MLbsTestStepObserver
    {
public:	
    virtual void SetResult(TVerdict aResult) = 0;
    virtual void StartLbsTestStepL(TRequestStatus& aStatus) = 0;
    virtual TBool DoLbsTestStepL(TRequestStatus& aStatus) = 0;
    virtual void FinishLbsTestStep(TRequestStatus& aStatus) = 0;
    virtual void SubscribeToOomBus(TRequestStatus& aStatus) = 0;
    };


class CTinyTestClass : public CActive, MLbsCallbackTimerObserver
    {
enum
    {
    EWaitingToStart,
    EWaitingForFinish    	
    };
public:
	static CTinyTestClass* NewL(MLbsTestStepObserver* aOwner);

protected:
    CTinyTestClass(MLbsTestStepObserver* aOwner);
    void ConstructL();	
    void RunL();
    void DoCancel();

	//from LbsTimer
	virtual void OnTimerEventL(TInt aTimerId);
	virtual TInt OnTimerError(TInt aTimerId, TInt aError);

private:
    MLbsTestStepObserver	*iOwner;
    TInt     				iState;
    TBool    				iFinished;
	CLbsCallbackTimer* 		iTimer;
    };


class CTe_LbsDebugSuiteStepBase : public CTestStep, public MLbsTestStepObserver
	{
public:
	virtual ~CTe_LbsDebugSuiteStepBase();
	CTe_LbsDebugSuiteStepBase();
	virtual TVerdict doTestStepPreambleL(); 
	virtual TVerdict doTestStepPostambleL();

//Please add your class members which will be common to all individual test steps:
protected:
    virtual void StartLbsTestStepL(TRequestStatus& aStatus);
    virtual TBool DoLbsTestStepL(TRequestStatus& aStatus);
    virtual void FinishLbsTestStep(TRequestStatus& aStatus);
    virtual void SubscribeToOomBus(TRequestStatus& aStatus);
    void SetResult(TVerdict aResult);


protected:
	HBufC8*				iReadData;
	HBufC8*				iWriteData;
	CActiveScheduler* 	rootScheduler;
	ROomRequest 		iOomRequest;
	CLbsOomListener*	iOomListener;
    TUint16				iCounter;
	};

#endif
