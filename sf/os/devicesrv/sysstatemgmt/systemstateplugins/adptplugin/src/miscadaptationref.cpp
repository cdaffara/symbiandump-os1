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
// The test P & S key which it looks for is KMiscPluginPropertyKey(0x2000E658)
//

#include "ssmdebug.h"
#include "miscadaptationref.h"
#include <ssm/startupadaptationcommands.h>
#include <e32property.h> 

_LIT(KTestCmdSecurityCheckTestFile, "c:\\cmdsecuritychecktest\\pinchecksecuritycaseno.txt");

const TUint32 KMiscPluginPropertyKey = 0x2000E658;
const TUid KPropertyCategory={0x2000D75B};              // tcustomcmd_server SID = KSsmServerName SID (changed in tcustomcmd_server.mmp file)
/**
 Function to create new Misc Adaptation Plugin. Used for a collection of activities.

 @return	new instance of MEmergencyCallRfAdaptation for Miscellaneous Adaptations.

 @internalComponent
 @released
*/
EXPORT_C MMiscAdaptation* CreateMiscAdaptationRefL()
	{
	CMiscAdaptationRef* miscAdaptationRef = CMiscAdaptationRef::NewL();
	return (static_cast<MMiscAdaptation*>(miscAdaptationRef));
	}

CMiscAdaptationRef* CMiscAdaptationRef::NewL()
	{
	CMiscAdaptationRef* self = new(ELeave) CMiscAdaptationRef;
	self->ConstructL();
	return self;
	}

CMiscAdaptationRef::~CMiscAdaptationRef()
	{
	iFs.Close();
	iSaaMiscAdaptationLib.Close();
	}

CMiscAdaptationRef::CMiscAdaptationRef()
	{
	}

void CMiscAdaptationRef::ConstructL()
	{
	DEBUGPRINT1A("Loading Actual plugins");
    _LIT(KSaaMiscAdaptationDLL, "saamiscadaptation.dll");
    User::LeaveIfError(iSaaMiscAdaptationLib.Load(KSaaMiscAdaptationDLL));
    iSaaMiscAdaptationDll = (MMiscAdaptation *)(iSaaMiscAdaptationLib.Lookup(1)());   
    
	iTestCaseNum = 1;
	User::LeaveIfError(iFs.Connect());
	}

/** 
 Deletes and frees memory allocated.
*/
void CMiscAdaptationRef::Release()
	{
	if(!IsTestPsKeyDefined())
	    {
	    DEBUGPRINT1A("Release :: Calling Actual plugins functions (saamiscadaptation.dll)");
	    iSaaMiscAdaptationDll->Release();
	    }
	delete this;
	}

/**
 Get the start-up mode from the Cooperating System.
 Reference implementation completes the request with KErrNone. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing

 @param aModePckg should contain start-up of the Cooperating System on completion
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CMiscAdaptationRef::GetGlobalStartupMode(TDes8& aModePckg, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
	    {
	    DEBUGPRINT1A("GetGlobalStartupMode :: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->GetGlobalStartupMode(aModePckg,aStatus);
	    }
	else
	    {
	    DEBUGPRINT1A("GetGlobalStartupMode :: Calling ref plugins functions (miscadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete(status, KErrNone);        
	    }
	}

/**
 Prepare language codes stored in the SIM in preferred language lists. The next call should be GetSimLanguagesL()
 Reference implementation completes with KErrNotSupported. This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 
 @param aPriority priority for which the language lists need to be prepared
 @param aSizePckg on return contains the size of the buffer, which a client should allocate and pass as one of the parameters (TInt aCount) to GetSimLanguagesL()
 @param aStatus to complete when the operation has finished

 @see TSsmLanguageListPriority
 @see TRequestStatus
 @see GetSimLanguagesL
*/
void CMiscAdaptationRef::PrepareSimLanguages(TSsmLanguageListPriority aPriority, TDes8& aSizePckg, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("PrepareSimLanguages :: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->PrepareSimLanguages(aPriority,aSizePckg,aStatus);
        }
    else
        {
        DEBUGPRINT1A("PrepareSimLanguages :: Calling ref plugins functions (miscadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        // No support for SIM on H4hrp/Techview.
        User::RequestComplete(status, KErrNotSupported);
        }
	}

/**
 Get language codes stored in the SIM in preferred language lists. PrepareSimLanguages() call should precede this call.
 Reference implementation completes with KErrNotSupported.  This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
 
 @param aBuf should contain the language lists on completion
 @param aCount to contain the count of the languages

 @see PrepareSimLanguages
*/
void CMiscAdaptationRef::GetSimLanguagesL(CBufBase* aBuf, TInt aCount)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("GetSimLanguagesL :: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->GetSimLanguagesL(aBuf,aCount);
        }
    else
        {
        DEBUGPRINT1A("GetSimLanguagesL :: Calling ref plugins functions (miscadaptationref.dll)");
        User::Leave(KErrNotSupported);
        }	
	}

void CMiscAdaptationRef::GetHiddenReset(TDes8& aHiddenResetPckg, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("GetHiddenReset :: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->GetHiddenReset(aHiddenResetPckg,aStatus);
        }
    else
        {
        DEBUGPRINT1A("GetHiddenReset :: Calling ref plugins functions (miscadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        // No support for Reset in HRP/Techview.
        User::RequestComplete(status, KErrNotSupported);        
        }
	}


/**
  Cancel the outstanding request. Reference implementation completes the requests immediately so there is nothing to Cancel.
  On a device, Cancel() needs an implementation as the Request might be outstanding and it needs to be cancelled.
*/
void CMiscAdaptationRef::Cancel()
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("Cancel :: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->Cancel();
        }
	}

/**
 Request the next security state.
 Reference implementation would always return 'KErrNotSupported'.  This is required for automated testing.
 Actual plugins return expected values and this can be verified by manual testing
  
 The function is modified to suit testing needs for PinCheck Security Command.
 A real implementation is intended to differ a lot based on the runtime changes/events on the device.
 
 Each switch case represents a PIN Check security plug-in functionality in different status are as follows..
 
 EPINCHECK01 : SIM present and PIN1 required.
 EPINCHECK02 : SIM present and rejected
 EPINCHECK03 : SIM blocked PUK required
 EPINCHECK04 : SIM locked security code required.
 EPINCHECK05 : SIM locked security code required when sim lockRestrictionOn
 EPINCHECK06 : SIM blocked and UPUK.
 EPINCHECK07 : SIM blocked security code required when sim lockRestrictionOn
 EPINCHECK08 : SIM present and PIN1 required and SimLock status is wrong
 iTestCaseNum will be reset to 1 at EPINCHECK08 to test all above scenarios for wrong entered pin
 
 @param aState next security state
 @param aResponsePckg response to state change request on completion
 @param aStatus to complete when the operation has finished

 @see TRequestStatus
*/
void CMiscAdaptationRef::SecurityStateChange(TInt aState, TDes8& aResponsePckg, TRequestStatus& aStatus)
	{
	if(!IsTestPsKeyDefined())
        {
        DEBUGPRINT1A("SecurityStateChange:: Calling Actual plugins functions (saamiscadaptation.dll)");
        iSaaMiscAdaptationDll->SecurityStateChange(aState,aResponsePckg,aStatus);
        }
    else
        {
        DEBUGPRINT1A("SecurityStateChange:: Calling ref plugins functions (miscadaptationref.dll)");
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        StartupAdaptation::TSecurityStateInfo info = StartupAdaptation::EYes;
        
        //Read Test case number from the file 
        TInt err = iFile.Open(iFs, KTestCmdSecurityCheckTestFile, EFileRead);
        if (err == KErrNone)
            {
            TRAP(err,iTestCaseNum = iFile.ReadInt32L());
            iFile.Close();
            if(err)
                {
                RDebug::Printf("Error while reading tst case number from the file pinchecksecuritycaseno.txt");
                User::RequestComplete(status, err);
                return;
                }
            }   
        switch(iTestCaseNum)
            {
            case EPINCHECK01:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::EPINRequired:
                    info = StartupAdaptation::EPIN1Required;
                    break;
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockOk;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes;
                    break;  
                default:
                    break;
                }
                break;
            case EPINCHECK02:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes;
                    break;  
                default:
                break;
                }
                break;
            case EPINCHECK03:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::EPUK1Required;
                    break;  
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockOk;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes;
                    break;  
                default:
                    break;
                }
                break;
            case EPINCHECK04:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::EPUK1Required;
                    break;  
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockOk;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes;
                    break;  
                default:
                    break;
                }   
                break;
            case EPINCHECK05:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::EUPUKRequired;
                    break;  
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockRestrictionOn;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes;
                    break;  
                default:
                    break;  
                }   
                break;
            case EPINCHECK06:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::EUPUKRequired;
                    break;  
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockRestrictionPending;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes; 
                    break;  
                default:
                    break;  
                }
                break;
            case EPINCHECK07:
                switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::EUPUKRequired;
                    break;  
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockRestricted;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes; 
                    break;  
                default:
                    break;  
                }
                break;
            case EPINCHECK08:
            switch(aState)
                {
                case StartupAdaptation::ESIMPresent:
                    info = StartupAdaptation::EYes;
                    break;
                case StartupAdaptation::ESIMInvalid:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMRejected:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::ESIMBlocked:
                    info = StartupAdaptation::ENo;
                    break;
                case StartupAdaptation::EPINRequired:
                    info = StartupAdaptation::EPIN1Required;
                    break;
                case StartupAdaptation::ESIMLock:
                    info = StartupAdaptation::ESimLockOk;
                    break;
                case StartupAdaptation::ESecurityCheckOK:
                    info = StartupAdaptation::EYes; 
                    break;  
                case StartupAdaptation::ESecurityCheckFailed:
                    info = StartupAdaptation::EYes; 
                    break;  
                default:
                    break;
                }   
                break;  
            default:
                break;
            }
        TPckgBuf<StartupAdaptation::TSecurityStateInfo> securityStateInfoResult(info);
        aResponsePckg = securityStateInfoResult;    
        User::RequestComplete(status, KErrNone);
        }
	}

/**
    Helper function to check for P&S Key
*/
TBool CMiscAdaptationRef::IsTestPsKeyDefined()
    {
    TBool testPsKeyDefined = EFalse;
    TInt result = RProperty::Get(KPropertyCategory, KMiscPluginPropertyKey, testPsKeyDefined);
    DEBUGPRINT3(_L("KMiscPluginPropertyKey %d Error %d"), testPsKeyDefined, result);
    if ((KErrNone != result) && (KErrNotFound != result))
        {
        //Could not retrieve property value. Tests might fail 
        DEBUGPRINT1A("IsTestPsKeyDefined ERROR :: Could not retrieve property value)");
        }
    return testPsKeyDefined;
    }

