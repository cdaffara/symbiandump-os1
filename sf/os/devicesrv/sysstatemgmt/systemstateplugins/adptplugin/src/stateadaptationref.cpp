// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Description: This adaptation plugin implementation is for test/reference purposes.   
// The loading of this plugin is controlled through test macro defined in the iby file "ssmcompatibility.iby".
// If the macro "TEST_SSM_MACRO" is not defined, original plugins are loaded and this plugin is not loaded.
// If the test P & S key is set in the test code, the calls are routed to the reference or dummy implementations.
// Else the actual plugins are loaded and the calls are routed to the actual implementations.
// The test P & S key which it looks for is KStateAdaptationPluginPropertyKey (0x2000D76A)
//

#include <e32property.h>
#include <ssm/ssmstate.h>
#include "stateadaptationref.h"
#include "ssmdebug.h"

const TUint32 KStateAdaptationPluginPropertyKey = 0x2000D76A;
const TUid KPropertyCategory={0x2000D75B};

/**
Static method to create new State Adaptation Plugin.

@return	a new plugin object for State Adaptation.
*/
EXPORT_C MStateAdaptation* CreateStateAdaptationL()
	{
	CStateAdaptationRef* stateAdaptationRef = CStateAdaptationRef::NewL();
	return (static_cast<MStateAdaptation*>(stateAdaptationRef));
	}

CStateAdaptationRef* CStateAdaptationRef::NewL()
	{
	CStateAdaptationRef* self = new(ELeave) CStateAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}

CStateAdaptationRef::~CStateAdaptationRef()
	{
	delete iTimer;
	iSaaStateAdaptationLib.Close();
	}

CStateAdaptationRef::CStateAdaptationRef()
	{
	}

void CStateAdaptationRef::ConstructL()
	{
    DEBUGPRINT1A("Loading Actual plugins");
    _LIT(KSaaStateAdaptationDLL, "saastateadaptation.dll");
    User::LeaveIfError(iSaaStateAdaptationLib.Load(KSaaStateAdaptationDLL));
    iSaaStateAdaptationDll = (MStateAdaptation *)(iSaaStateAdaptationLib.Lookup(1)()); 
	iTimer = CStateRefAdaptationTimer::NewL();
	}

//from MStateAdaptation
void CStateAdaptationRef::Release()
	{
	delete this;
	}

void CStateAdaptationRef::RequestCoopSysStateChange(TSsmState aState, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
	    {
	    DEBUGPRINT1A("RequestCoopSysStateChange:: Calling Actual plugins functions (saastateadaptation.dll)");
	    iSaaStateAdaptationDll->RequestCoopSysStateChange(aState, aStatus);
	    }
	else
	    {
	    DEBUGPRINT1A("RequestCoopSysStateChange:: Calling ref plugins functions (stateadaptationref.dll)");
	    aStatus = KRequestPending;
	    TRequestStatus* status = &aStatus;
	    User::RequestComplete(status, KErrNone);
	    }
	}

void CStateAdaptationRef::RequestCoopSysSelfTest(TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RequestCoopSysSelfTest:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->RequestCoopSysSelfTest(aStatus);
        }
    else
        {
        DEBUGPRINT1A("RequestCoopSysSelfTest:: Calling ref plugins functions (stateadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
	}

void CStateAdaptationRef::RequestCoopSysPerformRestartActions(TInt aReason, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RequestCoopSysPerformRestartActions:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->RequestCoopSysPerformRestartActions(aReason, aStatus);
        }
    else
        {
        DEBUGPRINT1A("RequestCoopSysPerformRestartActions:: Calling ref plugins functions (stateadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
	}

void CStateAdaptationRef::RequestCoopSysPerformShutdownActions(TInt aReason, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RequestCoopSysPerformShutdownActions:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->RequestCoopSysPerformShutdownActions(aReason, aStatus);
        }
    else
        {
        DEBUGPRINT1A("RequestCoopSysPerformShutdownActions:: Calling ref plugins functions (stateadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
	}

void CStateAdaptationRef::RequestCoopSysPerformRfsActions(TSsmRfsType aRfsType, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RequestCoopSysPerformRfsActions:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->RequestCoopSysPerformRfsActions(aRfsType, aStatus);
        }
    else
        {
        DEBUGPRINT1A("RequestCoopSysPerformRfsActions:: Calling ref plugins functions (stateadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
	}

/**
  Cancel the notification request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CStateAdaptationRef::RequestCancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("RequestCancel:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->RequestCancel();
        }
	}

/**
  The reference implementation completes with KErrNotSupported. This is required for automated testing.
  Actual plugins return expected values and this can be verified by manual testing
  On a device, State Adaptation Plug-in would request for notification from the Cooperating System for 'aEvent'.
  
  The above mentioned implementation is modified to facilitate testing and increase the code coverage of the Adaptation 
  server code.The modified functionality is as follows.
  
  
  Instead of completing the notification request with KErrNotSupported the it is passed to 
  CStateAdaptationRefEventHandler which is an active object wiht lower priority compared to 
  the server active object.Because of this a delay will be introduced in completing the notification
  request so that other active objects can run avoiding infinite loop and starvation of other 
  active objects,which will be the case if the notification request is immediately completed.
  This is strictly for testing purposes.On a device this call will be replaced by a notification request
  to cooperative system.
  
  
*/
void CStateAdaptationRef::NotifyCoopSysEvent(TDes8& /*aEvent*/, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        /* Only ssmpowersup.dll has an outstanding request. If this is passed to the actual plugin, the
        request will never complete till a power event happens. This would add the test code requests in a queue
        and the test code waits indefinitely. Hence, complete the request with KErrServerTerminated. This would free 
        the queue for test code to be executed. It has not impact on the test environment */
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrServerTerminated);
        }
    else
        {
        DEBUGPRINT1A("NotifyCoopSysEvent:: Calling ref plugins functions (stateadaptationref.dll)");
        aStatus = KRequestPending;
        iTimer->After(2000000,aStatus);
        }
	}

/**
  Cancel the notification request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CStateAdaptationRef::NotifyCancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("NotifyCancel:: Calling Actual plugins functions (saastateadaptation.dll)");
        iSaaStateAdaptationDll->NotifyCancel();
        }
    else
        {
        DEBUGPRINT1A("NotifyCancel:: Calling ref plugins functions (stateadaptationref.dll)");
        if(iTimer->IsActive())
            {
            iTimer->Cancel();
            }
        }
	}

/**
    Helper function to check for P&S Key
*/
TBool CStateAdaptationRef::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KStateAdaptationPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KStateAdaptationPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        //Could not retrieve property value. Tests might fail 
        DEBUGPRINT1A("IsTestPsKeyDefined ERROR :: Could not retrieve property value)");
        }
    return testPsKeyDefined;
    }

CStateRefAdaptationTimer::CStateRefAdaptationTimer():CTimer(CActive::EPriorityUserInput)
	{
   	CActiveScheduler::Add(this);
	}

CStateRefAdaptationTimer::~CStateRefAdaptationTimer()
	{
	Cancel();	
	}

CStateRefAdaptationTimer* CStateRefAdaptationTimer::NewL()
	{
   	CStateRefAdaptationTimer* self=new (ELeave) CStateRefAdaptationTimer();
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop();
    return self;		
	}

void CStateRefAdaptationTimer::After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus)
	{
	iReqStatus = &aStatus;
	if(!IsActive())
	CTimer::After(aCancelDelay);	
	}



void CStateRefAdaptationTimer::DoCancel()
	{
	User::RequestComplete(iReqStatus, KErrCancel);	
	CTimer::DoCancel();	
	}


void CStateRefAdaptationTimer::RunL()
	{
	User::RequestComplete(iReqStatus, KErrNone);	
	}

	
