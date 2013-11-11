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
// This is the base class for the integration and state transition SUPL Push API unit test steps
// 
//

#include <e32property.h>
#include <centralrepository.h>

#include "lbssuplpushprops.h"

#include "Te_LbsSuplPushBaseStep.h"
#include "Te_LbsSuplErrorPattern.h"
#include "lbsrootcenrepdefs.h"

/**
Constructor. 

@param  aStep [In] A reference on the object-owner.
*/
CTe_LbsSuplPushBaseStep::CMainActive::CMainActive(CTe_LbsSuplPushBaseStep& aStep) : 
CActive(EPriorityStandard),
iStep(aStep)
	{
	CActiveScheduler::Add(this);
	}

/**
Destructor.
*/
CTe_LbsSuplPushBaseStep::CMainActive::~CMainActive()
	{
	Cancel();
	}
/**
Calls the CTe_LbsSuplPushBaseStep::ReallyDoTestStepL and sets the verdict according to the result.

It never leaves. So, the implementation of the RunError is not required.

@see CActive::RunL
*/
void CTe_LbsSuplPushBaseStep::CMainActive::RunL()
	{
	TRAPD(err, iStep.ReallyDoTestStepL());
	if(err!=KErrNone)
		{
		iVerdict = EFail;
		}
	CActiveScheduler::Stop();
	}
/**
Overrides the base class pure virtual function. 
*/
void CTe_LbsSuplPushBaseStep::CMainActive::DoCancel()
	{
	//Intentionally left blank
	}

/**
Returns the result of the test step.

@return The result of the test step.
*/
TVerdict CTe_LbsSuplPushBaseStep::CMainActive::Verdict()
	{
	return iVerdict;
	}

/**
Completes the asynchronous request amd makes the object active.

@see CTe_LbsSuplPushBaseStep::CMainActive::RunL
*/
void CTe_LbsSuplPushBaseStep::CMainActive::Start()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	CActiveScheduler::Start();
	}
		

/**
Destructor.
*/
CTe_LbsSuplPushBaseStep::~CTe_LbsSuplPushBaseStep()
	{
	RProperty::Set(iPropOwnerSecureId, iBusyPropKey, 1);
	
	delete iSchedulerWait;
	delete iMainActive;
	delete iScheduler;
	}


/**
Overrides the base class pure virtual function. Contains the initialization code required for 
the test step. 

Creates and installs the CActiveScheduler object, creates CMainActive and CActiveSchedulerWait.
The error pattern is also opened here. Property keys, secure id, channle id are assignded 
to the correct values.
 
@return - The result of the preamble.

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepPreambleL
*/
TVerdict CTe_LbsSuplPushBaseStep::doTestStepPreambleL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	iMainActive = new(ELeave) CMainActive(*this);
	iSchedulerWait = new(ELeave) CActiveSchedulerWait();
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(KSuplPushAPIKey, category));
	CleanupStack::PopAndDestroy(rep);
	iPropOwnerSecureId = TUid::Uid(category);
	
	iChannel = ELbsSuplPushChannelSMS;
	
	if(iChannel==ELbsSuplPushChannelSMS)
		{
		iBusyPropKey=KLbsSuplPushSmsBusyKey;
		iAckPropKey=KLbsSuplPushSmsAckKey;
		}
	else
		{	
		iBusyPropKey=KLbsSuplPushWapBusyKey;
		iAckPropKey=KLbsSuplPushWapAckKey;
		}
	
	RTe_LbsSuplErrorPattern::OpenL();
		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
Override of the base class pure virtual function. Contains the test code to run. 

It only calls the CMainActive::Start method. The actual test code is run asynchronously 
when CMainActive::RunL method is called and must be implemented in the step's 
CTe_LbsSuplPushBaseStep method.

The memory leak check is implemented here. The error pattern must be closed before the check done as it
is not placed on the clean-up stack and not cleaned up when a leave occurs.
 
@return - The result of the test 

@see CTestStep::doTestStepL
@see CTe_LbsSuplPushBaseStep::ReallyDoTestStepL
*/
TVerdict CTe_LbsSuplPushBaseStep::doTestStepL()
	{
	__UHEAP_MARK;
	iMainActive->Start();
	RTe_LbsSuplErrorPattern::Close();
	__UHEAP_MARKEND;
	
	SetTestStepResult(iMainActive->Verdict());
	return TestStepResult();
	}

/**
Override of the base class virtual function. 
 
@see CTestStep::doTestStepPostambleL
*/
TVerdict CTe_LbsSuplPushBaseStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

/**
Returns the BUSY property key. It is used by the state transition classes.

@return The BUSY property key.

@see CTe_LbsSuplPushBaseTrans
*/
TUint CTe_LbsSuplPushBaseStep::BusyPropKey()
	{
	return iBusyPropKey;
	}

/**
Returns the ACK property key. It is used by the state transition classes.

@return The ACK property key.

@see CTe_LbsSuplPushBaseTrans
*/
TUint CTe_LbsSuplPushBaseStep::AckPropKey()
	{
	return iAckPropKey;
	}

/**
Returns the secure id of the process owning the properties. It is used by the state transition classes.

@return The secure id of the process owning the properties.

@see CTe_LbsSuplPushBaseTrans
*/
TUid CTe_LbsSuplPushBaseStep::PropOwnerSecureId()
	{
	return iPropOwnerSecureId;
	}





