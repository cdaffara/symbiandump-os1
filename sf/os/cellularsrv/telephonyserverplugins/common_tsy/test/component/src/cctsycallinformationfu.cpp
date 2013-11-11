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
// The TEFUnit test suite for CallInformation in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsycallinformationfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <sys/unistd.h>


CTestSuite* CCTsyCallInformationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0006bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0006cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallParams00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallDuration0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallDuration0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallDuration00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetOwnershipStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetOwnershipStatus0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetOwnershipStatus00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCaps0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCaps00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetInfo00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetStatus0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetStatus00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetCallInfo00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestEnumerateCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestEnumerateCall0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestEnumerateCall00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCapsChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyHookChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyHookChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyHookChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyStatusChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallDurationChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallAdded00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCall00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyCallEvent00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallCaps00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo0006bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallInfo00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallStatus0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetMobileCallStatus00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyAudioToneEvent0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyAudioToneEvent0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyAudioToneEvent00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallCapsChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange00013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyRemotePartyInfoChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange00012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyMobileCallStatusChange00014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetIncomingCallType0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetIncomingCallType0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestGetIncomingCallType00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyIncomingCallTypeChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyLineHookChange0001L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyLineHookChange0006L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyCallInformationFU, TestNotifyLineHookChange00011L);	

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
    TBool activeCallFlag = EFalse;
    //----------------------------------------

    RMobileCall::TMobileCallParamsV1 callParams1;
    RMobileCall::TMobileCallParamsV1Pckg paramPckg1(callParams1);
    
    TInt ret = call.GetCallParams(paramPckg1);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg1, activeCallFlag);
    
    //----------------------------------------
    RMobileCall::TMobileCallParamsV2 callParams2;
    RMobileCall::TMobileCallParamsV2Pckg paramPckg2(callParams2);

    ret = call.GetCallParams(paramPckg2);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg2, activeCallFlag);
    
  	
	//----------------------------------------
    RMobileCall::TMobileCallParamsV7 callParams7;
    RMobileCall::TMobileCallParamsV7Pckg paramPckg7(callParams7);

    ret = call.GetCallParams(paramPckg7);

    ValidateCallParams(&paramPckg7, activeCallFlag);
    
    AssertMockLtsyStatusL();
	
	//-- now create incoming call and get params when dialing, just to show that they are changed 
	
    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
	RCall call2;	
	TInt callId = 1;
	TName incomingCallName;

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	
	CreateAndOpenIncomingCalLC(line, call2, incomingCallName, 
		lineName, callId, RMobileCall::EStatusIdle, mobileService);
	
    TRequestStatus requestStatus;
			
 	//-- dial -----------------------------------------------------------------------

    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
    // TMobileCallParamsV1 params for Dial
	RMobileCall::TMobileCallParamsV1 params;
	RMobileCall::TMobileCallParamsV1Pckg paramsPckg(params);
    params.iInterval = 2;
    params.iCug.iCugIndex = 0;
    params.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    params.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    params.iWaitForDialTone = RCall::EDialToneWait;
    
   	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KNum);
	callInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown;
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	callInfo.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;
	
	// Dialing and Verfication of params for TMobileCallParamsV1
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		dialData(-1, mobileService, params, callInfo);
	completeData.Close();
	dialData.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
    call2.Dial(requestStatus, paramsPckg, callInfo.iDialledParty.iTelNumber);
    TMockLtsyCallData0 dialCompleteData(0,mobileService);
	completeData.Close();
	dialCompleteData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData,0);	
	
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL(); 
       
    RMobileCall::TMobileCallParamsV1 recParams;
    RMobileCall::TMobileCallParamsV1Pckg recParamsPckg(recParams);
    ret = call2.GetCallParams(recParamsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    activeCallFlag = ETrue;
    
    ValidateCallParams(&recParamsPckg, activeCallFlag);

    // TMobileCallParamsV2 params for Dial
   	RMobileCall::TMobileCallParamsV2 params2;
   	RMobileCall::TMobileCallParamsV2Pckg paramsPckg2(params2);
    params2.iInterval = 2;
    params2.iCug.iCugIndex = 0;
    params2.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    params2.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    params2.iWaitForDialTone = RCall::EDialToneWait;
    params2.iBearerMode = RMobileCall::EMulticallNotSupported;
       
    // Dialing and Verfication of params for TMobileCallParamsV2
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		dialData2(-1, mobileService, params2, callInfo);
	completeData.Close();
	dialData2.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
    call2.Dial(requestStatus, paramsPckg2, callInfo.iDialledParty.iTelNumber);
    completeData.Close();
	dialCompleteData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData,0);	
		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    
    RMobileCall::TMobileCallParamsV2 recParams2;
    RMobileCall::TMobileCallParamsV2Pckg recParamsPckg2(recParams2);
    
    ret = call2.GetCallParams(recParamsPckg2);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&recParamsPckg2, activeCallFlag);

    // TMobileCallParamsV7 params for Dial
	RMobileCall::TMobileCallParamsV7 params7;
	RMobileCall::TMobileCallParamsV7Pckg paramsPckg7(params7);
	params7.iInterval = 2;
	params7.iCug.iCugIndex = 0;
	params7.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
	params7.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
	params7.iWaitForDialTone = RCall::EDialToneWait;
	params7.iBearerMode = RMobileCall::EMulticallNotSupported;
	params7.iAlphaId.Copy(_L("Alpha Id"));
    
    // Dialing and Verfication of params for TMobileCallParamsV7
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		dialData7(-1, mobileService, params7, callInfo);
	completeData.Close();
	dialData7.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
    call2.Dial(requestStatus, paramsPckg7, callInfo.iDialledParty.iTelNumber);
    completeData.Close();
	dialCompleteData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData,0);	
		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    RMobileCall::TMobileCallParamsV7 recParams7;
    RMobileCall::TMobileCallParamsV7Pckg recParamsPckg7(recParams7);
    ret = call2.GetCallParams(recParamsPckg7);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&recParamsPckg7, activeCallFlag);

    CleanupStack::PopAndDestroy(5, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);  
    
    TInt8 callParams;    // bad param
    TPckg<TInt8> paramPckg(callParams);
        
    TInt ret = call.GetCallParams(paramPckg);
    ASSERT_EQUALS(KErrArgument, ret);
    
    RCall::TCallParams callParams2;
    RCall::TCallParamsPckg paramPckg2(callParams2);
  
    ret = call.GetCallParams(paramPckg2);
    ASSERT_EQUALS(KErrArgument, ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	}

/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
       
	OpenLineLC(line, iPhone, lineName);
	
    
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
   
   
    //- using different types of params struct---------------------------------------
    
    RMobileCall::TMobileCallParamsV1 callParams1;
    RMobileCall::TMobileCallParamsV1Pckg paramPckg1(callParams1);
    TBool activeCallFlag = EFalse; 
    TInt ret = call.GetCallParams(paramPckg1);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg1, activeCallFlag);
         
    //----------------------------------------
    RMobileCall::TMobileCallParamsV2 callParams2;
    RMobileCall::TMobileCallParamsV2Pckg paramPckg2(callParams2);
    ret = call.GetCallParams(paramPckg2);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg2, activeCallFlag);
      
    //---------------------------------------------
    RMobileCall::TMobileCallParamsV7 callParams7;
    RMobileCall::TMobileCallParamsV7Pckg paramPckg7(callParams7);

    ret = call.GetCallParams(paramPckg7);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateCallParams(&paramPckg7, activeCallFlag);
    //-- data param --------------------------------------
    RMobileCall::TMobileDataCallParamsV1 dataCallParams1;
    RMobileCall::TMobileDataCallParamsV1Pckg dataCallParamPckg1(dataCallParams1);
    
    ret = call.GetCallParams(dataCallParamPckg1);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateDataCallParams(&dataCallParamPckg1, activeCallFlag);
    
    //--------------------------------------------------
    RMobileCall::TMobileDataCallParamsV2 dataCallParams2;
    RMobileCall::TMobileDataCallParamsV2Pckg dataCallParamPckg2(dataCallParams2);
    
    ret = call.GetCallParams(dataCallParamPckg2);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateDataCallParams(&dataCallParamPckg2, activeCallFlag);
    
    //------------------------------------------------------
    RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
    RMobileCall::TMobileDataCallParamsV8Pckg dataCallParamPckg8(dataCallParams8);
    
    ret = call.GetCallParams(dataCallParamPckg8);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateDataCallParams(&dataCallParamPckg8, activeCallFlag);

 	//-- Dial -----------------------------------------------------------------------

    TRequestStatus requestStatus;			    
    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
    RBuf8 completeData;
    CleanupClosePushL(completeData);
		
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
   
    
    RMobileCall::TMobileDataCallParamsV1 recDataCallParams1;
    RMobileCall::TMobileDataCallParamsV1Pckg recDataCallParamPckg1(recDataCallParams1);
        
    // TMobileDataCallParamsV1 params for Dial
    dataCallParams1.iInterval = 2;
    dataCallParams1.iCug.iCugIndex = 0xFFFF;
    dataCallParams1.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    dataCallParams1.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    dataCallParams1.iWaitForDialTone = RCall::EDialToneWait;
    dataCallParams1.iQoS = RMobileCall::EQosTransparentPreferred;
    dataCallParams1.iV42bisReq = RMobileCall::EV42bisTxDirection;
    
    RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KNum);
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	callInfo.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;

	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		dialDataCall(0, mobileService, dataCallParams1, callInfo);
	completeData.Close();
	dialDataCall.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInfo.iDialledParty.iTelNumber);
	
	TMockLtsyCallData0 dialCompleteData(callId,mobileService);
   	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);
	
    call.Dial(requestStatus, dataCallParamPckg1, callInfo.iDialledParty.iTelNumber);
	
    CompleteDialStatusNotificationsL(call, callId, mobileService );
	
    User::WaitForRequest(requestStatus);
   	AssertMockLtsyStatusL(); 
    
    ret = call.GetCallParams(recDataCallParamPckg1);
    ASSERT_EQUALS(KErrNone, ret);
	activeCallFlag = ETrue;
    ValidateDataCallParams(&recDataCallParamPckg1, activeCallFlag);
  
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
    call.Close();
    //------------------------------------------------------------------------    
   
    RMobileCall::TMobileDataCallParamsV2 recDataCallParams2;
    RMobileCall::TMobileDataCallParamsV2Pckg recDataCallParamPckg2(recDataCallParams2);
      
    // TMobileDataCallParamsV2 params for Dial
    dataCallParams2.iInterval = 2;
    dataCallParams2.iCug.iCugIndex = 0xFFFF;
    dataCallParams2.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    dataCallParams2.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    dataCallParams2.iWaitForDialTone = RCall::EDialToneWait;
    dataCallParams2.iQoS = RMobileCall::EQosTransparentPreferred;
    dataCallParams2.iV42bisReq = RMobileCall::EV42bisTxDirection;
    dataCallParams2.iBearerMode = RMobileCall::EMulticallShareBearer;
       
    // open new call
  	CleanupClosePushL(call);
  	TInt errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		dialDataCall2(0, mobileService, dataCallParams2, callInfo);
	completeData.Close();
	dialDataCall2.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInfo.iDialledParty.iTelNumber);
	
	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);
		
	call.Dial(requestStatus, dataCallParamPckg2, callInfo.iDialledParty.iTelNumber);
	
	CompleteDialStatusNotificationsL(call, callId, mobileService );
	
   	User::WaitForRequest(requestStatus);
   	AssertMockLtsyStatusL();
    
   	ret = call.GetCallParams(recDataCallParamPckg2);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateDataCallParams(&recDataCallParamPckg2, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    call.Close();
    
    //--------------------------------------------------------------------
    
    RMobileCall::TMobileDataCallParamsV8 recDataCallParams8;
    RMobileCall::TMobileDataCallParamsV8Pckg recDataCallParamPckg8(recDataCallParams8);
        
    RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KNum);
    callInformation.iService = mobileService;
    callInformation.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;
   
     
    // TMobileDataCallParamsV8 params for Dial
    dataCallParams8.iInterval = 2;
    dataCallParams8.iCug.iCugIndex = 0xFFFF;
    dataCallParams8.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    dataCallParams8.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    dataCallParams8.iWaitForDialTone = RCall::EDialToneWait;
    dataCallParams8.iQoS = RMobileCall::EQosTransparentPreferred;
    dataCallParams8.iV42bisReq = RMobileCall::EV42bisTxDirection;
    dataCallParams8.iBearerMode = RMobileCall::EMulticallShareBearer;
    dataCallParams8.iSubAddress.Copy(_L("An Unknown SubAddress"));
    dataCallParams8.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    dataCallParams8.iAlphaId.Copy(_L("Alpha Id"));

    // open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		dialDataCall8(0, mobileService, dataCallParams8, callInformation);
	completeData.Close();
	dialDataCall8.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService,  callInformation.iDialledParty.iTelNumber);
	
	completeData.Close();
	dialCompleteData.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);
	
	call.Dial(requestStatus, dataCallParamPckg8,  callInformation.iDialledParty.iTelNumber);
	
	CompleteDialStatusNotificationsL(call, callId, mobileService );
	
	User::WaitForRequest(requestStatus);
   	AssertMockLtsyStatusL(); 
        	
    ret = call.GetCallParams(recDataCallParamPckg8);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateDataCallParams(&recDataCallParamPckg8, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
    call.Close();
   
    CleanupStack::PopAndDestroy(6, this); 

    }


/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0006b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams for data calls with HscsdCallParams. It is in connection to BA-CTSY-CALLI-CGCP-0006
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0006bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
       
	OpenLineLC(line, iPhone, lineName);
	
    
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
   
   
    //- using different types of params struct---------------------------------------
    TBool activeCallFlag = EFalse;

		    
    //-- Hscsd param --------------------------------------    
    RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams1;
    RMobileCall::TMobileHscsdCallParamsV1Pckg hscsdCallParamPckg1(hscsdCallParams1);
    TInt ret = call.GetCallParams(hscsdCallParamPckg1);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateHscsdCallParams(&hscsdCallParamPckg1, activeCallFlag);
    //----------------------------------------
    
    RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;
    RMobileCall::TMobileHscsdCallParamsV2Pckg hscsdCallParamPckg2(hscsdCallParams2);

    ret = call.GetCallParams(hscsdCallParamPckg2);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateHscsdCallParams(&hscsdCallParamPckg2, activeCallFlag);
    
    //----------------------------------------
        
    RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
    RMobileCall::TMobileHscsdCallParamsV7Pckg hscsdCallParamPckg7(hscsdCallParams7);

    ret = call.GetCallParams(hscsdCallParamPckg7);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateHscsdCallParams(&hscsdCallParamPckg7, activeCallFlag);
    
    call.Close();
    //-- Dial -----------------------------------------------------------------------

    TRequestStatus requestStatus;			    
    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
    RBuf8 completeData;
    CleanupClosePushL(completeData);
		
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
   
    RMobileCall::TMobileHscsdCallParamsV1 recHscsdCallParams1;
    RMobileCall::TMobileHscsdCallParamsV1Pckg recHscsdCallParamPckg1(recHscsdCallParams1);

    // TMobileHscsdCallParamsV1 params for Dial
    hscsdCallParams1.iInterval = 2;
    hscsdCallParams1.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams1.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams1.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams1.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams1.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams1.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams1.iWantedRxTimeSlots = 2; 
    hscsdCallParams1.iCodings = 3;
    
    RMobileCall::TMobileCallInfoV8 callInfo;
   	callInfo.iDialledParty.iTelNumber.Copy(KNum);
   	callInfo.iService = mobileService;
   	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
   	callInfo.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;
    
    // open new call
   	CleanupClosePushL(call);
   	TInt errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);

    TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		dialHscsdCall(0, mobileService, hscsdCallParams1, callInfo);
	completeData.Close();
	dialHscsdCall.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInfo.iDialledParty.iTelNumber);
	
	TMockLtsyCallData0 dialCompleteData(callId,mobileService);
	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);
	
	call.Dial(requestStatus, hscsdCallParamPckg1, callInfo.iDialledParty.iTelNumber);
    
	CompleteDialStatusNotificationsL(call, callId, mobileService );
	 
   	User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();  
        
    ret = call.GetCallParams(recHscsdCallParamPckg1);
    ASSERT_EQUALS(KErrNone, ret);
    activeCallFlag = ETrue;
    ValidateHscsdCallParams(&recHscsdCallParamPckg1, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
   
    call.Close();
    //-----------------------------------------------------------
    
    RMobileCall::TMobileHscsdCallParamsV2 recHscsdCallParams2;
    RMobileCall::TMobileHscsdCallParamsV2Pckg recHscsdCallParamPckg2(recHscsdCallParams2);
    
    // TMobileHscsdCallParamsV2 params for Dial
    hscsdCallParams2.iInterval = 2;
    hscsdCallParams2.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams2.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams2.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams2.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams2.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams2.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams2.iWantedRxTimeSlots = 2; 
    hscsdCallParams2.iCodings = 3;
    hscsdCallParams2.iBearerMode = RMobileCall::EMulticallShareBearer;
    
    // open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV2, RMobileCall::TMobileCallInfoV8> 
		dialHscsdCall2(0, mobileService, hscsdCallParams2, callInfo);
	completeData.Close();
	dialHscsdCall2.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInfo.iDialledParty.iTelNumber);
	
	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);	
	
	call.Dial(requestStatus, hscsdCallParamPckg2, callInfo.iDialledParty.iTelNumber);
    
	CompleteDialStatusNotificationsL(call, callId, mobileService );
    		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();  
    
    ret = call.GetCallParams(recHscsdCallParamPckg2);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateHscsdCallParams(&recHscsdCallParamPckg2, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    
    call.Close();
    //---------------------------------------------------------------
 
    RMobileCall::TMobileHscsdCallParamsV7 recHscsdCallParams7;
    RMobileCall::TMobileHscsdCallParamsV7Pckg recHscsdCallParamPckg7(recHscsdCallParams7);
    
    // TMobileHscsdCallParamsV7 params for Dial
    hscsdCallParams7.iInterval = 2;
    hscsdCallParams7.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams7.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams7.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams7.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams7.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams7.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams7.iWantedRxTimeSlots = 2; 
    hscsdCallParams7.iCodings = 3;
    hscsdCallParams7.iBearerMode = RMobileCall::EMulticallShareBearer;
    hscsdCallParams7.iAlphaId.Copy(_L("Alpha Id"));
    hscsdCallParams7.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    
    RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KNum);
    callInformation.iService = mobileService;
    callInformation.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;
    
    // open new call
   	CleanupClosePushL(call);
   	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		dialHscsdCall7(0, mobileService, hscsdCallParams7, callInformation);
	completeData.Close();
	dialHscsdCall7.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInfo.iDialledParty.iTelNumber);
	
	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);	
	
	call.Dial(requestStatus, hscsdCallParamPckg7, callInfo.iDialledParty.iTelNumber);
        
	CompleteDialStatusNotificationsL(call, callId, mobileService );
    		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();  
        
    ret = call.GetCallParams(recHscsdCallParamPckg7);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateHscsdCallParams(&recHscsdCallParamPckg7, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    call.Close();
   
    CleanupStack::PopAndDestroy(7, this); 

    }

/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0006c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams for data calls with HscsdCallParamsV8. It is in connection to BA-CTSY-CALLI-CGCP-0006b 
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0006cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
       
	OpenLineLC(line, iPhone, lineName);
	
    
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
   
   
    //- using different types of params struct---------------------------------------
    TBool activeCallFlag = EFalse;
        
    RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
    RMobileCall::TMobileHscsdCallParamsV8Pckg hscsdCallParamPckg8(hscsdCallParams8);

    TInt ret = call.GetCallParams(hscsdCallParamPckg8);
    ASSERT_EQUALS(KErrNone, ret);
    
    ValidateHscsdCallParams(&hscsdCallParamPckg8, activeCallFlag);
    
    call.Close();
    //-- Dial -----------------------------------------------------------------------

    TRequestStatus requestStatus;			    
    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
    RBuf8 completeData;
    CleanupClosePushL(completeData);
		
	TInt callId = 1;
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
   
    // TMobileHscsdCallParamsV8 params for Dial
    hscsdCallParams8.iInterval = 2;
    hscsdCallParams8.iCug.iCugIndex = 0xFFFF;
    hscsdCallParams8.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    hscsdCallParams8.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    hscsdCallParams8.iWaitForDialTone = RCall::EDialToneWait;
    hscsdCallParams8.iQoS = RMobileCall::EQosTransparentPreferred;
    hscsdCallParams8.iV42bisReq = RMobileCall::EV42bisTxDirection;
    hscsdCallParams8.iWantedRxTimeSlots = 2; 
    hscsdCallParams8.iCodings = 3;
    hscsdCallParams8.iBearerMode = RMobileCall::EMulticallShareBearer;
    hscsdCallParams8.iAlphaId.Copy(_L("Alpha Id"));
    hscsdCallParams8.iSubAddress.Copy(_L("An Unknown SubAddress"));
    hscsdCallParams8.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    
    RMobileCall::TMobileCallInfoV8 callInformation;
    callInformation.iDialledParty.iTelNumber.Copy(KNum);
    callInformation.iService = mobileService;
    callInformation.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
    callInformation.iAlphaId.Copy(_L("Alpha Id"));
    callInformation.iValid |= RMobileCall::KCallAlphaId;
    callInformation.iCallParamOrigin = RMobileCall::EOriginatorSIM;
    callInformation.iValid |= RMobileCall::KCallParamOrigin;
    
    RMobileCall::TMobileHscsdCallParamsV8 recHscsdCallParams8;
    RMobileCall::TMobileHscsdCallParamsV8Pckg recHscsdCallParamPckg8(recHscsdCallParams8);
   
    // open new call
   	CleanupClosePushL(call);
   	TInt errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    
    TMockLtsyCallData2<RMobileCall::TMobileHscsdCallParamsV8, RMobileCall::TMobileCallInfoV8> 
		dialHscsdCall8(0, mobileService, hscsdCallParams8, callInformation);
	completeData.Close();
	dialHscsdCall8.SerialiseL(completeData);
	iMockLTSY.ExpectL(EEtelCallDial,completeData);    
	
	CallGetMobileCallInfoL(callId, mobileService, callInformation.iDialledParty.iTelNumber);
	
	TMockLtsyCallData0 dialCompleteData(callId,mobileService);
	completeData.Close();
   	dialCompleteData.SerialiseL(completeData);
   	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,completeData);	
	
	call.Dial(requestStatus, hscsdCallParamPckg8, callInformation.iDialledParty.iTelNumber);
      
	CompleteDialStatusNotificationsL(call, callId, mobileService );
   	    		
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();  
    
    ret = call.GetCallParams(recHscsdCallParamPckg8);
    ASSERT_EQUALS(KErrNone, ret);
    activeCallFlag = ETrue;
    ValidateHscsdCallParams(&recHscsdCallParamPckg8, activeCallFlag);
    
    CompleteCallDisconnectNotificationsL(callId, mobileService);
    call.Close();
   
    CleanupStack::PopAndDestroy(5, this); 

    }


/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams with bad parameters data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams with bad parameters for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams0008L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
       
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
	   
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------

	TBuf8<1> buf(KNullDesC8);
	TInt ret = call.GetCallParams(buf);
	// Passing a small buffer to function doesn't cause an error.
	ASSERT_EQUALS(KErrArgument, ret)
	
	RCall::TCallParams callParams;
    RCall::TCallParamsPckg paramPckg(callParams);
       
    ret = call.GetCallParams(paramPckg);
   	ASSERT_EQUALS(KErrArgument, ret)

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGCP-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallParams for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallParams for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallParams00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);

   
    TBool activeCallFlag = EFalse;  
    //----------------------------------------
    
    RMobileCall::TMobileCallParamsV1 callParams1;
    RMobileCall::TMobileCallParamsV1Pckg paramPckg1(callParams1);
   
    TInt ret = call.GetCallParams(paramPckg1);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg1, activeCallFlag);
        
    //----------------------------------------
    RMobileCall::TMobileCallParamsV2 callParams2;
    RMobileCall::TMobileCallParamsV2Pckg paramPckg2(callParams2);

    ret = call.GetCallParams(paramPckg2);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateCallParams(&paramPckg2, activeCallFlag);
     
    //---------------------------------------------
    RMobileCall::TMobileCallParamsV7 callParams7;
    RMobileCall::TMobileCallParamsV7Pckg paramPckg7(callParams7);

    ret = call.GetCallParams(paramPckg7);

    ValidateCallParams(&paramPckg7, activeCallFlag);

 	//--  now GetCallParams for an incoming dialing call -----

    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
	RCall call2;	
	TInt callId = 1;
	TName incomingCallName;

    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	
	CreateAndOpenIncomingCalLC(line, call2, incomingCallName, 
		lineName, callId, RMobileCall::EStatusIdle, mobileService);
	
 	//---  Dial ---------------------------------------------
    TRequestStatus requestStatus;   

    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
    
    // TMobileCallParamsV1 params for Dial
   	RMobileCall::TMobileCallParamsV1 params;
   	RMobileCall::TMobileCallParamsV1Pckg paramsPckg(params);
    params.iInterval = 2;
    params.iCug.iCugIndex = 0xFFFF;
    params.iSpeakerControl = RCall::EMonitorSpeakerControlAlwaysOff;
    params.iSpeakerVolume = RCall::EMonitorSpeakerVolumeMedium;
    params.iWaitForDialTone = RCall::EDialToneWait;
   
  	RMobileCall::TMobileCallInfoV8 callInfo;
  	callInfo.iDialledParty.iTelNumber.Copy(KNum);
  	callInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown;
  	callInfo.iService = mobileService;
  	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
  	callInfo.iAlternatingCall = RMobilePhone::EAlternatingModeUnspecified;

   	call2.Dial(requestStatus, paramsPckg, callInfo.iDialledParty.iTelNumber);
  	User::WaitForRequest(requestStatus);
  	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
   	
   	CleanupStack::PopAndDestroy(5, this); 
	}



void CCTsyCallInformationFU::AuxGetCallDuration0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RLine line;
    
	OpenLineLC(line, iPhone, aLineName);
	
    TName name;
    RCall call1;
    
    OpenNewCallLC(call1, line, &name);

    //---  check not active call -------------------------------------
    const TTimeIntervalSeconds KZeroTime(0);
    
    TTimeIntervalSeconds time;
    TInt ret = call1.GetCallDuration(time);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(KZeroTime, time);
    
    //--- try to make duration not zero ---------------------------------------
			
	RCall call;	
	TInt callId = 1;
	TName incomingCallName;

    RMobilePhone::TMobileService mobileService = aMobileService;
	
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		aLineName, callId, RMobileCall::EStatusAnswering, mobileService);	
    
 	//-------------------------------------------------------------------------
    RBuf8 completeData;
    CleanupClosePushL(completeData);

    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

	// this is to allow duration > 0
    sleep(2);

    ret = call.GetCallDuration(time);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(KZeroTime != time);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this);
	
	}

/**
@SYMTestCaseID BA-CTSY-CALLI-CGCD-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallDuration for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallDuration for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallDuration0001L()
	{

	AuxGetCallDuration0001L(KMmTsyVoice1LineName, RMobilePhone::EVoiceService);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGCD-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallDuration for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallDuration for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallDuration0006L()
	{

	AuxGetCallDuration0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGCD-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCallDuration for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCallDuration for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallDuration00011L()
	{

	AuxGetCallDuration0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGOS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetOwnershipStatus for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetOwnershipStatus for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetOwnershipStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;

	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	
    RCall::TOwnershipStatus ownershipStatus;
    
	// Ownership is not supported for voice calls, so we just check for UnOwned
    TInt ret = call.GetOwnershipStatus(ownershipStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RCall::EOwnershipUnowned, ownershipStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	
	}



void CCTsyCallInformationFU::AuxGetOwnershipStatus0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RLine line;

	OpenLineLC(line, iPhone, aLineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		aLineName, callId, RMobileCall::EStatusRinging, aMobileService);

    //----------------------------------------

    RCall::TOwnershipStatus ownershipStatus;
    	
	// UnOwned
    TInt ret = call.GetOwnershipStatus(ownershipStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RCall::EOwnershipUnowned, ownershipStatus);

	TRequestStatus requestStatus1;
	
	RMobileCall::TMobileCallParamsV1 callParams;
	RMobileCall::TMobileCallParamsV1Pckg callParamsPckg(callParams);
    call.Connect(requestStatus1,callParamsPckg);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	
	// EOwnershipOwnedByThisClient
    ret = call.GetOwnershipStatus(ownershipStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RCall::EOwnershipOwnedByThisClient, ownershipStatus);
	 
	 
    //- now set ownership to EOwnershipOwnedByAnotherClient -------------------	
	
	RTelServer telServer2;
	ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line2;

	OpenLineLC(line2, phone2, aLineName);

	RCall call2;
	
	// open existing call to send take ownership request
	ret = call2.OpenExistingCall(line2, incomingCallName);
	ASSERT_EQUALS(KErrNone, ret)
	CleanupClosePushL(call2);

	TRequestStatus reqStatus;
	call2.AcquireOwnership(reqStatus);

	// TransferOwnership to call2
	ret = call.TransferOwnership();
	ASSERT_EQUALS(KErrNone, ret)
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	//EOwnershipOwnedByAnotherClient
    ret = call.GetOwnershipStatus(ownershipStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RCall::EOwnershipOwnedByAnotherClient, ownershipStatus);


    //-now set ownership to EOwnershipThisIsPriorityClient -------------------	

	// There must be "etel_KPriorityClientSid <SID>" in epoc.ini (emulator)
	// or "patchdata etel.dll@KPriorityClientSid <SID>" in etel.iby (ROM)
	// If this test fails, ensure that the lines:
	// etel_KPriorityClientSid 0x101F777C
    // etel_KSatEngineSid 0x101F777C
	// are present in \epoc32\data\epoc.ini

   	ret  = iTelServer.SetPriorityClientV2();
	ASSERT_EQUALS(KErrNone, ret);

    ret = call.GetOwnershipStatus(ownershipStatus);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RCall::EOwnershipThisIsPriorityClient, ownershipStatus);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(7, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGOS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetOwnershipStatus for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetOwnershipStatus for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetOwnershipStatus0006L()
	{

	AuxGetOwnershipStatus0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGOS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetOwnershipStatus for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetOwnershipStatus for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetOwnershipStatus00011L()
	{
	
	AuxGetOwnershipStatus0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCaps for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCaps for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCaps0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);

	// get caps of a just created call	
	RCall::TCaps caps;
	TInt ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsDial) == caps.iFlags);
    
	CleanupStack::PopAndDestroy(1);

	// now get caps of an incoming call to ensure that they are changed
	
	TInt callId = 1;
	TName incomingCallName;
			
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

	
	ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsAnswer) == caps.iFlags);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	}

/**
@SYMTestCaseID BA-CTSY-CALLI-CGC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCaps for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCaps for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCaps0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);

	// get caps of a just created call	
	
	RCall::TCaps caps;
	TInt ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsDial | RCall::KCapsConnect) == caps.iFlags);
    
	CleanupStack::PopAndDestroy(1);

	
	// now get caps of an incoming call to ensure that they are changed
	
	TInt callId = 1;
	TName incomingCallName;
			
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::ECircuitDataService);
	
	ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsAnswer | RCall::KCapsHangUp) == caps.iFlags);
	
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetCaps for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetCaps for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCaps00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);

	// get caps of a just created call	
	
	RCall::TCaps caps;
	TInt ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsConnect | RCall::KCapsFax) == caps.iFlags);
    
	CleanupStack::PopAndDestroy(1);

	// now get caps of an incoming call to ensure that they are changed
	
	TInt callId = 1;
	TName incomingCallName;
			
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EFaxService);
	
	ret = call.GetCaps(caps);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE((RCall::KCapsFax | RCall::KCapsAnswer | RCall::KCapsConnect) == caps.iFlags);
	    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	}



void CCTsyCallInformationFU::AuxGetInfo0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(aLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
	
	// get call info for a just created call
	RCall::TCallInfo info;
	TInt ret = call.GetInfo(info);
    ASSERT_EQUALS(KErrNone, ret);
       
    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));
    ASSERT_TRUE(0 == info.iCallName.Compare(name));
    ASSERT_TRUE(RCall::EHookStatusUnknown == info.iHookStatus);
    ASSERT_TRUE(RCall::EStatusIdle == info.iStatus);
    ASSERT_TRUE(0 == info.iDuration.Int());
	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	
	// now get call info for an incoming call
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, aMobileService);
	
	ret = call.GetInfo(info);
    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    ASSERT_TRUE(RCall::EHookStatusUnknown == info.iHookStatus);
    ASSERT_TRUE(RCall::EStatusRinging == info.iStatus);
    ASSERT_TRUE(0 == info.iDuration.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetInfo for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetInfo for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetInfo0001L()
	{
	
	AuxGetInfo0001L(KMmTsyVoice1LineName, RMobilePhone::EVoiceService);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetInfo for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetInfo for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetInfo0006L()
	{
	
	AuxGetInfo0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetInfo for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetInfo for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetInfo00011L()
	{

	AuxGetInfo0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}


void CCTsyCallInformationFU::AuxGetStatus0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(aLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
	
	// get call status for a just created call
	RCall::TStatus status;
	TInt ret = call.GetStatus(status);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_EQUALS(RCall::EStatusIdle, status);

	CleanupStack::PopAndDestroy(1);

	AssertMockLtsyStatusL();
	
	// now get call info for an incoming call
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, aMobileService);
	
	ret = call.GetStatus(status);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_EQUALS(RCall::EStatusRinging, status);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);

	}
/**
@SYMTestCaseID BA-CTSY-CALLI-CGS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetStatus for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetStatus for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetStatus0001L()
	{

	AuxGetStatus0001L(KMmTsyVoice1LineName, RMobilePhone::EVoiceService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetStatus for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetStatus for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetStatus0006L()
	{

	AuxGetInfo0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetStatus for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetStatus for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetStatus00011L()
	{

	AuxGetInfo0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name); 
	
	// get call info for a just created call
	RLine::TCallInfo callInfo;
    TInt ret = line.GetCallInfo( 0, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(name));
    ASSERT_EQUALS(RCall::EStatusIdle, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsDial) == callInfo.iCallCapsFlags);
     
    
	//-- get call info for an incoming call ---------------------	
	RCall call1;
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call1, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);

    ret = line.GetCallInfo( 1, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(incomingCallName));
    ASSERT_EQUALS(RCall::EStatusRinging, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsAnswer) == callInfo.iCallCapsFlags);	
	   
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}


void CCTsyCallInformationFU::AuxGetCallInfo0003L(const TDesC& aLineName)
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RLine line;
    
	OpenLineLC(line, iPhone, aLineName);
	
	// try get info when there is no calls
	RLine::TCallInfo callInfo;
    TInt ret = line.GetCallInfo( 0, callInfo);

    ASSERT_EQUALS(KErrNotFound, ret);

    //----------------------------------------
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name); 

	// try to get info of a call with negative Id	
    ret = line.GetCallInfo( -1, callInfo);

    ASSERT_EQUALS(KErrNotFound, ret);
    
	//----------------------------------------	
	// try to get info of a call a too big Id	
    ret = line.GetCallInfo( 3, callInfo);

    ASSERT_EQUALS(KErrNotFound, ret);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo with bad parameter data for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo with bad parameter data for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo0003L()
	{

	AuxGetCallInfo0003L(KMmTsyVoice1LineName);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name); 
	
	// get call info for a just created call
	RLine::TCallInfo callInfo;
    TInt ret = line.GetCallInfo( 0, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(name));
    ASSERT_EQUALS(RCall::EStatusIdle, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsDial | RCall::KCapsConnect) == callInfo.iCallCapsFlags);
     
    
	//-- get call info for an incoming call ---------------------	
	RCall call1;
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call1, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::ECircuitDataService);

    ret = line.GetCallInfo( 1, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(incomingCallName));
    ASSERT_EQUALS(RCall::EStatusRinging, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsAnswer | RCall::KCapsHangUp) == callInfo.iCallCapsFlags);	
	   
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo with bad parameter data for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo with bad parameter data for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo0008L()
	{
	
	AuxGetCallInfo0003L(KMmTsyDataLineName);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name); 
	
	// get call info for a just created call
	RLine::TCallInfo callInfo;
    TInt ret = line.GetCallInfo( 0, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(name));
    ASSERT_EQUALS(RCall::EStatusIdle, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsFax | RCall::KCapsConnect) == callInfo.iCallCapsFlags);
     
    
	//-- get call info for an incoming call ---------------------	
	RCall call1;
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call1, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EFaxService);

    ret = line.GetCallInfo( 1, callInfo);

    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE(0 == callInfo.iCallName.Compare(incomingCallName));
    ASSERT_EQUALS(RCall::EStatusRinging, callInfo.iStatus);
    ASSERT_TRUE((RCall::KCapsFax | RCall::KCapsAnswer | RCall::KCapsConnect) == callInfo.iCallCapsFlags);	
	   
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LGCI-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::GetCallInfo with bad parameter data for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::GetCallInfo with bad parameter data for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetCallInfo00013L()
	{

	AuxGetCallInfo0003L(KMmTsyFaxLineName);

	}


void CCTsyCallInformationFU::AuxEnumerateCall0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RLine line;
    
	OpenLineLC(line, iPhone, aLineName);
	
	// enumerate when there is no calls
	TInt numCalls;
	TInt ret = line.EnumerateCall(numCalls);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE(0 == numCalls);

    
    TName name;
    RCall call;
    
    OpenNewCallLC(call, line, &name);
	
	// get number of calls when we have one call 
	ret = line.EnumerateCall(numCalls);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE(1 == numCalls);
    
	
	RCall call1;	
	TInt callId = 1;
	TName incomingCallName;

	CreateAndOpenIncomingCalLC(line, call1, incomingCallName, 
		aLineName, callId, RMobileCall::EStatusRinging, aMobileService);	
		
	// check if the number of calls incremented correctly
	ret = line.EnumerateCall(numCalls);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE(2 == numCalls);
	
    call.Close();
    
	// check if the number of calls decremented correctly
	ret = line.EnumerateCall(numCalls);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE(1 == numCalls);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LEC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::EnumerateCall for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestEnumerateCall0001L()
	{

	AuxEnumerateCall0001L(KMmTsyVoice1LineName, RMobilePhone::EVoiceService);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LEC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::EnumerateCall for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestEnumerateCall0006L()
	{

	AuxEnumerateCall0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LEC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::EnumerateCall for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::EnumerateCall for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestEnumerateCall00011L()
	{
    
	AuxEnumerateCall0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus mockLtsyStatus;

			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);
	    
    TRequestStatus notifyStatus;
    RCall::TCaps caps;

	// test C1, completed from AnswerIncomingCall() ---------------------------------	    

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);

    call.NotifyCapsChange( notifyStatus, caps);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsHangUp) == caps.iFlags);

    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	AssertMockLtsyStatusL();

	// test C2, completed from HangUp()  ---------------------------------	   

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
    
    call.NotifyCapsChange( notifyStatus, caps);

    call.HangUp(requestStatus1);

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsVoice) == caps.iFlags);

	// test C3, completed from CompleteHangUp()  ---------------------------------	   
	
    TMockLtsyCallData0 mockData2(callId, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);

    call.NotifyCapsChange( notifyStatus, caps);

    User::WaitForRequest(requestStatus1);

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsDial) == caps.iFlags);
    
    
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    

	// test C4 ---------------------------------	
   
      
    call.NotifyCapsChange( notifyStatus, caps);
        
    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
	RMobileCall::TMobileCallParamsV7 callParams;
	RMobileCall::TMobileCallParamsV7Pckg callParamsPckg(callParams);
    callParams.iInterval = 0;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iSpeakerControl = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	
	callInfo.iDialledParty.iTelNumber.Copy(KNum);
	callInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown;
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
	
	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8> 
		dialData(-1, mobileService, callParams, callInfo);
	data.Close();
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallDial,data);    
	
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = _L("");
	
	call.Dial(requestStatus,callParamsPckg, callInfo.iDialledParty.iTelNumber);

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsHangUp) == caps.iFlags);
    
	// complete Dial
	TMockLtsyCallData0 dialCompleteData(0,mobileService);
	data.Close();
	dialCompleteData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,data,0);	
	       
    User::WaitForRequest(requestStatus);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	
	TInt callId = 1;
	TName incomingCallName;
	RCall call;
	
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EVoiceService);
	    
    TRequestStatus notifyStatus;
    RCall::TCaps caps;
    
    call.NotifyCapsChange( notifyStatus, caps);

    call.NotifyCapsChangeCancel();

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());
	       
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName, 
		lineName2, callId2, RMobileCall::EStatusRinging, mobileService);

    //----------------------------------------
    RCall::TCaps caps;
    RCall::TCaps caps2;
	    
    call.NotifyCapsChange( requestStatus, caps);
    
    //----------------------------------------
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    //----------------------------------------
    TMockLtsyCallData0 mockData02(callId2, mobileService);
    data.Close();
    mockData02.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call2.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    call2.NotifyCapsChange( requestStatus2, caps2);

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId2, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
    

    TMockLtsyCallData0 mockData2(callId2, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);
    call2.HangUp();


    User::WaitForRequest(requestStatus);
    User::WaitForRequest(requestStatus2);
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_TRUE((RCall::KCapsVoice | RCall::KCapsHangUp) == caps.iFlags);
    ASSERT_TRUE((RCall::KCapsVoice) == caps2.iFlags);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus mockLtsyStatus;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);

	    
	//---------------------------------------------------------------------
	//Test C1, completed from AnswerIncomingCall()
	//---------------------------------------------------------------------
    TRequestStatus notifyStatus;
    RCall::TCaps caps;
    
    call.NotifyCapsChange( notifyStatus, caps);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsHangUp) == caps.iFlags);

    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    AssertMockLtsyStatusL();  

	//Test C2, completed from HangUp()

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
   
    call.NotifyCapsChange( notifyStatus, caps);
    
    call.HangUp(requestStatus1);    

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsData) == caps.iFlags);
    
	//Test C3, completed from CompleteHangUp()

    TMockLtsyCallData0 mockData2(callId, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);

    call.NotifyCapsChange( notifyStatus, caps);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsConnect | RCall::KCapsDial) == caps.iFlags);
    
    //---------------------------------	
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


	//---------------------------------------------------------------------
	//Test C4, completed from Dial()
	//---------------------------------------------------------------------
    call.NotifyCapsChange( notifyStatus, caps);
        
    _LIT(KNum, "23467");
    RCall::TTelNumberC num(KNum);
    
	RMobileCall::TMobileDataCallParamsV1 dataCallParams;
	RMobileCall::TMobileDataCallParamsV1Pckg dataCallParamsPckg(dataCallParams);
	dataCallParams.iInterval = 0;
	dataCallParams.iCug.iCugIndex = 0xFFFF;
	dataCallParams.iSpeakerControl = RCall::EMonitorSpeakerControlOnUntilCarrier;
	dataCallParams.iSpeakerVolume = RCall::EMonitorSpeakerVolumeLow;
	dataCallParams.iWaitForDialTone = RCall::EDialToneWait;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	callInfo.iDialledParty.iTelNumber.Copy(KNum);
	callInfo.iRemoteParty.iDirection = RMobileCall::EMobileTerminated;
	callInfo.iService = mobileService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;
		
	TMockLtsyCallData2<RMobileCall::TMobileDataCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		dialData(-1, mobileService, dataCallParams, callInfo);
	data.Close();
	dialData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallDial,data);    
	
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = _L("");

    call.Dial(requestStatus, dataCallParamsPckg, callInfo.iDialledParty.iTelNumber);
    
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsHangUp) == caps.iFlags);

    TMockLtsyCallData0 dialCompleteData(0,mobileService);
	data.Close();
	dialCompleteData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelCallDial,KErrNone,data,0);	
       
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    AssertMockLtsyStatusL();
   
    
    //---------------------------------------------------------------------
	//Test C5, completed from LoanDataPort
	//---------------------------------------------------------------------
    caps.iFlags = 0;
    call.NotifyCapsChange( notifyStatus, caps);

	callId = -1;

    _LIT(KCom, "Com");    
	TFileName csy(KCom);
    TName portName(KCom);    
    RCall::TCommPort port;
    port.iCsy = csy;
    port.iPort = portName;            
    TMockLtsyCallData1<RCall::TCommPort> mockLoanData(callId, mobileService, port);
    data.Close();
    mockLoanData.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);    
    call.LoanDataPort(requestStatus1, port);    
    User::WaitForRequest(requestStatus1);   
     
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());    
  	ASSERT_TRUE( (RCall::KCapsData | RCall::KCapsHangUp | RCall::KCapsRecoverDataPort) == caps.iFlags);
  
  	AssertMockLtsyStatusL();
  	 
  	// Recover Data Port
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);    
    call.RecoverDataPort(); 
    //---------------------------------------    
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    

	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
			
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::ECircuitDataService);
	    
    TRequestStatus notifyStatus;
    RCall::TCaps caps;
    
    call.NotifyCapsChange( notifyStatus, caps);

    call.NotifyCapsChangeCancel();

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());
	
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}




/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange0009L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    

	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, RMobileCall::EStatusRinging, mobileService);

    //----------------------------------------
    RCall::TCaps caps;
    RCall::TCaps caps2;
	    
    call.NotifyCapsChange( requestStatus, caps);
    
    //----------------------------------------
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    //----------------------------------------
    TMockLtsyCallData0 mockData02(callId2, mobileService);
    data.Close();
    mockData02.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call2.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    call2.NotifyCapsChange( requestStatus2, caps2);

	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId2, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
    

    TMockLtsyCallData0 mockData2(callId2, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);
    call2.HangUp();


    User::WaitForRequest(requestStatus);
    User::WaitForRequest(requestStatus2);
    
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_TRUE((RCall::KCapsData | RCall::KCapsHangUp) == caps.iFlags);
    ASSERT_TRUE((RCall::KCapsData) == caps2.iFlags);

	CleanupStack::PopAndDestroy(8, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);
	    
    //---------------------------------------

    TRequestStatus notifyStatus;
    RCall::TCaps caps;

    call.NotifyCapsChange( requestStatus, caps);

    _LIT(KCom, "Com");
    
	TFileName csy(KCom);
    TName portName(KCom);    
    RCall::TCommPort port;
    port.iCsy = csy;
    port.iPort = portName;
        
    TMockLtsyCallData1<RCall::TCommPort> mockData01(callId, mobileService, port);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
    
    call.LoanDataPort(requestStatus1, port);       

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_TRUE( (RCall::KCapsData | 
        RCall::KCapsFax |
            RCall::KCapsConnect |
                RCall::KCapsHangUp |
                    RCall::KCapsAnswer |
                        RCall::KCapsRecoverDataPort) == caps.iFlags);
    //---------------------------------------

	AssertMockLtsyStatusL();
    
	TInt hangUpCause = KErrGsmBusyUserRequest;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileService, 
                                             hangUpCause, 
                                             autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
    
	// test C2, completed from HangUp()
    call.NotifyCapsChange( requestStatus, caps);

    call.HangUp(requestStatus1);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | 
        RCall::KCapsFax |
            RCall::KCapsConnect |
                    RCall::KCapsAnswer) == caps.iFlags);



	// test C3, completed from CompleteHangUp()

    TMockLtsyCallData0 mockData2(callId, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);

    call.NotifyCapsChange( requestStatus, caps);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | 
        RCall::KCapsFax |
            RCall::KCapsConnect |
                    RCall::KCapsDial) == caps.iFlags);
    
    
    //-- loaned port has to be recovered --------
    TMockLtsyCallData1<RCall::TCommPort> mockData01x(callId, mobileService, port);
    data.Close();
    mockData01x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);

    call.RecoverDataPort();       
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange00012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
			
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, RMobilePhone::EFaxService);
	    
    TRequestStatus notifyStatus;
    RCall::TCaps caps;
    
    call.NotifyCapsChange( notifyStatus, caps);

    call.NotifyCapsChangeCancel();

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());
	
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCapsChange00014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);  

	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);
	
    //----------------------------------------
    // create new incoming call
        
	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, RMobileCall::EStatusRinging, mobileService);
	
    //----------------------------------------
    RCall::TCaps caps;
    RCall::TCaps caps2;
	    
   _LIT(KCom, "Com");
    
	TFileName csy(KCom);
    TName portName(KCom);    
    RCall::TCommPort port;
    port.iCsy = csy;
    port.iPort = portName;
        
    TMockLtsyCallData1<RCall::TCommPort> mockData01(callId, mobileService, port);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);

    call.LoanDataPort(requestStatus1, port);       

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
    AssertMockLtsyStatusL(); 
    //---------------------------------------
    
    call.NotifyCapsChange( requestStatus, caps);
    call2.NotifyCapsChange( requestStatus2, caps2);

    TMockLtsyCallData1<RCall::TCommPort> mockData02(callId2, mobileService, port);
    data.Close();
    mockData02.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallLoanDataPort, data);
    
    call2.LoanDataPort(requestStatus1, port);       

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());
    AssertMockLtsyStatusL(); 
    //---------------------------------------
    
    TInt hangUpCause = KErrGsmBusyUserRequest;
    TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2x(callId, mobileService, 
                                                 hangUpCause, 
                                                 autoStChangeDisable);
    data.Close();
    mockData2x.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallHangUp, data);
        
    TMockLtsyCallData0 mockData2(callId, mobileService);
    data.Close();
    mockData2.SerialiseL(data);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, data);
    call.HangUp();

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE((RCall::KCapsData | 
        RCall::KCapsFax |
            RCall::KCapsConnect |
                    RCall::KCapsAnswer) == caps.iFlags);


    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE((RCall::KCapsData | 
        RCall::KCapsFax |
            RCall::KCapsConnect |
                    RCall::KCapsAnswer |
                    	RCall::KCapsHangUp |
                    		RCall::KCapsRecoverDataPort) == caps2.iFlags);
    AssertMockLtsyStatusL();
    //---------------------------------------    

    TMockLtsyCallData1<RCall::TCommPort> mockData03(callId, mobileService, port);
    data.Close();
    mockData03.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);

    call.RecoverDataPort();       
    AssertMockLtsyStatusL();
    
    TMockLtsyCallData1<RCall::TCommPort> mockData04(callId2, mobileService, port);
    data.Close();
    mockData04.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallRecoverDataPort, data);
    
    call2.RecoverDataPort();       
    AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(8, this);

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNHCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyHookChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyHookChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyHookChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);
	
	
    TRequestStatus notifyStatus;
	RCall::THookStatus hookStatus;
	
	call.NotifyHookChange(requestStatus, hookStatus); 
	    
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	// this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNHCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyHookChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyHookChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyHookChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);

	
    TRequestStatus notifyStatus;
	RCall::THookStatus hookStatus;
	call.NotifyHookChange(requestStatus, hookStatus); 
	    
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

	// this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this);
		
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNHCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyHookChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyHookChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyHookChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
			
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, RMobileCall::EStatusRinging, mobileService);
	
    TRequestStatus notifyStatus;
	RCall::THookStatus hookStatus;
	call.NotifyHookChange(requestStatus, hookStatus); 
	
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNotSupported, requestStatus1.Int());
    
	// this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this); // this, data, line, call

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //-- test cases for different status values----------------------------------------------

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    
    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusRinging, statusToGet);

    //--------------------------------------------------    
    mobileCallStatus = RMobileCall::EStatusDialling; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, statusToGet);

    //--------------------------------------------------    

    mobileCallStatus = RMobileCall::EStatusAnswering; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusAnswering, statusToGet);

    //--------------------------------------------------    

    mobileCallStatus = RMobileCall::EStatusConnecting; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusConnecting, statusToGet);

    //--------------------------------------------------    

    mobileCallStatus = RMobileCall::EStatusConnected; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusConnected, statusToGet);

    //--------------------------------------------------    

    mobileCallStatus = RMobileCall::EStatusDisconnecting; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(RCall::EStatusHangingUp == statusToGet);

    //--------------------------------------------------    

    mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband; 
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(RCall::EStatusHangingUp == statusToGet);

	
    // for other status values the CompleteNotifyStatusChange() does not complete request
            	    

 	//- making status Idle + emergencyCalled ----------------------------------------
    mobileCallStatus = RMobileCall::EStatusIdle; 
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
  
    TUint32 duration = 4; // this dummy param will be ignored by mocksy engine validation check	
    TMockLtsyData1<TUint32> ltsyData( duration );
    data.Close();
    ltsyData.SerialiseL(data);
    // when the call state becomes idle, there is update LifeTime param into LTSY:	
    iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // make this for callId > 0
	line.NotifyIncomingCall(requestStatus, incomingCallName);

    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = mobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> idleCallData(callId, mobileService, mobileCallInfo);
    data.Close();
    idleCallData.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    CleanupStack::PopAndDestroy(&call);
    
    //---------------------------------------------------------

    RMobileCall emerCall;

    TInt ret = emerCall.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(emerCall);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TRfStateInfo rfInfo = ERfsStateInfoNormal;
	TMockLtsyData1<TRfStateInfo> mockData1(rfInfo);
	
    data.Close();
    mockData1.SerialiseL(data);
    
	iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	

	RMobileENStore::TEmergencyNumber number(_L("911"));
	TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData(number);
	data.Close();
	numberLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    emerCall.DialEmergencyCall(requestStatus, number);
    
    mobileCallStatus = RMobileCall::EStatusIdle;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockEmCallData(callId, mobileService, mobileCallStatus);
    data.Close();
    mockEmCallData.SerialiseL(data);

    TRequestStatus requestStatus2;
    emerCall.NotifyStatusChange(requestStatus2, statusToGet); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> emerCallData(callId, mobileService, mobileCallStatus);
    data.Close();
    emerCallData.SerialiseL(data);
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrGsmReleaseByUser, requestStatus.Int());
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet);

    AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&emerCall);

	//-- for checking branches with ghost calls, EStatusRinging --------------------------------------------
	callId = 2;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;     
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData1.SerialiseL(data);  

	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);

    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());	
    
    //-------------------------------------------------------------------------------------------------
       
	RMobileCall call2;
    ret = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call2);    
	   
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
       
    RMobileCall::TMobileCallStatus mStatusToGet;
    TRequestStatus mRequestStatus;
    
    call2.NotifyMobileCallStatusChange(mRequestStatus, mStatusToGet); 
    call2.NotifyStatusChange(requestStatus, statusToGet); 
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, statusToGet);

    User::WaitForRequest(mRequestStatus);

    CloseCallL(call2, callId, mobileService);

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(&call2);
    

	//-- EStatusIdle --------------------------------------------
	callId = 3;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData5(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData5.SerialiseL(data);  

	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);

    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	

    RMobileCall call3;
    ret = call3.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData6(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData6.SerialiseL(data);
        
    call3.NotifyMobileCallStatusChange(mRequestStatus, mStatusToGet); 
    call3.NotifyStatusChange(requestStatus, statusToGet); 
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet);

    User::WaitForRequest(mRequestStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(&call3);
    
    //-- EStatusDisconnecting ---------------------------------------------

	callId = 4;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData7(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData7.SerialiseL(data);  

	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    RMobileCall call4;
    ret = call4.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call4);

    mobileCallStatus = RMobileCall::EStatusDisconnecting;    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData8(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData8.SerialiseL(data);
        
    call4.NotifyStatusChange(requestStatus, statusToGet); 
    call4.NotifyMobileCallStatusChange(mRequestStatus, mStatusToGet); 
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusHangingUp, statusToGet);

    User::WaitForRequest(mRequestStatus);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this);	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    
    call.NotifyStatusChangeCancel();

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);    
	
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
    
	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    RCall::TStatus statusToGet;
    RCall::TStatus statusToGet2;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    call2.NotifyStatusChange(requestStatus2, statusToGet2); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusRinging, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet2);

	CleanupStack::PopAndDestroy(8, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
    
    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusRinging, statusToGet);

    //-- test cases for different status values----------------------------------------------
	mobileCallStatus = RMobileCall::EStatusConnecting;
    data.Close();
    mockCallData3.SerialiseL(data);
	    
    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusConnecting, statusToGet);
	    
	mobileCallStatus = RMobileCall::EStatusDialling;
    data.Close();
    mockCallData3.SerialiseL(data);

    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, statusToGet);

	RArray<RMobileCall::TMobileCallStatus> arrayStatus;
	CleanupClosePushL(arrayStatus);
	
	arrayStatus.AppendL(RMobileCall::EStatusUnknown);
	arrayStatus.AppendL(RMobileCall::EStatusReconnectPending);
	arrayStatus.AppendL(RMobileCall::EStatusWaitingAlternatingCallSwitch);
	arrayStatus.AppendL(RMobileCall::EStatusTransferring);
	arrayStatus.AppendL(RMobileCall::EStatusTransferAlerting);
	arrayStatus.AppendL(static_cast<RMobileCall::TMobileCallStatus>(RMobileCall::EStatusTransferAlerting+1));

	for (TInt i = 0; i < arrayStatus.Count(); i++)
		{
		mobileCallStatus = arrayStatus[i];
	    data.Close();
	    mockCallData3.SerialiseL(data);
	    
	    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
	    User::WaitForRequest(mockLtsyStatus);
	    AssertMockLtsyStatusL();
	    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		}

 	//- making status Idle + KErrNone != aResult ----------------------------------------
	mobileCallStatus = RMobileCall::EStatusIdle;
    data.Close();
    mockCallData3.SerialiseL(data);
	    
    call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGeneral, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet);
    
 	//- making status Idle + KErrNone != KErrGsmReleaseByUser ----------------------------

    // make this for callId > 0
	line.NotifyIncomingCall(requestStatus, incomingCallName);

	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = mobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> idleCallData4(callId, mobileService, mobileCallInfo);
    data.Close();
    idleCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    RMobileCall em_call;
    TInt ret = em_call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(em_call);

	mobileCallStatus = RMobileCall::EStatusIdle;
    data.Close();
    mockCallData3.SerialiseL(data);

    em_call.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet);

	//-- for checking branches with ghost calls, EStatusRinging --------------------------------------------
	callId = 2;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;  
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData1.SerialiseL(data);  

	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	RMobileCall call3;
    ret = call3.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
    
    RMobileCall::TMobileCallStatus mStatusToGet;
    TRequestStatus mRequestStatus;
    
    call3.NotifyMobileCallStatusChange(mRequestStatus, mStatusToGet); 
    call3.NotifyStatusChange(requestStatus, statusToGet); 
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, statusToGet);

    User::WaitForRequest(mRequestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8, this); 	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    
    call.NotifyStatusChangeCancel();

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}





/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);


    RCall::TStatus statusToGet;
    RCall::TStatus statusToGet2;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    call2.NotifyStatusChange(requestStatus2, statusToGet2); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusRinging, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet2);


	CleanupStack::PopAndDestroy(8, this); 
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet);


	//-- for checking branches with ghost calls, EStatusRinging --------------------------------------------	
	callId = 1;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;  
    
   	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    data.Close();
    mockCallData1.SerialiseL(data);  

	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	RMobileCall call2;
    TInt ret = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call2);
	
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
    
    RMobileCall::TMobileCallStatus mStatusToGet;
    TRequestStatus mRequestStatus;
    
    call2.NotifyMobileCallStatusChange(mRequestStatus, mStatusToGet); 
    call2.NotifyStatusChange(requestStatus, statusToGet); 
    
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusDialling, statusToGet);

    User::WaitForRequest(mRequestStatus);
    AssertMockLtsyStatusL();
    
	RArray<RMobileCall::TMobileCallStatus> arrayStatus;
	CleanupClosePushL(arrayStatus);
	
	arrayStatus.AppendL(RMobileCall::EStatusUnknown);
	arrayStatus.AppendL(RMobileCall::EStatusReconnectPending);
	arrayStatus.AppendL(RMobileCall::EStatusWaitingAlternatingCallSwitch);
	arrayStatus.AppendL(RMobileCall::EStatusTransferring);
	arrayStatus.AppendL(RMobileCall::EStatusTransferAlerting);
	arrayStatus.AppendL(RMobileCall::EStatusDisconnectingWithInband);
	arrayStatus.AppendL(RMobileCall::EStatusConnecting);
	arrayStatus.AppendL(static_cast<RMobileCall::TMobileCallStatus>(RMobileCall::EStatusTransferAlerting+1));

	for (TInt i = 0; i < arrayStatus.Count(); i++)
		{
		mobileCallStatus = arrayStatus[i];
	    data.Close();
	    mockCallData3.SerialiseL(data);
	    
	    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
	    User::WaitForRequest(mockLtsyStatus);
	    AssertMockLtsyStatusL();
	    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		}
    
 	//- making status Idle + aResult  == KErrGsmReleaseByUser ----------------------------
	mobileCallStatus = RMobileCall::EStatusIdle;
    data.Close();
    mockCallData3.SerialiseL(data);

    call2.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
 	//- making status Idle + aResult != KErrGsmReleaseByUser + aResult != KErrGsmBusyUserRequest  	
    call2.Close();
    
	callId = 2;
	mobileCallStatus = RMobileCall::EStatusRinging;    
	
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;  
    
	line.NotifyCallAdded(requestStatus, incomingCallName);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    data.Close();
    mockCallData1.SerialiseL(data);  
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
    ret = call2.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, ret);
    
	mobileCallStatus = RMobileCall::EStatusIdle;
    data.Close();
    mockCallData3.SerialiseL(data);

    call2.NotifyStatusChange(requestStatus, statusToGet); 

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNotSupported, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this); 
	

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);


    RCall::TStatus statusToGet;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    
    call.NotifyStatusChangeCancel();

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());	
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNSC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyStatusChange00014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
	
	        
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    RCall::TStatus statusToGet;
    RCall::TStatus statusToGet2;
    call.NotifyStatusChange(requestStatus, statusToGet); 
    call2.NotifyStatusChange(requestStatus2, statusToGet2); 

    mobileCallStatus = RMobileCall::EStatusConnected;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RCall::EStatusConnected, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RCall::EStatusIdle, statusToGet2);

	CleanupStack::PopAndDestroy(8, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCallDurationChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCallDurationChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    call.NotifyCallDurationChange(requestStatus, time);


    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCallDurationChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCallDurationChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);


    call.NotifyCallDurationChange(requestStatus, time);

    call.NotifyCallDurationChangeCancel();

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCallDurationChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCallDurationChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
       
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    //----------------------------------------    

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
 	//-------------------------------------------------------------------------
    TTimeIntervalSeconds time;
    TTimeIntervalSeconds time2;
	    
    call.NotifyCallDurationChange(requestStatus, time);
    call2.NotifyCallDurationChange(requestStatus2, time2);
 	

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(KZeroTime != time2);

	CleanupStack::PopAndDestroy(8, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCallDurationChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCallDurationChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    call.NotifyCallDurationChange(requestStatus, time);


    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCallDurationChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCallDurationChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    call.NotifyCallDurationChange(requestStatus, time);

    call.NotifyCallDurationChangeCancel();

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	// Stop the timer by calling NotifyCallDurationChangeCancel and then check that the last call duration returned is accurate.
	call.NotifyCallDurationChange(requestStatus, time);
	sleep(1);
	call.NotifyCallDurationChangeCancel();
	User::WaitForRequest(requestStatus);
	TInt ret = call.GetCallDuration(time);
	ASSERT_EQUALS(KErrNone, ret);
	const TTimeIntervalSeconds KZeroTime(0);
	ASSERT_TRUE(KZeroTime != time);
			
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCallDurationChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCallDurationChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
    
	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
    //----------------------------------------    

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
 	//-------------------------------------------------------------------------
    TTimeIntervalSeconds time;
    TTimeIntervalSeconds time2;
	    
    call.NotifyCallDurationChange(requestStatus, time);
    call2.NotifyCallDurationChange(requestStatus2, time2);
 	

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(KZeroTime != time2);

	CleanupStack::PopAndDestroy(8, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::NotifyCallDurationChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::NotifyCallDurationChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    call.NotifyCallDurationChange(requestStatus, time);


    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::NotifyCallDurationChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::NotifyCallDurationChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    call.NotifyCallDurationChange(requestStatus, time);

    call.NotifyCallDurationChangeCancel();

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
 	//-------------------------------------------------------------------------

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCDCC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::NotifyCallDurationChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::NotifyCallDurationChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallDurationChange00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RCall call;
	RCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
 	//-------------------------------------------------------------------------
    TTimeIntervalSeconds time;
    TTimeIntervalSeconds time2;
	    
    call.NotifyCallDurationChange(requestStatus, time);
    call2.NotifyCallDurationChange(requestStatus2, time2);
 	

    const TTimeIntervalSeconds KZeroTime(0);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(KZeroTime != time);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(KZeroTime != time2);

	CleanupStack::PopAndDestroy(8, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyCallAdded for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyCallAdded for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);  
	
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			

    TName tmpname;
	RCall tmpcall;

    TRequestStatus mockLtsyStatus;

    // test C1, complete called from CMmVoiceLineTsy::OpenNewObjectL()
	TName callName;
	line.NotifyCallAdded(requestStatus, callName);

   	OpenNewCallLC(tmpcall, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));


	//test C2, complete called from CMmLineTsy::CompleteNotifyAddBypassingCall()
	line.NotifyCallAdded(requestStatus, callName);
	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
	   
	RMobileCall::TMobileCallInfoV1 callInfo;
	
	callInfo.iStatus = RMobileCall::EStatusDialling;
	callInfo.iService = mobileService; 
		
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
	completeData.Close();
	callInfoData.SerialiseL(completeData);	

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo,KErrNone,completeData,0);
    User::WaitForRequest(mockLtsyStatus);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    TName callName3(lineName);
    callName3.Append('3');
    ASSERT_TRUE(0 == callName.Compare(callName3));
    
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCallAdded for voice lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCallAdded for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    TName tmpname;
	RCall tmpcall;

    TRequestStatus mockLtsyStatus;

	TName callName;
	line.NotifyCallAdded(requestStatus, callName);
	
	line.NotifyCallAddedCancel();

   	OpenNewCallLC(tmpcall, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyCallAdded for voice lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyCallAdded for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    //----------------------------------------
    
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	TName callName;
	TName callName2;
	
	line.NotifyCallAdded(requestStatus, callName);
	line2.NotifyCallAdded(requestStatus2, callName2);

 	RCall call;
    TName tmpname;
	
   	OpenNewCallLC(call, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == callName2.Compare(tmpname));
    

	CleanupStack::PopAndDestroy(6, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyCallAdded for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyCallAdded for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			

    TName tmpname;
	RCall tmpcall;

    TRequestStatus mockLtsyStatus;

    // test C1, complete called from CMmDataLineTsy::OpenNewObjectL()
	TName callName;
	line.NotifyCallAdded(requestStatus, callName);

   	OpenNewCallLC(call, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));

	//test C2, complete called from CMmLineTsy::CompleteNotifyAddBypassingCall()
	line.NotifyCallAdded(requestStatus, callName);
	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	   
	RMobileCall::TMobileCallInfoV1 callInfo;
	
	callInfo.iStatus = RMobileCall::EStatusDialling;
	callInfo.iService = mobileService; 
		
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
	completeData.Close();
	callInfoData.SerialiseL(completeData);	

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo,KErrNone,completeData,0);
    User::WaitForRequest(mockLtsyStatus);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    TName callName3(lineName);
    callName3.Append('3');
    ASSERT_TRUE(0 == callName.Compare(callName3));

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCallAdded for data lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCallAdded for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    TName tmpname;
	RCall tmpcall;

	TName callName;
	line.NotifyCallAdded(requestStatus, callName);
	
	line.NotifyCallAddedCancel();

   	OpenNewCallLC(tmpcall, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyCallAdded for data lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyCallAdded for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    //----------------------------------------
    
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	TName callName;
	TName callName2;
	
	line.NotifyCallAdded(requestStatus, callName);
	line2.NotifyCallAdded(requestStatus2, callName2);

 	RCall call;
    TName tmpname;
	
   	OpenNewCallLC(call, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == callName2.Compare(tmpname));    

	CleanupStack::PopAndDestroy(7, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyCallAdded for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyCallAdded for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    TName tmpname;
	RCall tmpcall;

    TRequestStatus mockLtsyStatus;

    // test C1, complete called from CMmFaxLineTsy::OpenNewObjectL()
	TName callName;
	line.NotifyCallAdded(requestStatus, callName);

   	OpenNewCallLC(tmpcall, line, &tmpname);
   	
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));

 	AssertMockLtsyStatusL();
 	
	//test C2, complete called from CMmLineTsy::CompleteNotifyAddBypassingCall()
	line.NotifyCallAdded(requestStatus, callName);
	
	TInt callId = 1;
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	   
	RMobileCall::TMobileCallInfoV1 callInfo;
	
	callInfo.iStatus = RMobileCall::EStatusDialling;
	callInfo.iService = mobileService; 
		
	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
	completeData.Close();
	callInfoData.SerialiseL(completeData);	

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo,KErrNone,completeData,0);
    User::WaitForRequest(mockLtsyStatus);
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    TName callName3(lineName);
    callName3.Append('3');
    ASSERT_TRUE(0 == callName.Compare(callName3));
 	
 	
	CleanupStack::PopAndDestroy(4, this); 
	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyCallAdded for fax lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyCallAdded for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
  
    TTimeIntervalSeconds time;
    
	RCall call;
	
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    TName tmpname;
	RCall tmpcall;

    TRequestStatus mockLtsyStatus;

	TName callName;
	line.NotifyCallAdded(requestStatus, callName);
	
	line.NotifyCallAddedCancel();

   	OpenNewCallLC(tmpcall, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNCA-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyCallAdded for fax lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyCallAdded for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallAdded00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    //----------------------------------------
    
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;

	TName callName;
	TName callName2;
	
	line.NotifyCallAdded(requestStatus, callName);
	line2.NotifyCallAdded(requestStatus2, callName2);

 	RCall call;
    TName tmpname;
	
   	OpenNewCallLC(call, line, &tmpname);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == callName.Compare(tmpname));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == callName2.Compare(tmpname));
    
	CleanupStack::PopAndDestroy(7, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyIncomingCall for voice lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyIncomingCall for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EVoiceService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EVoiceService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Voice11");

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrNone, requestNotify.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyIncomingCall for voice lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyIncomingCall for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);

	line.CancelAsyncRequest(EEtelLineNotifyIncomingCall);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EVoiceService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EVoiceService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrCancel, requestNotify.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyIncomingCall for voice lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyIncomingCall for voice lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);


      
	TInt callId = 1;
	TInt callId2= 2;
	
	TName incomingCallName;
	TName incomingCallName2;
	
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestStatus, incomingCallName);
	line2.NotifyIncomingCall(requestStatus2, incomingCallName2);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EVoiceService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EVoiceService, mobileCallInfo);
    mockCallData1.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId2, RMobilePhone::EVoiceService, mobileCallInfo);
    data.Close();
    mockCallData2.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Voice11");

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == incomingCallName2.Compare(KName));    
    
	CleanupStack::PopAndDestroy(6, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyIncomingCall for data lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyIncomingCall for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
	    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::ECircuitDataService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Data1");

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrNone, requestNotify.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyIncomingCall for data lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyIncomingCall for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);

	line.CancelAsyncRequest(EEtelLineNotifyIncomingCall);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::ECircuitDataService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrCancel, requestNotify.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 		
	
	}




/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyIncomingCall for data lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyIncomingCall for data lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	
    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
	
	    
	TInt callId = 1;
	TInt callId2= 2;
	
	TName incomingCallName;
	TName incomingCallName2;
	
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestStatus, incomingCallName);
	line2.NotifyIncomingCall(requestStatus2, incomingCallName2);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::ECircuitDataService, mobileCallInfo);
    mockCallData1.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId2, RMobilePhone::ECircuitDataService, mobileCallInfo);
    data.Close();
    mockCallData2.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Data1");

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == incomingCallName2.Compare(KName));    
    
	CleanupStack::PopAndDestroy(6, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyIncomingCall for fax lines
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyIncomingCall for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EFaxService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EFaxService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Fax1");

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrNone, requestNotify.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RLine::NotifyIncomingCall for fax lines
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RLine::NotifyIncomingCall for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestNotify, incomingCallName);

	line.CancelAsyncRequest(EEtelLineNotifyIncomingCall);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EFaxService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EFaxService, mobileCallInfo);
    completeData.Close();
    mockCallData1.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestNotify);
    ASSERT_EQUALS(KErrCancel, requestNotify.Int());

 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNIC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RLine::NotifyIncomingCall for fax lines
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RLine::NotifyIncomingCall for fax lines
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCall00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    //----------------------------------------
    
	TInt callId = 1;
	TInt callId2= 2;
	
	TName incomingCallName;
	TName incomingCallName2;
	
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus requestNotify;
			
    TRequestStatus mockLtsyStatus;

	line.NotifyIncomingCall(requestStatus, incomingCallName);
	line2.NotifyIncomingCall(requestStatus2, incomingCallName2);
	
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iService = RMobilePhone::EFaxService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, RMobilePhone::EFaxService, mobileCallInfo);
    mockCallData1.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId2, RMobilePhone::EFaxService, mobileCallInfo);
    data.Close();
    mockCallData2.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    _LIT(KName, "Fax1");

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0 == incomingCallName.Compare(KName));

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0 == incomingCallName2.Compare(KName));
  
    
	CleanupStack::PopAndDestroy(6, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyCallEvent for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyCallEvent for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
	    
    //----------------------------------------
	RMobileCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallEvent ev;

    //----------------------------------------
    call.NotifyCallEvent(requestStatus, ev);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev);
    
    //----------------------------------------
    ev1 = RMobileCall::ELocalHold;
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId, mobileService, ev1);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    //----------------------------------------

    call.NotifyCallEvent(requestStatus, ev);
    
    mobileCallStatus = RMobileCall::EStatusHold;

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData5(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData5.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ELocalHold, ev);

    //----------------------------------------

    call.NotifyCallEvent(requestStatus, ev);
    
    mobileCallStatus = RMobileCall::EStatusConnected;

    data.Close();
    mockCallData5.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ELocalResume, ev);
	
    //----------------------------------------
    // see CMmCallTsy::CompleteNotifyRemoteCallEvent in CMmCallTsy.cpp

	mobileService = RMobilePhone::EVoiceService;
	
	RArray<RMobileCall::TMobileCallEvent> arrayRemote;
	CleanupClosePushL(arrayRemote);

	
	// complete is called for these events from CMmCallTsy::CompleteNotifyRemoteCallEvent
	arrayRemote.AppendL(RMobileCall::ERemoteHold);
	arrayRemote.AppendL(RMobileCall::ERemoteResume);
	arrayRemote.AppendL(RMobileCall::ERemoteTerminated);
	arrayRemote.AppendL(RMobileCall::ERemoteConferenceCreate);
	// and these are ingnored in CompleteNotifyRemoteCallEvent
	arrayRemote.AppendL(RMobileCall::ELocalHold);
	arrayRemote.AppendL(RMobileCall::ELocalResume);
	arrayRemote.AppendL(RMobileCall::ELocalDeflectCall);
	arrayRemote.AppendL(RMobileCall::ELocalTransfer);
	arrayRemote.AppendL(RMobileCall::ERemoteTransferring);
	arrayRemote.AppendL(RMobileCall::ERemoteTransferAlerting);
	arrayRemote.AppendL(RMobileCall::ERemoteBarred);
	arrayRemote.AppendL(RMobileCall::ERemoteForwarding);
	arrayRemote.AppendL(RMobileCall::ERemoteWaiting);
	arrayRemote.AppendL(RMobileCall::ELocalBarred);
	arrayRemote.AppendL(static_cast<RMobileCall::TMobileCallEvent>(RMobileCall::ELocalBarred+1));

	for (TInt i = 0; i < 4; i++)
		{
	    RMobileCall::TMobileCallEvent evTemp(arrayRemote[i]);
	    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallDataTemp(callId, mobileService, evTemp);
	    data.Close();
	    mockCallDataTemp.SerialiseL(data);

	    call.NotifyCallEvent(requestStatus, ev);

	    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
	    User::WaitForRequest(mockLtsyStatus);
	    AssertMockLtsyStatusL();
	    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	    User::WaitForRequest(requestStatus);
	    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	    ASSERT_EQUALS(evTemp, ev);
		}
		
	for (TInt i = 4; i < arrayRemote.Count(); i++)
		{
	    RMobileCall::TMobileCallEvent evTemp(arrayRemote[i]);
	    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallDataTemp(callId, mobileService, evTemp);
	    data.Close();
	    mockCallDataTemp.SerialiseL(data);

	    iMockLTSY.NotifyTerminated(mockLtsyStatus);
	    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
	    User::WaitForRequest(mockLtsyStatus);
	    AssertMockLtsyStatusL();
	    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
		}

    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyCallEvent for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyCallEvent for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    //----------------------------------------
    
    RMobileCall::TMobileCallEvent ev;

    call.NotifyCallEvent(requestStatus, ev);
    call.CancelAsyncRequest(EMobileCallNotifyCallEvent);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyCallEvent for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyCallEvent for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);

    
    
	RMobileCall call;
	RMobileCall call2;
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);	

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
			
    //----------------------------------------    
    RMobileCall::TMobileCallEvent ev;
    RMobileCall::TMobileCallEvent ev2;

    call.NotifyCallEvent(requestStatus, ev);
    call2.NotifyCallEvent(requestStatus2, ev2);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    RMobileCall::TMobileCallEvent ev11(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId2, mobileService, ev11);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteHold, ev);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev2);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyCallEvent for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyCallEvent for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    //----------------------------------------
    RMobileCall::TMobileCallEvent ev;

    call.NotifyCallEvent(requestStatus, ev);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev);
    
    //----------------------------------------
    ev1 = RMobileCall::ELocalHold;
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId, mobileService, ev1);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyCallEvent for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyCallEvent for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    //----------------------------------------
    RMobileCall::TMobileCallEvent ev;

    call.NotifyCallEvent(requestStatus, ev);
    call.CancelAsyncRequest(EMobileCallNotifyCallEvent);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
       
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
		
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyCallEvent for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyCallEvent for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
	
    //----------------------------------------    
    RMobileCall::TMobileCallEvent ev;
    RMobileCall::TMobileCallEvent ev2;

    call.NotifyCallEvent(requestStatus, ev);
    call2.NotifyCallEvent(requestStatus2, ev2);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    RMobileCall::TMobileCallEvent ev11(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId2, mobileService, ev11);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteHold, ev);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev2);    


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyCallEvent for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyCallEvent for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    //----------------------------------------
    RMobileCall::TMobileCallEvent ev;

    call.NotifyCallEvent(requestStatus, ev);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev);
    
    //----------------------------------------
    ev1 = RMobileCall::ELocalHold;
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId, mobileService, ev1);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyCallEvent for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyCallEvent for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //----------------------------------------
    RMobileCall::TMobileCallEvent ev;

    call.NotifyCallEvent(requestStatus, ev);
    call.CancelAsyncRequest(EMobileCallNotifyCallEvent);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNCE-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyCallEvent for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyCallEvent for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyCallEvent00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RMobileCall call;
	RMobileCall call2;	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
	
    //----------------------------------------    
    RMobileCall::TMobileCallEvent ev;
    RMobileCall::TMobileCallEvent ev2;

    call.NotifyCallEvent(requestStatus, ev);
    call2.NotifyCallEvent(requestStatus2, ev2);

    RMobileCall::TMobileCallEvent ev1(RMobileCall::ERemoteHold);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData3(callId, mobileService, ev1);
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    RMobileCall::TMobileCallEvent ev11(RMobileCall::ERemoteTerminated);
    TMockLtsyCallData1<RMobileCall::TMobileCallEvent> mockCallData4(callId2, mobileService, ev11);
    data.Close();
    mockCallData4.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyCallEvent, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteHold, ev);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::ERemoteTerminated, ev2);
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
    OpenNewCallLC(call1, line, &name);
    
    RMobileCall::TMobileCallCapsV1 caps;
    RMobileCall::TMobileCallCapsV1Pckg capsPckg(caps);
    
    TInt ret = call1.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsDial) == caps.iCallControlCaps);
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    ret = call.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsAnswer |  RMobileCall::KCapsHangUp) == caps.iCallControlCaps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;

   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badCaps;
    TPckg<TInt8> badPckg(badCaps);
    
    TInt ret = call1.GetMobileCallCaps(badPckg);        
    ASSERT_TRUE(KErrArgument == ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
    
   	OpenNewCallLC(call1, line, &name);
    
    RMobileCall::TMobileCallCapsV1 caps;
    RMobileCall::TMobileCallCapsV1Pckg capsPckg(caps);
    
    TInt ret = call1.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | RMobileCall::KCapsDial | RMobileCall::KCapsConnect) == caps.iCallControlCaps);
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    ret = call.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | RMobileCall::KCapsAnswer |  RMobileCall::KCapsHangUp) == caps.iCallControlCaps);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
    
   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badCaps;
    TPckg<TInt8> badPckg(badCaps);
    
    TInt ret = call1.GetMobileCallCaps(badPckg);
    
    ASSERT_TRUE(KErrArgument == ret);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 

	}




/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps00011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
	TName name;
    
   	OpenNewCallLC(call1, line, &name);
    
    RMobileCall::TMobileCallCapsV1 caps;
    RMobileCall::TMobileCallCapsV1Pckg capsPckg(caps);
    
    TInt ret = call1.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | RMobileCall::KCapsDial) == caps.iCallControlCaps);
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

 	//-------------------------------------------------------------------------

    mobileCallStatus = RMobileCall::EStatusDisconnecting;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    ret = call.GetMobileCallCaps(capsPckg);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_TRUE(0xFF == caps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax  | RMobileCall::KCapsAnswer |  RMobileCall::KCapsHangUp) == caps.iCallControlCaps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCC-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallCaps with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallCaps with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallCaps00013L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badCaps;
    TPckg<TInt8> badPckg(badCaps);
    
    TInt ret = call1.GetMobileCallCaps(badPckg);
    
    ASSERT_TRUE(KErrArgument == ret);
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    RMobileCall::TMobileCallInfoV1 info;
    RMobileCall::TMobileCallInfoV1Pckg  infoPckg(info);

    _LIT(KEmpty, "");
    const TDateTime KZeroTime(0,TMonth(0),0,0,0,0,0);
    
    TInt ret = call1.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE((RMobileCall::KCallStartTime | 
        RMobileCall::KCallDuration | 
            RMobileCall::KCallId | 
                RMobileCall::KCallExitCode | 
                    RMobileCall::KCallEmergency) == info.iValid);
    ASSERT_TRUE(RMobileCall::EServiceUnspecified == info.iService);
    ASSERT_TRUE(RMobileCall::EStatusIdle == info.iStatus);
    ASSERT_TRUE(0 == info.iCallId);
    ASSERT_TRUE(-1 == info.iExitCode);
    ASSERT_TRUE(0 == info.iEmergency);
    ASSERT_TRUE(0 == info.iForwarded);
    ASSERT_TRUE(RMobilePhone::EPrivacyUnspecified == info.iPrivacy);     
    ASSERT_TRUE(RMobilePhone::EAlternatingModeUnspecified == info.iAlternatingCall);
    ASSERT_TRUE(RMobileCall::ETchUnknown == info.iTch);
    ASSERT_TRUE(RMobileCall::ERemoteIdentityUnknown == info.iRemoteParty.iRemoteIdStatus);
    ASSERT_TRUE(RMobileCall::EDirectionUnknown == info.iRemoteParty.iDirection);
    ASSERT_TRUE(0 == info.iRemoteParty.iCallingName.Compare(KEmpty));
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumber == info.iDialledParty.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumberingPlan == info.iDialledParty.iNumberPlan);
    ASSERT_TRUE(0 == info.iDialledParty.iTelNumber.Compare(KEmpty));
    ASSERT_TRUE(0 == memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE(0 == info.iCallName.Compare(name));
    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));

    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
    mobileCallStatus = RMobileCall::EStatusConnected;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
 	//-----------------------------------------------------------------------

    ret = call.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE((RMobileCall::KCallStartTime | 
        RMobileCall::KCallDuration | 
            RMobileCall::KCallId | 
                RMobileCall::KCallExitCode | 
                    RMobileCall::KCallEmergency) == info.iValid);
    ASSERT_TRUE(RMobilePhone::EVoiceService == info.iService);
    ASSERT_TRUE(RMobileCall::EStatusConnected == info.iStatus);
    ASSERT_TRUE(1 == info.iCallId);
    ASSERT_TRUE(0 == info.iExitCode);
    ASSERT_TRUE(0 == info.iEmergency);
    ASSERT_TRUE(0 == info.iForwarded);
    ASSERT_TRUE(RMobilePhone::EPrivacyUnspecified == info.iPrivacy);     
    ASSERT_TRUE(RMobilePhone::EAlternatingModeUnspecified == info.iAlternatingCall);
    ASSERT_TRUE(RMobileCall::ETchUnknown == info.iTch);
    ASSERT_TRUE(RMobileCall::ERemoteIdentityUnknown == info.iRemoteParty.iRemoteIdStatus);
    ASSERT_TRUE(RMobileCall::EMobileTerminated == info.iRemoteParty.iDirection);
    ASSERT_TRUE(0 == info.iRemoteParty.iCallingName.Compare(KEmpty));
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumber == info.iDialledParty.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumberingPlan == info.iDialledParty.iNumberPlan);
    ASSERT_TRUE(0 == info.iDialledParty.iTelNumber.Compare(KEmpty));
    ASSERT_TRUE(0 == memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badInfo;
    TPckg<TInt8> badPckg(badInfo);
    
    TInt ret = call1.GetMobileCallInfo(badPckg);
    
    ASSERT_TRUE(KErrArgument == ret);        

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);   //call1, line, this 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    TBool activeCall = EFalse;
   	
    RMobileCall::TMobileCallInfoV1 info;
    RMobileCall::TMobileCallInfoV1Pckg infoPckg(info);

    const TDateTime KZeroTime(0,TMonth(0),0,0,0,0,0);
    
    TInt ret = call1.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);
     
    ValidateMobileCallInfo(&infoPckg, name, lineName, activeCall);
   
    // For TMobileCallInfoV3
    RMobileCall::TMobileCallInfoV3 info3;
    RMobileCall::TMobileCallInfoV3Pckg  infoPckg3(info3);

    ret = call1.GetMobileCallInfo(infoPckg3);
    ASSERT_EQUALS(KErrNone, ret);
 
    ValidateMobileCallInfo(&infoPckg3,name, lineName, activeCall);
    
    // For TMobileCallInfoV7
    RMobileCall::TMobileCallInfoV7 info7;
    RMobileCall::TMobileCallInfoV7Pckg  infoPckg7(info7);

    ret = call1.GetMobileCallInfo(infoPckg7);
    ASSERT_EQUALS(KErrNone, ret);
 
    ValidateMobileCallInfo(&infoPckg7,name, lineName, activeCall);
      
    //---  check after activating call -------------------------------------
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    activeCall = ETrue;
    
    mobileCallStatus = RMobileCall::EStatusAnswering;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

 	//-----------------------------------------------------------------------
     
    ret = call.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateMobileCallInfo(&infoPckg, name, lineName, activeCall, RMobileCall::EStatusAnswering);
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    
    // For TMobileCallinfoV3
    ret = call.GetMobileCallInfo(infoPckg3);
    ASSERT_EQUALS(KErrNone, ret);
    ValidateMobileCallInfo(&infoPckg3, name, lineName, activeCall, RMobileCall::EStatusAnswering);
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    
    // For TMobileCallinfoV7
    ret = call.GetMobileCallInfo(infoPckg7);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateMobileCallInfo(&infoPckg7, name, lineName, activeCall, RMobileCall::EStatusAnswering);
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    
    // iStatus can be RMobileCall::EStatusAnswering or RMobileCall::EStatusConnected depending on the network answer.
    ASSERT_TRUE(RMobileCall::EStatusAnswering == info3.iStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
		
	}

/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-0006b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo for data calls with MobileCallInfoV8. It is in connection to BA-CTSY-CALLI-CGMCI-0006.
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo0006bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    TBool activeCall = EFalse;
   	
    RMobileCall::TMobileCallInfoV8 info8;
    RMobileCall::TMobileCallInfoV8Pckg  infoPckg8(info8);
   
    TInt ret = call1.GetMobileCallInfo(infoPckg8);
    ASSERT_EQUALS(KErrNone, ret);
 
    ValidateMobileCallInfo(&infoPckg8,name, lineName, activeCall);

    //---  check after activating call -------------------------------------
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
       
	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    activeCall = ETrue;

 	//-----------------------------------------------------------------------

    ret = call.GetMobileCallInfo(infoPckg8);
    ASSERT_EQUALS(KErrNone, ret);

    ValidateMobileCallInfo(&infoPckg8, name, lineName, activeCall);
    ASSERT_TRUE(0 == info8.iCallName.Compare(incomingCallName));
                 
   	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
		
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badInfo;
    TPckg<TInt8> badPckg(badInfo);
    
    TInt ret = call1.GetMobileCallInfo(badPckg);
    
    ASSERT_TRUE(KErrArgument == ret);
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);   // call1, line, this

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    RMobileCall::TMobileCallInfoV1 info;
    RMobileCall::TMobileCallInfoV1Pckg  infoPckg(info);

    _LIT(KEmpty, "");
    const TDateTime KZeroTime(0,TMonth(0),0,0,0,0,0);
    
    TInt ret = call1.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE((RMobileCall::KCallStartTime | 
        RMobileCall::KCallDuration | 
            RMobileCall::KCallId | 
                RMobileCall::KCallExitCode | 
                    RMobileCall::KCallEmergency) == info.iValid);
    ASSERT_TRUE(RMobileCall::EServiceUnspecified == info.iService);
    ASSERT_TRUE(RMobileCall::EStatusIdle == info.iStatus);
    ASSERT_TRUE(0 == info.iCallId);
    ASSERT_TRUE(-1 == info.iExitCode);
    ASSERT_TRUE(0 == info.iEmergency);
    ASSERT_TRUE(0 == info.iForwarded);
    ASSERT_TRUE(RMobilePhone::EPrivacyUnspecified == info.iPrivacy);     
    ASSERT_TRUE(RMobilePhone::EAlternatingModeUnspecified == info.iAlternatingCall);
    ASSERT_TRUE(RMobileCall::ETchUnknown == info.iTch);
    ASSERT_TRUE(RMobileCall::ERemoteIdentityUnknown == info.iRemoteParty.iRemoteIdStatus);
    ASSERT_TRUE(RMobileCall::EDirectionUnknown == info.iRemoteParty.iDirection);
    ASSERT_TRUE(0 == info.iRemoteParty.iCallingName.Compare(KEmpty));
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumber == info.iDialledParty.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumberingPlan == info.iDialledParty.iNumberPlan);
    ASSERT_TRUE(0 == info.iDialledParty.iTelNumber.Compare(KEmpty));
    ASSERT_TRUE(0 == memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE(0 == info.iCallName.Compare(name));
    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));

    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

 	//-----------------------------------------------------------------------

    ret = call.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);

    ASSERT_TRUE((RMobileCall::KCallStartTime | 
        RMobileCall::KCallDuration | 
            RMobileCall::KCallId | 
                RMobileCall::KCallExitCode | 
                    RMobileCall::KCallEmergency) == info.iValid);
    ASSERT_TRUE(RMobilePhone::EFaxService == info.iService);
    ASSERT_TRUE(RMobileCall::EStatusRinging == info.iStatus);
    ASSERT_TRUE(1 == info.iCallId);
    ASSERT_TRUE(-1 == info.iExitCode);
    ASSERT_TRUE(0 == info.iEmergency);
    ASSERT_TRUE(0 == info.iForwarded);
    ASSERT_TRUE(RMobilePhone::EPrivacyUnspecified == info.iPrivacy);     
    ASSERT_TRUE(RMobilePhone::EAlternatingModeUnspecified == info.iAlternatingCall);
    ASSERT_TRUE(RMobileCall::ETchUnknown == info.iTch);
    ASSERT_TRUE(RMobileCall::ERemoteIdentityUnknown == info.iRemoteParty.iRemoteIdStatus);
    ASSERT_TRUE(RMobileCall::EMobileTerminated == info.iRemoteParty.iDirection);
    ASSERT_TRUE(0 == info.iRemoteParty.iCallingName.Compare(KEmpty));
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumber == info.iDialledParty.iTypeOfNumber);
    ASSERT_TRUE(RMobilePhone::EUnknownNumberingPlan == info.iDialledParty.iNumberPlan);
    ASSERT_TRUE(0 == info.iDialledParty.iTelNumber.Compare(KEmpty));
    ASSERT_TRUE(0 == memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
    ASSERT_TRUE(0 == info.iCallName.Compare(incomingCallName));
    ASSERT_TRUE(0 == info.iLineName.Compare(lineName));
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCI-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallInfo with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallInfo with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallInfo00013L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TInt8 badInfo;
    TPckg<TInt8> badPckg(badInfo);
    
    TInt ret = call1.GetMobileCallInfo(badPckg);
    
    ASSERT_TRUE(KErrArgument == ret);
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this);   // call1, line, this
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallStatus for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallStatus for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    

    RMobileCall::TMobileCallStatus statusToGet;
    
    TInt ret = call1.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet);
    
    RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    mobileCallStatus = RMobileCall::EStatusAnswering;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    ret = call.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);

	//This iStatus can be RMobileCall::EStatusAnswering or RMobileCall::EStatusConnected depending on the network answer.
	ASSERT_EQUALS(RMobileCall::EStatusAnswering, statusToGet);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCS-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallStatus for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallStatus for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallStatus0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);

    RMobileCall::TMobileCallStatus statusToGet;
    
    TInt ret = call1.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet);
    
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus, mmParamsPckgV1);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
    mobileCallStatus = RMobileCall::EStatusAnswering;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(callId, mobileService, mobileCallStatus);
    data.Close();
    mockCallData.SerialiseL(data);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    ret = call.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);

    //This iStatus can be RMobileCall::EStatusAnswering or RMobileCall::EStatusConnected depending on the network answer.	
	ASSERT_EQUALS(RMobileCall::EStatusAnswering, statusToGet);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CGMCS-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileCallStatus for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileCallStatus for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetMobileCallStatus00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    //---  check not active call -------------------------------------
    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    

    RMobileCall::TMobileCallStatus statusToGet;
    
    TInt ret = call1.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet);
        
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestNotify;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    ret = call.GetMobileCallStatus(statusToGet);
    ASSERT_EQUALS(KErrNone, ret);
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNATE-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAudioToneEvent for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAudioToneEvent for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyAudioToneEvent0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TRequestStatus requestStatus;
    TInt8 tmp;
    TPckg<TInt8> tmpPckg(tmp);
    
	call1.NotifyAudioToneEvent(requestStatus, tmpPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNATE-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAudioToneEvent for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAudioToneEvent for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyAudioToneEvent0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TRequestStatus requestStatus;
    TInt8 tmp;
    TPckg<TInt8> tmpPckg(tmp);
    
	call1.NotifyAudioToneEvent(requestStatus, tmpPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNATE-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyAudioToneEvent for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyAudioToneEvent for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyAudioToneEvent00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);


    RMobileCall call1;
    TName name;
   	OpenNewCallLC(call1, line, &name);
    
    TRequestStatus requestStatus;
    TInt8 tmp;
    TPckg<TInt8> tmpPckg(tmp);
    
	call1.NotifyAudioToneEvent(requestStatus, tmpPckg);
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

	mobileCallStatus = RMobileCall::EStatusRinging;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);


	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	


    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | 
                   RMobileCall::KCapsHangUp | 
                     RMobileCall::KCapsAnswer) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice 
      | RMobileCall::KCapsHangUp
        | RMobileCall::KCapsHold) == mcaps.iCallControlCaps);

	//-----------------------		

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusIdle;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsDial) == mcaps.iCallControlCaps);
	       	       
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
		
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //---------------------------------------

    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);
    
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());   

    
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
    //---------------------------------------
    
    TRequestStatus notifyStatus;
    TBuf8<1> caps(KNullDesC8);
    
    call.NotifyMobileCallCapsChange( notifyStatus, caps );

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_TRUE(KErrNone != notifyStatus.Int());    
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0004L()
	{
					
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
        
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
		
    //----------------------------------------   

    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    RMobileCall::TMobileCallCapsV1 mcaps2;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg2(mcaps2);
    
    call.NotifyMobileCallCapsChange( requestStatus, mcapsPckg);
    call2.NotifyMobileCallCapsChange( requestStatus2, mcapsPckg2);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsDial) == mcaps.iCallControlCaps);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0xFF == mcaps2.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsVoice | RMobileCall::KCapsDial) == mcaps2.iCallControlCaps);	    
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

	mobileCallStatus = RMobileCall::EStatusRinging;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);


	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	


    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | 
                   RMobileCall::KCapsHangUp | 
                     RMobileCall::KCapsAnswer) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData 
      | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusIdle;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | 
                    RMobileCall::KCapsDial |
                       RMobileCall::KCapsConnect) == mcaps.iCallControlCaps);
	       	       
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
		
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //---------------------------------------

    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);
    
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());   
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);	
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
    //---------------------------------------

    TRequestStatus notifyStatus;
    TBuf8<1> caps(KNullDesC8);
    
    call.NotifyMobileCallCapsChange( notifyStatus, caps );

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_TRUE(KErrNone != notifyStatus.Int());    
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);

    
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    //----------------------------------------
    

    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    RMobileCall::TMobileCallCapsV1 mcaps2;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg2(mcaps2);
    
    call.NotifyMobileCallCapsChange( requestStatus, mcapsPckg);
    call2.NotifyMobileCallCapsChange( requestStatus2, mcapsPckg2);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | RMobileCall::KCapsDial | RMobileCall::KCapsConnect) == mcaps.iCallControlCaps);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0xFF == mcaps2.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsData | RMobileCall::KCapsDial | RMobileCall::KCapsConnect) == mcaps2.iCallControlCaps);	    
	    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

	mobileCallStatus = RMobileCall::EStatusRinging;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);


	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	


    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | 
                   RMobileCall::KCapsHangUp | 
                     RMobileCall::KCapsAnswer) == mcaps.iCallControlCaps);

	//-----------------------	
	// just to change back mobile caps
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusRinging;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | 
    			  RMobileCall::KCapsHangUp | 
    			   RMobileCall::KCapsAnswer | 
    			    RMobileCall::KCapsDeflect) == mcaps.iCallControlCaps);

	//-----------------------	
	
    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax 
      | RMobileCall::KCapsHangUp) == mcaps.iCallControlCaps);

	//-----------------------	

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);

    mobileCallStatus = RMobileCall::EStatusIdle;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrNone, notifyStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | 
                    RMobileCall::KCapsDial) == mcaps.iCallControlCaps);
	       	       
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
		
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //---------------------------------------
    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    call.NotifyMobileCallCapsChange( notifyStatus, mcapsPckg);
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallCapsChange);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_EQUALS(KErrCancel, notifyStatus.Int());   
                
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallCapsChange with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallCapsChange with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange00013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //---------------------------------------
    TRequestStatus notifyStatus;
    TBuf8<1> caps(KNullDesC8);

    call.NotifyMobileCallCapsChange( notifyStatus, caps);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	    
    User::WaitForRequest(notifyStatus);
    ASSERT_TRUE(KErrNone != notifyStatus.Int());    

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCCC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallCapsChange00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
    
	RMobileCall call;
	RMobileCall call2;	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);
   
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);

    RMobileCall::TMobileCallCapsV1 mcaps2;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg2(mcaps2);
    
    call.NotifyMobileCallCapsChange( requestStatus, mcapsPckg);
    call2.NotifyMobileCallCapsChange( requestStatus2, mcapsPckg2);

    mobileCallStatus = RMobileCall::EStatusIdle;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE(0xFF == mcaps.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | RMobileCall::KCapsDial) == mcaps.iCallControlCaps);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_TRUE(0xFF == mcaps2.iCallEventCaps);
    ASSERT_TRUE((RMobileCall::KCapsFax | RMobileCall::KCapsDial) == mcaps2.iCallControlCaps);	    
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    //---------------------------------------
	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));     

    CloseIncomingCallL(call, callId, mobileService );
        
    //---------------------------------------

	RMobileCall call2;

	TInt ret = OpenNewCall(line, call2, KMmTsyVoice1LineName);
	ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(call2);

	// now we're giving the call a concrete callId > 0 and getting a name
	TName name;
	line.NotifyCallAdded(requestStatus, name);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    callId = 2;
    callInfo.iCallId = callId;
    callInfo.iStatus = RMobileCall::EStatusConnecting;  
    
    data.Close();
    mockCallData2.SerialiseL(data);		
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
		
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// now we have an opened call with known name and callId
	RMobileCall call3;
    ret = call3.OpenExistingCall(line, name);
    ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);


    call3.NotifyRemotePartyInfoChange(requestStatus, pckg);

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileOriginated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));     
   
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(6, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call.CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);

    RMobileCall::TMobileCallInfoV1 callInfo;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    TInt8 badInfo;
    TPckg<TInt8> pckg(badInfo);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_TRUE(KErrArgument == requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //data, line, call, this
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RMobileCall call;
	RMobileCall call2;
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    //----------------------------------------
    
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);

    RMobileCall::TMobileCallRemotePartyInfoV1 info2;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg2(info2);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call2.NotifyRemotePartyInfoChange(requestStatus2, pckg2);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    _LIT(KNum2, "Num2");
    _LIT(KName2, "Name2");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentitySuppressed;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum2;
	callInfo.iRemoteParty.iCallingName = KName2;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData3(callId2, mobileService, callInfo);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));    

    ASSERT_EQUALS(RMobileCall::ERemoteIdentitySuppressed, info2.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info2.iDirection);     
    ASSERT_TRUE(0 == info2.iCallingName.Compare(KName2));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info2.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info2.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info2.iRemoteNumber.iTelNumber.Compare(KNum2));    	    

	AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(8, this); 

	}




/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));     

    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call.CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);

    RMobileCall::TMobileCallInfoV1 callInfo;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());       
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    TInt8 badInfo;
    TPckg<TInt8> pckg(badInfo);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_TRUE(KErrArgument == requestStatus.Int());
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
    
	RMobileCall call;
	RMobileCall call2;	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    //----------------------------------------
    
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);

    RMobileCall::TMobileCallRemotePartyInfoV1 info2;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg2(info2);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call2.NotifyRemotePartyInfoChange(requestStatus2, pckg2);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    _LIT(KNum2, "Num2");
    _LIT(KName2, "Name2");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentitySuppressed;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum2;
	callInfo.iRemoteParty.iCallingName = KName2;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData3(callId2, mobileService, callInfo);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));    

    ASSERT_EQUALS(RMobileCall::ERemoteIdentitySuppressed, info2.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info2.iDirection);     
    ASSERT_TRUE(0 == info2.iCallingName.Compare(KName2));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info2.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info2.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info2.iRemoteNumber.iTelNumber.Compare(KNum2));    
	    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));     

    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call.CancelAsyncRequest(EMobileCallNotifyRemotePartyInfoChange);

    RMobileCall::TMobileCallInfoV1 callInfo;    
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyRemotePartyInfoChange with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange00013L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    TInt8 badInfo;
    TPckg<TInt8> pckg(badInfo);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);

    RMobileCall::TMobileCallInfoV1 callInfo;
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

	TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_TRUE(KErrArgument == requestStatus.Int());
        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNRPIC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyRemotePartyInfoChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyRemotePartyInfoChange00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);


	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

    //----------------------------------------
    
    RMobileCall::TMobileCallRemotePartyInfoV1 info;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg(info);

    RMobileCall::TMobileCallRemotePartyInfoV1 info2;
    RMobileCall::TMobileCallRemotePartyInfoV1Pckg pckg2(info2);
    
    call.NotifyRemotePartyInfoChange(requestStatus, pckg);
    call2.NotifyRemotePartyInfoChange(requestStatus2, pckg2);

    RMobileCall::TMobileCallInfoV1 callInfo;   
    
    _LIT(KNum, "Num");
    _LIT(KName, "Name");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityAvailable;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum;
	callInfo.iRemoteParty.iCallingName = KName;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData2(callId, mobileService, callInfo);
    mockCallData2.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    _LIT(KNum2, "Num2");
    _LIT(KName2, "Name2");
    
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentitySuppressed;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNum2;
	callInfo.iRemoteParty.iCallingName = KName2;
    
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData3(callId2, mobileService, callInfo);
    data.Close();
    mockCallData3.SerialiseL(data);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());



    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    
    ASSERT_EQUALS(RMobileCall::ERemoteIdentityAvailable, info.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info.iDirection);     
    ASSERT_TRUE(0 == info.iCallingName.Compare(KName));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info.iRemoteNumber.iTelNumber.Compare(KNum));    

    ASSERT_EQUALS(RMobileCall::ERemoteIdentitySuppressed, info2.iRemoteIdStatus);     
    ASSERT_EQUALS(RMobileCall::EMobileTerminated, info2.iDirection);     
    ASSERT_TRUE(0 == info2.iCallingName.Compare(KName2));     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumber, info2.iRemoteNumber.iTypeOfNumber);     
    ASSERT_EQUALS(RMobilePhone::EUnknownNumberingPlan, info2.iRemoteNumber.iNumberPlan);     
    ASSERT_TRUE(0 == info2.iRemoteNumber.iTelNumber.Compare(KNum2));    
	    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

	//--------------------
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDialling;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDialling, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnecting, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusAnswering, statusToGet);

	//-----------------------	
	
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnected, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnecting, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnectingWithInband, statusToGet);

	//-----------------------	
	
	// for other status values CompleteNotifyStatusChange() does not complete the notify request
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	

    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallStatusChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyVoice1LineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    //----------------------------------------
    
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);


    RMobileCall::TMobileCallStatus statusToGet;
    RMobileCall::TMobileCallStatus statusToGet2;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call2.NotifyMobileCallStatusChange(requestStatus2, statusToGet2); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet2);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 
	}




/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

	//--------------------
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDialling;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDialling, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnecting, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusAnswering, statusToGet);

	//-----------------------	
	
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnected, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnecting, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnectingWithInband, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusIdle;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    
    TUint32 duration = 4; // this is a dummy value, which will be ignored, when mocksy engine is doing the validation check	
    TMockLtsyData1<TUint32> ltsyData( duration );
    data.Close();
    ltsyData.SerialiseL(data);
    // when the call state becomes idle, there is update LifeTime param into LTSY:	
    iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet);

	//-----------------------	
	
	// for other status values CompleteNotifyStatusChange() does not complete the notify request
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this);
	
	}



/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallStatusChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyDataLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);

    //----------------------------------------
      
	RMobileCall call;
	RMobileCall call2;	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);



    RMobileCall::TMobileCallStatus statusToGet;
    RMobileCall::TMobileCallStatus statusToGet2;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call2.NotifyMobileCallStatusChange(requestStatus2, statusToGet2); 

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet2);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusAnswering;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

	mobileCallStatus = RMobileCall::EStatusRinging;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusRinging, statusToGet);

	//--------------------
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDialling;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDialling, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnecting, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusAnswering;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusAnswering, statusToGet);

	//-----------------------	
	
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusConnected;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnected, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnecting;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnecting, statusToGet);

	//-----------------------		

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusDisconnectingWithInband;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusDisconnectingWithInband, statusToGet);

	//-----------------------	

    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 

    mobileCallStatus = RMobileCall::EStatusIdle;  
    data.Close();
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    TUint32 duration = 4; // this is a dummy value, which will be ignored, when mocksy engine is doing the validation check	
    TMockLtsyData1<TUint32> ltsyData( duration );
    data.Close();
    ltsyData.SerialiseL(data);
    // when the call state becomes idle, there is update LifeTime param into LTSY:	
    iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, data);
        
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet);

	//-----------------------	
	
	// for other status values CompleteNotifyStatusChange() does not complete the notify request
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange00012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
    //----------------------------------------
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

    RMobileCall::TMobileCallStatus statusToGet;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call.CancelAsyncRequest(EMobileCallNotifyMobileCallStatusChange);

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());

        
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); 
	}





/**
@SYMTestCaseID BA-CTSY-CALLI-CNMCSC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileCallStatusChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyMobileCallStatusChange00014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

    TName lineName2(KMmTsyFaxLineName);    
    RLine line2;
    
	OpenLineLC(line2, phone2, lineName2);
    
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);

	CreateAndOpenIncomingCalLC(line2, call2, incomingCallName2, 
		lineName2, callId2, mobileCallStatus, mobileService);

	
    RMobileCall::TMobileCallStatus statusToGet;
    RMobileCall::TMobileCallStatus statusToGet2;
    call.NotifyMobileCallStatusChange(requestStatus, statusToGet); 
    call2.NotifyMobileCallStatusChange(requestStatus2, statusToGet2); 
    
    mobileCallStatus = RMobileCall::EStatusConnected;  
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData3(callId, mobileService, mobileCallStatus);
    mockCallData3.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    mobileCallStatus = RMobileCall::EStatusIdle;    

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId2, mobileService, mobileCallStatus);
    data.Close();
    mockCallData4.SerialiseL(data);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());


    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_EQUALS(RMobileCall::EStatusConnected, statusToGet);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());
    ASSERT_EQUALS(RMobileCall::EStatusIdle, statusToGet2);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(8, this); 
	}


void CCTsyCallInformationFU::AuxGetIncomingCallType0001L(const TDesC& aLineName, RMobilePhone::TMobileService aMobileService)
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RLine line;
    
	OpenLineLC(line, iPhone, aLineName);
    
	RMobileCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		aLineName, callId, mobileCallStatus, aMobileService);

    //---------------------------------------
    TRequestStatus notifyStatus;
    RMobileCall::TMobileCallCapsV1 mcaps;
    RMobileCall::TMobileCallCapsV1Pckg   mcapsPckg(mcaps);
    
    RMobilePhone::TMobilePhoneIncomingCallType type;
    RMobileCall::TMobileDataCallParamsV1 callParams;
    RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);

    iPhone.GetIncomingCallType(requestStatus, type, callParamsPckg);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
           
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); 	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-MGICT-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIncomingCallType
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIncomingCallType
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetIncomingCallType0001L()
	{

	AuxGetIncomingCallType0001L(KMmTsyVoice1LineName, RMobilePhone::EVoiceService);

	}

/**
@SYMTestCaseID BA-CTSY-CALLI-MGICT-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIncomingCallType
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIncomingCallType
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetIncomingCallType0006L()
	{

	AuxGetIncomingCallType0001L(KMmTsyDataLineName, RMobilePhone::ECircuitDataService);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-MGICT-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIncomingCallType
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIncomingCallType
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestGetIncomingCallType00011L()
	{

	AuxGetIncomingCallType0001L(KMmTsyFaxLineName, RMobilePhone::EFaxService);

	}


/**
@SYMTestCaseID BA-CTSY-CALLI-MNICTC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIncomingCallTypeChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIncomingCallTypeChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyIncomingCallTypeChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TRequestStatus requestStatus;

    RMobilePhone::TMobilePhoneIncomingCallType type;
	RMobileCall::TMobileDataCallParamsV1 callParams;
    RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);
    iPhone.NotifyIncomingCallTypeChange(requestStatus, type, callParamsPckg);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); 
	}






/**
@SYMTestCaseID BA-CTSY-CALLI-LNHCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyHookChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyLineHookChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyVoice1LineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    TRequestStatus notifyStatus;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
	RCall::THookStatus hookStatus;
	line.NotifyHookChange(requestStatus, hookStatus); 
	    
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    // this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function
    
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();
    
	CleanupStack::PopAndDestroy(4, this);
	
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNHCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyHookChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyLineHookChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyDataLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);
    
	RCall call;
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;

    TRequestStatus notifyStatus;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
	RCall::THookStatus hookStatus;
	line.NotifyHookChange(requestStatus, hookStatus); 
	    
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    TMockLtsyCallData0 mockData01(callId, mobileService);
    mockData01.SerialiseL(data);
    iMockLTSY.ExpectL(EEtelCallAnswer, data);
    iMockLTSY.CompleteL(EEtelCallAnswer, KErrNone, data);
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());

    // this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();    
	CleanupStack::PopAndDestroy(4, this);
		
	}


/**
@SYMTestCaseID BA-CTSY-CALLI-LNHCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RLine::NotifyHookChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RLine::NotifyHookChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyCallInformationFU::TestNotifyLineHookChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

    TName lineName(KMmTsyFaxLineName);    
    RLine line;
    
	OpenLineLC(line, iPhone, lineName);

	RCall call;	
	TInt callId = 1;
	TName incomingCallName;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus1;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusRinging;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;

    TRequestStatus notifyStatus;

	CreateAndOpenIncomingCalLC(line, call, incomingCallName, 
		lineName, callId, mobileCallStatus, mobileService);
	
	RCall::THookStatus hookStatus;
	line.NotifyHookChange(requestStatus, hookStatus); 
	    
	RMobileCall::TMobileCallParamsV1 callParamsV1;
	TPckg<RMobileCall::TMobileCallParamsV1> mmParamsPckgV1(callParamsV1);
	
    call.AnswerIncomingCall(requestStatus1, mmParamsPckgV1);
    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNotSupported, requestStatus1.Int());

    // this test is written supposing that NotifyHookChange() is supported
    // however RCall::NotifyHookChange returns KErrNotSupported
    // because CTSY does NOT support this feature
    // the rationale would be that NotifyStatusChange performs the same function

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	AssertMockLtsyStatusL();    
	CleanupStack::PopAndDestroy(4, this);


	}





//  ExpectL and CompleteL must precede this function.
void CCTsyCallInformationFU::CallGetMobileCallInfoL(TInt aCallId, 
                                               RMobilePhone::TMobileService aService, 
                                               const TDesC &aTelNumber)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	RMobileCall::TMobileCallInfoV1 callInfo;

	callInfo.iDialledParty.iTelNumber.Copy(aTelNumber);
	callInfo.iService = aService;
	callInfo.iEmergency = EFalse;
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iForwarded = EFalse;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

	TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(aCallId, aService, callInfo);
	callInfoData.SerialiseL(data);
		
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);
    
    CleanupStack::PopAndDestroy(1);
	}


void CCTsyCallInformationFU::OpenLineLC(RLine& aLine, RPhone& aPhone, const TDesC& aName)
	{
	
	TInt res = aLine.Open(aPhone, aName);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(aLine);
	AssertMockLtsyStatusL();
	
	}

	
void CCTsyCallInformationFU::CreateAndOpenIncomingCalLC(RLine &aLine, 
														RCall &aCall,
														TName& aCallName,
														const TDesC& aLineName,
														const TInt aCallId,
														RMobileCall::TMobileCallStatus aMobileCallStatus,
														RMobilePhone::TMobileService aMobileService
														)
	{
	
	TInt ret = CreateIncomingCallL(aLine, aCallId, aLineName, aCallName, 
	                               aMobileService, aMobileCallStatus);	
	ASSERT_EQUALS(KErrNone, ret);
	
    ret = aCall.OpenExistingCall(aLine, aCallName);
    ASSERT_EQUALS(KErrNone, ret);
    
    CleanupClosePushL(aCall);
	}


void CCTsyCallInformationFU::OpenNewCallLC(RCall& aCall, RLine& aLine, TDes* aCallName)
	{
	TInt res;
	if (aCallName)
		{
		res = aCall.OpenNewCall(aLine, *aCallName);
		}
	else
		{
		res = aCall.OpenNewCall(aLine);
		}
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(aCall);
	AssertMockLtsyStatusL();
	}
void CCTsyCallInformationFU::ValidateMobileCallInfo(TDes8* aCallInfo, TName &aCallName, const TDesC& aLineName, TBool aCallFlagActive, RMobileCall::TMobileCallStatus aStatus)
	{
	
	_LIT(KEmpty, "");
	const TDateTime KZeroTime(0,TMonth(0),0,0,0,0,0);
	
	RMobilePhone::TMultimodeTypePckg* infoParamsPckg =
       reinterpret_cast<RMobilePhone::TMultimodeTypePckg*>(
    		   aCallInfo );
    RMobilePhone::TMultimodeType& callInfo = ( *infoParamsPckg )();
        
    // get extensionid 
    TInt extensionId( callInfo.ExtensionId() );
    
    if ( (KETelExtMultimodeV1 == extensionId) || 
    	 (KETelExtMultimodeV3 == extensionId) ||
    	 (KEtelExtMultimodeV7 == extensionId) ||
    	 (KEtelExtMultimodeV8 == extensionId) )
        {
           
        RMobileCall::TMobileCallInfoV1Pckg* paramsPckgV1 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV1Pckg*>(aCallInfo );
        RMobileCall::TMobileCallInfoV1& info = ( *paramsPckgV1 )();
        
        if(aCallFlagActive)
        	{
        	ASSERT_TRUE(RMobilePhone::ECircuitDataService == info.iService);
        	ASSERT_TRUE(1 == info.iCallId);
        	ASSERT_TRUE(0 == info.iExitCode);
        	ASSERT_TRUE(RMobileCall::EMobileTerminated == info.iRemoteParty.iDirection);
        	ASSERT_TRUE(aStatus == info.iStatus);
        	}
        else
        	{
        	ASSERT_TRUE(RMobileCall::EServiceUnspecified == info.iService);
        	ASSERT_TRUE(0 == info.iCallId);
        	ASSERT_TRUE(-1 == info.iExitCode);
        	ASSERT_TRUE(RMobileCall::EDirectionUnknown == info.iRemoteParty.iDirection);
        	ASSERT_TRUE(RMobileCall::EStatusIdle == info.iStatus);
        	ASSERT_TRUE(0 == info.iCallName.Compare(aCallName));
        	}
		ASSERT_TRUE((RMobileCall::KCallStartTime | 
	        RMobileCall::KCallDuration | 
	            RMobileCall::KCallId | 
	                RMobileCall::KCallExitCode | 
	                    RMobileCall::KCallEmergency) == info.iValid);
	    
	 
	    
	    ASSERT_TRUE(0 == info.iEmergency);
	    ASSERT_TRUE(0 == info.iForwarded);
	    ASSERT_TRUE(RMobilePhone::EPrivacyUnspecified == info.iPrivacy);     
	    ASSERT_TRUE(RMobilePhone::EAlternatingModeUnspecified == info.iAlternatingCall);
	    ASSERT_TRUE(RMobileCall::ETchUnknown == info.iTch);
	    ASSERT_TRUE(RMobileCall::ERemoteIdentityUnknown == info.iRemoteParty.iRemoteIdStatus);
	    ASSERT_TRUE(0 == info.iRemoteParty.iCallingName.Compare(KEmpty));
	    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTelNumber.Size());
	    ASSERT_TRUE(0 == info.iRemoteParty.iRemoteNumber.iTypeOfNumber);
	    ASSERT_TRUE(RMobilePhone::EUnknownNumber == info.iDialledParty.iTypeOfNumber);
	    ASSERT_TRUE(RMobilePhone::EUnknownNumberingPlan == info.iDialledParty.iNumberPlan);
	    ASSERT_TRUE(0 == info.iDialledParty.iTelNumber.Compare(KEmpty));
	    ASSERT_TRUE(0 == memcmp( &info.iStartTime, &KZeroTime, sizeof(KZeroTime)));
	    ASSERT_TRUE(0 == info.iLineName.Compare(aLineName));
        }
   
    if ( (KETelExtMultimodeV3 == extensionId) ||
         (KEtelExtMultimodeV7 == extensionId) ||
         (KEtelExtMultimodeV8 == extensionId) )
        {
           
        RMobileCall::TMobileCallInfoV3Pckg* paramsPckgV3 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV3Pckg*>(aCallInfo );
        RMobileCall::TMobileCallInfoV3& info = ( *paramsPckgV3 )();
        
        ASSERT_TRUE(RMobilePhone::ECipheringOff == info.iSecurity);
		}
    
    if ( (KEtelExtMultimodeV7 == extensionId) ||
         (KEtelExtMultimodeV8 == extensionId) )
        {
           
        RMobileCall::TMobileCallInfoV7Pckg* paramsPckgV7 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV7Pckg*>(aCallInfo );
        RMobileCall::TMobileCallInfoV7& info = ( *paramsPckgV7 )();
           
        ASSERT_TRUE(RMobileCall::EOriginatorUnknown == info.iCallParamOrigin);
        ASSERT_TRUE(RMobileCall::EIconQualifierNotSet == info.iIconId.iQualifier);
        ASSERT_TRUE(0 == info.iIconId.iIdentifier);
        ASSERT_TRUE(info.iAlphaId.Length() == 0);
        ASSERT_TRUE(EFalse == info.iParamsCallControlModified);
      
		}
    
    if (KEtelExtMultimodeV8 == extensionId )
        {
           
        RMobileCall::TMobileCallInfoV8Pckg* paramsPckgV8 =
            reinterpret_cast<RMobileCall::TMobileCallInfoV8Pckg*>(aCallInfo );
        RMobileCall::TMobileCallInfoV8& info = ( *paramsPckgV8 )();
        
        ASSERT_TRUE(0 ==  info.iSubAddress.Length());
        ASSERT_TRUE(0 ==  info.iBearerCap1.Length());
        ASSERT_TRUE(0 ==  info.iBearerCap2.Length());
        ASSERT_EQUALS(RMobileCall::EBCAlternateMode,info.iBCRepeatIndicator);
         
		}
            
    }

void CCTsyCallInformationFU::ValidateCallParams(const TDesC8* aCallParams, TBool aCallFlagActive)
	{
	
	
	//use base class to determine callparam version
    RCall::TCallParamsPckg* callParamsPckg = 
        reinterpret_cast<RCall::TCallParamsPckg*>(const_cast<TDesC8*>( aCallParams ) ); 

    RCall::TCallParams& callParams = ( *callParamsPckg )();
        
    // get extensionid 
    TInt extensionId( callParams.ExtensionId() );
    _LIT( KStringAlphaId,  "Alpha Id"  );
    
    if ( (RMobileCall::KETelMobileCallParamsV1 == extensionId) || 
    	 (RMobileCall::KETelMobileCallParamsV2 == extensionId) ||
    	 (RMobileCall::KETelMobileCallParamsV7 == extensionId))
        {
           
        RMobileCall::TMobileCallParamsV1Pckg* paramsPckgV1 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV1Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileCallParamsV1& callParams1 = (*paramsPckgV1)();
        
        if(aCallFlagActive)
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlAlwaysOff, callParams1.iSpeakerControl);
    	    ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeMedium, callParams1.iSpeakerVolume);
    	    ASSERT_TRUE(2 == callParams1.iInterval);
    	    ASSERT_EQUALS(RCall::EDialToneWait, callParams1.iWaitForDialTone);
    	    ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, callParams1.iIdRestrict);
    	    ASSERT_TRUE(EFalse == callParams1.iCug.iExplicitInvoke);
    	    ASSERT_TRUE(0 == callParams1.iCug.iCugIndex);
    	    ASSERT_TRUE(EFalse == callParams1.iCug.iSuppressOA);
    	    ASSERT_TRUE(EFalse == callParams1.iCug.iSuppressPrefCug);
    	    ASSERT_TRUE(EFalse == callParams1.iAutoRedial);
        	}
        else
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlOnUntilCarrier, callParams1.iSpeakerControl);
	        ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeLow, callParams1.iSpeakerVolume);
	        ASSERT_TRUE(4 == callParams1.iInterval);
	        ASSERT_EQUALS(RCall::EDialToneWait, callParams1.iWaitForDialTone);
	        ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, callParams1.iIdRestrict);
	        ASSERT_TRUE(EFalse == callParams1.iCug.iExplicitInvoke);
	        ASSERT_TRUE(0 == callParams1.iCug.iCugIndex);
	        ASSERT_TRUE(EFalse == callParams1.iCug.iSuppressOA);
	        ASSERT_TRUE(EFalse == callParams1.iCug.iSuppressPrefCug);
	        ASSERT_TRUE(EFalse == callParams1.iAutoRedial);
        	}
       
        }
    
    if ((RMobileCall::KETelMobileCallParamsV2 == extensionId) ||
        (RMobileCall::KETelMobileCallParamsV7 == extensionId))
        {
        RMobileCall::TMobileCallParamsV2Pckg* paramsPckgV2 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV2Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileCallParamsV2& callParams2 = (*paramsPckgV2)();
       if(aCallFlagActive)
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallNotSupported, callParams2.iBearerMode );
    	   }
       else
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallNotSupported, callParams2.iBearerMode );
    	   }
        }
    
    if ((RMobileCall::KETelMobileCallParamsV7 == extensionId))
        {
        RMobileCall::TMobileCallParamsV7Pckg* paramsPckgV7 = 
            reinterpret_cast<RMobileCall::TMobileCallParamsV7Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileCallParamsV7& callParams7 = (*paramsPckgV7)();
        if(aCallFlagActive)
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorUnknown, callParams7.iCallParamOrigin);
    	    ASSERT_TRUE(0 ==  callParams7.iSubAddress.Length());
    	    ASSERT_TRUE(0 ==  callParams7.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  callParams7.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,callParams7.iBCRepeatIndicator);
    	    ASSERT_TRUE(RMobileCall::EIconQualifierNotSet == callParams7.iIconId.iQualifier);
	        ASSERT_TRUE(0 == callParams7.iIconId.iIdentifier);
    	    ASSERT_TRUE(0 == callParams7.iAlphaId.Compare(KStringAlphaId));
        	}
        else
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorUnknown, callParams7.iCallParamOrigin);
    	    ASSERT_TRUE(0 ==  callParams7.iSubAddress.Length());
    	    ASSERT_TRUE(0 ==  callParams7.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  callParams7.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,callParams7.iBCRepeatIndicator);
    	    ASSERT_TRUE(RMobileCall::EIconQualifierNotSet == callParams7.iIconId.iQualifier);
	        ASSERT_TRUE(0 == callParams7.iIconId.iIdentifier);
    	    ASSERT_TRUE(0 == callParams7.iAlphaId.Length());
        	}
       
         }
    
    }


void CCTsyCallInformationFU::ValidateDataCallParams(const TDesC8* aCallParams, TBool aDataCallFlagActive)
	{
	
	//use base class to determine datacallparam version
    RMobileCall::TMobileDataCallParamsV1Pckg* paramsPckgV1 =
        reinterpret_cast<RMobileCall::TMobileDataCallParamsV1Pckg*>(
            const_cast<TDesC8*>( aCallParams ) );

    RMobileCall::TMobileDataCallParamsV1& dataParams = (*paramsPckgV1)();
        
    // get extensionid 
    TInt extensionId( dataParams.ExtensionId() );
    _LIT( KStringSubAddress,  "An Unknown SubAddress");
    _LIT( KStringAlphaId,  "Alpha Id"  );
    
    if ( (RMobileCall::KETelMobileDataCallParamsV1 == extensionId) || 
    	 (RMobileCall::KETelMobileDataCallParamsV2 == extensionId) ||
    	 (RMobileCall::KETelMobileDataCallParamsV8 == extensionId))
        {
          
        if(aDataCallFlagActive)
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlAlwaysOff, dataParams.iSpeakerControl);
    	    ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeMedium, dataParams.iSpeakerVolume);
    	    ASSERT_TRUE(2 == dataParams.iInterval);
    	    ASSERT_EQUALS(RCall::EDialToneWait, dataParams.iWaitForDialTone);
    	    ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, dataParams.iIdRestrict);
    	    ASSERT_TRUE(EFalse == dataParams.iCug.iExplicitInvoke);
    	    ASSERT_TRUE(0xFFFF == dataParams.iCug.iCugIndex);
    	    ASSERT_TRUE(EFalse == dataParams.iCug.iSuppressOA);
    	    ASSERT_TRUE(EFalse == dataParams.iCug.iSuppressPrefCug);
    	    ASSERT_TRUE(EFalse == dataParams.iAutoRedial);
    	    ASSERT_EQUALS(RMobileCall::EServiceDataCircuitAsync, dataParams.iService);
		    ASSERT_EQUALS(RMobileCall::ESpeedUnspecified, dataParams.iSpeed);
		    ASSERT_EQUALS(RMobileCall::EProtocolUnspecified, dataParams.iProtocol);
		    ASSERT_EQUALS(RMobileCall::EQosTransparentPreferred, dataParams.iQoS);
		    ASSERT_EQUALS(RMobileCall::ERLPNotRequested, dataParams.iRLPVersion);
		    ASSERT_EQUALS(RMobileCall::EV42bisTxDirection, dataParams.iV42bisReq);
		    ASSERT_TRUE(0 == dataParams.iModemToMSWindowSize);
		    ASSERT_TRUE(0 == dataParams.iMSToModemWindowSize);
		    ASSERT_TRUE(0 == dataParams.iAckTimer);
		    ASSERT_TRUE(0 == dataParams.iRetransmissionAttempts);
		    ASSERT_TRUE(0 == dataParams.iResequencingPeriod);
		    ASSERT_TRUE(0 == dataParams.iV42bisCodewordsNum);
		    ASSERT_TRUE(0 == dataParams.iV42bisMaxStringLength);
		    ASSERT_TRUE(EFalse == dataParams.iUseEdge);
        	}
        else
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlOnUntilCarrier, dataParams.iSpeakerControl);
	        ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeLow, dataParams.iSpeakerVolume);
	        ASSERT_TRUE(4 == dataParams.iInterval);
	        ASSERT_EQUALS(RCall::EDialToneWait, dataParams.iWaitForDialTone);
	        ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, dataParams.iIdRestrict);
	        ASSERT_TRUE(EFalse == dataParams.iCug.iExplicitInvoke);
	        ASSERT_TRUE(0 == dataParams.iCug.iCugIndex);
	        ASSERT_TRUE(EFalse == dataParams.iCug.iSuppressOA);
	        ASSERT_TRUE(EFalse == dataParams.iCug.iSuppressPrefCug);
	        ASSERT_TRUE(EFalse == dataParams.iAutoRedial);
	            
		    ASSERT_EQUALS(RMobileCall::EServiceDataCircuitAsync, dataParams.iService);
		    ASSERT_EQUALS(RMobileCall::ESpeedUnspecified, dataParams.iSpeed);
		    ASSERT_EQUALS(RMobileCall::EProtocolUnspecified, dataParams.iProtocol);
		    ASSERT_EQUALS(RMobileCall::EQoSNonTransparent, dataParams.iQoS);
		    ASSERT_EQUALS(RMobileCall::ERLPNotRequested, dataParams.iRLPVersion);
		    ASSERT_EQUALS(RMobileCall::EV42bisNeitherDirection, dataParams.iV42bisReq);
		    ASSERT_TRUE(0 == dataParams.iModemToMSWindowSize);
		    ASSERT_TRUE(0 == dataParams.iMSToModemWindowSize);
		    ASSERT_TRUE(0 == dataParams.iAckTimer);
		    ASSERT_TRUE(0 == dataParams.iRetransmissionAttempts);
		    ASSERT_TRUE(0 == dataParams.iResequencingPeriod);
		    ASSERT_TRUE(0 == dataParams.iV42bisCodewordsNum);
		    ASSERT_TRUE(0 == dataParams.iV42bisMaxStringLength);
		    ASSERT_TRUE(EFalse == dataParams.iUseEdge);
		 	}
       
        }
    
    if ((RMobileCall::KETelMobileDataCallParamsV2 == extensionId) ||
        (RMobileCall::KETelMobileDataCallParamsV8 == extensionId))
        {
        RMobileCall::TMobileDataCallParamsV2Pckg* dataParamsPckgV2 = 
            reinterpret_cast<RMobileCall::TMobileDataCallParamsV2Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
       RMobileCall::TMobileDataCallParamsV2& dataCallParams2 = (*dataParamsPckgV2)();
       if(aDataCallFlagActive)
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallShareBearer, dataCallParams2.iBearerMode );
    	   }
       else
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallNotSupported, dataCallParams2.iBearerMode );
    	   }
        }
    
    if ((RMobileCall::KETelMobileDataCallParamsV8 == extensionId))
        {
        RMobileCall::TMobileDataCallParamsV8Pckg* dataParamsPckgV8 = 
            reinterpret_cast<RMobileCall::TMobileDataCallParamsV8Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileDataCallParamsV8& dataCallParams8 = (*dataParamsPckgV8)();
       
        if(aDataCallFlagActive)
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorSIM, dataCallParams8.iCallParamOrigin);
    	    ASSERT_TRUE(0 ==  dataCallParams8.iSubAddress.Compare(KStringSubAddress));
    	    ASSERT_TRUE(0 ==  dataCallParams8.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  dataCallParams8.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,dataCallParams8.iBCRepeatIndicator);
    	    ASSERT_TRUE(0 == dataCallParams8.iAlphaId.Compare(KStringAlphaId))
    	   	}
        else
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorUnknown, dataCallParams8.iCallParamOrigin);
    	    ASSERT_TRUE(0 ==  dataCallParams8.iSubAddress.Length());
    	    ASSERT_TRUE(0 ==  dataCallParams8.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  dataCallParams8.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,dataCallParams8.iBCRepeatIndicator);
    	    ASSERT_TRUE(0 == dataCallParams8.iAlphaId.Length())
    	   	}
       
         }
    
    }


void CCTsyCallInformationFU::ValidateHscsdCallParams(const TDesC8* aCallParams, TBool aHscsdCallFlagActive)
	{
	//use base class to determine hscsdcallparam version
    RMobileCall::TMobileHscsdCallParamsV1Pckg* paramsPckgV1 =
        reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV1Pckg*>(
            const_cast<TDesC8*>( aCallParams ) );

    RMobileCall::TMobileHscsdCallParamsV1& hscsdParams = (*paramsPckgV1)();
        
    // get extensionid 
    TInt extensionId( hscsdParams.ExtensionId() );
    _LIT( KStringAlphaId,  "Alpha Id"  );
    _LIT( KStringSubAddress,  "An Unknown SubAddress");
    
    if ( (RMobileCall::KETelMobileHscsdCallParamsV1 == extensionId) || 
    	 (RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId) ||
    	 (RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId) ||
    	 (RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId))
        {
          
        if(aHscsdCallFlagActive)
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlAlwaysOff, hscsdParams.iSpeakerControl);
    	    ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeMedium, hscsdParams.iSpeakerVolume);
    	    ASSERT_TRUE(2 == hscsdParams.iInterval);
    	    ASSERT_EQUALS(RCall::EDialToneWait, hscsdParams.iWaitForDialTone);
    	    ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, hscsdParams.iIdRestrict);
    	    ASSERT_TRUE(EFalse == hscsdParams.iCug.iExplicitInvoke);
    	    ASSERT_TRUE(0xFFFF == hscsdParams.iCug.iCugIndex);
    	    ASSERT_TRUE(EFalse == hscsdParams.iCug.iSuppressOA);
    	    ASSERT_TRUE(EFalse == hscsdParams.iCug.iSuppressPrefCug);
    	    ASSERT_TRUE(EFalse == hscsdParams.iAutoRedial);
    	    ASSERT_EQUALS(RMobileCall::EServiceDataCircuitAsync, hscsdParams.iService);
		    ASSERT_EQUALS(RMobileCall::ESpeedUnspecified, hscsdParams.iSpeed);
		    ASSERT_EQUALS(RMobileCall::EProtocolUnspecified, hscsdParams.iProtocol);
		    ASSERT_EQUALS(RMobileCall::EQosTransparentPreferred, hscsdParams.iQoS);
		    ASSERT_EQUALS(RMobileCall::ERLPNotRequested, hscsdParams.iRLPVersion);
		    ASSERT_EQUALS(RMobileCall::EV42bisTxDirection, hscsdParams.iV42bisReq);
		    ASSERT_TRUE(0 == hscsdParams.iModemToMSWindowSize);
		    ASSERT_TRUE(0 == hscsdParams.iMSToModemWindowSize);
		    ASSERT_TRUE(0 == hscsdParams.iAckTimer);
		    ASSERT_TRUE(0 == hscsdParams.iRetransmissionAttempts);
		    ASSERT_TRUE(0 == hscsdParams.iResequencingPeriod);
		    ASSERT_TRUE(0 == hscsdParams.iV42bisCodewordsNum);
		    ASSERT_TRUE(0 == hscsdParams.iV42bisMaxStringLength);
		    ASSERT_TRUE(EFalse == hscsdParams.iUseEdge);
		    
		    ASSERT_TRUE(RMobileCall::EAiurBpsUnspecified == hscsdParams.iWantedAiur);
	        ASSERT_TRUE(RMobileCall::EAsymmetryNoPreference == hscsdParams.iAsymmetry);
	        ASSERT_TRUE(2 == hscsdParams.iWantedRxTimeSlots);
	        ASSERT_TRUE(3 == hscsdParams.iCodings);
	        ASSERT_TRUE(EFalse == hscsdParams.iUserInitUpgrade);    
		    
        	}
        else
        	{
        	ASSERT_EQUALS(RCall::EMonitorSpeakerControlOnUntilCarrier, hscsdParams.iSpeakerControl);
	        ASSERT_EQUALS(RCall::EMonitorSpeakerVolumeLow, hscsdParams.iSpeakerVolume);
	        ASSERT_TRUE(4 == hscsdParams.iInterval);
	        ASSERT_EQUALS(RCall::EDialToneWait, hscsdParams.iWaitForDialTone);
	        ASSERT_EQUALS(RMobileCall::EIdRestrictDefault, hscsdParams.iIdRestrict);
	        ASSERT_TRUE(EFalse == hscsdParams.iCug.iExplicitInvoke);
	        ASSERT_TRUE(0 == hscsdParams.iCug.iCugIndex);
	        ASSERT_TRUE(EFalse == hscsdParams.iCug.iSuppressOA);
	        ASSERT_TRUE(EFalse == hscsdParams.iCug.iSuppressPrefCug);
	        ASSERT_TRUE(EFalse == hscsdParams.iAutoRedial);
	            
		    ASSERT_EQUALS(RMobileCall::EServiceDataCircuitAsync, hscsdParams.iService);
		    ASSERT_EQUALS(RMobileCall::ESpeedUnspecified, hscsdParams.iSpeed);
		    ASSERT_EQUALS(RMobileCall::EProtocolUnspecified, hscsdParams.iProtocol);
		    ASSERT_EQUALS(RMobileCall::EQoSNonTransparent, hscsdParams.iQoS);
		    ASSERT_EQUALS(RMobileCall::ERLPNotRequested, hscsdParams.iRLPVersion);
		    ASSERT_EQUALS(RMobileCall::EV42bisNeitherDirection, hscsdParams.iV42bisReq);
		    ASSERT_TRUE(0 == hscsdParams.iModemToMSWindowSize);
		    ASSERT_TRUE(0 == hscsdParams.iMSToModemWindowSize);
		    ASSERT_TRUE(0 == hscsdParams.iAckTimer);
		    ASSERT_TRUE(0 == hscsdParams.iRetransmissionAttempts);
		    ASSERT_TRUE(0 == hscsdParams.iResequencingPeriod);
		    ASSERT_TRUE(0 == hscsdParams.iV42bisCodewordsNum);
		    ASSERT_TRUE(0 == hscsdParams.iV42bisMaxStringLength);
		    ASSERT_TRUE(EFalse == hscsdParams.iUseEdge);
		    
		    ASSERT_TRUE(RMobileCall::EAiurBpsUnspecified == hscsdParams.iWantedAiur);
	        ASSERT_TRUE(RMobileCall::EAsymmetryNoPreference == hscsdParams.iAsymmetry);
	        ASSERT_TRUE(0 == hscsdParams.iWantedRxTimeSlots);
	        ASSERT_TRUE(0 == hscsdParams.iCodings);
	        ASSERT_TRUE(EFalse == hscsdParams.iUserInitUpgrade); 
		 	}
       
        }
   
    if ((RMobileCall::KETelMobileHscsdCallParamsV2 == extensionId) ||
		(RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId) ||
        (RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId))
        {
        RMobileCall::TMobileHscsdCallParamsV2Pckg* paramsPckgV2 =
            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV2Pckg*>(
                const_cast<TDesC8*>( aCallParams ) );

        RMobileCall::TMobileHscsdCallParamsV2& hscsdParams2 = (*paramsPckgV2)();
                
       if(aHscsdCallFlagActive)
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallShareBearer, hscsdParams2.iBearerMode );
    	   }
       else
    	   {
    	   ASSERT_EQUALS(RMobileCall::EMulticallNotSupported, hscsdParams2.iBearerMode );
    	   }
        }
   
    if ((RMobileCall::KETelMobileHscsdCallParamsV7 == extensionId)||
    	(RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId))
        {
        RMobileCall::TMobileHscsdCallParamsV7Pckg* hscsdParamsPckgV7 = 
            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV7Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileHscsdCallParamsV7& hscsdParams7 = (*hscsdParamsPckgV7)();
       
        if(aHscsdCallFlagActive)
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorSIM, hscsdParams7.iCallParamOrigin);
        	ASSERT_TRUE(RMobileCall::EIconQualifierNotSet == hscsdParams7.iIconId.iQualifier);
        	ASSERT_TRUE(0 == hscsdParams7.iIconId.iIdentifier);
        	ASSERT_TRUE(0 == hscsdParams7.iAlphaId.Compare(KStringAlphaId));
    	   	}
        else
        	{
        	ASSERT_EQUALS(RMobileCall::EOriginatorUnknown, hscsdParams7.iCallParamOrigin);
        	ASSERT_TRUE(RMobileCall::EIconQualifierNotSet == hscsdParams7.iIconId.iQualifier);
	        ASSERT_TRUE(0 == hscsdParams7.iIconId.iIdentifier);
    	    ASSERT_TRUE(0 == hscsdParams7.iAlphaId.Length());
    	   	}
        }
   
    if (RMobileCall::KETelMobileHscsdCallParamsV8 == extensionId)
        {
        RMobileCall::TMobileHscsdCallParamsV8Pckg* hscsdParamsPckgV8 = 
            reinterpret_cast<RMobileCall::TMobileHscsdCallParamsV8Pckg*>( 
            const_cast<TDesC8*>( aCallParams ) );
        RMobileCall::TMobileHscsdCallParamsV8& hscsdParams8 = (*hscsdParamsPckgV8)();
       
        if(aHscsdCallFlagActive)
        	{
        	ASSERT_TRUE(0 ==  hscsdParams8.iSubAddress.Compare(KStringSubAddress));
    	    ASSERT_TRUE(0 ==  hscsdParams8.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  hscsdParams8.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,hscsdParams8.iBCRepeatIndicator);
    	   	}
        else
        	{
        	ASSERT_TRUE(0 ==  hscsdParams8.iSubAddress.Length());
    	    ASSERT_TRUE(0 ==  hscsdParams8.iBearerCap1.Length());
    	    ASSERT_TRUE(0 ==  hscsdParams8.iBearerCap2.Length());
    	    ASSERT_EQUALS(RMobileCall::EBCAlternateMode,hscsdParams8.iBCRepeatIndicator);
    	   	}
        
         }

    }


	


	

