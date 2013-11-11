// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// These are the unit tests for the MLbsSuplPushRecObserver, CLbsSuplPushRec, CLbsSuplPushRecImpl classes
// 
//

#include <e32property.h>

#include "Te_LbsSuplPushApiReceiverStep.h"
#include "Te_LbsSuplPushRecObserver.h"

/**
 * Constructor. It must	call the SetTestStepName as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID. 
 */
CTe_LbsSuplPushApiReceiverStep::CTe_LbsSuplPushApiReceiverStep()
	{
	SetTestStepName(KLbsSuplPushApiReceiverStep);
	}


/**
 * Destructor.
 */
CTe_LbsSuplPushApiReceiverStep::~CTe_LbsSuplPushApiReceiverStep()
	{
	delete iScheduler;
	}


/**
Overrides the base class pure virtual function. Creates and installs the CActiveScheduler object. 

@return - always returns EPass.

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepPreambleL
*/
TVerdict CTe_LbsSuplPushApiReceiverStep::doTestStepPreambleL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
Overrides the base class pure virtual function. Contains the test code to run. 

This test step runs OOM tests while constructing the CLbsSuplPushRec. It also calls some "exotic" 
functions to improve code coverage statistics.
 
@return - The result of the test 

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepL
*/
TVerdict CTe_LbsSuplPushApiReceiverStep::doTestStepL()
	{
	__UHEAP_MARK;
	TLbsSuplPushRecObserver testObserver;
	TVersion version = testObserver.Version();
	if(version.iMajor!=1 || version.iMinor!=0 || version.iBuild!=0)
		{
		SetTestStepResult(EFail);
		return TestStepResult();
		}
	
	testObserver.ExtendedInterface(0,0,0);
	
	CLbsSuplPushRec* receiver = CLbsSuplPushRec::NewL(testObserver);
	CleanupStack::PushL(receiver);
	receiver->SuplInitComplete(ELbsSuplPushChannelSMS, 0, 0, 0);
	CleanupStack::PopAndDestroy(receiver);
	
	//Out of memory tests
	User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, 1);
	TInt* marker = new TInt(0);
	delete marker;
		
	if(marker) 
		{
		__UHEAP_MARKEND;
		//The OS is in the urel mode. So, stops here.
		return TestStepResult();
		}
	else
		{
		marker = new TInt(0);
		}
	
	for(TInt i=1; marker; i++)
		{
		delete marker;
		User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, i);
		receiver = 0;
		TInt err=KErrNone;
		TRAP(err, receiver = CLbsSuplPushRec::NewL(testObserver));
		delete receiver;
		marker = new TInt(0);
		}
	
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/**
Override of the base class virtual function. 
 
@see CTestStep::doTestStepPostambleL
*/
TVerdict CTe_LbsSuplPushApiReceiverStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

