// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The TEFUnit test suite for TelephonyAudioControl in the Common TSY.
// 
//

#include "cctsytelephonyaudiocontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <ctsy/mmtsy_names.h>
#include <et_clsvr.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <e32property.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/pluginapi/mtelephonyaudiocontrol.h>

_LIT( KPhoneNumber, "101632960000" );  

CTestSuite* CCTsyTelephonyAudioControlFU::CreateSuiteL(const TDesC& aName)
    {
    SUB_SUITE;

    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestSetupTelephonyAudio0001aL);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestSetupTelephonyAudio0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestSetupTelephonyAudio0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestTeardownTelephonyAudio0001aL);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestTeardownTelephonyAudio0001bL);    
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestTeardownTelephonyAudio0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestCallStateChange0001aL);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestCallStateChange0001bL);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestCallStateChange0004L);
    ADD_TEST_STEP_ISO_CPP(CCTsyTelephonyAudioControlFU, TestNotifyTelephonyAudioControlError0001L );

    END_SUITE;
    }

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-CRC-CRCSUCR-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::SetUpCallRouting
@SYMTestPriority High
@SYMTestActions Invokes MTelephonyAudioControl::SetUpCallRouting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */   
void CCTsyTelephonyAudioControlFU::TestSetupTelephonyAudio0001aL()
    {
    DefinePropeties( ETrue, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( EFalse );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::SetUpCallRouting when result is not cached.
    //-------------------------------------------------------------------------

    // DialL

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that values passed to the SetupCallRouting were valid
    CheckCallRoutingValues( line, call );

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // DialNoFdnCheck 

    // reset line and call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);
    completeData.Close();
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);

    // make the call    
    call.DialNoFdnCheck( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that values passed to the SetupCallRouting were valid
    CheckCallRoutingValues( line, call ); 

    // cleanup 
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );    
    AssertMockLtsyStatusL();

    // DialISV

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line and call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    completeData.Close();
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    call.DialISV( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that values passed to the SetupCallRouting were valid
    CheckCallRoutingValues( line, call ); 

    // cleanup 
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    // Ghost Call

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // order NotifyStatusChange event
    RCall::TStatus callStatus( RCall::EStatusIdle );  
    line.NotifyStatusChange( status, callStatus );

    // prepare MockLTSY for NotifyStatusChange event
    iMockLTSY.NotifyTerminated(mockLtsyStatus);       
    RMobileCall::TMobileCallInfoV1 mobileCallInfo;
    mobileCallInfo.iCallId = callId;
    mobileCallInfo.iService = mobileService;
    mobileCallInfo.iStatus = RMobileCall::EStatusDialling;
    mobileCallInfo.iCallName.Copy( _L8("Voice12") );

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData1(callId, mobileService, mobileCallInfo);
    mockCallData1.SerialiseL(data);        
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);    

    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    User::WaitForRequest(status);
    ASSERT_EQUALS(KErrNone, status.Int());    
    ASSERT_EQUALS(RCall::EStatusDialling, callStatus);   

    error = call.OpenExistingCall( line, mobileCallInfo.iCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );  

    data.Close();
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    call.NotifyMobileCallStatusChange( status, mobileCallStatus );  
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() ); 

    AssertMockLtsyStatusL();

    CheckCallStatus( mobileCallInfo.iCallName, RMobileCall::EStatusConnecting );  

    // prepare MockLTSY for hangup
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL( EEtelCallHangUp, expectData, KErrNone );

    // hangup the call by popping the call from the cleanupstack
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    

    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();    

    // DialEmergencyCall

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line and call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // set rf on
    iMockLTSY.NotifyTerminated( mockLtsyStatus );    
    TRfStateInfo rfInfo = ERfsStateInfoNormal;    
    TMockLtsyData1<TRfStateInfo> mockData1( rfInfo );
    expectData.Close();
    mockData1.SerialiseL(expectData);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // the emergency number
    RMobileENStore::TEmergencyNumber number(_L("911"));
    TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData( number );    

    CallGetMobileCallInfoL(callId, mobileService, number );

    // prepare MockLTSY for EMobileCallDialEmergencyCall
    data.Close();
    numberLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData( callId, mobileService, callInfo );

    // send EStatusDialling, EStatusConnecting and EStatusConnected events to complete DialEmergencyCall
    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    mobileCallStatus = RMobileCall::EStatusDialling;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    // On return, KErrNone if the emergency call successfully reaches the connected state.
    call.DialEmergencyCall( status, number );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that values passed to the SetupCallRouting were valid
    CheckCallRoutingValues( line, call ); 

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    // hangup the call by popping the call from the cleanupstack
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    

    // cleanup
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();   

    // answer incoming call

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    TName incomingCallName;
    TRequestStatus requestNotify( KErrNone );
    line.NotifyIncomingCall( status, incomingCallName);

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );

    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );

    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    User::WaitForRequest( requestNotify );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, requestNotify.Int() );

    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that values passed to the SetupCallRouting were valid
    CheckCallRoutingValues( line, call ); 

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    // hangup the call by popping the call from the cleanupstack
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    

    // cleanup
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL(); 

    // cleanup 
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( this );
    }


/**
@SYMTestCaseID BA-CTSY-CRC-CRCSUCR-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::SetUpCallRouting by using LTSY version1
@SYMTestPriority High
@SYMTestActions Test that call routing is not set up
@SYMTestExpectedResults Pass
@SYMTestType CT
 */ 
void CCTsyTelephonyAudioControlFU::TestSetupTelephonyAudio0001bL()
    {
    DefinePropeties( ETrue, EVersion1 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    RBuf8 data;
    CleanupClosePushL(data);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::SetUpCallRouting when result is not cached.
    //-------------------------------------------------------------------------

    // Test that CMockLtsyFactoryV1 is used instead of CMockLtsyFactoryV2

    // DialL

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // DialNoFdnCheck 

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion1 );

    // reset line and call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY

    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    expectData.Close();
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EMobileCallDialNoFdnCheck, expectData);
    completeData.Close();
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EMobileCallDialNoFdnCheck, KErrNone, completeData);

    // make the call    
    call.DialNoFdnCheck( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup 
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );    
    AssertMockLtsyStatusL();

    // DialEmergencyCall

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion1 );

    // reset line and call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // set rf on
    TRequestStatus mockLtsyStatus(KErrNone);
    iMockLTSY.NotifyTerminated( mockLtsyStatus );    
    TRfStateInfo rfInfo = ERfsStateInfoNormal;    
    TMockLtsyData1<TRfStateInfo> mockData1( rfInfo );
    expectData.Close();
    mockData1.SerialiseL(expectData);
    iMockLTSY.CompleteL(EMmTsyBootGetRFStatusIPC, KErrNone, expectData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // the emergency number
    RMobileENStore::TEmergencyNumber number(_L("911"));
    TMockLtsyData1<RMobileENStore::TEmergencyNumber> numberLtsyData( number );    

    CallGetMobileCallInfoL(callId, mobileService, number );

    // prepare MockLTSY for EMobileCallDialEmergencyCall
    data.Close();
    numberLtsyData.SerialiseL(data);
    iMockLTSY.ExpectL(EMobileCallDialEmergencyCall, data, KErrNone);

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> completeCallInfoData( callId, mobileService, callInfo );

    // send EStatusDialling, EStatusConnecting and EStatusConnected events to complete DialEmergencyCall
    data.Close();
    completeCallInfoData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallGetMobileCallInfo, KErrNone, data);

    RMobileCall::TMobileCallStatus mobileCallStatus(RMobileCall::EStatusDialling);
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnecting;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    data.Close();
    mobileCallStatus = RMobileCall::EStatusConnected;
    completeCallStatusData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, data);

    // On return, KErrNone if the emergency call successfully reaches the connected state.
    call.DialEmergencyCall( status, number );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    // prepare MockLTSY for hangup 
    expectData.Close();
    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( EFalse );
    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    // hangup the call by popping the call from the cleanupstack
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    

    // cleanup
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();  

    // cleanup 
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( this );
    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCSUCR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to MTelephonyAudioControl::SetUpCallRouting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to MTelephonyAudioControl::SetUpCallRouting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestSetupTelephonyAudio0004L()
    {
    DefinePropeties( EFalse, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting MTelephonyAudioControl::SetUpCallRouting
    //-------------------------------------------------------------------------

    TInt callId1( 1 );
    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId1, mobileService);

    // reset line & call
    RLine line;
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    RMobileCall call;
    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    TRequestStatus status( KErrNone );
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // Open second client
    RTelServer telServer2;
    error = telServer2.Connect();
    ASSERT_EQUALS( KErrNone, error );
    CleanupClosePushL( telServer2 );

    RMobilePhone phone2;
    error = phone2.Open( iTelServer,KMmTsyPhoneName );
    ASSERT_EQUALS( KErrNone, error );
    CleanupClosePushL( phone2 );

    RLine line2;
    error = line2.Open( phone2, KMmTsyVoice1LineName );
    CleanupClosePushL( line2 );
    ASSERT_EQUALS( KErrNone, error );

    RMobileCall call2;
    error = call2.OpenNewCall( line2 );
    CleanupClosePushL( call2 );
    ASSERT_EQUALS( KErrNone, error );

    TInt callId2( 2 );
    // prepare MockLTSY
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData0 mockData01(callId2, mobileService);
    CallGetMobileCallInfoL(callId2, mobileService, KPhoneNumber);
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData3(0, mobileService, callParams, callInfo2);
    mockData3.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    call2.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call2.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    TInt callCount(0);
    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should be 2 calls now
    ASSERT_EQUALS( 2, callCount );

    // cleanup 

    // prepare MockLTSY for hangup 
    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( EFalse );
    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId2, mobileService, hangUpCause, autoStChangeDisable );
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    CleanupStack::PopAndDestroy( &call2 );
    CleanupStack::PopAndDestroy( &line2 );
    CleanupStack::PopAndDestroy( &phone2 );
    CleanupStack::PopAndDestroy( &telServer2 );

    // prepare MockLTSY for hangup 
    TMockLtsyCallData2<TInt, TBool> mockHangUpData2( callId1, mobileService, hangUpCause, autoStChangeDisable );
    expectData.Close();
    mockHangUpData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( this );
    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCTCR-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::TeardownCallRouting
@SYMTestPriority High
@SYMTestActions Invokes MTelephonyAudioControl::TeardownCallRouting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestTeardownTelephonyAudio0001aL()
    {
    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    RLine line;
    RMobileCall call;

    TInt error( KErrNone );

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::TeardownCallRouting when result is not cached.
    //-------------------------------------------------------------------------

    // MO Call Audio Teardown

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hangup 
    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );
    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData3(callId, mobileService);
    mockData3.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );


    TInt callCount(0);
    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // MT Call Audio Teardown 

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband    

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL();

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    TName incomingCallName;
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusAnswering );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();  

    DoCleanup();

    // MT Call Audio Setup - Immediate Failure Case

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL();    

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // create error by changing call state back to idle
    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();   

    DoCleanup();

    // MT Call Audio Setup - In Progress Failure Case

    // EStatusAnswering -> EStatusIdle

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mobileCallStatus = RMobileCall::EStatusRinging;
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call state and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusAnswering );

    // create error by changing call state back to idle
    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();         

    DoCleanup();

    // MT Call Audio Setup - Immediate Failure Case

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData4(0, mobileService, callParams, callInfo2);
    mockData4.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // MO Call Audio Setup - In Progress Failure Case

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData4.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // create error by setting the call back to the idle
    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    // cleanup
    AssertMockLtsyStatusL();

    DeletePropeties();
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( this );
    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCTCR-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::TeardownCallRouting by using LTSY version1
@SYMTestPriority High
@SYMTestActions Invokes MTelephonyAudioControl::TeardownCallRouting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestTeardownTelephonyAudio0001bL()
    {
    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RLine line;
    RMobileCall call;

    TInt error( KErrNone );

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::TeardownCallRouting when result is not cached.
    //-------------------------------------------------------------------------

    // MO Call Audio Teardown

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    // check that there is no call routing done
    CheckNoCallRouting();

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData3(callId, mobileService);
    mockData3.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // MT Call Audio Teardown 

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband    

    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    TName incomingCallName;
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );

    // check that there is no call routing done
    CheckNoCallRouting(); 

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting(); 

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting(); 

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );

    // check that there is no call routing done
    CheckNoCallRouting(); 

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting(); 

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();  

    // cleanup
    DeletePropeties();
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( this );    
    }


/**
@SYMTestCaseID BA-CTSY-CRC-CRCTCR-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to MTelephonyAudioControl::TeardownCallRouting
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to MTelephonyAudioControl::TeardownCallRouting
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestTeardownTelephonyAudio0004L()
    {
    DefinePropeties( EFalse, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );

    TInt callId1( 1 );    
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId1, mobileService, hangUpCause, autoStChangeDisable );

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId1, mobileService);

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting MTelephonyAudioControl::SetUpCallRouting
    //-------------------------------------------------------------------------

    // reset line & call
    RLine line;
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    RMobileCall call;
    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId1, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    TRequestStatus status( KErrNone );
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );

    ASSERT_EQUALS( KErrNone, status.Int() );
    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    error = call.GetMobileCallInfo( callInfoPckg );

    ASSERT_EQUALS( KErrNone, error );
    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    TInt callCount(0);
    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );
    // there should be 1 call now
    ASSERT_EQUALS( 1, callCount );

    // Open second client
    RTelServer telServer2;
    error = telServer2.Connect();
    ASSERT_EQUALS( KErrNone, error );
    CleanupClosePushL( telServer2 );

    RMobilePhone phone2;
    error = phone2.Open( telServer2,KMmTsyPhoneName );
    CleanupClosePushL( phone2 );
    ASSERT_EQUALS( KErrNone, error );

    RLine line2;
    error = line2.Open( phone2, KMmTsyVoice1LineName );
    CleanupClosePushL( line2 );
    ASSERT_EQUALS( KErrNone, error );

    RMobileCall call2;
    error = call2.OpenNewCall( line2 );
    CleanupClosePushL( call2 );
    ASSERT_EQUALS( KErrNone, error );

    TInt callId2( 2 );
    // prepare MockLTSY
    TMockLtsyCallData0 mockData01(callId2, mobileService);
    CallGetMobileCallInfoL(callId2, mobileService, KPhoneNumber);
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData3(0, mobileService, callParams, callInfo2);
    mockData3.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call
    call2.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call2.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should be 2 calls now
    ASSERT_EQUALS( 2, callCount ); 

    // hangup call 1
    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );   
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should 1 call
    ASSERT_EQUALS( 1, callCount );   

    // hangup call 2
    error = call2.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );   
    UpdateCallStatusL( call2, RMobileCall::EStatusDisconnectingWithInband, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );    

    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls
    ASSERT_EQUALS( 0, callCount );

    // cleanup 
    CleanupStack::PopAndDestroy( &call2 );
    CleanupStack::PopAndDestroy( &line2 );
    CleanupStack::PopAndDestroy( &phone2 );
    CleanupStack::PopAndDestroy( &telServer2 );
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( this );    
    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCCRST-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::CallStateChange
@SYMTestPriority High
@SYMTestActions Invokes MTelephonyAudioControl::CallStateChange
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestCallStateChange0001aL()

    {

    /* CallStateChange function is called when call has changed its state to the following states:
   EStatusDialling
   EStatusAnswering
   EStatusConnecting
   EStatusHold
   EStatusDisconnecting
   EStatusDisconnectingWithInband
   EStatusConnected
     */
    DefinePropeties( ETrue, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::CallStateChange when result is not cached.
    //-------------------------------------------------------------------------

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData3(callId, mobileService);
    mockData3.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData6(0, mobileService, callParams, callInfo2);
    mockData6.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );    

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();   

    DoCleanup();

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    TName incomingCallName;
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    completeData.Close();
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );
    CheckCallStatus( incomingCallName, RMobileCall::EStatusAnswering );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( incomingCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( incomingCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData4(callId, mobileService);
    mockData4.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );
    CheckCallStatus( incomingCallName, RMobileCall::EStatusDisconnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( incomingCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband    

    // reset properties
    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusAnswering ); 

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();    

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusHold -> EStatusConnected -> EStatusDisconnectingWithInband -> EStatusIdle   

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData6.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hold 
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL( EMobileCallHold, expectData );    
    iMockLTSY.CompleteL( EMobileCallHold, KErrNone, expectData );  

    call.Hold( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusHold );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusHold );

    // prepare MockLTSY for resume 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EMobileCallResume, expectData );
    iMockLTSY.CompleteL( EMobileCallResume, KErrNone, expectData );

    call.Resume( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusHold -> EStatusDisconnecting -> EStatusIdle   

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData6.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hold 
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL( EMobileCallHold, expectData );    
    iMockLTSY.CompleteL( EMobileCallHold, KErrNone, expectData );  

    call.Hold( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusHold );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusHold );

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData5(callId, mobileService);
    mockData5.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusHold -> EStatusDisconnectingWithInband -> EStatusIdle   

    DefinePropeties( ETrue, EVersion2 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData6.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    // prepare MockLTSY for hold 
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL( EMobileCallHold, expectData );    
    iMockLTSY.CompleteL( EMobileCallHold, KErrNone, expectData );  

    call.Hold( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusHold );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusHold );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    // cleanup 
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( this );
    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCCRST-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for MTelephonyAudioControl::CallStateChange by using LTSY version1
@SYMTestPriority High
@SYMTestActions Invokes MTelephonyAudioControl::CallStateChange
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestCallStateChange0001bL()

    {

    /* CallStateChange function is called when call has changed its state to the following states:
   EStatusDialling
   EStatusAnswering
   EStatusConnecting
   EStatusDisconnecting
   EStatusDisconnectingWithInband
   EStatusConnected
     */
    DefinePropeties( ETrue, EVersion1 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // TEST C: Successful completion request of
    // MTelephonyAudioControl::CallStateChange when result is not cached.
    //-------------------------------------------------------------------------

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData3(callId, mobileService);
    mockData3.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband

    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData5(0, mobileService, callParams, callInfo2);
    mockData5.SerialiseL(expectData);

    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();    

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();   

    DoCleanup();

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting

    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    TName incomingCallName;
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    completeData.Close();
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // check that there is no call routing done
    CheckNoCallRouting();

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // check that there is no call routing done
    CheckNoCallRouting();

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();

    // prepare MockLTSY for hangup 
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone );

    completeData.Close();
    TMockLtsyCallData0 mockData4(callId, mobileService);
    mockData4.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
    // hangup the call
    call.HangUp( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    DoCleanup();

    // EStatusAnswering -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband    

    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    // notify incoming call
    line.NotifyIncomingCall( status, incomingCallName );

    // create incoming call notification
    mockCallData.SerialiseL( completeData );
    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    // wait for notification
    User::WaitForRequest( status );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, status.Int() );

    // create call ringing notification
    completeData.Close();
    mockCallData2.SerialiseL(completeData);
    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // prepare MockLTSY for answer 
    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // check that there is no call routing done
    CheckNoCallRouting();

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();    

    DoCleanup();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusHold -> EStatusDisconnectingWithInband -> EStatusIdle   

    DefinePropeties( ETrue, EVersion1 );
    OpenEtelServerL(EUseExtendedError);
    OpenPhoneL(); 

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData5.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );

    // check that there is no call routing done
    CheckNoCallRouting();

    // prepare MockLTSY for hold 
    expectData.Close();
    mockData0.SerialiseL(expectData);
    iMockLTSY.ExpectL( EMobileCallHold, expectData );    
    iMockLTSY.CompleteL( EMobileCallHold, KErrNone, expectData );  

    call.Hold( status );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusHold );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband );

    // check that there is no call routing done
    CheckNoCallRouting();

    // change call status
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );

    // check that there is no call routing done
    CheckNoCallRouting();

    // cleanup
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy( &call );
    AssertMockLtsyStatusL();    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL();

    // cleanup 
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( this );

    }

/**
@SYMTestCaseID BA-CTSY-CRC-CRCCRST-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to MTelephonyAudioControl::CallStateChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to MTelephonyAudioControl::CallStateChange
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsyTelephonyAudioControlFU::TestCallStateChange0004L()
    {
    DefinePropeties( EFalse, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> completeCallStatusData( callId, mobileService, mobileCallStatus );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // Test A: Test multiple clients requesting MTelephonyAudioControl::CallStateChange
    //-------------------------------------------------------------------------
    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnectingWithInband -> EStatusIdle 

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // dial the call 
    call.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );   

    AssertMockLtsyStatusL();

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    AssertMockLtsyStatusL();

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    AssertMockLtsyStatusL();

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    AssertMockLtsyStatusL();

    // EStatusDialling -> EStatusConnecting -> EStatusConnected -> EStatusDisconnecting 

    // Open second client
    RTelServer telServer2;
    error = telServer2.Connect();
    ASSERT_EQUALS( KErrNone, error );
    CleanupClosePushL( telServer2 );

    RMobilePhone phone2;
    error = phone2.Open( telServer2,KMmTsyPhoneName );
    ASSERT_EQUALS( KErrNone, error );
    CleanupClosePushL( phone2 );

    RLine line2;
    error = line2.Open( phone2, KMmTsyVoice1LineName );
    CleanupClosePushL( line2 );
    ASSERT_EQUALS( KErrNone, error );

    RMobileCall call2;
    error = call2.OpenNewCall( line2 );
    CleanupClosePushL( call2 );
    ASSERT_EQUALS( KErrNone, error );

    TInt callId2( 2 );
    // prepare MockLTSY for call 2 dial
    TMockLtsyCallData0 mockData01(callId2, mobileService);
    CallGetMobileCallInfoL(callId2, mobileService, KPhoneNumber);
    RMobileCall::TMobileCallInfoV8 callInfo2;
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData3(0, mobileService, callParams, callInfo2);
    mockData3.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData01.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call 2
    call2.Dial( status, pckgCallParamsX1, KPhoneNumber );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    AssertMockLtsyStatusL();

    error = call2.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // check that call status it is OK
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusUnknown );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    AssertMockLtsyStatusL();

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnecting );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    AssertMockLtsyStatusL();

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusConnected );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );  

    TInt callCount(0);
    error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should be 2 calls now
    ASSERT_EQUALS( 2, callCount );

    // hangup call 1  
    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );          
    UpdateCallStatusL( call, RMobileCall::EStatusDisconnectingWithInband, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    error = call2.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // hangup call 2            
    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusDisconnectingWithInband, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDisconnectingWithInband );

    // change call status and check it is OK
    UpdateCallStatusL( call2, RMobileCall::EStatusIdle, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup 
    CleanupStack::PopAndDestroy( &call2 );
    CleanupStack::PopAndDestroy( &line2 );
    CleanupStack::PopAndDestroy( &phone2 );
    CleanupStack::PopAndDestroy( &telServer2 );
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( this );
    }

void CCTsyTelephonyAudioControlFU::UpdateCallStatusL( RMobileCall& aMobileCall, 
        RMobileCall::TMobileCallStatus aMobileCallStatus,
        TBool aUpdateTimer,
        TBool aHangUp )
    {
    RMobileCall::TMobileCallInfoV1 callInfo;
    RMobileCall::TMobileCallInfoV1Pckg callInfoPckg( callInfo );
    TInt error = aMobileCall.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData(callInfo.iCallId, callInfo.iService, aMobileCallStatus );
    completeData.Close();
    mockCallData.SerialiseL(completeData);

    TRequestStatus mockLtsyStatus( KErrNone );

    iMockLTSY.NotifyTerminated(mockLtsyStatus);   
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData );

    if ( aHangUp )
        {    
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV1 callParams;    
    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( ETrue );
    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );
    expectData.Close();
    mockHangUpData.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData, KErrNone ); 
        }

    if ( aUpdateTimer )
        {
    // when call becomes idle, remaining duration of the call is added to life time param in LTSY:        
    TUint32 duration = 9;    // this is a dummy value, which won't be checked by mocksy engine
    TMockLtsyData1<TUint32> timerData( duration );
    expectData.Close();
    timerData.SerialiseL(expectData);    
    iMockLTSY.ExpectL(EMmTsyUpdateLifeTimeIPC, expectData);
        }

    User::WaitForRequest(mockLtsyStatus);

    AssertMockLtsyStatusL();

    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int() );

    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &completeData );

    error = aMobileCall.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );  
    }

void CCTsyTelephonyAudioControlFU::CheckCallRoutingValues( RLine& aLine, RMobileCall& aCall )
    {
    TInt callCount(0);
    TInt error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    TInt count(0);
    error = aLine.EnumerateCall( count );
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( callCount, count );

    RMobileCall::TMobileCallInfoV1 callInfo;
    RMobileCall::TMobileCallInfoV1Pckg callInfoPckg( callInfo );
    error = aCall.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    TName callName;
    error = RProperty::Get( KMockLtsyFactoryProperty, ECallName, callName );
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( 0, callInfo.iCallName.Compare( callName ) ); 

    TBool emergency( ETrue );
    error = RProperty::Get( KMockLtsyFactoryProperty, EEmergencyCall, emergency );
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( emergency, callInfo.iEmergency ); 

    TInt direction(0);
    error = RProperty::Get( KMockLtsyFactoryProperty, ECallDirection, direction );
    RMobileCall::TMobileCallDirection calldirection = (RMobileCall::TMobileCallDirection) direction;
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( calldirection, callInfo.iRemoteParty.iDirection ); 

    TInt callType( 0 );
    error = RProperty::Get( KMockLtsyFactoryProperty, ECallType, callType );
    MTelephonyAudioControl::TTelephonyAudioControlCallType callRoutingType = ( MTelephonyAudioControl::TTelephonyAudioControlCallType ) callType;
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( callRoutingType, MTelephonyAudioControl::ECallTypeVoice ); 
    }


void CCTsyTelephonyAudioControlFU::DefinePropeties( TBool aReset, TMockLtsyFactoryVersions aVersion )
    {
    DeletePropeties();

    TInt error = RProperty::Define( KMockLtsyFactoryProperty, ENumberOfCalls, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, ECallName, RProperty::EText );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EEmergencyCall, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, ECallDirection, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EHangUpReason, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, ECallType, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EReturnErrorCode, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EReset, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, ECallStatus, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Set( KMockLtsyFactoryProperty, EReset, aReset );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EVersionProperty, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EVersionProperty, aVersion );
    ASSERT_EQUALS( KErrNone, error );

    error = RProperty::Define( KMockLtsyFactoryProperty, EComplete, RProperty::EInt );
    ASSERT_EQUALS( KErrNone, error );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EComplete, EFalse );
    ASSERT_EQUALS( KErrNone, error );
    }


void CCTsyTelephonyAudioControlFU::CheckCallStatus( TName& aCallName, RMobileCall::TMobileCallStatus aMobileCallStatus )
    {
    TName callName;
    TInt error = RProperty::Get( KMockLtsyFactoryProperty, ECallName, callName );
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( 0, aCallName.Compare( callName ) ); 

    TInt status( 0 );
    error = RProperty::Get( KMockLtsyFactoryProperty, ECallStatus, status );
    RMobileCall::TMobileCallStatus callStatus = ( RMobileCall::TMobileCallStatus ) status;
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( callStatus, aMobileCallStatus ); 
    }

void CCTsyTelephonyAudioControlFU::DeletePropeties()  
    {
    TInt error = RProperty::Delete(KMockLtsyFactoryProperty, ENumberOfCalls );    
    error = RProperty::Delete(KMockLtsyFactoryProperty, ECallName );
    error = RProperty::Delete(KMockLtsyFactoryProperty, EEmergencyCall );
    error = RProperty::Delete(KMockLtsyFactoryProperty, ECallDirection );
    error = RProperty::Delete(KMockLtsyFactoryProperty, EHangUpReason );
    error = RProperty::Delete(KMockLtsyFactoryProperty, ECallType );
    error = RProperty::Delete(KMockLtsyFactoryProperty, EReturnErrorCode );
    error = RProperty::Delete(KMockLtsyFactoryProperty, EReset );
    error = RProperty::Delete(KMockLtsyFactoryProperty, ECallStatus ); 
    error = RProperty::Delete(KMockLtsyFactoryProperty, EVersionProperty );    
    error = RProperty::Delete(KMockLtsyFactoryProperty, EComplete );
    }

void CCTsyTelephonyAudioControlFU::CheckNoCallRouting()
    {
    // check that there is no calls in call routing control
    TInt callCount(0);
    TInt error = RProperty::Get( KMockLtsyFactoryProperty, ENumberOfCalls, callCount );
    ASSERT_EQUALS( KErrNone, error );

    // there should not be any calls 
    ASSERT_EQUALS( 0, callCount );    

    // check that the call name is still undefined
    TName callName;
    error = RProperty::Get( KMockLtsyFactoryProperty, ECallName, callName );
    ASSERT_EQUALS( KErrNone, error );
    ASSERT_EQUALS( 0, callName.Compare( KNullDesC ) );
    }

CCTsyTelephonyAudioControlFU::~CCTsyTelephonyAudioControlFU()
    {
    DeletePropeties();
    }

void CCTsyTelephonyAudioControlFU::TestNotifyTelephonyAudioControlError0001L()
    {
    DefinePropeties( ETrue, EVersion2 );

    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    RBuf8 data;
    CleanupClosePushL(data);

    RBuf8 expectData;
    CleanupClosePushL(expectData);

    RBuf8 completeData;
    CleanupClosePushL(completeData);

    TRequestStatus status( KErrNone );
    TInt callId( 1 );
    RMobilePhone::TMobileService mobileService( RMobilePhone::EVoiceService );
    RMobileCall::TMobileCallParamsV7 callParams; 
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
    callInfo.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo.iDialledParty.iTelNumber.Copy( KPhoneNumber );    
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData2(0, mobileService, callParams, callInfo);
    TMockLtsyCallData0 mockData0(callId, mobileService);

    RCall::TCallParams callParamsX1;
    TPckg<RCall::TCallParams> pckgCallParamsX1(callParamsX1);

    TRequestStatus mockLtsyStatus( KErrNone );
    RMobileCall::TMobileCallStatus mobileCallStatus( RMobileCall::EStatusConnecting );

    TInt hangUpCause( KErrGsmReleaseByUser );
    TBool autoStChangeDisable ( EFalse );

    TMockLtsyCallData2<TInt, TBool> mockHangUpData( callId, mobileService, hangUpCause, autoStChangeDisable );

    RMobileCall::TMobileCallInfoV8Pckg callInfoPckg( callInfo );

    RLine line;
    RMobileCall call;

    //-------------------------------------------------------------------------
    // TEST A: failure to dispatch request to LTSY
    //-------------------------------------------------------------------------

    // DialL

    // error when call is in RMobileCall::EStatusDialling

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line & call
    TInt error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call where SetupTelephonyAudio fails
    call.Dial( status, pckgCallParamsX1, KPhoneNumber ); 
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNotSupported );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling, EFalse, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNone );
    ASSERT_EQUALS( KErrNone, error );

    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // DialL

    // error when call is in RMobileCall::EStatusDialling
    // in VoiceLine 2

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice2LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    RMobilePhone::TMobileService mobileService2( RMobilePhone::EAuxVoiceService );
    RMobileCall::TMobileCallInfoV8 callInfo2;   
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
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService2;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );    
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData3(0, mobileService2, callParams, callInfo2);
    TMockLtsyCallData0 mockData4(callId, mobileService2);

    RCall::TCallParams callParamsX2;
    TPckg<RCall::TCallParams> pckgCallParamsX2(callParamsX2);

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService2, KPhoneNumber );
    mockData3.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData4.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call where SetupTelephonyAudio fails
    call.Dial( status, pckgCallParamsX2, KPhoneNumber ); 
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNotSupported );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling, EFalse, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNone );
    ASSERT_EQUALS( KErrNone, error );

    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // error when call is in RMobileCall::EStatusConnecting

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    callInfo2.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;    
    callInfo2.iService   = mobileService;
    callInfo2.iStatus    = RMobileCall::EStatusUnknown;
    callInfo2.iCallId    =-1;
    callInfo2.iExitCode  =0; 
    callInfo2.iEmergency =0;
    callInfo2.iForwarded =0; 
    callInfo2.iPrivacy               = RMobilePhone::EPrivacyUnspecified;
    callInfo2.iAlternatingCall       = RMobilePhone::EAlternatingModeUnspecified;    
    callInfo2.iDialledParty.iNumberPlan = RMobilePhone::EUnknownNumberingPlan;
    callInfo2.iDialledParty.iTypeOfNumber = RMobilePhone::EUnknownNumber;   
    callInfo2.iDialledParty.iTelNumber.Copy( KPhoneNumber );
    TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV1> mockData5(0, mobileService, callParams, callInfo2);
    mockData5.SerialiseL(expectData);

    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call where SetupTelephonyAudio fails
    call.Dial( status, pckgCallParamsX1, KPhoneNumber ); 
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNotSupported );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting, EFalse, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNone );
    ASSERT_EQUALS( KErrNone, error );

    UpdateCallStatusL( call, RMobileCall::EStatusIdle );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // error when call is in RMobileCall::EStatusConnected

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line & call
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    error = call.OpenNewCall( line );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error );

    // prepare MockLTSY
    CallGetMobileCallInfoL(callId, mobileService, KPhoneNumber);
    mockData5.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallDial, expectData);
    mockData0.SerialiseL(completeData);
    iMockLTSY.CompleteL(EEtelCallDial, KErrNone, completeData);

    // make the call where SetupTelephonyAudio fails
    call.Dial( status, pckgCallParamsX1, KPhoneNumber ); 
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusDialling );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusDialling );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnecting, EFalse, EFalse );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnecting );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNotSupported );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusConnected, EFalse, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusConnected );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNone );
    ASSERT_EQUALS( KErrNone, error );

    UpdateCallStatusL( call, RMobileCall::EStatusIdle, ETrue, EFalse );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    // cleanup 
    AssertMockLtsyStatusL();   
    CleanupStack::PopAndDestroy( &call );
    CleanupStack::PopAndDestroy( &line );

    // error when call is in RMobileCall::EStatusAnswering

    // reset the properties to ensure to get valid data
    DefinePropeties( ETrue, EVersion2 );

    // reset line
    error = line.Open( iPhone, KMmTsyVoice1LineName );
    CleanupClosePushL( line );
    ASSERT_EQUALS( KErrNone, error );

    TName incomingCallName;
    TRequestStatus requestNotify( KErrNone );
    line.NotifyIncomingCall( status, incomingCallName);

    TMockLtsyCallData1<RMobileCall::TMobileCallInfoV1> mockCallData( callId, mobileService, callInfo );
    mockCallData.SerialiseL( completeData );

    iMockLTSY.NotifyTerminated( mockLtsyStatus );
    iMockLTSY.CompleteL( EEtelLineNotifyIncomingCall, KErrNone, completeData) ;
    User::WaitForRequest( mockLtsyStatus );

    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, mockLtsyStatus.Int() );

    User::WaitForRequest( requestNotify );
    AssertMockLtsyStatusL();
    ASSERT_EQUALS( KErrNone, requestNotify.Int() );

    mobileCallStatus = RMobileCall::EStatusRinging;
    TMockLtsyCallData1<RMobileCall::TMobileCallStatus> mockCallData2(callId, mobileService, mobileCallStatus );
    completeData.Close();
    mockCallData2.SerialiseL(completeData);

    iMockLTSY.NotifyTerminated(mockLtsyStatus);
    iMockLTSY.CompleteL(EMobileCallNotifyMobileCallStatusChange, KErrNone, completeData);
    User::WaitForRequest(mockLtsyStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    expectData.Close();
    mockData0.SerialiseL( expectData );
    iMockLTSY.ExpectL( EEtelCallAnswer, expectData );
    iMockLTSY.CompleteL( EEtelCallAnswer, KErrNone, expectData );

    // reset the call
    error = call.OpenExistingCall( line, incomingCallName );
    CleanupClosePushL( call );
    ASSERT_EQUALS( KErrNone, error ); 

    // answer the call
    call.AnswerIncomingCall( status, pckgCallParamsX1 );
    User::WaitForRequest( status );
    ASSERT_EQUALS( KErrNone, status.Int() );

    error = call.GetMobileCallInfo( callInfoPckg );
    ASSERT_EQUALS( KErrNone, error );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNotSupported );
    ASSERT_EQUALS( KErrNone, error );

    // change call status and check it is OK
    UpdateCallStatusL( call, RMobileCall::EStatusAnswering, EFalse, ETrue );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusAnswering );

    error =  RProperty::Set( KMockLtsyFactoryProperty, EReturnErrorCode, KErrNone );
    ASSERT_EQUALS( KErrNone, error );

    UpdateCallStatusL( call, RMobileCall::EStatusIdle, EFalse, EFalse );
    CheckCallStatus( callInfo.iCallName, RMobileCall::EStatusIdle );

    CleanupStack::PopAndDestroy( &call );    
    CleanupStack::PopAndDestroy( &line );
    AssertMockLtsyStatusL(); 

    // cleanup 
    CleanupStack::PopAndDestroy( &completeData );
    CleanupStack::PopAndDestroy( &expectData );    
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( this );
    }

//  End of File
