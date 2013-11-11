// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//



#include "McFramework.h"
#include "TestScheduler.h"
#include <e32debug.h>


/* This is active object class that track changes to 
 * Publish and Subscribe KMyHWRMTestProperty 
 */

CTestCasePropertyWatch* CTestCasePropertyWatch::NewL(TInt aKey)
	{
	CTestCasePropertyWatch* me=new(ELeave) CTestCasePropertyWatch;
	CleanupStack::PushL(me);
	me->ConstructL(aKey);
	CleanupStack::Pop(me);
	return me;
	}

CTestCasePropertyWatch::CTestCasePropertyWatch()
	:CActive(EPriority)
	{}

void CTestCasePropertyWatch::ConstructL(TInt aKey)
	{
	
	// Check which key is used
	
	switch(aKey)
	{
		case EMultiClientCase:
		{
			User::LeaveIfError(iProperty.Attach(KMyHWRMTestProperty,EHWRMNextTestToRun));
			break;
		}
		case EMultiClientResult:
		{
			User::LeaveIfError(iProperty.Attach(KMyHWRMTestProperty,EHWRMStepResult));
			break;
		}
		case EPSBatteryLevel:
		{
			User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState,KHWRMBatteryLevel));
			break;			
		}
		case EPSBatteryStatus:
		{
			User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState,KHWRMBatteryStatus));
			break;			
		}
		case EPSChargingStatus:
		{
			User::LeaveIfError(iProperty.Attach(KPSUidHWRMPowerState,KHWRMChargingStatus));
			break;			
		}
				
	}
	
	
	iStatusReceived = EFalse;
		
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	Start();
	
	}

CTestCasePropertyWatch::~CTestCasePropertyWatch()
	{
	Cancel();
	iProperty.Close();
	//iTimer.Close();
	}

void CTestCasePropertyWatch::DoCancel()
	{
	iProperty.Cancel();
	}

void CTestCasePropertyWatch::RunL()
	{
	
	
	//if(KErrNone == iStatus.Int())
	//	{
	iStatusReceived = ETrue;
	iProperty.Get(iKeyResult);
	//	}
	
	//if(IsActive())
	//{
	//	iProperty.Subscribe(iStatus);
	//	SetActive();	
	//}
	
	
	// resubscribe before processing new value to prevent missing updates	
	}

void CTestCasePropertyWatch::Start()
{	
	iStatusReceived = EFalse;
	iProperty.Subscribe(iStatus);
	SetActive();		
}

// -----------------------------------------------------------------------------
// CTimeOutTimer
//
// -----------------------------------------------------------------------------
//

CTimeOutTimer* CTimeOutTimer::NewL() 
{
	CTimeOutTimer* self=new(ELeave) CTimeOutTimer();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	return self;
}

CTimeOutTimer::CTimeOutTimer()
: CActive(CActive::EPriorityUserInput)
{
	
}
	
void CTimeOutTimer::ConstructL()
{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this);
}

CTimeOutTimer::~CTimeOutTimer()
	{
	Cancel();
	iTimer.Close();
	}

void CTimeOutTimer::RequestTimeOut(TInt aSeconds)
{
	iTimeOutOccured = EFalse;
	iTimer.After(iStatus, aSeconds * 1000000);
	SetActive();	
}

void CTimeOutTimer::RunL()
{
	//CEikonEnv::Static()->InfoMsg(_L("TimeOut"));
	
	iTimeOutOccured = ETrue;
}

void CTimeOutTimer::DoCancel()
{
	iTimer.Cancel();
}


// -----------------------------------------------------------------------------
// CMcFramework::NewL
//
// -----------------------------------------------------------------------------
//

CMcFramework* CMcFramework::NewL()
	{
	RDebug::Print(_L("HWRMTest # CMcFramework::NewL()"));
	
	CMcFramework* me=new(ELeave) CMcFramework;
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}


void CMcFramework::ConstructL()
{
	
	RDebug::Print(_L("HWRMTest # Define()"));
	
	TInt r=RProperty::Define(KMyHWRMTestProperty, EHWRMNextTestToRun, RProperty::EInt, TSecurityPolicy::EAlwaysPass, TSecurityPolicy::EAlwaysPass);
	if (r!=KErrAlreadyExists)
		User::LeaveIfError(r);
	
	r=RProperty::Define(KMyHWRMTestProperty, EHWRMStepResult, RProperty::EInt, TSecurityPolicy::EAlwaysPass, TSecurityPolicy::EAlwaysPass); 
	if (r!=KErrAlreadyExists)
		User::LeaveIfError(r);
	
	iTimeOutTimer = CTimeOutTimer::NewL();
	
	//RDebug::Print(_L("HWRMTest # Attach()"));
	//User::LeaveIfError( iTcaseProp.Attach(KMyHWRMTestProperty, EHWRMNextTestToRun ));
	//User::LeaveIfError( iTstepProp.Attach(KMyHWRMTestProperty, EHWRMStepResult ));
		
}

CMcFramework::~CMcFramework()
	{
	delete iTimeOutTimer;
	}

// -----------------------------------------------------------------------------
// CMcFramework::StopMultiClientTestInSlave
// 
// -----------------------------------------------------------------------------
//
void CMcFramework::StopMultiClientTestInSlave()
{
	
	RDebug::Print(_L("HWRMTest # CMcFramework::StopMultiClientTestInSlave()"));
	
	// Needed only in Master-application
	
	// Publish test case value that will end testing in SLAVE application
	TRequestStatus s;
	
	// Application publish	
	TInt r=RProperty::Set(KMyHWRMTestProperty,EHWRMNextTestToRun,EHWRMQuitTest);
	User::LeaveIfError(r);
		
}

// -----------------------------------------------------------------------------
// CMcFramework::StartTestCaseInSlave
//
// Publish test case in P&S -> Start test case execution in Slave
// test application
// NOTE! Slave application should be running and be subscribed to property
// (WaitTestToStart)
// -----------------------------------------------------------------------------
//
TInt CMcFramework::StartTestCaseInSlave( TInt aTcase)
{
	RDebug::Print(_L("HWRMTest # CMcFramework::StartTestCaseInSlave(%d)"),aTcase);
	
	// Needed only in Master-application
	
	// Publish new test case value ( which slave application has subscribed to )
	TRequestStatus s;
	
	// Application publish	
	TInt r=RProperty::Set(KMyHWRMTestProperty,EHWRMNextTestToRun,aTcase);
	User::LeaveIfError(r);
	
	iTestCasePropertyWatch = CTestCasePropertyWatch::NewL(EMultiClientResult);	
	iTimeOutTimer->Cancel();
	iTimeOutTimer->RequestTimeOut(30);
	
	RDebug::Print(_L("HWRMTest # Waiting notify from slave..."));
	while(iTestCasePropertyWatch->iStatusReceived == EFalse && iTimeOutTimer->iTimeOutOccured == EFalse)
	{
		CTestScheduler::StartAndStopL(1000);	
	}
		
	if ( iTimeOutTimer->iTimeOutOccured == EFalse )
	{
		iTimeOutTimer->DoCancel();
		iStepResult = iTestCasePropertyWatch->iKeyResult;
	}
	else
	{
		/* Time out happened while waiting result */
		RDebug::Print(_L("HWRMTest # ERROR: Timeout in test execution"));
		iStepResult = KErrGeneral;
	}
	
	delete iTestCasePropertyWatch;
	
	RDebug::Print(_L("HWRMTest # CMcFramework::StartTestCaseInSlave() -> %d"),iStepResult);
	
	return iStepResult;
	
}

// -----------------------------------------------------------------------------
// CMcFramework::WaitTestToStart
//
// Subsrcribe to P6S test case property
// This method should be called when application is ready to start tests
// -----------------------------------------------------------------------------
//
TInt CMcFramework::WaitTestToStart()
{
	RDebug::Print(_L("HWRMTest # CMcFramework::WaitTestToStart()"));
	
	// Needed only in slave application?
	
	// Application wait for test case number notification from P&S
	// and then return received value
	
	iTestCasePropertyWatch = CTestCasePropertyWatch::NewL(EMultiClientCase);	
	//RDebug::Print(_L("HWRMTest # CTestCasePropertyWatch..."));
	iTimeOutTimer->Cancel();	
	iTimeOutTimer->RequestTimeOut(30);
	//RDebug::Print(_L("HWRMTest # RequestTimeOut..."));
	RDebug::Print(_L("HWRMTest # Waiting test case from Master..."));
	while(iTestCasePropertyWatch->iStatusReceived == EFalse && iTimeOutTimer->iTimeOutOccured == EFalse)
	{
		CTestScheduler::StartAndStopL(1000);
	}
		
	if ( iTimeOutTimer->iTimeOutOccured == EFalse )
	{
		iTimeOutTimer->DoCancel();
		iTcNumber = iTestCasePropertyWatch->iKeyResult;
	}
	else
	{
		/* Time out happened while waiting result */
		RDebug::Print(_L("HWRMTest # ERROR: Timeout in test execution"));
		iTcNumber = KErrGeneral;
	}
	
	delete iTestCasePropertyWatch;

	RDebug::Print(_L("HWRMTest # CMcFramework::WaitTestToStart() -> %d"),iTcNumber);
	return iTcNumber;
}

// -----------------------------------------------------------------------------
// CMcFramework::WaitNextTestStep
// 
// Subscribe to P&S and wait for test step notification from peer application
// -----------------------------------------------------------------------------
//
TInt CMcFramework::WaitNextTestStep()
{	
	RDebug::Print(_L("HWRMTest # CMcFramework::WaitNextTestStep()"));
	
	// Application wait for test step execution notification from P&S
	// and then return received value
	
	iTestCasePropertyWatch = CTestCasePropertyWatch::NewL(EMultiClientResult);	
	//iTimeOutTimer->Cancel();	
	iTimeOutTimer->RequestTimeOut(30);
	RDebug::Print(_L("HWRMTest # Waiting next step..."));
	
	while(iTestCasePropertyWatch->iStatusReceived == EFalse && iTimeOutTimer->iTimeOutOccured == EFalse)
	{
		CTestScheduler::StartAndStopL(1000);
	}
		
	if ( iTimeOutTimer->iTimeOutOccured == EFalse )
	{
		iTimeOutTimer->DoCancel();
		iStepResult = iTestCasePropertyWatch->iKeyResult;
	}
	else
	{
		/* Time out happened while waiting result */
		iStepResult = KErrGeneral;
	}
	
	delete iTestCasePropertyWatch;	
		
	RDebug::Print(_L("HWRMTest # CMcFramework::WaitNextTestStep() -> %d"),iStepResult);
	
	return iStepResult;
	
	// If master publish test step EHWRMQuitTest it means that test case is over */
}

// -----------------------------------------------------------------------------
// CMcFramework::StepExecuted
//
// This method should be called when one test step is executed and application
// goes to wait next step.
// NOTE Test case should notice when last step is executed and not to call this
// method anymore
// -----------------------------------------------------------------------------
//
void CMcFramework::StepExecuted(TInt aTstep)
{
	RDebug::Print(_L("HWRMTest # CMcFramework::StepExecuted(%d)"),aTstep);
	
	// This method should be called when test step execution is done
	// and application is ready to move to next step

	// Application publish	
	TInt r=RProperty::Set(KMyHWRMTestProperty,EHWRMStepResult,aTstep);
	User::LeaveIfError(r);
	
	// Slave should publish same test step that it received
}
