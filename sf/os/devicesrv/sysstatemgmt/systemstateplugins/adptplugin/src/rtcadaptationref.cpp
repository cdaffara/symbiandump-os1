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
// The test P & S key which it looks for is KRtcAdaptationPluginPropertyKey (0x2000D76A)
//

#include "rtcadaptationref.h"

#include <e32property.h>
#include "ssmdebug.h"

const TUid KAlarmServerUID = {0x101f5027};
const TInt KTestRTCValueKey = 200;


const TUint32 KRtcAdaptationPluginPropertyKey = 0x2000D76C;
const TUid KPropertyCategory={0x2000D75B};

/**
Function to create new Rtc Adaptation Plugin.

@return	a new instance of MRtcAdaptation for RTC (Real Time Clock) Adaptations.
*/
EXPORT_C MRtcAdaptation* CreateRtcAdaptationRefL()
	{
	CRtcAdaptationRef* rtcAdaptationRef = CRtcAdaptationRef::NewL();
	return (static_cast<MRtcAdaptation*>(rtcAdaptationRef));
	}

CRtcAdaptationRef* CRtcAdaptationRef::NewL()
	{
	CRtcAdaptationRef* self = new(ELeave) CRtcAdaptationRef;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	
	return self;	
	}

void CRtcAdaptationRef::ConstructL()
    {
    DEBUGPRINT1A("Loading Actual plugins");
    _LIT(KSaaRtcAdaptationDLL, "saartcadaptation.dll");
    User::LeaveIfError(iSaaRtcAdaptationLib.Load(KSaaRtcAdaptationDLL));
    iSaaRtcAdaptationDll = (MRtcAdaptation *)(iSaaRtcAdaptationLib.Lookup(1)());    
    }

CRtcAdaptationRef::~CRtcAdaptationRef()
	{
	iSaaRtcAdaptationLib.Close();
	}

CRtcAdaptationRef::CRtcAdaptationRef()
	{
	}

/**
 Deletes and frees memory allocated.
*/
void CRtcAdaptationRef::Release()
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("Release:: Calling Actual plugins functions (saartcadaptation.dll)");
        iSaaRtcAdaptationDll->Release();
        }
    delete this;
	}

/**
 Check that the RTC is valid.
 Reference implementation completes the request with KErrNotSupported. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 
 @param aValidityPckg on return contains the status of the validity of the RTC as a boolean value
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus)
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("ValidateRtc:: Calling Actual plugins functions (saartcadaptation.dll)");
        iSaaRtcAdaptationDll->ValidateRtc(aValidityPckg, aStatus);
        }
    else
        {
        DEBUGPRINT1A("ValidateRtc:: Calling ref plugins functions (rtcadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNotSupported);
        }
	}

/**
 Set a device wake-up alarm time, in UTC (coordinated universal time), in the RTC.
 Reference implementation completes the request with KErrNotSupported. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 For testing purposes it sets a pub sub property defined in test code.

 @param aAlarmTimePckg requested wake up time
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus)
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("SetWakeupAlarm:: Calling Actual plugins functions (saartcadaptation.dll)");
        iSaaRtcAdaptationDll->SetWakeupAlarm(aAlarmTimePckg, aStatus);
        }
    else
        {
        DEBUGPRINT1A("SetWakeupAlarm:: Calling ref plugins functions (rtcadaptationref.dll)");
        // Set this pub sub property (for testing purposes)
        // The property is defined in the test code.  In normal operation this will fail silently because the property has not been defined.
        RProperty::Set(KAlarmServerUID, KTestRTCValueKey, aAlarmTimePckg);
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        // No support for RTC on HRP/Techview.
        User::RequestComplete(status, KErrNotSupported);
        }
	}


/**
 Delete the current device wake-up alarm time in the RTC.
 Reference implementation completes the request with KErrNotSupported. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 For testing purposes it sets a pub sub property to a NULL value defined in test code.

 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CRtcAdaptationRef::UnsetWakeupAlarm(TRequestStatus& aStatus)
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("UnsetWakeupAlarm:: Calling Actual plugins functions (saartcadaptation.dll)");
        iSaaRtcAdaptationDll->UnsetWakeupAlarm(aStatus);
        }
    else
        {
        DEBUGPRINT1A("UnsetWakeupAlarm:: Calling ref plugins functions (rtcadaptationref.dll)");
        // Set this pub sub property to a NULL value because we are unsetting the RTC (for testing purposes)
        // The property is defined in the test code. In normal operation this will fail silently because the property has not been defined.
        TTime nullTime(Time::NullTTime());
        TPckgC<TTime> wakeupAlarmTimePckg(nullTime);
        RProperty::Set(KAlarmServerUID, KTestRTCValueKey, wakeupAlarmTimePckg);
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        // No support for RTC on HRP/Techview.
        User::RequestComplete(status, KErrNotSupported);
        }
	}

/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CRtcAdaptationRef::Cancel()
	{
    if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("Cancel:: Calling Actual plugins functions (saartcadaptation.dll)");
        iSaaRtcAdaptationDll->Cancel();
        }
	}

/**
    Helper function to check for P&S Key
*/
TBool CRtcAdaptationRef::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KRtcAdaptationPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KRtcAdaptationPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        //Could not retrieve property value. Tests might fail 
        DEBUGPRINT1A("IsTestPsKeyDefined ERROR :: Could not retrieve property value)");
        }
    return testPsKeyDefined;
    }
