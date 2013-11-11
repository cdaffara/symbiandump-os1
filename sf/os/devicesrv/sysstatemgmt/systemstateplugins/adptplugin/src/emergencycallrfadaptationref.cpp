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
// The test P & S key which it looks for is KEmergencyCallRfAdaptationPluginPropertyKey(0x2000E657)
//

#include "ssmdebug.h"
#include <e32property.h>
#include "emergencycallrfadaptationref.h"

const TUint32 KEmergencyCallRfAdaptationPluginPropertyKey = 0x2000E657;
const TUid KPropertyCategory={0x2000D75B};

/**
 Function to create new Emergency Call RF Adaptation Plugin.

 @return	new instance of MEmergencyCallRfAdaptation for Emergency Call RF Adaptations.

 @internalComponent
 @released
*/
EXPORT_C MEmergencyCallRfAdaptation* CreateEmergencyCallRfAdaptationRefL()
	{
	CEmergencyCallRfAdaptationRef* emergencyCallRfAdaptation = CEmergencyCallRfAdaptationRef::NewL();
	return (static_cast<MEmergencyCallRfAdaptation*> (emergencyCallRfAdaptation));
	}

CEmergencyCallRfAdaptationRef* CEmergencyCallRfAdaptationRef::NewL()
	{
	CEmergencyCallRfAdaptationRef* self = new(ELeave) CEmergencyCallRfAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;
	}

void CEmergencyCallRfAdaptationRef::ConstructL()
    {
    DEBUGPRINT1A("Loading Actual plugins");
    _LIT(KSaaEmergencyCallRfAdaptationDLL, "saaemergencycallrfadaptation.dll");
    User::LeaveIfError(iSaaEmergencyCallRfAdaptationLib.Load(KSaaEmergencyCallRfAdaptationDLL));
    iSaaEmergencyCallRfAdaptationDll = (MEmergencyCallRfAdaptation *)(iSaaEmergencyCallRfAdaptationLib.Lookup(1)());
    }

CEmergencyCallRfAdaptationRef::~CEmergencyCallRfAdaptationRef()
	{
	iSaaEmergencyCallRfAdaptationLib.Close();
	}

CEmergencyCallRfAdaptationRef::CEmergencyCallRfAdaptationRef()
	{
	}

/**
 Deletes and frees memory allocated.
*/
void CEmergencyCallRfAdaptationRef::Release()
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("Release:: Calling Actual plugins functions (saaemergencycallrfadaptation.dll)");
        iSaaEmergencyCallRfAdaptationDll->Release();
        }
    else
        {
        DEBUGPRINT1A("Release:: Calling Reference plugins functions (emergencycallrfadaptationref.dll)");
        }
    delete this;
	}

/**
 Request RF activation so that an emergency call can be made.
 Reference implementation completes the request with KErrNone. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing

 @param aStatus to complete when the operation has finished
 @see TRequestStatus 
*/
void CEmergencyCallRfAdaptationRef::ActivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("ActivateRfForEmergencyCall:: Calling Actual plugins functions (saaemergencycallrfadaptation.dll)");
        iSaaEmergencyCallRfAdaptationDll->ActivateRfForEmergencyCall(aStatus);
        }
    else
        {
        DEBUGPRINT1A("ActivateRfForEmergencyCall:: Calling Reference plugins functions (emergencycallrfadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
	}

/**
 Request RF deactivation after an emergency call have been made.
 Reference implementation completes the request with KErrNone. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 
 @param aStatus to complete when the operation has finished
 @see TRequestStatus 
*/
void CEmergencyCallRfAdaptationRef::DeactivateRfForEmergencyCall(TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("DeactivateRfForEmergencyCall:: Calling Actual plugins functions (saaemergencycallrfadaptation.dll)");
        iSaaEmergencyCallRfAdaptationDll->DeactivateRfForEmergencyCall(aStatus);
        }
    else
        {
        DEBUGPRINT1A("DeactivateRfForEmergencyCall:: Calling Reference plugins functions (emergencycallrfadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);
        }
 	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CEmergencyCallRfAdaptationRef::Cancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("Cancel:: Calling Actual plugins functions (saaemergencycallrfadaptation.dll)");
        iSaaEmergencyCallRfAdaptationDll->Cancel();
        }
	}

/**
    Helper function to check for P&S Key
*/
TBool CEmergencyCallRfAdaptationRef::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KEmergencyCallRfAdaptationPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        //Could not retrieve property value. Tests might fail 
        DEBUGPRINT1A("IsTestPsKeyDefined ERROR :: Could not retrieve property value)");
        }
    return testPsKeyDefined;
    }

