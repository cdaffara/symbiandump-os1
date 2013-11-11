// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// The test P & S key which it looks for is KSimPluginPropertyKey (0x2000D76A)
//

#include "simadaptationref.h"
#include "ssmdebug.h"
#include <e32property.h> 

const TUint32 KSimPluginPropertyKey = 0x2000D76B;
const TUid KPropertyCategory={0x2000D75B};

/**
Function to create new Sim Adaptation Plugin.

@return	a new plugin object for Sim Adaptations.
*/
EXPORT_C MSimAdaptation* CreateSimAdaptationRefL()
	{
	CSimAdaptationRef* simAdaptationRef = CSimAdaptationRef::NewL();
	return (static_cast<MSimAdaptation*>(simAdaptationRef));
	}

CSimAdaptationRef* CSimAdaptationRef::NewL()
	{
	CSimAdaptationRef* self = new(ELeave) CSimAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}

CSimAdaptationRef::~CSimAdaptationRef()
	{
	delete iTimer;
	iSaaSimAdaptationLib.Close();
	}

CSimAdaptationRef::CSimAdaptationRef()
	{
	}

void CSimAdaptationRef::ConstructL()
	{
	DEBUGPRINT1A("Loading Actual plugins");
    _LIT(KSaaSimAdaptationDLL, "saaSimadaptation.dll");
    User::LeaveIfError(iSaaSimAdaptationLib.Load(KSaaSimAdaptationDLL));
    iSaaSimAdaptationDll = (MSimAdaptation *)(iSaaSimAdaptationLib.Lookup(1)()); 
    
	iTimer = CSimRefAdaptationTimer::NewL();
	}

//from MSimAdaptation
void CSimAdaptationRef::Release()
	{
	delete this;
	}

void CSimAdaptationRef::GetSimOwned(TDes8& aOwnedPckg, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("GetSimOwned:: Calling Actual plugins functions (saaSimadaptation.dll)");
        iSaaSimAdaptationDll->GetSimOwned(aOwnedPckg,aStatus);
        }
    else
        {
        DEBUGPRINT1A("GetSimOwned :: Calling ref plugins functions (Simadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* pStatus = &aStatus;
        User::RequestComplete(pStatus, KErrNone);      
        }	
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CSimAdaptationRef::GetCancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("GetCancel ::Calling Actual plugins functions (saaSimadaptation.dll)");
        iSaaSimAdaptationDll->GetCancel();
        }
	}

/**
  The reference implementation completes with KErrNotSupported. This is required for automated testing.
  Actual plugins return expected values and this can be verified by manual testing
  On a device, Sim Adaptation Plug-in would complete 'aTypePckg' with one of the event types in TSsmSimEventType.
  
  
  The above mentioned implementation is modified to facilitate testing and increase the code coverage of the Adaptation 
  server code.The modified functionality is as follows.
  
  
  Instead of completing the notification request with KErrNotSupported the it is passed to 
  CSimAdaptationRefEventHandler which is an active object wiht lower priority compared to 
  the server active object.Because of this a delay will be introduced in completing the notification
  request so that other active objects can run avoiding infinite loop and starvation of other 
  active objects,which will be the case if the notification request is immediately completed.
  This is strictly for testing purposes.On a device this call will be replaced by Sim Adaptation
  Plug-in  completing 'aTypePckg' with one of the event types in TSsmSimEventType.

  
  
*/
void CSimAdaptationRef::NotifySimEvent(TDes8& /*aTypePckg*/, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("NotifySimEvent :: Calling Actual plugins functions (saaSimadaptation.dll)");
        /* Only clayersup.dll has an outstanding request. If this is passed to the actual plugin, the
        request will never complete till a SIM event happens. This would add the test code requests in a queue
        and the test code waits indefinitely. Hence, complete the request with KErrCancel. This would free the 
        queue for test code to be executed. It has not impact on the test environment */
        TRequestStatus *request = &aStatus; 
        User::RequestComplete(request, KErrCancel);
        }
    else
        {
        DEBUGPRINT1A("NotifySimEvent :: Calling ref plugins functions (Simadaptationref.dll)");
        aStatus = KRequestPending;
        iTimer->After(2000000,aStatus);      
        }   

	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CSimAdaptationRef::NotifyCancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("NotifyCancel :: Calling Actual plugins functions (saaSimadaptation.dll)");
        iSaaSimAdaptationDll->NotifyCancel();
        }
    else
        {
        DEBUGPRINT1A("NotifyCancel :: Calling ref plugins functions (Simadaptationref.dll)");
        if(iTimer->IsActive())
            {
            iTimer->Cancel();           
            }     
        }	
	}

/**
    Helper function to check for P&S Key
*/
TBool CSimAdaptationRef::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KSimPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KSimPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        //Could not retrieve property value. Tests might fail 
        DEBUGPRINT1A("IsTestPsKeyDefined ERROR :: Could not retrieve property value)");
        }
    return testPsKeyDefined;
    }


CSimRefAdaptationTimer::CSimRefAdaptationTimer():CTimer(CActive::EPriorityUserInput)
	{
   	CActiveScheduler::Add(this);
	}

CSimRefAdaptationTimer::~CSimRefAdaptationTimer()
	{
	Cancel();	
	}

CSimRefAdaptationTimer* CSimRefAdaptationTimer::NewL()
	{
   	CSimRefAdaptationTimer* self=new (ELeave) CSimRefAdaptationTimer();
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop();
    return self;		
	}

void CSimRefAdaptationTimer::After(TTimeIntervalMicroSeconds32 aCancelDelay, TRequestStatus& aStatus)
	{
	iReqStatus = &aStatus;
	if(!IsActive())
	CTimer::After(aCancelDelay);	
	}



void CSimRefAdaptationTimer::DoCancel()
	{
	User::RequestComplete(iReqStatus, KErrCancel);	
	CTimer::DoCancel();	
	}


void CSimRefAdaptationTimer::RunL()
	{
	User::RequestComplete(iReqStatus, KErrNone);	
	}

	
