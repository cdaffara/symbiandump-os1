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
// The TEFUnit test suite for FixedDialling in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyfixeddiallingfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsyFixedDiallingFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0001cL);
    ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0001dL);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0004L);	
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0005L);
		
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0006bL);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0007L);  still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0008L);  still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck0009L);  still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00010L); still not supported
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00011L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00012L); still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00013L); still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00014L); still not supported
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestDialNoFdnCheck00015L); still not supported
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestGetFdnStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestGetFdnStatus0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestGetFdnStatus0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestGetFdnStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestGetFdnStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestSetFdnSetting0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestSetFdnSetting0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestSetFdnSetting0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestSetFdnSetting0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestSetFdnSetting0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestNotifyFdnStatusChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestNotifyFdnStatusChange0002L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestNotifyFdnStatusChange0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestNotifyFdnStatusChange0004L);
	//ADD_TEST_STEP_ISO_CPP(CCTsyFixedDiallingFU, TestNotifyFdnStatusChange0005L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
    
    TInt errorCode = KErrNone;    
    
    //-- For Voice1 -------------------------
        
    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   
    
    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");    
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	// data for ExpextL
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expecteCallId = 0;
        
    RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
   
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
	callInfo.iService	= mobileService;
	callInfo.iStatus	= RMobileCall::EStatusUnknown;
	callInfo.iCallId	= -1;
	callInfo.iExitCode	=0; 
	callInfo.iEmergency	=0;
	callInfo.iForwarded	=0;	
	callInfo.iPrivacy				= RMobilePhone::EPrivacyUnspecified;
	callInfo.iAlternatingCall		= RMobilePhone::EAlternatingModeUnspecified;	
	//callInfo.iDuration.iIntervel	= 0;			// this is protected value
	callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
	callInfo.iDialledParty.iNumberPlan = 
        RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
        RMobilePhone::EUnknownNumber;	
	    
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8 >
    											 mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData, KErrNotSupported);
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());      
    CleanupStack::PopAndDestroy();// call  
       
    
    //-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::DialNoFdnCheck
 	//-------------------------------------------------------------------------
 	TInt callId = 1;
 	// create new incoming call
 	TName incomingCallName;
 	RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle; 
 	
	errorCode = CreateIncomingCallL(line, callId, lineName, incomingCallName, 
                             mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);	 	
 	   
    // --------  make for this call callId > 0 --------------------------------------
	line.NotifyIncomingCall(requestStatus, incomingCallName);

    callInfo.iService = mobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> idleCallData3(callId, mobileService, callInfo);
    idleCallData3.SerialiseL(completeData);
	
	TRequestStatus mockLtsyStatus;
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(call);
	//-------------------------------------------------------------------------      
    
 	// data for ExpectL 		
 	callInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown; 	
 	
 	TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData2(callId, mobileService, callParams, callInfo);
    expectData.Close();
    mockCallData2.SerialiseL(expectData);
 	// data for CompleteL 	
 	TMockLtsyCallData0 mockDataComplete(callId, mobileService);
 	completeData.Close();
 	mockDataComplete.SerialiseL(completeData);
 	
 	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);   	
  	iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrGeneral, completeData);   	
  	
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);   
    
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());    
    AssertMockLtsyStatusL();  
    
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialNoFdnCheck when result is not cached.
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);   	
  	iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);   	
  	
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);   
    
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::DialNoFdnCheck
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();	
		
	//-------------------------------------------------------------------------
	// TEST 1  for coverage increasing
 	//------------------------------------------------------------------------- 	
 	// change status of call
 	mobileCallStatus = RMobileCall::EStatusRinging;
 	    
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData4(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallData4.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
   	call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);   
    
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNotReady, requestStatus.Int());
    AssertMockLtsyStatusL();   
        
    // data for ExpectL
    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable = EFalse; 
            
    TMockLtsyCallData2< TInt, TBool >
    							mockCallDataClose(callId, mobileService, hangUpCause, autoStChangeDisable);
    expectData.Close();
    mockCallDataClose.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData); // this expect is necessary for call closing when EStatusRinging  
   	CleanupStack::PopAndDestroy(); // call
    AssertMockLtsyStatusL(); 
   
    //-------------------------------------------------------------------------
	// TEST 2  for coverage increasing
 	//-------------------------------------------------------------------------    
    // create new call    
    callId = 2;
    mobileCallStatus = RMobileCall::EStatusIdle;
    errorCode = CreateIncomingCallL(line, callId, lineName, incomingCallName, 
                             mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, errorCode);	 	
    
    errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(call);
    
	// set RFStatus to Inactive state
	TRfStateInfo rfInfo (ERfsStateInfoInactive);
	
	TMockLtsyData1<TRfStateInfo> mockCallData5(rfInfo);
    completeData.Close();
    mockCallData5.SerialiseL(completeData);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    	
  	call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);       
    User::WaitForRequest(requestStatus); 
    
    TInt expectedErrorCode = ( KErrGeneral & 0x0000FFFF ) |( KErrGsmOfflineOpNotAllowed << 16 );
	expectedErrorCode = (expectedErrorCode>>16) | 0xFFFF0000;

             
    ASSERT_EQUALS(expectedErrorCode, requestStatus.Int());
    AssertMockLtsyStatusL();   
	
 	// close call, cause theres no way to use it again
   	CleanupStack::PopAndDestroy(&call);
   	AssertMockLtsyStatusL();
		
	
	// create new incoming call
	errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call); 
    
    // set RFStatus to Normal state
	rfInfo = ERfsStateInfoNormal;
	
	TMockLtsyData1<TRfStateInfo> mockCallData6(rfInfo);
	    
    completeData.Close();
    mockCallData6.SerialiseL(completeData);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
   
    // data for ExpectL
    callInfo.iRemoteParty.iDirection = RMobileCall::EDirectionUnknown; 	
	    
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >    
    											 mockCallData7(expecteCallId, mobileService, callParams, callInfo);
	expectData.Close();    											 
    mockCallData7.SerialiseL(expectData);
	
	// Test A: ... 
	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData, KErrNotSupported);    
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int()); 
	
	// ...and now repeat this test A
    // This repetition is here to test that SetDialFlag(EFalse) is called when
    // the EMobileCallDialNoFdnCheck request completes with an error
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData, KErrNotSupported);
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
    	
	//-------------------------------------------------------------------------
	// TEST to show defect 120102
 	//-------------------------------------------------------------------------

  	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);   	
 	CallGetMobileCallInfoL(callId, mobileService, KSomeNumber); 
 	
 	completeData.Close();
 	TMockLtsyCallData0 mockDataComplete2(callId, mobileService);
 	mockDataComplete2.SerialiseL(completeData);
  	iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);   	
  	
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);   
    
    User::WaitForRequest(requestStatus);      
     
    ASSERT_EQUALS(KErrNone, requestStatus.Int());      
    
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // ... this
	}
	
	
/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	    
    TInt errorCode = KErrNone;    
    
    //-- For Voice1 -------------------------    
    TBuf<256> lineName(KMmTsyVoice1LineName);
    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);  
    
    // open call
    _LIT(KDoubleColon, "::");
    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);    
    
    TRequestStatus requestStatus;
    
    _LIT(KSomeNumber, "123456789");
    	
		
	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
	// for API version without params
	// i.e. for void RMobileCall::DialNoFdnCheck(TRequestStatus& aReqStatus,const TDesC& aTelNumber)
 	//-------------------------------------------------------------------------
	// data for ExpextL
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expecteCallId = 0;
        
    RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV1 callInfo;   
   
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
	callInfo.iService	= mobileService;
	callInfo.iStatus	= RMobileCall::EStatusUnknown;
	callInfo.iCallId	= -1;
	callInfo.iExitCode	=0; 
	callInfo.iEmergency	=0;
	callInfo.iForwarded	=0;	
	callInfo.iPrivacy				= RMobilePhone::EPrivacyUnspecified;
	callInfo.iAlternatingCall		= RMobilePhone::EAlternatingModeUnspecified;	
	//callInfo.iDuration.iIntervel	= 0;			// this is protected value
	callInfo.iTch = RMobileCall::ETchUnknown;				
	callInfo.iRemoteParty.iCallingName = KNullDesC;
	callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
	callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
	callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
	callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
	callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
	callInfo.iDialledParty.iNumberPlan = 
        RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
        RMobilePhone::EUnknownNumber;	
	    
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 120101);
    iMockLTSY.ExpectL(EEtelCallDial, expectData, KErrNone);    
    
    // Cause to fail to show defect 120101
    ASSERT_TRUE(EFalse);
    
    call.DialNoFdnCheck(requestStatus, KSomeNumber);    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    
        
    CleanupStack::PopAndDestroy(4, this); // call,line, expectData, this    
	}

/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for voice calls with minimal parameters (RCall::TCallParams)
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0001cL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   
    _LIT(KAlphaID, "Alpha ID");

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expecteCallId = 0;

    RMobileCall::TMobileCallParamsV7 callParams; 
    RMobileCall::TMobileCallParamsV7Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    
    callParams.iAutoRedial = EFalse;
    callParams.iBearerMode = RMobileCall::EMulticallNewBearer;
    callParams.iCallParamOrigin = RMobileCall::EOriginatorSIM;   
    callParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;      
    callParams.iIconId.iQualifier = RMobileCall::ENoIconId;   
    callParams.iAlphaId = KAlphaID;

    
    
    
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    //callInfo.iDuration.iIntervel  = 0;            // this is protected value
    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = 
            RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
            RMobilePhone::EUnknownNumber;   

    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);

    RCall::TCallParams callParamsX;
    callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParamsX.iInterval        = 100;
    callParamsX.iWaitForDialTone = RCall::EDialToneWait;
    TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
        
    
    TPckg<RMobileCall::TMobileCallParamsV7> mmParamsPckgV7(callParams);
    call.DialNoFdnCheck(requestStatus, mmParamsPckgV7, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    RMobileCall::TMobileCallInfoV7 info;
    RMobileCall::TMobileCallInfoV7Pckg  infoPckg(info);
        
    TInt ret = call.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);
    
    ASSERT_TRUE(info.iAlphaId.Compare(KAlphaID)==KErrNone);
    
    CleanupStack::PopAndDestroy(5,this);
    
    }

/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0001d
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for voice calls with extended parameters (RMobileCall::TCallParamsV7)
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0001dL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   
    // Alpha ID to pass through TMobileCallParamsV7
    _LIT(KAlphaID, "Alpha ID");

    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expecteCallId = 0;

    RMobileCall::TMobileCallParamsV7 callParams; 
    RMobileCall::TMobileCallParamsV7Pckg    pckgCallParams(callParams); 
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    
    callParams.iAutoRedial = EFalse;
    callParams.iBearerMode = RMobileCall::EMulticallNewBearer;
    callParams.iCallParamOrigin = RMobileCall::EOriginatorSIM;   
    callParams.iBCRepeatIndicator = RMobileCall::EBCAlternateMode;      
    callParams.iIconId.iQualifier = RMobileCall::ENoIconId;   
    callParams.iIconId.iIdentifier = 123;   
    callParams.iAlphaId = KAlphaID;

    // Create the basic TMobileCallInfoV3 object to pass back
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    //callInfo.iDuration.iIntervel  = 0;            // this is protected value
    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = 
            RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
            RMobilePhone::EUnknownNumber;   

    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV7, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);
    
    TPckg<RMobileCall::TMobileCallParamsV7> mmParamsPckgV7(callParams);
    call.DialNoFdnCheck(requestStatus, mmParamsPckgV7, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    RMobileCall::TMobileCallInfoV7 info;
    RMobileCall::TMobileCallInfoV7Pckg  infoPckg(info);
        
    TInt ret = call.GetMobileCallInfo(infoPckg);
    ASSERT_EQUALS(KErrNone, ret);
    
    // The check to make sure that the AlphaID persisted in CTSY and returned properly.
    ASSERT_TRUE(info.iAlphaId.Compare(KAlphaID)==KErrNone);
    // The check to make sure that the IconID persisted in CTSY and returned properly.
    ASSERT_TRUE(info.iIconId.iIdentifier == 123);
    ASSERT_EQUALS(info.iIconId.iQualifier, RMobileCall::ENoIconId);
    
    CleanupStack::PopAndDestroy(5,this);
    
    }

/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::DialNoFdnCheck for voice calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0002L()
	{

     OpenEtelServerL(EUseExtendedError);
     CleanupStack::PushL(TCleanupItem(Cleanup,this));
     OpenPhoneL();

     RBuf8 expectData;
     CleanupClosePushL(expectData);
     
     RBuf8 completeData;
     CleanupClosePushL(completeData);
     
     TInt errorCode = KErrNone;    
     
     //-- For Voice1 -------------------------
         
     TBuf<256> lineName(KMmTsyVoice1LineName);    
     // Open new line
     RLine line;
     errorCode = line.Open(iPhone, lineName);
     ASSERT_EQUALS(KErrNone, errorCode);
     CleanupClosePushL(line);      
     // open call
     _LIT(KDoubleColon, "::");    
     TBuf<256> name;
     name = KMmTsyPhoneName;
     name.Append(KDoubleColon);
     name.Append(lineName);
     name.Append(KDoubleColon);
     
     RMobileCall call;
     errorCode = call.OpenNewCall(line, name);
     ASSERT_EQUALS(KErrNone, errorCode);
     CleanupClosePushL(call);   
     
     TRequestStatus requestStatus;    
     _LIT(KSomeNumber, "123456789");
     
     RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;
     
     TInt expectCallId = 0;
 	   
    // --------  make for this call callId > 0 --------------------------------------   
    RMobileCall::TMobileCallInfoV8 callInfo;  

    //-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::DialNoFdnCheck
 	//-------------------------------------------------------------------------
 	
    // data for ExpextL
 	RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams); 	
 	 	
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    = -1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo.iTch = RMobileCall::ETchUnknown;               
    callInfo.iRemoteParty.iCallingName = KNullDesC;
    callInfo.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
    callInfo.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
    callInfo.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = 
            RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
            RMobilePhone::EUnknownNumber;   
	
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData(expectCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    // Expect the dial
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData, KErrNone);   
     
    // Dial and cancel
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    call.CancelAsyncRequest(EMobileCallDialNoFdnCheck);
    
    // prepare and complete Mobile Call info
    
    TInt callId = 1;
    
    _LIT(KNullDesC , "");
        
    RBuf8 data;
    CleanupClosePushL(data);

    RMobileCall::TMobileCallInfoV1 callInfo2;
    callInfo2.iDialledParty.iTelNumber.Copy(KSomeNumber);
    callInfo2.iService = mobileService;
    callInfo2.iEmergency = EFalse;
    callInfo2.iRemoteParty.iCallingName = KNullDesC;
    callInfo2.iRemoteParty.iRemoteIdStatus = RMobileCall::ERemoteIdentityUnknown;
    callInfo2.iRemoteParty.iRemoteNumber.iTelNumber = KNullDesC;
    callInfo2.iRemoteParty.iRemoteNumber.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iRemoteParty.iRemoteNumber.iTypeOfNumber = RMobilePhone::EUnknownNumber;
    callInfo2.iForwarded = EFalse;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallRemoteParty;

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo2);
    callInfoData.SerialiseL(data);

    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    //Pause the MockLTSY so we can program the sequence of Expect - Completes involved 
    //with cancelling 
    iMockLTSY.PauseCompletion();
    
    RMobileCall::TMobileCallStatus  mobileCallStatus = RMobileCall::EStatusDialling; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus(callId, mobileService, mobileCallStatus);
    completeData.Close();
    mockCallDataStatus.SerialiseL(completeData);
    
    //Change state to dialling, this set the call ID
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);

    
    TInt hangUpCause = KErrGsmReleaseByUser;
    TBool autoStChangeDisable = ETrue;
    TMockLtsyCallData2<TInt, TBool> mockCallData2(callId, mobileService, 
                                                          hangUpCause, 
                                                          autoStChangeDisable);    
    expectData.Close();
    mockCallData2.SerialiseL(expectData);
    // Since the call ID has been set, the CTSY will go ahead with the cancellation
    // by hanging up.
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
    // Cancelled Dial request. After this CTSY hangs up the call and it goes to idle state.
   
    
    RMobileCall::TMobileCallStatus  mobileCallStatus2 = RMobileCall::EStatusIdle; 
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallDataStatus2(callId, mobileService, mobileCallStatus2);
    completeData.Close();
    mockCallDataStatus2.SerialiseL(completeData);

    // Complete the hangup
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrGsmReleaseByUser, completeData);
 
    // Release the MockLTSY so that the above sequence is triggered.
    iMockLTSY.ResumeCompletion();
    
    
    // Check results.
    User::WaitForRequest(requestStatus);    
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());      

     
	CleanupStack::PopAndDestroy(6, this); // expectData, this ...
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::DialNoFdnCheck for voice calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);	
	
	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	RBuf8 expectData2;
	CleanupClosePushL(expectData2);
	
	RBuf8 completeData;
	CleanupClosePushL(completeData);
	
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);
		
	
	 //-- Voice1 -------------------------    
    TBuf<256> lineName(KMmTsyVoice1LineName);    
    // Open new line
    RLine line;
    ret  = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(line);  
    
    
    //-- Voice1-------------------------
    TBuf<256> lineName2(KMmTsyVoice1LineName);   
    // Open new line
    RLine line2;
    ret  = line2.Open(phone2, lineName2);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(line2);  
    //----------------------------------------
    
    // open call
    _LIT(KDoubleColon, "::");
    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
	RMobileCall call;
	RMobileCall call2;
	
	TInt callId = 1;
	TInt callId2 = 2;
	TName incomingCallName;
	TName incomingCallName2;
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    TRequestStatus mockLtsyStatus;
			
    RMobileCall::TMobileCallStatus mobileCallStatus = RMobileCall::EStatusIdle;    
    RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;

    // create new incoming call
	ret = CreateIncomingCallL(line, callId, lineName, incomingCallName, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, ret);	
	 // --------  make for the first call callId > 0 --------------------------------------   
    RMobileCall::TMobileCallInfoV1 callInfo;  
            
	line.NotifyIncomingCall(requestStatus, incomingCallName);

    callInfo.iService = mobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> idleCallData(callId, mobileService, callInfo);
    idleCallData.SerialiseL(completeData);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
  
    TInt errorCode = call.OpenExistingCall(line, incomingCallName);
    ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(call);
 

    // create new incoming call
	ret = CreateIncomingCallL(line2, callId2, lineName2, incomingCallName2, 
	                               mobileService, mobileCallStatus);	
	ASSERT_EQUALS(KErrNone, ret); 	  
    // -------  make for the second call callId > 0 --------------------------------------   
    RMobileCall::TMobileCallInfoV1 callInfo2;    
           
	line2.NotifyIncomingCall(requestStatus2, incomingCallName2);

    callInfo2.iService = mobileService;
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> idleCallData2(callId2, mobileService, callInfo2);
    idleCallData2.SerialiseL(completeData2);
	
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EEtelLineNotifyIncomingCall, KErrNone, completeData2);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    User::WaitForRequest(requestStatus2);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    
   
    errorCode = call2.OpenExistingCall(line2, incomingCallName2);
    ASSERT_EQUALS(KErrNone, errorCode);
	CleanupClosePushL(call2);
	//------------------------------------------------------------------------- 	
	
	_LIT(KSomeNumber,  "123456789"); 
	_LIT(KSomeNumber2, "987654321"); 	

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::GetCallParams
 	//-------------------------------------------------------------------------
 	
 	// --- data for first ExpectL  ---
 	RMobileCall::TMobileCallParamsV1 		callParams; 	
 	RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);	
 	
 	// set up callInfo
 	callInfo.iRemoteParty.iDirection = RMobileCall::EMobileTerminated;  
 	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
	callInfo.iService	= mobileService;  
	callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber ); 	
 	TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData2(callId, mobileService, callParams, callInfo);
    mockCallData2.SerialiseL(expectData);
    
 	// --- data for the first CompleteL ---
 	TMockLtsyCallData0 mockDataComplete(callId, mobileService);
 	completeData.Close();
 	mockDataComplete.SerialiseL(completeData);
    	
  	// --- data for second ExpectL  ---
    RMobileCall::TMobileCallParamsV1 		callParams2; 	
 	RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams2(callParams2);	
 	
 	// set up callInfo
 	callInfo2.iRemoteParty.iDirection = RMobileCall::EMobileTerminated;  
 	callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
	callInfo2.iService	= mobileService;  
	callInfo2.iDialledParty.iTelNumber.Copy( KSomeNumber2 ); 	
 	TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData3(callId2, mobileService, callParams2, callInfo2);
    mockCallData3.SerialiseL(expectData2);
    
 	// --- data for the second CompleteL ---
 	TMockLtsyCallData0 mockDataComplete2(callId2, mobileService);
 	completeData2.Close();
 	mockDataComplete2.SerialiseL(completeData2);
         
   
  	// first DialNoFdnCheck
  	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);   	
  	iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);   
      
   	// second DialNoFdnCheck
   	iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData2);   	
  	iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData2);   
    call2.DialNoFdnCheck(requestStatus2, pckgCallParams2, KSomeNumber2);   
    
    //wait for the first answer    
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    
    //wait for the second answer    
    User::WaitForRequest(requestStatus2);              
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    
    
    
    AssertMockLtsyStatusL();   
	// Done !
	CleanupStack::PopAndDestroy(12, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck with timeout for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck and tests for timeout for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expectData;
	CleanupClosePushL(expectData);
	
	TBuf<256> lineName(KMmTsyVoice1LineName);
    
    // Open new line
    RLine line;
    TInt errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);  
    
    // open call
    _LIT(KDoubleColon, "::");
    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);    
    
    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::GetCallParams
 	//-------------------------------------------------------------------------
	// data for ExpectL
	RMobilePhone::TMobileService mobileService = RMobilePhone::EVoiceService;    
    TInt expecteCallId = 0;
        
    RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV1 callInfo;   
   
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
	callInfo.iService	= mobileService;
	callInfo.iStatus	= RMobileCall::EStatusUnknown;
	callInfo.iCallId	= -1;
	callInfo.iExitCode	=0; 
	callInfo.iEmergency	=0;
	callInfo.iForwarded	=0;	
	callInfo.iPrivacy				= RMobilePhone::EPrivacyUnspecified;
	callInfo.iAlternatingCall		= RMobilePhone::EAlternatingModeUnspecified;	
	//callInfo.iDuration.iIntervel	= 0;			// this is protected value
	callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
	callInfo.iDialledParty.iNumberPlan = 
        RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
        RMobilePhone::EUnknownNumber;	
	    
    TMockLtsyCallData2< RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1 >
    											 mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);    
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());      
   
	// Done !
	CleanupStack::PopAndDestroy(4, this); //... this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    TInt errorCode = KErrNone;    
    
    //-- For Data -------------------------    
    TBuf<256> lineName(KMmTsyDataLineName);
    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);  
    
    // open call
    _LIT(KDoubleColon, "::");
    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);    
    
    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");    
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialNoFdnCheck when result is not cached.
 	//-------------------------------------------------------------------------
    RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
   
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);    
    
    //  DialNoFdnCheck isn't supported for Data Line (or rather, it's broken)  
    ASSERT_EQUALS(KErrNone, requestStatus.Int());      
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ... this	
	}

/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0006b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for data calls with minimal parameters (RCall::TCallParams)
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0006bL()
    {
    
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt errorCode = KErrNone;    

    //-- For Voice1 -------------------------

    TBuf<256> lineName(KMmTsyDataLineName);    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);      
    // open call
    _LIT(KDoubleColon, "::");    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);

    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);   

    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");   

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;    
    TInt expecteCallId = 0;

    RMobileCall::TMobileDataCallParamsV8 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
    RMobileCall::TMobileCallInfoV8 callInfo;   
    callParams.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParams.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParams.iInterval        = 100;
    callParams.iWaitForDialTone = RCall::EDialToneWait;
    callParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
    callParams.iCug.iExplicitInvoke = EFalse;
    callParams.iCug.iCugIndex = 0xFFFF;
    callParams.iCug.iSuppressPrefCug = EFalse;
    callParams.iCug.iSuppressOA = EFalse;
    callParams.iAutoRedial = EFalse;
    callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo.iService   = mobileService;
    callInfo.iStatus    = RMobileCall::EStatusUnknown;
    callInfo.iCallId    =-1;
    callInfo.iExitCode  =0; 
    callInfo.iEmergency =0;
    callInfo.iForwarded =0; 
    callInfo.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    //callInfo.iDuration.iIntervel  = 0;            // this is protected value
    callInfo.iDialledParty.iTelNumber.Copy( KSomeNumber );
    callInfo.iDialledParty.iNumberPlan = 
            RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = 
            RMobilePhone::EUnknownNumber;   

    TMockLtsyCallData2< RMobileCall::TMobileDataCallParamsV8, RMobileCall::TMobileCallInfoV8 >
    mockCallData(expecteCallId, mobileService, callParams, callInfo);
    mockCallData.SerialiseL(expectData);
    
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);

    TInt callId = 1;
    
    completeData.Close();
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> callInfoData(callId, mobileService, callInfo);
    callInfoData.SerialiseL(completeData);
    //Complete Mobile Call Info in order to set the call ID 
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, completeData);
    
    TMockLtsyCallData0 mockDataComplete(callId, mobileService);
    completeData.Close();
    mockDataComplete.SerialiseL(completeData);
    // Complete the Dial
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);

    RCall::TCallParams callParamsX;
    callParamsX.iSpeakerControl  = RCall::EMonitorSpeakerControlOnUntilCarrier;
    callParamsX.iSpeakerVolume   = RCall::EMonitorSpeakerVolumeLow;
    callParamsX.iInterval        = 100;
    callParamsX.iWaitForDialTone = RCall::EDialToneWait;
    TPckg<RCall::TCallParams> pckgCallParamsX(callParamsX);
        
    call.DialNoFdnCheck(requestStatus, pckgCallParamsX, KSomeNumber);   
        
    User::WaitForRequest(requestStatus);          
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    AssertMockLtsyStatusL();   
    
    CleanupStack::PopAndDestroy(5,this);
    
    }



/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::DialNoFdnCheck for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::DialNoFdnCheck for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0007L()
	{
//  
// DialNoFdnCheck isn't supported for Data Line, so this test is not relevant
//
	
	
// This test should test cancellation of GetCallParams
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::GetCallParams
 	//-------------------------------------------------------------------------
 	
	// Remove the test above if this is not relevant for this API

	// Wait for completion of iMockLTSY.NotifyTerminated
	// User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0008L()
	{
//	
// DialNoFdnCheck isn't supported for Data Line, so this test is not relevant
//
	
// This test should test sending bad parameter data for GetCallParams
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// Remove the test above if it is not relevant for this API

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// Remove the test above if it is not relevant for this API
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------
 	
 	// Remove the test above if it is not relevant for this API

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::DialNoFdnCheck for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::DialNoFdnCheck for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck0009L()
	{
//  
// DialNoFdnCheck isn't supported for Data Line, so this test is not relevant
//
					
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
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.


	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck with timeout for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck and tests for timeout for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00010L()
	{
//  
// DialNoFdnCheck isn't supported for Data Line, so this test is not relevant
//


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// If this API does not have a timeout, then remove this test completely.

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
    
    TInt errorCode = KErrNone;    
    
    //-- For Fax -------------------------    
    TBuf<256> lineName(KMmTsyFaxLineName);
    
    // Open new line
    RLine line;
    errorCode = line.Open(iPhone, lineName);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(line);  
    
    // open call
    _LIT(KDoubleColon, "::");
    
    TBuf<256> name;
    name = KMmTsyPhoneName;
    name.Append(KDoubleColon);
    name.Append(lineName);
    name.Append(KDoubleColon);
    
    RMobileCall call;
    errorCode = call.OpenNewCall(line, name);
    ASSERT_EQUALS(KErrNone, errorCode);
    CleanupClosePushL(call);    
    
    TRequestStatus requestStatus;    
    _LIT(KSomeNumber, "123456789");    
	
	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::DialNoFdnCheck when result is not cached.
 	//-------------------------------------------------------------------------
    RMobileCall::TMobileCallParamsV1 callParams; 
    RMobileCall::TMobileCallParamsV1Pckg    pckgCallParams(callParams);
   
    call.DialNoFdnCheck(requestStatus, pckgCallParams, KSomeNumber);
    
    User::WaitForRequest(requestStatus);    
    
    //  DialNoFdnCheck isn't supported for Fax Line
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());      
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // ... this	
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::DialNoFdnCheck for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::DialNoFdnCheck for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00012L()
	{
//  
// DialNoFdnCheck isn't supported for fax calls, so this test is not relevant
//
	
// This test should test cancellation of GetCallParams
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::GetCallParams
 	//-------------------------------------------------------------------------
 	
	// Remove the test above if this is not relevant for this API

	// Wait for completion of iMockLTSY.NotifyTerminated
	// User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00013L()
	{
//  
// DialNoFdnCheck isn't supported for fax calls, so this test is not relevant
//
	
// This test should test sending bad parameter data for GetCallParams
// If this API does not have any parameters, then remove this test completely.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test passing wrong version of parameters to
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// Remove the test above if it is not relevant for this API

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// Remove the test above if it is not relevant for this API
	
	//-------------------------------------------------------------------------
	// Test C: Test passing out of bounds parameters to
	// RCall::GetCallParams
 	//-------------------------------------------------------------------------
 	
 	// Remove the test above if it is not relevant for this API

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::DialNoFdnCheck for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::DialNoFdnCheck for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00014L()
	{
//  
// DialNoFdnCheck isn't supported for fax calls, so this test is not relevant
//
					
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
	ret = phone2.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

	// To write this test:
	// If this API has parameters which have different versions, then each
	// client in this test should use a different version of parameter.
	// Otherwise, they should use the same version.
	// All versions of parameters should be used. i.e. If there are three
	// versions of a parameter, there should be three clients, one using
	// each one. If the parameter does not have different versions, use
	// two clients each using the same version.


	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-CDNFC-00015
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::DialNoFdnCheck with timeout for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::DialNoFdnCheck and tests for timeout for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestDialNoFdnCheck00015L()
	{
//  
// DialNoFdnCheck isn't supported for fax calls, so this test is not relevant
//

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RCall::GetCallParams
 	//-------------------------------------------------------------------------

	// If this API does not have a timeout, then remove this test completely.

	// Done !
	CleanupStack::PopAndDestroy(2, this); // data, this

	}

/**
@SYMTestCaseID BA-CTSY-FXD-MGFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetFdnStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetFdnStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestGetFdnStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;
    	
	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus, KErrNotSupported);
	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());    


	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus, KErrNotSupported);
    TInt ret = iPhone.GetFdnStatus(fdnStatus);
    ASSERT_EQUALS(KErrNotSupported, ret);    

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    RMobilePhone::TMobilePhoneFdnStatus fdnStatusComplete(RMobilePhone::EFdnActive);
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnStatus> ltsyData2(fdnStatusComplete);
	ltsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrGeneral, completeData);
	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());    

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetFdnStatus when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrNone, completeData);
	
	fdnStatus = RMobilePhone::EFdnNotActive;
	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    ASSERT_TRUE(fdnStatusComplete == fdnStatus);    


	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrNone, completeData);

	fdnStatus = RMobilePhone::EFdnNotActive;
	
    ret = iPhone.GetFdnStatus(fdnStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, ret);    
    ASSERT_TRUE(fdnStatusComplete == fdnStatus);    

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetFdnStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);


	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // completeData, this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-MGFS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetFdnStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetFdnStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestGetFdnStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

    RBuf8 completeData;
    CleanupClosePushL(completeData);
	
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus(RMobilePhone::EFdnNotActive);
    	
    RMobilePhone::TMobilePhoneFdnStatus fdnStatusComplete(RMobilePhone::EFdnActive);
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnStatus> ltsyData2(fdnStatusComplete);
	ltsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrNone, completeData);
	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneGetFdnStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());    

	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-MGFS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetFdnStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetFdnStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestGetFdnStatus0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetFdnStatus
 	//-------------------------------------------------------------------------

	// If this API is flow controlled by Etel or if this API is
	// synchronous, remove this test completely.

    RBuf8 completeData;
    CleanupClosePushL(completeData);
	

    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus(RMobilePhone::EFdnNotActive);
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus2(RMobilePhone::EFdnNotActive);
    	
    RMobilePhone::TMobilePhoneFdnStatus fdnStatusComplete(RMobilePhone::EFdnActive);
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnStatus> ltsyData2(fdnStatusComplete);
	ltsyData2.SerialiseL(completeData);

	iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetFdnStatus, KErrNone, completeData);
	
    iPhone.GetFdnStatus(requestStatus, fdnStatus);

    phone2.GetFdnStatus(requestStatus2, fdnStatus2);

    User::WaitForRequest(requestStatus);

    User::WaitForRequest(requestStatus2);
   
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    ASSERT_EQUALS(KErrServerBusy, requestStatus2.Int());    

	// Done !
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}

/**
@SYMTestCaseID BA-CTSY-FXD-MGFS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetFdnStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetFdnStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestGetFdnStatus0005L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    //-------------------------------------------------------------------------
    // Test A: Test timeout of RMobilePhone::SetFdnSetting
    //-------------------------------------------------------------------------
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus fdnStatus;

    iMockLTSY.ExpectL(EMobilePhoneGetFdnStatus);
    
    iPhone.GetFdnStatus(requestStatus, fdnStatus);
    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());    

    CleanupStack::PopAndDestroy(1, this); // data, this
    }




/**
@SYMTestCaseID BA-CTSY-FXD-MSFS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetFdnSetting
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetFdnSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestSetFdnSetting0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);
    	
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrGeneral);
	
    iPhone.SetFdnSetting(requestStatus, fdnSetting);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrGeneral, requestStatus.Int());    


 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::SetFdnSetting when result is not cached.
 	//-------------------------------------------------------------------------


    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    //to check what will be set
    iPhone.NotifyFdnStatusChange(requestStatus2, fdnStatus);

    fdnSetting = RMobilePhone::EFdnSetOff;
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
	
    iPhone.SetFdnSetting(requestStatus, fdnSetting);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnNotActive, fdnStatus);    


    iPhone.NotifyFdnStatusChange(requestStatus2, fdnStatus);

    fdnSetting = RMobilePhone::EFdnSetOn;
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
	
    iPhone.SetFdnSetting(requestStatus, fdnSetting);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnActive, fdnStatus);    

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::SetFdnSetting
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
	User::WaitForRequest(mockLtsyStatus);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-MSFS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::SetFdnSetting
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::SetFdnSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestSetFdnSetting0002L()
	{

// This test should test cancellation of SetFdnSetting
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::SetFdnSetting
 	//-------------------------------------------------------------------------
 	
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);
	
    iPhone.SetFdnSetting(requestStatus, fdnSetting);
    
    iPhone.CancelAsyncRequest(EMobilePhoneSetFdnSetting);

	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);

    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();

        //TSY has started a request and it is not possible to then
        //cancel this request. The best thing for the TSY to do in this case 
        //is to proceed as though the Cancel never happened. The server's call 
        //to the TSY cancel function will return synchronously. The TSY then 
        //continues to wait for the original acknowledgement and when it 
        //receives it, the TSY will complete the original request.
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // this
	
	}




/**
@SYMTestCaseID BA-CTSY-FXD-MSFS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::SetFdnSetting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::SetFdnSetting
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestSetFdnSetting0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::SetFdnSetting
 	//-------------------------------------------------------------------------
    TRequestStatus requestStatus1;
    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneFdnSetting  fdnSetting1(RMobilePhone::EFdnSetOff);
    RMobilePhone::TMobilePhoneFdnSetting  fdnSetting2(RMobilePhone::EFdnSetOn);
    iPhone.SetFdnSetting(requestStatus1, fdnSetting1);
    phone2.SetFdnSetting(requestStatus2, fdnSetting2);

	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);

    User::WaitForRequest(requestStatus1);
    ASSERT_EQUALS(KErrNone, requestStatus1.Int());    

	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    

	// Done !
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-FXD-MSFS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetFdnSetting with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetFdnSetting and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestSetFdnSetting0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::SetFdnSetting
 	//-------------------------------------------------------------------------
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);

	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 120302);
	// assert added because request stays uncompleted and this prevents later test from correct execution
	ASSERT_TRUE(EFalse);
	
    iPhone.SetFdnSetting(requestStatus, fdnSetting);

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    

	// Done !
	CleanupStack::PopAndDestroy(1, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-FXD-MNFSC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyFdnStatusChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyFdnStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestNotifyFdnStatusChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    RMobilePhone::TMobilePhoneFdnSetting fdnSetting(RMobilePhone::EFdnSetOn);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::NotifyFdnStatusChange when result is not cached.
 	//-------------------------------------------------------------------------

    iPhone.NotifyFdnStatusChange(requestStatus, fdnStatus);
    
    TRequestStatus requestStatus2;
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
    iPhone.SetFdnSetting(requestStatus2, fdnSetting);
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnActive, fdnStatus);    
	AssertMockLtsyStatusL();


    
    fdnSetting = RMobilePhone::EFdnSetOff;

    iPhone.NotifyFdnStatusChange(requestStatus, fdnStatus);
    
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
    iPhone.SetFdnSetting(requestStatus2, fdnSetting);
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnNotActive, fdnStatus);    

    // test E not needed cause complete for EMobilePhoneNotifyFdnStatusChange 
    // is called only in EMobilePhoneSetFdnSetting request

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-FXD-MNFSC-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyFdnStatusChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyFdnStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestNotifyFdnStatusChange0002L()
	{

// This test should test cancellation of NotifyFdnStatusChange
// If this API does not have a cancel, the test step should be completely removed.

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyFdnStatusChange
 	//-------------------------------------------------------------------------
    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    iPhone.NotifyFdnStatusChange(requestStatus, fdnStatus);
 	
 	iPhone.CancelAsyncRequest(EMobilePhoneNotifyFdnStatusChange);

    RMobilePhone::TMobilePhoneFdnSetting fdnSetting(RMobilePhone::EFdnSetOn);
    TRequestStatus requestStatus2;
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
    iPhone.SetFdnSetting(requestStatus2, fdnSetting);
    
    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrCancel, requestStatus.Int());    

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	CleanupStack::PopAndDestroy(1); // data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-FXD-MNFSC-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyFdnStatusChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyFdnStatusChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyFixedDiallingFU::TestNotifyFdnStatusChange0004L()
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::NotifyFdnStatusChange
 	//-------------------------------------------------------------------------
    TRequestStatus requestStatus;
    TRequestStatus requestStatus2;
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus2(RMobilePhone::EFdnUnknown);
    
    iPhone.NotifyFdnStatusChange(requestStatus, fdnStatus);
    phone2.NotifyFdnStatusChange(requestStatus2, fdnStatus2);

    RMobilePhone::TMobilePhoneFdnSetting fdnSetting(RMobilePhone::EFdnSetOff);
    TRequestStatus requestStatus3;
	iMockLTSY.CompleteL(EMobilePhoneSetFdnSetting, KErrNone);
    iPhone.SetFdnSetting(requestStatus3, fdnSetting);
    
    User::WaitForRequest(requestStatus3);
    ASSERT_EQUALS(KErrNone, requestStatus3.Int());    

    User::WaitForRequest(requestStatus);
    ASSERT_EQUALS(KErrNone, requestStatus.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnNotActive, fdnStatus);    

    User::WaitForRequest(requestStatus2);
    ASSERT_EQUALS(KErrNone, requestStatus2.Int());    
    ASSERT_EQUALS(RMobilePhone::EFdnNotActive, fdnStatus2);    


	// Done !
	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}




