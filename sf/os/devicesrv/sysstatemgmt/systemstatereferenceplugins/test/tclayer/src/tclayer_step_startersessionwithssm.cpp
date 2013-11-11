// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
//Check SetState functionality for ENormal,EBTSap,EOffline of RStarterSession class.
//Check ResetNetwork() behaviour of RStarterSession class.
//Check IsRTCTimeValid when property is valid and invalid
//Check rfstatusswp transition failure when process does not have WriteDeviceData or PowerMgmt Capabilities.

//In case of a system wide errors, Set error should be passed with the system wide error else 
//ETrue should be passed to mark the test block as Fail.
//Note that ETrue value is 1 and incase where test block expects the result to be 1 some appropriate value other than ETrue should be used.

/**
 @file
 @SetError
 @internalComponent - Internal Symbian SetError code  
*/

#include <ssm/clayerswp.hrh>
#include <ssm/ssmuiproviderdll.h>
#include <ssm/startupdomainpskeys.h>

#include "tclayer_step_startersessionwithssm.h"

const TUint KRFStatusPropertyKey = 0x2001D2A9;
const TUid KRFStatusPropertyCategory = {0x2000D75B};
const TUint32 KMiscPluginPropertyKey = 0x2000E658;

_LIT(KPolicyFilename,                   "rfstatusswppolicy.dll");

// Parameters
_LIT(KRTCTimeValidity,                  "RTCTimeValidity");
_LIT(KGlobalState,                      "GlobalState");
_LIT(KGlobalStateName,                  "GlobalStateName");
_LIT(KRfStatus,                         "RfStatus");
#ifdef TEST_SSM_GRACEFUL_OFFLINE 
_LIT(KWaitToSubscribe,                  "waitToSubscribe");
#endif

// Commands
_LIT(KDefineSwpCmd,                     "DefineSwpCmd");
_LIT(KSetStateCmd,                      "SetStateCmd");
_LIT(KRequestResetNetworkCmd,           "RequestResetNetworkCmd");
_LIT(KIsRTCTimeValidCmd,                "IsRTCTimeValidCmd");
_LIT(KRFSwpTransitionNotAllowCmd,       "RFSwpTransitionNotAllowCmd");

/**
 * Constructs a complete CCLayerTestRFSwpStatus object
 */
CCLayerTestWrapperWithSsm::CCLayerTestWrapperWithSsm():iObject()
	{
	
	}

CCLayerTestWrapperWithSsm::~CCLayerTestWrapperWithSsm()
	{
	iStarterSession.Close();
	iSwp.Close();
	}

CCLayerTestWrapperWithSsm* CCLayerTestWrapperWithSsm::NewL()
    {
    CCLayerTestWrapperWithSsm* ret = new (ELeave) CCLayerTestWrapperWithSsm();
    CleanupStack::PushL(ret);
    ret->ConstructL();
    CleanupStack::Pop(ret);
    return ret;
    }

void CCLayerTestWrapperWithSsm::ConstructL()
    {
    User::LeaveIfError(iStarterSession.Connect());
    }

/**
Purpose: Command fuction for a wrapper class

*/
TBool CCLayerTestWrapperWithSsm::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
    {
    __UHEAP_MARK;
    
    TInt error = RProperty::Define(KRFStatusPropertyCategory, KMiscPluginPropertyKey, RProperty::EInt);
    if((KErrNone != error) && (KErrAlreadyExists != error))
        {
        ERR_PRINTF2(_L("Defining KMiscPluginPropertyKey failed with error %d"), error);
        User::Leave(error);
        }
    error = RProperty::Set(KRFStatusPropertyCategory, KMiscPluginPropertyKey, 1);
    if(KErrNone != error)
        {
        ERR_PRINTF2(_L("Setting KMiscPluginPropertyKey with value 1 failed with error %d"), error);
        User::Leave(error);
        }
    TBool ret = ETrue;
    // Print out the parameters for debugging
    INFO_PRINTF2( _L("<font size=3 color=990000><b>aCommand = %S</b></font>"), &aCommand );
    INFO_PRINTF3( _L("aSection = %S aAsyncErrorIndex = %D"), &aSection,aAsyncErrorIndex );
    TRAPD(err, DoCommandProcessL(aCommand,aSection));
    if(err != KErrNone)
        {
        SetError(err);
        ret = EFalse;
        }
    error = RProperty::Delete(KRFStatusPropertyCategory, KMiscPluginPropertyKey);
    if(KErrNone != error)
        {
        ERR_PRINTF2(_L("Deleting KMiscPluginPropertyKey failed with error %d"), error);
        User::Leave(error);
        }
    __UHEAP_MARKEND;
    return ret;
    }

/*
 * Commands are called, implemented only to catch the error in case of Leave
 */
void CCLayerTestWrapperWithSsm::DoCommandProcessL(const TTEFFunction& aCommand, const TTEFSectionName& aSection)
    {
    if(KRFSwpTransitionNotAllowCmd() == aCommand)
        {
        doTestRFSwpTransitionNotAllowedCmdL(aSection);
        }
    else if(KDefineSwpCmd() == aCommand)
        {
        DefineAndRegisterPropertyL();
        }
    else if(KSetStateCmd() == aCommand)
        {
        doTestSetStateCmdL(aSection);
        }
    else if(KRequestResetNetworkCmd() == aCommand)
        {
        doTestRequestResetNetworkCmdL();
        }
    else if(KIsRTCTimeValidCmd() == aCommand)
        {
        doTestIsRTCTimeValidCmdL(aSection);
        }
    else
        {
        User::Leave(EFalse);
        }
    }

/**
 * SetError is set to KErrNotSupported and KErrArgument which is expected value of transition not allowed for RFStatusSwPPolicy
 * RSsmStateManager session request for swp change to check transition allowed or not 
 */
void CCLayerTestWrapperWithSsm::doTestRFSwpTransitionNotAllowedCmdL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Entering CCLayerTestWrapperWithSsm::doTestRFSwpTransitionNotAllowedCmdL()"));
	
	//Connect RSsmSystemWideProperty
    User::LeaveIfError(iSwp.Connect(KRFStatusPropertyKey));
         
    TInt value = 0;
    User::LeaveIfError(GetIntFromConfig(aSection, KGlobalState(), value));
    RStarterSession::TGlobalState state = static_cast<RStarterSession::TGlobalState>(value);
    TPtrC   name;
    User::LeaveIfError(GetStringFromConfig(aSection, KGlobalStateName(), name));
    INFO_PRINTF2(_L("SetState %S"),&name);
    TInt err = iStarterSession.SetState(state);
    SetError(err);
    if(value == RStarterSession::EOffline)
        {
        //RF SwP policy returns KErrNotSupported when process does not have required capability
        INFO_PRINTF3(_L("Completing SwpTransition with expected error %d, and actual error %d"),KErrNotSupported,err);
        }
    else if(value == RStarterSession::ENormal)
        {
        INFO_PRINTF3(_L("Completing SwpTransition with expected error %d, and actual error %d"),KErrArgument,err);
        }
    iSwp.Close();
    INFO_PRINTF1(_L("Exiting CCLayerTestWrapperWithSsm::doTestRFSwpTransitionNotAllowedCmdL()"));
	}

/**
 * SetError is set to KErrNone which is expected value to pass SetState functionality of RStarterSession class 
 * which is specific to RF SwP change 
 */
void CCLayerTestWrapperWithSsm::doTestSetStateCmdL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Entering CCLayerTestWrapperWithSsm::doTestSetStateCmdL()"));
	//Connect RSsmSystemWideProperty
	User::LeaveIfError(iSwp.Connect(KRFStatusPropertyKey));
			 
	TInt value = 0;
	User::LeaveIfError(GetIntFromConfig(aSection, KGlobalState(), value));
    RStarterSession::TGlobalState state = static_cast<RStarterSession::TGlobalState>(value);
    TPtrC   name;
    User::LeaveIfError(GetStringFromConfig(aSection, KGlobalStateName(), name));
    INFO_PRINTF2(_L("SetState %S"),&name);
    User::LeaveIfError(GetIntFromConfig(aSection, KRfStatus(), value));
    TInt currentSwpVal;
    RProperty::Get(KRFStatusPropertyCategory,KRFStatusPropertyKey,currentSwpVal);
    //iStarterSession.SetState only called when currentSwpVal and called setting state's SwP value are different
    if(currentSwpVal != value)
        {
        TInt err = iStarterSession.SetState(state);
        if(err != KErrNone)
            {
            SetError(err);
            INFO_PRINTF4(_L("Completing SetState(%S) with expected value %d, and actual value %d"),&name,KErrArgument,err);
            }
#ifdef TEST_SSM_GRACEFUL_OFFLINE
        //Wait until RequestStateTransition() completes and KPSGlobalSystemState is changed
        else
            {
            TBool waitToSubscribe = EFalse;
            GetBoolFromConfig(aSection, KWaitToSubscribe(), waitToSubscribe);
            if (waitToSubscribe)
                {
                TRequestStatus status;
                RProperty property;                
                if(property.Attach(KPSUidStartup, KPSGlobalSystemState, EOwnerThread ) == KErrNone)
                    {
                    property.Subscribe(status);
                    User::WaitForRequest(status);
                    if ((status.Int()!= KErrNone))
                        {
                        SetError(status.Int());
                        ERR_PRINTF2(_L("Subscribing for the status returned an error: %d"),status.Int());
                        }
                    property.Close();
                    }
                else
                    {
                    SetBlockResult(EFail);
                    ERR_PRINTF1(_L("Coudnot attach the property to KPSUidStartup, KPSGlobalSystemState"));
                    }
                }
            }
#else
        //wait until RequestSwpChange() completes and rfsatus is changed. 
        else
            {
            TRequestStatus status;
            TInt rfStatus = 0;
            iSwp.Subscribe(status);
            User::WaitForRequest(status);
            TInt err = iSwp.GetValue(rfStatus);
            //Test will fail if the new rfstatus value is not same as what it is set with
            //or if status completed with error 
            //or when GetValue returned an error
            if((rfStatus != value) || 
               (status.Int() != KErrNone) || 
               (err != KErrNone))
                {
                SetError(err);
                }
            INFO_PRINTF4(_L("Completing SetState(%S) with expected rfstatus value %d, and actual rfstatus value %d"),&name,value,rfStatus);
            }
#endif // TEST_SSM_GRACEFUL_OFFLINE
        }
    else
        {
        SetError(ETrue);
        }
    //closing swp
    iSwp.Close();
    INFO_PRINTF1(_L("Exiting CCLayerTestWrapperWithSsm::doTestSetStateCmdL()"));
	}

/**
 * SetError is set to KErrNone which is expected value to pass ResetNetwork functionality of RStarterSession class 
 * It checks rf SwP changes untill it will be ESsmRfOn
 */
void CCLayerTestWrapperWithSsm::doTestRequestResetNetworkCmdL()
	{
	INFO_PRINTF1(_L("Entering CCLayerTestWrapperWithSsm::doTestRequestResetNetworkCmdL()"));
	User::LeaveIfError(iSwp.Connect(KRFStatusPropertyKey));
	
	User::LeaveIfError(iStarterSession.ResetNetwork());
        
    TInt value = 1;
    iSwp.GetValue(value);
    if(value == ESsmRfOff)
        {
        TRequestStatus status;
        iSwp.Subscribe(status);
        User::WaitForRequest(status);
        iSwp.GetValue(value);
        }
    if(value != ESsmRfOn)
        {
        SetError(ETrue);
        }
    iSwp.Close();
    INFO_PRINTF1(_L("Exiting CCLayerTestWrapperWithSsm::doTestRequestResetNetworkCmdL()"));
	}

/**
 * SetError is set to KErrNone which is expected value to pass IsRTCTimeValid functionality of RStarterSession class 
 * It define the property with True and False value and checks its validity  
 */
void CCLayerTestWrapperWithSsm::doTestIsRTCTimeValidCmdL(const TDesC& aSection)
    {
    INFO_PRINTF1(_L("Entering CCLayerTestWrapperWithSsm::doTestIsRTCTimeValidCmdL()"));
    
    TBool value = EFalse;
    User::LeaveIfError(GetBoolFromConfig(aSection, KRTCTimeValidity(), value));
    INFO_PRINTF2(_L("Define Property for IsRTCTimeValid with value %d"), value);
    TInt ret = RProperty::Set(CSsmUiSpecific::ValidateRTCPropertyCategory(),
                              CSsmUiSpecific::ValidateRTCPropertyKey(), value);
    TBool validity = EFalse;
    if( KErrNone == ret )
        {
        validity = iStarterSession.IsRTCTimeValid();
        INFO_PRINTF2(_L("IsRTCTimeValid returns with validity %d"),validity);
        }
    else
        {
        SetError(ret);
        }
    
    //to pass the test value and validity should always be same
    if(value != validity)
        {
        SetError(ETrue);
        }
    INFO_PRINTF1(_L("Exiting CCLayerTestWrapperWithSsm::doTestIsRTCTimeValidCmdL()"));
    }
/*
 * Define property and register
 */
void CCLayerTestWrapperWithSsm::DefineAndRegisterPropertyL()
    {
    //Define the property for rfstatusswppolicy 
    _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
    TInt result = RProperty::Define(KRFStatusPropertyCategory, KRFStatusPropertyKey, RProperty::EInt,
                                    KAllowAllPolicy, KAllowAllPolicy);

    _LIT(KPRINT,"Return code for RProperty::Define for RFstate property key - %d -");
    RDebug::Print(KPRINT, result);
    
    if( KErrNone != result && KErrAlreadyExists != result)
        {
        User::Leave(result);
        }
    //define property for ValidateRTC
    result = RProperty::Define(CSsmUiSpecific::ValidateRTCPropertyCategory(),
                               CSsmUiSpecific::ValidateRTCPropertyKey(), RProperty::EInt,
                               KAllowAllPolicy, KAllowAllPolicy);
    
    _LIT(KPRINT2,"Return code for RProperty::Define for ValidateRTCPropertyKey - %d -");
    RDebug::Print(KPRINT2, result);
    
    if(KErrNone != result && KErrAlreadyExists != result)
        {
        User::Leave(result);
        }
    RSsmStateManager ssmSession;
    User::LeaveIfError(ssmSession.Connect());
    CleanupClosePushL(ssmSession);
    TInt err = ssmSession.RegisterSwpMapping(KRFStatusPropertyKey, KPolicyFilename);
    if(err != KErrAlreadyExists && KErrNone != err )
        {
        User::Leave(err);
        }
    RProperty::Set(KRFStatusPropertyCategory,KRFStatusPropertyKey,0xABAC);
    CleanupStack::PopAndDestroy();//RSsmStateManager ssmSession
    }
