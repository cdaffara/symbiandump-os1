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
// The TEFUnit test suite for DataCallControl in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsydatacallcontrolfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include "MmTsy_Csd_Defs.h"
#include <featmgr/featurecontrol.h>
#include <featureuids.h>

CTestSuite* CCTsyDataCallControlFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetBearerServiceInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetBearerServiceInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetBearerServiceInfo0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0012L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestConnect0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetCurrentHscsdInfo0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetCurrentHscsdInfo0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetCurrentHscsdInfo0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetCurrentHscsdInfo0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyVoiceFallback0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyVoiceFallback0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyVoiceFallback00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallCaps0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallCaps0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallCaps00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyHscsdInfoChange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams00010L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestSetDynamicHscsdParams00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallRLPRange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallRLPRange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestGetMobileDataCallRLPRange00011L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsyDataCallControlFU, TestNotifyMobileDataCallCapsChange00011L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-DATC-CGBSI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetBearerServiceInfo for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetBearerServiceInfo for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetBearerServiceInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RCall::GetBearerServiceInfo is not supported for voice calls
 	//-------------------------------------------------------------------------
	
	RCall call;
	RLine line;
	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::EVoiceService);
	RCall::TBearerService bearerService;
	TInt res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNotSupported, res)
	AssertMockLtsyStatusL();

	// same test for voice call from auxiliary voice line
	RCall call2;
	RLine line2;
	OpenLineLC(line2, KMmTsyVoice2LineName);
	OpenNewCallLC(line2, call2);
	DialL(call2, 2, RMobilePhone::EAuxVoiceService);
	res = call2.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNotSupported, res)
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call2, line2, call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CGBSI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetBearerServiceInfo for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetBearerServiceInfo for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetBearerServiceInfo0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when call is not active
 	//-------------------------------------------------------------------------

	RCall::TBearerService bearerService;
	TInt res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNotFound, res);
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::GetBearerServiceInfo.
 	//-------------------------------------------------------------------------

	DialL(call, 1, RMobilePhone::ECircuitDataService);
	ChangeCallStatusL(1, RMobilePhone::ECircuitDataService, RMobileCall::EStatusConnected);
	
	res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNone, res)
	ASSERT_EQUALS((TUint32) (RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown), bearerService.iBearerCaps)
	ASSERT_EQUALS(RCall::EBearerDataUnknown, bearerService.iBearerSpeed)
	AssertMockLtsyStatusL();
	
	// change data in CTSY
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	RCall::TBearerService changeBearerService;
	changeBearerService.iBearerCaps = RCall::KBearerCapsProtocolALT_CELLULAR;
	changeBearerService.iBearerSpeed = RCall::EBearerData57600;
	
	TMockLtsyCallData1<RCall::TBearerService> mockBearerData(1, mobileService, changeBearerService);
	data.Close();
	mockBearerData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EEtelCallGetBearerServiceInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
		
	// test again
	res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS((TUint32) (RCall::KBearerCapsProtocolALT_CELLULAR), bearerService.iBearerCaps)
	ASSERT_EQUALS(RCall::EBearerData57600, bearerService.iBearerSpeed)
	AssertMockLtsyStatusL();

	ChangeCallStatusL(1, RMobilePhone::ECircuitDataService, RMobileCall::EStatusIdle, KErrGsmReleaseByUser);
	
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGBSI-0011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::GetBearerServiceInfo for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::GetBearerServiceInfo for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetBearerServiceInfo0011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 expectData;
    CleanupClosePushL(expectData);

	RBuf8 completeData;
    CleanupClosePushL(completeData);
    
	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	RCall::TBearerService bearerService;
	TInt res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNotFound, res);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&call);

	TName name;
	RMobilePhone::TMobileService mobileService = RMobilePhone::EFaxService;
	CreateIncomingCallL(line, 1, KNullDesC, name, mobileService, RMobileCall::EStatusConnected);
	res = call.OpenExistingCall(line, name);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(call);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::GetBearerServiceInfo.
 	//-------------------------------------------------------------------------

	res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNone, res)
	ASSERT_EQUALS((TUint32) (RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown), bearerService.iBearerCaps)
	ASSERT_EQUALS(RCall::EBearerDataUnknown, bearerService.iBearerSpeed)
	AssertMockLtsyStatusL();
	
	// change data in CTSY
	RCall::TBearerService changeBearerService;
	changeBearerService.iBearerCaps = RCall::KBearerCapsProtocolALT_CELLULAR;
	changeBearerService.iBearerSpeed = RCall::EBearerData57600;
	
	TMockLtsyCallData1<RCall::TBearerService> mockBearerData(1, mobileService, changeBearerService);
	data.Close();
	mockBearerData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EEtelCallGetBearerServiceInfo, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();
	
	// test again
	res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS((TUint32) (RCall::KBearerCapsProtocolALT_CELLULAR), bearerService.iBearerCaps)
	ASSERT_EQUALS(RCall::EBearerData57600, bearerService.iBearerSpeed)
	AssertMockLtsyStatusL();
	
	// Fax call is not in idle state while closing the call object.
	// Therefore CTSY sends HangUp request to LTSY.
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TInt hangUpCause = KErrGsmReleaseByUser;
	TBool autoStChangeDisable = EFalse;
    TMockLtsyCallData2<TInt, TBool> mockData2(1, RMobilePhone::EFaxService, hangUpCause, autoStChangeDisable);
    mockData2.SerialiseL(expectData);
    iMockLTSY.ExpectL(EEtelCallHangUp, expectData);
    
    TMockLtsyCallData0 mockData0(1, RMobilePhone::EFaxService);
	mockData0.SerialiseL(completeData);
	iMockLTSY.CompleteL(EEtelCallHangUp, KErrNone, completeData);
	
	call.Close();
	User::WaitForRequest(mockLtsyStatus);
	CleanupStack::Pop(&call);
	
	AssertMockLtsyStatusL();
	
 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when call is not active
 	//-------------------------------------------------------------------------
	
	OpenNewCallLC(line, call);
	
	res = call.GetBearerServiceInfo(bearerService);
	ASSERT_EQUALS(KErrNotFound, res);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(6, this); // data, expectData, completeData, call, line, this
	
	}



/**
@SYMTestCaseID BA-CTSY-DATC-CC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Connect for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Connect for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RCall::Connect is not supported for voice calls
 	//-------------------------------------------------------------------------

	// synchronous version
	TInt res = call.Connect();
	ASSERT_EQUALS(KErrNotSupported, res)

	// synchronous version with parameters
	RCall::TCallParams callParams;
	RCall::TCallParamsPckg callParamsPckg(callParams);
	res = call.Connect(callParamsPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	
	// asynchronous version
	TRequestStatus reqStatus;
	call.Connect(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	// asynchronous version with parameters
	call.Connect(reqStatus, callParamsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);

	// synchronous version
	res = call.Connect();
	ASSERT_EQUALS(KErrNotSupported, res)
	
	// synchronous version with parameters
	res = call.Connect(callParamsPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	
	// asynchronous version
	call.Connect(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	// asynchronous version with parameters
	call.Connect(reqStatus, callParamsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Connect for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Connect for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::Connect.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
		
	// synchronous version with no parameters
	TInt res = call.Connect();
	ASSERT_EQUALS(KErrNone, res)

	// synchronous version with RCall::TCallParams parameter
	RCall::TCallParams callParams;
	callParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOff;
	callParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeOff;
	callParams.iInterval = 0;
	callParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
	RCall::TCallParamsPckg callParamsPckg(callParams);
	res = call.Connect(callParamsPckg);
	ASSERT_EQUALS(KErrNone, res)
	
	// synchronous version with RMobileCall::TMobileCallParamsV1Pckg parameter
	RMobileCall::TMobileCallParamsV1 mobileCallParams;
	mobileCallParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOn;
	mobileCallParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeMedium;
	mobileCallParams.iInterval = 0;
	mobileCallParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
	mobileCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
	mobileCallParams.iCug.iExplicitInvoke = EFalse;
	mobileCallParams.iCug.iCugIndex = -1;
	mobileCallParams.iCug.iSuppressOA = EFalse;
	mobileCallParams.iCug.iSuppressPrefCug = EFalse;
	mobileCallParams.iAutoRedial = EFalse;
	RMobileCall::TMobileCallParamsV1Pckg mobileCallParamsPckg(mobileCallParams);

	res = call.Connect(mobileCallParamsPckg);
	ASSERT_EQUALS(KErrNone, res)

	// synchronous version with RMobileCall::TMobileDataCallParamsV1Pckg parameter
	RMobileCall::TMobileDataCallParamsV1 dataCallParams;
	dataCallParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOn;
	dataCallParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeMedium;
	dataCallParams.iInterval = 0;
	dataCallParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
	dataCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
	dataCallParams.iCug.iExplicitInvoke = EFalse;
	dataCallParams.iCug.iCugIndex = -1;
	dataCallParams.iCug.iSuppressOA = EFalse;
	dataCallParams.iCug.iSuppressPrefCug = EFalse;
	dataCallParams.iAutoRedial = EFalse;
	dataCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	dataCallParams.iSpeed = RMobileCall::ESpeedAutobauding;
	dataCallParams.iProtocol = RMobileCall::EProtocolV32;
	dataCallParams.iQoS = RMobileCall::EQoSTransparent;
	dataCallParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	dataCallParams.iModemToMSWindowSize = 100;
	dataCallParams.iMSToModemWindowSize = 100;
	dataCallParams.iAckTimer = 100;
	dataCallParams.iRetransmissionAttempts = 3;
	dataCallParams.iResequencingPeriod = 100;
	dataCallParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataCallParams.iV42bisCodewordsNum = 2;
	dataCallParams.iV42bisMaxStringLength = 64;
	dataCallParams.iUseEdge = EFalse;
	RMobileCall::TMobileDataCallParamsV1Pckg dataCallParamsPckg(dataCallParams);

	res = call.Connect(dataCallParamsPckg);
	ASSERT_EQUALS(KErrNone, res)

	// asynchronous version with RMobileCall::TMobileHscsdCallParamsV1 parameter
	RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg hscsdCallParamsPckg(hscsdCallParams);
	
	hscsdCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams.iModemToMSWindowSize = 100;
	hscsdCallParams.iMSToModemWindowSize = 100;
	hscsdCallParams.iAckTimer = 100;
	hscsdCallParams.iRetransmissionAttempts = 3;
	hscsdCallParams.iResequencingPeriod = 100;
	hscsdCallParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams.iV42bisCodewordsNum = 2;
	hscsdCallParams.iV42bisMaxStringLength = 64;
	hscsdCallParams.iUseEdge = EFalse;
	hscsdCallParams.iWantedAiur = RMobileCall::EAiurBps19200;
	hscsdCallParams.iWantedRxTimeSlots = 2;
	hscsdCallParams.iMaxTimeSlots = 3;
	hscsdCallParams.iCodings = RMobileCall::KCapsAiurCoding96;
	hscsdCallParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	hscsdCallParams.iUserInitUpgrade = ETrue;
	

	call.Connect(reqStatus, hscsdCallParamsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
		
	
	//-------------------------------------------------------------------------
	// Test: Test passing RMobileCall::TMobileDataCallParamsV2 to
	// RCall::Connect
	//-------------------------------------------------------------------------
						
	RMobileCall::TMobileDataCallParamsV2 callParams2;
	RMobileCall::TMobileDataCallParamsV2Pckg callParamsPckg2(callParams2);
					
	callParams2.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams2.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams2.iProtocol = RMobileCall::EProtocolV32;
	callParams2.iQoS = RMobileCall::EQoSTransparent;
	callParams2.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams2.iModemToMSWindowSize = 100;
	callParams2.iMSToModemWindowSize = 100;
	callParams2.iAckTimer = 100;
	callParams2.iRetransmissionAttempts = 3;
	callParams2.iResequencingPeriod = 100;
	callParams2.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams2.iV42bisCodewordsNum = 2;
	callParams2.iV42bisMaxStringLength = 64;
	callParams2.iUseEdge = EFalse;
	callParams2.iBearerMode = RMobileCall::EMulticallNotSupported;
		
	call.Connect(reqStatus, callParamsPckg2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
			
	//-------------------------------------------------------------------------
	// Test: Test passing RMobileCall::TMobileDataCallParamsV8 to
	// RCall::Connect
	//-------------------------------------------------------------------------
					
	RMobileCall::TMobileDataCallParamsV8 callParams8;
	RMobileCall::TMobileDataCallParamsV8Pckg callParamsPckg8(callParams8);
			
	callParams8.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams8.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams8.iProtocol = RMobileCall::EProtocolV32;
	callParams8.iQoS = RMobileCall::EQoSTransparent;
	callParams8.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams8.iModemToMSWindowSize = 100;
	callParams8.iMSToModemWindowSize = 100;
	callParams8.iAckTimer = 100;
	callParams8.iRetransmissionAttempts = 3;
	callParams8.iResequencingPeriod = 100;
	callParams8.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams8.iV42bisCodewordsNum = 2;
	callParams8.iV42bisMaxStringLength = 64;
	callParams8.iUseEdge = EFalse;
			
	callParams8.iBearerMode = RMobileCall::EMulticallNotSupported;
	callParams8.iCallParamOrigin = RMobileCall::EOriginatorUnknown ;
	callParams8.iSubAddress.Zero() ;
	callParams8.iBearerCap1.Zero();
	callParams8.iBearerCap2.Zero();
	callParams8.iBCRepeatIndicator = RMobileCall::EBCServiceChangeAndFallbackMode ;
	callParams8.iAlphaId.Copy(_L("Alpha Id"));
		
	call.Connect(reqStatus, callParamsPckg8);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
		
	//-------------------------------------------------------------------------
	// Test: Test passing RMobileCall::TMobileHscsdCallParamsV2 to
	// RCall::Connect
	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;
	RMobileCall::TMobileHscsdCallParamsV2Pckg hscsdCallParamsPckg2(hscsdCallParams2);
				
	hscsdCallParams2.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams2.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams2.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams2.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams2.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams2.iModemToMSWindowSize = 100;
	hscsdCallParams2.iMSToModemWindowSize = 100;
	hscsdCallParams2.iAckTimer = 100;
	hscsdCallParams2.iRetransmissionAttempts = 3;
	hscsdCallParams2.iResequencingPeriod = 100;
	hscsdCallParams2.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams2.iV42bisCodewordsNum = 2;
	hscsdCallParams2.iV42bisMaxStringLength = 64;
	hscsdCallParams2.iUseEdge = EFalse;
	hscsdCallParams2.iWantedRxTimeSlots = 3;
				
	hscsdCallParams2.iBearerMode = RMobileCall::EMulticallNotSupported;
	
	call.Connect(reqStatus, hscsdCallParamsPckg2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	//-------------------------------------------------------------------------
   	// Test: Test passing RMobileCall::TMobileHscsdCallParamsV7 to
   	// RCall::Connect
   	//-------------------------------------------------------------------------

   	RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
   	RMobileCall::TMobileHscsdCallParamsV7Pckg hscsdCallParamsPckg7(hscsdCallParams7);
    
   	hscsdCallParams7.iService = RMobileCall::EServiceDataCircuitAsync;
   	hscsdCallParams7.iSpeed = RMobileCall::ESpeedAutobauding;
   	hscsdCallParams7.iProtocol = RMobileCall::EProtocolV32;
   	hscsdCallParams7.iQoS = RMobileCall::EQoSTransparent;
   	hscsdCallParams7.iRLPVersion = RMobileCall::ERLPNotRequested;
   	hscsdCallParams7.iModemToMSWindowSize = 100;
   	hscsdCallParams7.iMSToModemWindowSize = 100;
   	hscsdCallParams7.iAckTimer = 100;
   	hscsdCallParams7.iRetransmissionAttempts = 3;
   	hscsdCallParams7.iResequencingPeriod = 100;
   	hscsdCallParams7.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
   	hscsdCallParams7.iV42bisCodewordsNum = 2;
   	hscsdCallParams7.iV42bisMaxStringLength = 64;
   	hscsdCallParams7.iUseEdge = EFalse;
   	hscsdCallParams7.iWantedRxTimeSlots = 3;
    			
   	hscsdCallParams7.iBearerMode = RMobileCall::EMulticallNotSupported;
   	hscsdCallParams7.iCallParamOrigin = RMobileCall::EOriginatorUnknown;
   	hscsdCallParams7.iIconId.iQualifier = RMobileCall::ENoIconId;
   	hscsdCallParams7.iIconId.iIdentifier = 2;
   	hscsdCallParams7.iAlphaId.Zero();
	    		
   	call.Connect(reqStatus, hscsdCallParamsPckg7);
   	User::WaitForRequest(reqStatus);
   	ASSERT_EQUALS(KErrNone, reqStatus.Int())
   	
	//-------------------------------------------------------------------------
	// Test: Test passing RMobileCall::TMobileHscsdCallParamsV8 to
	// RCall::Connect
	//-------------------------------------------------------------------------

	RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
	RMobileCall::TMobileHscsdCallParamsV8Pckg hscsdCallParamsPckg8(hscsdCallParams8);
	
	hscsdCallParams8.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams8.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams8.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams8.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams8.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams8.iModemToMSWindowSize = 100;
	hscsdCallParams8.iMSToModemWindowSize = 100;
	hscsdCallParams8.iAckTimer = 100;
	hscsdCallParams8.iRetransmissionAttempts = 3;
	hscsdCallParams8.iResequencingPeriod = 100;
	hscsdCallParams8.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams8.iV42bisCodewordsNum = 2;
	hscsdCallParams8.iV42bisMaxStringLength = 64;
	hscsdCallParams8.iUseEdge = EFalse;
	hscsdCallParams8.iWantedRxTimeSlots = 3;
			
	hscsdCallParams8.iBearerMode = RMobileCall::EMulticallNotSupported;
	hscsdCallParams8.iCallParamOrigin = RMobileCall::EOriginatorUnknown;
	hscsdCallParams8.iIconId.iQualifier = RMobileCall::ENoIconId;
	hscsdCallParams8.iIconId.iIdentifier = 2;
	hscsdCallParams8.iAlphaId.Zero();
		
	hscsdCallParams8.iSubAddress.Zero();
	hscsdCallParams8.iBearerCap1.Zero();
	hscsdCallParams8.iBearerCap2.Zero();
	hscsdCallParams8.iBCRepeatIndicator = RMobileCall::EBCServiceChangeAndFallbackMode ;

	call.Connect(reqStatus, hscsdCallParamsPckg8);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::Connect for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::Connect for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::Connect (Not Supported)
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallParamsV1 mobileCallParams;
	mobileCallParams.iSpeakerControl = RMobileCall::EMonitorSpeakerControlAlwaysOn;
	mobileCallParams.iSpeakerVolume = RMobileCall::EMonitorSpeakerVolumeMedium;
	mobileCallParams.iInterval = 0;
	mobileCallParams.iWaitForDialTone = RMobileCall::EDialToneNoWait;
	mobileCallParams.iIdRestrict = RMobileCall::EIdRestrictDefault;
	mobileCallParams.iCug.iExplicitInvoke = EFalse;
	mobileCallParams.iCug.iCugIndex = -1;
	mobileCallParams.iCug.iSuppressOA = EFalse;
	mobileCallParams.iCug.iSuppressPrefCug = EFalse;
	mobileCallParams.iAutoRedial = EFalse;
	RMobileCall::TMobileCallParamsV1Pckg mobileCallParamsPckg(mobileCallParams);

	TRequestStatus reqStatus;
	call.Connect(reqStatus, mobileCallParamsPckg);

	// Cancels connecting of a (data) call.
	call.ConnectCancel();
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Connect with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Connect with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::Connect
 	//-------------------------------------------------------------------------

	_LIT8(KOneCharDesc,"1");
	TBuf8<1> buf(KOneCharDesc);
	TInt res = call.Connect(buf);
	// Passing a small buffer to function does cause an error.
	ASSERT_EQUALS(KErrArgument, res)
	
	TRequestStatus reqStatus;
	call.Connect(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	// Passing a small buffer to function does cause an error.
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())
	

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Connect for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Connect for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	
	RLine line2;
	RCall call2;
	OpenLineLC(line2, KMmTsyDataLineName);
	OpenNewCallLC(line2, call2);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;
	
	RMobileCall::TMobileDataCallParamsV1 callParams;
	callParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams.iProtocol = RMobileCall::EProtocolV32;
	callParams.iQoS = RMobileCall::EQoSTransparent;
	callParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams.iModemToMSWindowSize = 100;
	callParams.iMSToModemWindowSize = 100;
	callParams.iAckTimer = 100;
	callParams.iRetransmissionAttempts = 3;
	callParams.iResequencingPeriod = 100;
	callParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams.iV42bisCodewordsNum = 2;
	callParams.iV42bisMaxStringLength = 64;
	callParams.iUseEdge = EFalse;
	
	RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::Connect
 	//-------------------------------------------------------------------------

	call.Connect(reqStatus, callParamsPckg);
	call2.Connect(reqStatus2, callParamsPckg);

	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);

	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(5, this); // call2, line2, call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Connect for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Connect for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RCall::Connect.
 	//-------------------------------------------------------------------------

	// synchronous version
	TInt res = call.Connect();
	
	// RCall::Connect() without call parameters is not supported by CTSY. 
	// Because of this it is completed with KErrArgument.
	ASSERT_EQUALS(KErrArgument, res)

	// asynchronous version
	TRequestStatus reqStatus;
	call.Connect(reqStatus);
	User::WaitForRequest(reqStatus);
	
	// RCall::Connect() without call parameters is not supported by CTSY. 
	// Because of this it is completed with KErrArgument.
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())

	// synchronous version with RMobileCall::TMobileDataCallParamsV1Pckg parameter
	RMobileCall::TMobileDataCallParamsV1 callParams;
	callParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams.iProtocol = RMobileCall::EProtocolV32;
	callParams.iQoS = RMobileCall::EQoSTransparent;
	callParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams.iModemToMSWindowSize = 100;
	callParams.iMSToModemWindowSize = 100;
	callParams.iAckTimer = 100;
	callParams.iRetransmissionAttempts = 3;
	callParams.iResequencingPeriod = 100;
	callParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams.iV42bisCodewordsNum = 2;
	callParams.iV42bisMaxStringLength = 64;
	callParams.iUseEdge = EFalse;
	RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);

	res = call.Connect(callParamsPckg);
	ASSERT_EQUALS(KErrNone, res)
	
	// asynchronous version with RMobileCall::TMobileHscsdCallParamsV1Pckg parameter
	RMobileCall::TMobileHscsdCallParamsV1 hscsdCallParams;
	hscsdCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams.iModemToMSWindowSize = 100;
	hscsdCallParams.iMSToModemWindowSize = 100;
	hscsdCallParams.iAckTimer = 100;
	hscsdCallParams.iRetransmissionAttempts = 3;
	hscsdCallParams.iResequencingPeriod = 100;
	hscsdCallParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams.iV42bisCodewordsNum = 2;
	hscsdCallParams.iV42bisMaxStringLength = 64;
	hscsdCallParams.iUseEdge = EFalse;
	hscsdCallParams.iWantedAiur = RMobileCall::EAiurBps19200;
	hscsdCallParams.iWantedRxTimeSlots = 2;
	hscsdCallParams.iMaxTimeSlots = 3;
	hscsdCallParams.iCodings = RMobileCall::KCapsAiurCoding96;
	hscsdCallParams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	hscsdCallParams.iUserInitUpgrade = ETrue;
	RMobileCall::TMobileHscsdCallParamsV1Pckg hscsdCallParamsPckg(hscsdCallParams);

	call.Connect(reqStatus, hscsdCallParamsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	//------------------------------------------------------------
	// TMobileDataCallParamsV2
	RMobileCall::TMobileDataCallParamsV2 dataCallParams2;
	dataCallParams2.iService = RMobileCall::EServiceDataCircuitAsync;
	dataCallParams2.iSpeed = RMobileCall::ESpeedAutobauding;
	dataCallParams2.iProtocol = RMobileCall::EProtocolV32;
	dataCallParams2.iQoS = RMobileCall::EQoSTransparent;
	dataCallParams2.iRLPVersion = RMobileCall::ERLPNotRequested;
	dataCallParams2.iModemToMSWindowSize = 100;
	dataCallParams2.iMSToModemWindowSize = 100;
	dataCallParams2.iAckTimer = 100;
	dataCallParams2.iRetransmissionAttempts = 3;
	dataCallParams2.iResequencingPeriod = 100;
	dataCallParams2.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataCallParams2.iV42bisCodewordsNum = 2;
	dataCallParams2.iV42bisMaxStringLength = 64;
	dataCallParams2.iUseEdge = EFalse;
	dataCallParams2.iBearerMode = RMobileCall::EMulticallShareBearer;
	RMobileCall::TMobileDataCallParamsV2Pckg dataCallParamsPckg2(dataCallParams2);

	res = call.Connect(dataCallParamsPckg2);
	ASSERT_EQUALS(KErrNone, res)
		
	//------------------------------------------------------------
	// TMobileDataCallParamsV8
	
	RMobileCall::TMobileDataCallParamsV8 dataCallParams8;
	dataCallParams8.iService = RMobileCall::EServiceDataCircuitAsync;
	dataCallParams8.iSpeed = RMobileCall::ESpeedAutobauding;
	dataCallParams8.iProtocol = RMobileCall::EProtocolV32;
	dataCallParams8.iQoS = RMobileCall::EQoSTransparent;
	dataCallParams8.iRLPVersion = RMobileCall::ERLPNotRequested;
	dataCallParams8.iModemToMSWindowSize = 100;
	dataCallParams8.iMSToModemWindowSize = 100;
	dataCallParams8.iAckTimer = 100;
	dataCallParams8.iRetransmissionAttempts = 3;
	dataCallParams8.iResequencingPeriod = 100;
	dataCallParams8.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataCallParams8.iV42bisCodewordsNum = 2;
	dataCallParams8.iV42bisMaxStringLength = 64;
	dataCallParams8.iUseEdge = EFalse;
	dataCallParams8.iBearerMode = RMobileCall::EMulticallShareBearer;
	RMobileCall::TMobileDataCallParamsV8Pckg dataCallParamsPckg8(dataCallParams8);

	res = call.Connect(dataCallParamsPckg8);
	ASSERT_EQUALS(KErrNone, res)

	//------------------------------------------------------------
	// TMobileHscsdCallParamsV2
	
	RMobileCall::TMobileHscsdCallParamsV2 hscsdCallParams2;
	hscsdCallParams2.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams2.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams2.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams2.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams2.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams2.iModemToMSWindowSize = 100;
	hscsdCallParams2.iMSToModemWindowSize = 100;
	hscsdCallParams2.iAckTimer = 100;
	hscsdCallParams2.iRetransmissionAttempts = 3;
	hscsdCallParams2.iResequencingPeriod = 100;
	hscsdCallParams2.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams2.iV42bisCodewordsNum = 2;
	hscsdCallParams2.iV42bisMaxStringLength = 64;
	hscsdCallParams2.iUseEdge = EFalse;
	hscsdCallParams2.iWantedAiur = RMobileCall::EAiurBps19200;
	hscsdCallParams2.iWantedRxTimeSlots = 2;
	hscsdCallParams2.iMaxTimeSlots = 3;
	hscsdCallParams2.iCodings = RMobileCall::KCapsAiurCoding96;
	hscsdCallParams2.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	hscsdCallParams2.iUserInitUpgrade = ETrue;
	RMobileCall::TMobileHscsdCallParamsV2Pckg hscsdCallParamsPckg2(hscsdCallParams2);

	call.Connect(reqStatus, hscsdCallParamsPckg2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	//------------------------------------------------------------
	// TMobileHscsdCallParamsV2
	
	RMobileCall::TMobileHscsdCallParamsV7 hscsdCallParams7;
	hscsdCallParams7.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams7.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams7.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams7.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams7.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams7.iModemToMSWindowSize = 100;
	hscsdCallParams7.iMSToModemWindowSize = 100;
	hscsdCallParams7.iAckTimer = 100;
	hscsdCallParams7.iRetransmissionAttempts = 3;
	hscsdCallParams7.iResequencingPeriod = 100;
	hscsdCallParams7.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams7.iV42bisCodewordsNum = 2;
	hscsdCallParams7.iV42bisMaxStringLength = 64;
	hscsdCallParams7.iUseEdge = EFalse;
	hscsdCallParams7.iWantedAiur = RMobileCall::EAiurBps19200;
	hscsdCallParams7.iWantedRxTimeSlots = 2;
	hscsdCallParams7.iMaxTimeSlots = 3;
	hscsdCallParams7.iCodings = RMobileCall::KCapsAiurCoding96;
	hscsdCallParams7.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	hscsdCallParams7.iUserInitUpgrade = ETrue;
	RMobileCall::TMobileHscsdCallParamsV7Pckg hscsdCallParamsPckg7(hscsdCallParams7);

	call.Connect(reqStatus, hscsdCallParamsPckg7);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	//------------------------------------------------------------
	// TMobileHscsdCallParamsV8
	
	RMobileCall::TMobileHscsdCallParamsV8 hscsdCallParams8;
	hscsdCallParams8.iService = RMobileCall::EServiceDataCircuitAsync;
	hscsdCallParams8.iSpeed = RMobileCall::ESpeedAutobauding;
	hscsdCallParams8.iProtocol = RMobileCall::EProtocolV32;
	hscsdCallParams8.iQoS = RMobileCall::EQoSTransparent;
	hscsdCallParams8.iRLPVersion = RMobileCall::ERLPNotRequested;
	hscsdCallParams8.iModemToMSWindowSize = 100;
	hscsdCallParams8.iMSToModemWindowSize = 100;
	hscsdCallParams8.iAckTimer = 100;
	hscsdCallParams8.iRetransmissionAttempts = 3;
	hscsdCallParams8.iResequencingPeriod = 100;
	hscsdCallParams8.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	hscsdCallParams8.iV42bisCodewordsNum = 2;
	hscsdCallParams8.iV42bisMaxStringLength = 64;
	hscsdCallParams8.iUseEdge = EFalse;
	hscsdCallParams8.iWantedAiur = RMobileCall::EAiurBps19200;
	hscsdCallParams8.iWantedRxTimeSlots = 2;
	hscsdCallParams8.iMaxTimeSlots = 3;
	hscsdCallParams8.iCodings = RMobileCall::KCapsAiurCoding96;
	hscsdCallParams8.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	hscsdCallParams8.iUserInitUpgrade = ETrue;
	RMobileCall::TMobileHscsdCallParamsV8Pckg hscsdCallParamsPckg8(hscsdCallParams8);

	call.Connect(reqStatus, hscsdCallParamsPckg8);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0012
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RCall::Connect for fax calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RCall::Connect for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0012L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// Test cancelling of RCall::Connect (Not Supported)
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileDataCallParamsV1 callParams;
	callParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams.iProtocol = RMobileCall::EProtocolV32;
	callParams.iQoS = RMobileCall::EQoSTransparent;
	callParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams.iModemToMSWindowSize = 100;
	callParams.iMSToModemWindowSize = 100;
	callParams.iAckTimer = 100;
	callParams.iRetransmissionAttempts = 3;
	callParams.iResequencingPeriod = 100;
	callParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams.iV42bisCodewordsNum = 2;
	callParams.iV42bisMaxStringLength = 64;
	callParams.iUseEdge = EFalse;
	RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);

	TRequestStatus reqStatus;
	call.Connect(reqStatus, callParamsPckg);

	// Cancels connecting of a (data) call (Not Supported).
	call.ConnectCancel();

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0013
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RCall::Connect with bad parameter data for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RCall::Connect with bad parameter data for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0013L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RCall::Connect
 	//-------------------------------------------------------------------------

	TBuf8<1> buf(KNullDesC8);
	TInt res = call.Connect(buf);
	// Passing a small buffer to function doesn't cause an error.
	ASSERT_EQUALS(KErrArgument, res)
	
	TRequestStatus reqStatus;
	call.Connect(reqStatus, buf);
	User::WaitForRequest(reqStatus);
	// Passing a small buffer to function doesn't cause an error.
	ASSERT_EQUALS(KErrArgument, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CC-0014
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RCall::Connect for fax calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RCall::Connect for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestConnect0014L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RCall call;
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	RLine line2;
	RCall call2;
	OpenLineLC(line2, KMmTsyFaxLineName);
	OpenNewCallLC(line2, call2);

	TRequestStatus reqStatus;
	TRequestStatus reqStatus2;
	
	RMobileCall::TMobileDataCallParamsV1 callParams;
	callParams.iService = RMobileCall::EServiceDataCircuitAsync;
	callParams.iSpeed = RMobileCall::ESpeedAutobauding;
	callParams.iProtocol = RMobileCall::EProtocolV32;
	callParams.iQoS = RMobileCall::EQoSTransparent;
	callParams.iRLPVersion = RMobileCall::ERLPNotRequested;
	callParams.iModemToMSWindowSize = 100;
	callParams.iMSToModemWindowSize = 100;
	callParams.iAckTimer = 100;
	callParams.iRetransmissionAttempts = 3;
	callParams.iResequencingPeriod = 100;
	callParams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	callParams.iV42bisCodewordsNum = 2;
	callParams.iV42bisMaxStringLength = 64;
	callParams.iUseEdge = EFalse;
	RMobileCall::TMobileDataCallParamsV1Pckg callParamsPckg(callParams);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RCall::Connect
 	//-------------------------------------------------------------------------

	call.Connect(reqStatus, callParamsPckg);
	call2.Connect(reqStatus2, callParamsPckg);
	
	User::WaitForRequest(reqStatus);
	User::WaitForRequest(reqStatus2);
	
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())

	CleanupStack::PopAndDestroy(5, this); // call2, line2, call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGCHI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetCurrentHscsdInfo for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetCurrentHscsdInfo for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetCurrentHscsdInfo0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;

	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetCurrentHscsdInfo is not supported for voice calls
 	//-------------------------------------------------------------------------

	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	
	TInt res = call.GetCurrentHscsdInfo(infoPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	
	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);
	
	res = call.GetCurrentHscsdInfo(infoPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CGCHI-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetCurrentHscsdInfo for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetCurrentHscsdInfo for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetCurrentHscsdInfo0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when dial request is pending
 	//-------------------------------------------------------------------------

	// create dial request
	RMobileCall::TMobileCallParamsV1 callParams;
    callParams.iInterval = 4;
    callParams.iCug.iCugIndex = 0xFFFF;
    
	RMobileCall::TMobileCallInfoV8 callInfo;
	_LIT(KPhoneNumber, "101632960000");   	
	callInfo.iDialledParty.iTelNumber.Copy(KPhoneNumber);
	callInfo.iService = RMobilePhone::ECircuitDataService;
	callInfo.iValid = RMobileCall::KCallDialledParty | RMobileCall::KCallAlternating;

	TMockLtsyCallData2<RMobileCall::TMobileCallParamsV1, RMobileCall::TMobileCallInfoV8> 
		expDialData(0, RMobilePhone::ECircuitDataService, callParams, callInfo);

	data.Close();
	expDialData.SerialiseL(data);
	iMockLTSY.ExpectL(EEtelCallDial, data);
	TRequestStatus requestStatus;
	TPckg<RMobileCall::TMobileCallParamsV1> callParamsPckg(callParams);
	call.Dial(requestStatus, callParamsPckg, KPhoneNumber);

	// test
	RMobileCall::TMobileCallHscsdInfoV8 resInfo;
	RMobileCall::TMobileCallHscsdInfoV8Pckg resInfoPckg(resInfo);
	TInt res = call.GetCurrentHscsdInfo(resInfoPckg);
	ASSERT_EQUALS(KErrEtelCallNotActive, res);
	ASSERT_EQUALS(RMobileCall::EAiurBpsUnspecified, resInfo.iAiur);
	ASSERT_EQUALS(0, resInfo.iRxTimeSlots);
	ASSERT_EQUALS(0, resInfo.iTxTimeSlots);
	ASSERT_EQUALS(RMobileCall::ETchCodingUnspecified, resInfo.iCodings);
	ASSERT_TRUE(0 ==  resInfo.iAlphaId.Length());
	ASSERT_TRUE(0 ==  resInfo.iSubAddress.Length());
	AssertMockLtsyStatusL();

	// complete dial request
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	CallGetMobileCallInfoL(1, RMobilePhone::ECircuitDataService, KPhoneNumber);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TMockLtsyCallData0 completeDialData(1, RMobilePhone::ECircuitDataService);
	data.Close();
	completeDialData.SerialiseL(data);
	iMockLTSY.CompleteL(EEtelCallDial, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(requestStatus); // dial request completed
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(&call);

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when info is not ready
 	//-------------------------------------------------------------------------

	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	res = call.GetCurrentHscsdInfo(resInfoPckg);
	ASSERT_EQUALS(KErrNotReady, res)
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::GetCurrentHscsdInfo.
 	//-------------------------------------------------------------------------

    RMobileCall::TMobileCallHscsdInfoV8 hscsdInfo;
      
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    hscsdInfo.iAlphaId.Copy(_L("Alpha Id")); 
    hscsdInfo.iSubAddress.Copy(_L("An unknown SubAddress")); 
    hscsdInfo.iCallParamOrigin = RMobileCall::EOriginatorSIM ;
       
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
    dataCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    dataCallCaps.iUserInitUpgrade = EFalse;
    dataCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    dataCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    dataCallCaps.iHscsdSupport = ETrue;
    dataCallCaps.iMaxRxTimeSlots = 2;
    dataCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    dataCallCaps.iMaxTxTimeSlots = 2;
    dataCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    dataCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    dataCallCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    dataCallCaps.iMClass = 1;
    dataCallCaps.iTotalRxTxTimeSlots = 4;
    dataCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV8, RMobileCall::TMobileCallDataCapsV1> hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, dataCallCaps);
    data.Close();
    hscsdData.SerialiseL(data);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	RMobileCall::TMobileCallHscsdInfoV8 resInfo8;
	RMobileCall::TMobileCallHscsdInfoV8Pckg resInfoPckg8(resInfo8);
	res = call.GetCurrentHscsdInfo(resInfoPckg8);
	ASSERT_EQUALS(KErrNone, res);
	ASSERT_EQUALS(hscsdInfo.iAiur, resInfo8.iAiur);
	ASSERT_EQUALS(hscsdInfo.iRxTimeSlots, resInfo8.iRxTimeSlots);
	ASSERT_EQUALS(hscsdInfo.iTxTimeSlots, resInfo8.iTxTimeSlots);
	ASSERT_EQUALS(hscsdInfo.iCodings, resInfo8.iCodings);
	ASSERT_EQUALS(hscsdInfo.iCallParamOrigin, resInfo8.iCallParamOrigin); 
	ASSERT_EQUALS(hscsdInfo.iAlphaId, resInfo8.iAlphaId); 
	ASSERT_EQUALS(hscsdInfo.iSubAddress, resInfo8.iSubAddress); 

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// when Hscsd is not supported
 	//-------------------------------------------------------------------------

	// prepare CTSY (make Hscsd unsupported)
    dataCallCaps.iHscsdSupport = EFalse;
    data.Close();
    hscsdData.SerialiseL(data);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	// test
	res = call.GetCurrentHscsdInfo(resInfoPckg);
	ASSERT_EQUALS(KErrEtelCallNotActive, res);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CGCHI-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetCurrentHscsdInfo with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetCurrentHscsdInfo with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetCurrentHscsdInfo0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	// prepare CTSY (make info ready)
	RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
    dataCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    dataCallCaps.iUserInitUpgrade = EFalse;
    dataCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    dataCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    dataCallCaps.iHscsdSupport = ETrue;
    dataCallCaps.iMaxRxTimeSlots = 2;
    dataCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    dataCallCaps.iMaxTxTimeSlots = 2;
    dataCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    dataCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    dataCallCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    dataCallCaps.iMClass = 1;
    dataCallCaps.iTotalRxTxTimeSlots = 4;
    dataCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV1, RMobileCall::TMobileCallDataCapsV1> hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, dataCallCaps);
    data.Close();
    hscsdData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::GetCurrentHscsdInfo
 	//-------------------------------------------------------------------------

	TBuf8<1> info(KNullDesC8);
	TInt res = call.GetCurrentHscsdInfo(info);
	
	ASSERT_EQUALS(KErrArgument, res)
	
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}



/**
@SYMTestCaseID BA-CTSY-DATC-CGCHI-0011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetCurrentHscsdInfo for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetCurrentHscsdInfo for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetCurrentHscsdInfo0011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetCurrentHscsdInfo is not supported for fax calls
 	//-------------------------------------------------------------------------

	RLine line;
	RMobileCall call;

	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);

	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);

	TInt res = call.GetCurrentHscsdInfo(infoPckg);
	ASSERT_EQUALS(KErrNotSupported, res)

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNVF-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyVoiceFallback for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyVoiceFallback for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyVoiceFallback0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyVoiceFallback is not supported
 	//-------------------------------------------------------------------------

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::EVoiceService);
	
	TRequestStatus reqStatus;
	TName name;
	call.NotifyVoiceFallback(reqStatus, name);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	// same test for voice call from auxiliary voice line
	RLine line2;
	RMobileCall call2;

	OpenLineLC(line2, KMmTsyVoice2LineName);
	OpenNewCallLC(line2, call2);
	DialL(call2, 2, RMobilePhone::EAuxVoiceService);
	
	call2.NotifyVoiceFallback(reqStatus, name);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(5, this); // call2, line2, call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNVF-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyVoiceFallback for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyVoiceFallback for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyVoiceFallback0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyVoiceFallback is not supported
 	//-------------------------------------------------------------------------

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;
	TName name;
	call.NotifyVoiceFallback(reqStatus, name);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNVF-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyVoiceFallback for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyVoiceFallback for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyVoiceFallback00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyVoiceFallback is not supported
 	//-------------------------------------------------------------------------

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);

	TName name;
	CreateIncomingCallL(line, 1, KNullDesC, name, RMobilePhone::EFaxService, RMobileCall::EStatusConnected);
	TInt res = call.OpenExistingCall(line, name);
	ASSERT_EQUALS(KErrNone, res);
	CleanupClosePushL(call);
	
	TRequestStatus reqStatus;
	call.NotifyVoiceFallback(reqStatus, name);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallCaps for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallCaps for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetMobileDataCallCaps is not supported for voice calls
 	//-------------------------------------------------------------------------

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);

	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCallCapsPckg(dataCallCaps);

	TInt res = call.GetMobileDataCallCaps(dataCallCapsPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	AssertMockLtsyStatusL();

	// same test for voice call from auxiliary voice line
	RLine line2;
	RMobileCall call2;

	OpenLineLC(line2, KMmTsyVoice2LineName);
	OpenNewCallLC(line2, call2);

	res = call2.GetMobileDataCallCaps(dataCallCapsPckg);
	ASSERT_EQUALS(KErrNotSupported, res)

	CleanupStack::PopAndDestroy(5, this); // call2, line2, call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallCaps for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallCaps for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallCaps0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::GetMobileDataCallCaps.
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCallCapsPckg(dataCallCaps);
	TInt res = call.GetMobileDataCallCaps(dataCallCapsPckg);
	ASSERT_EQUALS(KErrNone, res)
	
	// check results
	TBool usingFeatureManager(EFalse);
	RFeatureControl featureControl;
	CleanupClosePushL(featureControl);
	res = featureControl.Open();
	if ( KErrNone == res )
		{
		usingFeatureManager = ETrue;
		}
    // additional flags for videotelephony
	if ( (usingFeatureManager) && (featureControl.FeatureSupported(NFeature::KCsVideoTelephony) == KFeatureSupported) )
		{
		ASSERT_EQUALS((TUint32)(KMultimodeTsyDataSpeedCaps | KMultimodeTsyDataSpeedCapsVT), dataCallCaps.iSpeedCaps)
		ASSERT_EQUALS((TUint32)(KMultimodeTsyDataProtocolCaps | KMultimodeTsyDataProtocolCapsVT), dataCallCaps.iProtocolCaps)
		ASSERT_EQUALS((TUint32)(KMultimodeTsyDataServiceCaps | KMultimodeTsyDataServiceCapsVT), dataCallCaps.iServiceCaps)
		}
	else
		{
		ASSERT_EQUALS((TUint32)KMultimodeTsyDataSpeedCaps, dataCallCaps.iSpeedCaps)
		ASSERT_EQUALS((TUint32)KMultimodeTsyDataProtocolCaps, dataCallCaps.iProtocolCaps)
		ASSERT_EQUALS((TUint32)KMultimodeTsyDataServiceCaps, dataCallCaps.iServiceCaps)
		}
	ASSERT_EQUALS((TUint32)KMultimodeTsyDataQoSCaps, dataCallCaps.iQoSCaps)
	ASSERT_EQUALS((TBool)KMultimodeTsyDataHscsdSupport, dataCallCaps.iHscsdSupport)
	ASSERT_EQUALS((TInt)KMultimodeTsyDataMClassDefValue, dataCallCaps.iMClass)
	ASSERT_EQUALS((TInt)KMultimodeTsyDataMaxRxTSDefValue, dataCallCaps.iMaxRxTimeSlots)
	ASSERT_EQUALS((TInt)KMultimodeTsyDataMaxTxTSDefValue, dataCallCaps.iMaxTxTimeSlots)
	ASSERT_EQUALS((TInt)KMultimodeTsyDataTotalRxTxTSDefValue, dataCallCaps.iTotalRxTxTimeSlots)
	ASSERT_EQUALS((TUint32)KMultimodeTsyDataTchCodingsCaps, dataCallCaps.iCodingCaps)
	ASSERT_EQUALS((TUint32)KMultimodeTsyDataAsymmetryCaps, dataCallCaps.iAsymmetryCaps)
	ASSERT_EQUALS((TBool)KMultimodeTsyDataUserInitUpgrade, dataCallCaps.iUserInitUpgrade)
	ASSERT_EQUALS((TUint32)KMultimodeTsyDataRLPVersionCaps, dataCallCaps.iRLPVersionCaps)
	ASSERT_EQUALS((TUint32)KMultimodeTsyDataV42bisCaps, dataCallCaps.iV42bisCaps)
	CleanupStack::PopAndDestroy(&featureControl);
	AssertMockLtsyStatusL();

	// change data in CTSY

	//This is needed for assign a callId to Call for setting new callDataCaps values.

	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	RMobileCall::TMobileCallDataCapsV1 newCaps;
    newCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    newCaps.iUserInitUpgrade = EFalse;
    newCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    newCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    newCaps.iHscsdSupport = ETrue;
    newCaps.iMaxRxTimeSlots = 2;
    newCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    newCaps.iMaxTxTimeSlots = 2;
    newCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    newCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    newCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    newCaps.iMClass = 1;
    newCaps.iTotalRxTxTimeSlots = 4;
    newCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1> newCapsData(1, mobileService, newCaps);
    newCapsData.SerialiseL(data);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	
	// test again
	res = call.GetMobileDataCallCaps(dataCallCapsPckg);
	ASSERT_EQUALS(KErrNone, res)

	ASSERT_EQUALS(newCaps.iSpeedCaps, dataCallCaps.iSpeedCaps)
	ASSERT_EQUALS(newCaps.iProtocolCaps, dataCallCaps.iProtocolCaps)
	ASSERT_EQUALS(newCaps.iServiceCaps, dataCallCaps.iServiceCaps)
	ASSERT_EQUALS(newCaps.iQoSCaps, dataCallCaps.iQoSCaps)
	ASSERT_EQUALS(newCaps.iHscsdSupport, dataCallCaps.iHscsdSupport)
	ASSERT_EQUALS(newCaps.iMClass, dataCallCaps.iMClass)
	ASSERT_EQUALS(newCaps.iMaxRxTimeSlots, dataCallCaps.iMaxRxTimeSlots)
	ASSERT_EQUALS(newCaps.iMaxTxTimeSlots, dataCallCaps.iMaxTxTimeSlots)
	ASSERT_EQUALS(newCaps.iTotalRxTxTimeSlots, dataCallCaps.iTotalRxTxTimeSlots)
	ASSERT_EQUALS(newCaps.iCodingCaps, dataCallCaps.iCodingCaps)
	ASSERT_EQUALS(newCaps.iAsymmetryCaps, dataCallCaps.iAsymmetryCaps)
	ASSERT_EQUALS(newCaps.iUserInitUpgrade, dataCallCaps.iUserInitUpgrade)
	ASSERT_EQUALS(newCaps.iRLPVersionCaps, dataCallCaps.iRLPVersionCaps)
	ASSERT_EQUALS(newCaps.iV42bisCaps, dataCallCaps.iV42bisCaps)
	
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
		
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallCaps with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallCaps with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallCaps0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::GetMobileDataCallCaps
 	//-------------------------------------------------------------------------
	
	TBuf8<1> caps(KNullDesC8);
	TInt res = call.GetMobileDataCallCaps(caps);
	ASSERT_EQUALS(KErrArgument, res);
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong parameter to
	// RMobileCall::GetMobileDataCallCaps method
 	//-------------------------------------------------------------------------
	
	const TInt bufSize = sizeof ( RMobileCall::TMobileCallDataCapsV1 );
	TBuf8<bufSize> caps2;
	caps2.FillZ();      //Fill buffer with zeros
	res = call.GetMobileDataCallCaps(caps2);		
	ASSERT_EQUALS(KErrArgument, res);

	CleanupStack::PopAndDestroy(3, this); // call, line, this

	}

/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallCaps for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallCaps for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallCaps00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg dataCallCapsPckg(dataCallCaps);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetMobileDataCallCaps is not supported for fax calls
 	//-------------------------------------------------------------------------

	TInt res = call.GetMobileDataCallCaps(dataCallCapsPckg);
	ASSERT_EQUALS(KErrNotSupported, res)
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}



/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyHscsdInfoChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyHscsdInfoChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyHscsdInfoChange is not supported for voice calls
 	//-------------------------------------------------------------------------

	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
	call.NotifyHscsdInfoChange(reqStatus, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);
	
	call.NotifyHscsdInfoChange(reqStatus, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;

	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
	
    RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps;

    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    
    dataCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    dataCallCaps.iUserInitUpgrade = EFalse;
    dataCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    dataCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    dataCallCaps.iHscsdSupport = ETrue; // should be ETrue
    dataCallCaps.iMaxRxTimeSlots = 2;
    dataCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    dataCallCaps.iMaxTxTimeSlots = 2;
    dataCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    dataCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    dataCallCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    dataCallCaps.iMClass = 1;
    dataCallCaps.iTotalRxTxTimeSlots = 4;
    dataCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV1, RMobileCall::TMobileCallDataCapsV1> hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, dataCallCaps);
    data.Close();
    hscsdData.SerialiseL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::NotifyHscsdInfoChange.
 	//-------------------------------------------------------------------------

	call.NotifyHscsdInfoChange(reqStatus, infoPckg);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	
	ASSERT_EQUALS(hscsdInfo.iAiur, info.iAiur)
	ASSERT_EQUALS(hscsdInfo.iRxTimeSlots, info.iRxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iTxTimeSlots, info.iTxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iCodings, info.iCodings)

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::NotifyHscsdInfoChange
	// from LTSY.
 	//-------------------------------------------------------------------------
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::NotifyHscsdInfoChange
 	//-------------------------------------------------------------------------
 	
	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
	call.NotifyHscsdInfoChange(reqStatus, infoPckg);

	call.CancelAsyncRequest(EMobileCallNotifyHscsdInfoChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int())

	CleanupStack::PopAndDestroy(4, this); // call, line, data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyHscsdInfoChange with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyHscsdInfoChange with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;

    RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
    
    RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
    dataCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    dataCallCaps.iUserInitUpgrade = EFalse;
    dataCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    dataCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    dataCallCaps.iHscsdSupport = ETrue; // should be ETrue
    dataCallCaps.iMaxRxTimeSlots = 2;
    dataCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    dataCallCaps.iMaxTxTimeSlots = 2;
    dataCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    dataCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    dataCallCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    dataCallCaps.iMClass = 1;
    dataCallCaps.iTotalRxTxTimeSlots = 4;
    dataCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV1, RMobileCall::TMobileCallDataCapsV1> hscsdData(1, mobileService, hscsdInfo, dataCallCaps);
    data.Close();
    hscsdData.SerialiseL(data);
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::NotifyHscsdInfoChange
 	//-------------------------------------------------------------------------
 	
    TBuf8<1> smallBuf(KNullDesC8);
	call.NotifyHscsdInfoChange(reqStatus, smallBuf);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	
	CleanupStack::PopAndDestroy(4, this); // call, line, data, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyHscsdInfoChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange0009L()
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

	// prepare first call
	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	TName callName;
	ret = call.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call);
	DialL(call, 1, RMobilePhone::ECircuitDataService);
	
	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
		
	TRequestStatus reqStatus;

	// prepare second call
	RLine line2;
	RMobileCall call2;

	TInt errorCode = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, errorCode)
   	CleanupClosePushL(line2);

	OpenNewCallLC(line2, call2);
	DialL(call2, 2, RMobilePhone::ECircuitDataService);

	RMobileCall::TMobileCallHscsdInfoV1 info2;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg2(info2);
		
	TRequestStatus reqStatus2;

	// prepare third call
	RMobileCall call3;
	ret = call3.OpenExistingCall(line, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);

	RMobileCall::TMobileCallHscsdInfoV1 info3;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg3(info3);
		
	TRequestStatus reqStatus3;

	// prepare ltsy data
    RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo;
    hscsdInfo.iAiur = RMobileCall::EAiurBps38400;
    hscsdInfo.iRxTimeSlots = 2;
    hscsdInfo.iTxTimeSlots = 2;
    hscsdInfo.iCodings = RMobileCall::ETchCoding320;
    
    RMobileCall::TMobileCallHscsdInfoV1 hscsdInfo2;
    hscsdInfo2.iAiur = RMobileCall::EAiurBps43200;
    hscsdInfo2.iRxTimeSlots = 4;
    hscsdInfo2.iTxTimeSlots = 4;
    hscsdInfo2.iCodings = RMobileCall::ETchCoding432;
    
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
    dataCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference;
    dataCallCaps.iUserInitUpgrade = EFalse;
    dataCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding144;
    dataCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
    dataCallCaps.iHscsdSupport = ETrue; // should be ETrue
    dataCallCaps.iMaxRxTimeSlots = 2;
    dataCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed14400;
    dataCallCaps.iMaxTxTimeSlots = 2;
    dataCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV32 | RMobileCall::KCapsPstnMultimedia;
    dataCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
    dataCallCaps.iServiceCaps = RMobileCall::KCapsDataCircuitAsynchronousRDI;
    dataCallCaps.iMClass = 1;
    dataCallCaps.iTotalRxTxTimeSlots = 4;
    dataCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
    
    RMobileCall::TMobileCallDataCapsV1 dataCallCaps2;
    dataCallCaps2.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
    dataCallCaps2.iUserInitUpgrade = ETrue;
    dataCallCaps2.iCodingCaps = RMobileCall::KCapsAiurCoding96;
    dataCallCaps2.iRLPVersionCaps = RMobileCall::KCapsRLPMultiLinkVersion2;
    dataCallCaps2.iHscsdSupport = ETrue; // should be ETrue
    dataCallCaps2.iMaxRxTimeSlots = 4;
    dataCallCaps2.iSpeedCaps = RMobileCall::KCapsSpeed28800;
    dataCallCaps2.iMaxTxTimeSlots = 4;
    dataCallCaps2.iProtocolCaps = RMobileCall::KCapsProtocolV34 | RMobileCall::KCapsIsdnMultimedia;
    dataCallCaps2.iQoSCaps = RMobileCall::KCapsTransparent;
    dataCallCaps2.iServiceCaps = RMobileCall::KCapsDataCircuitSynchronous;
    dataCallCaps2.iMClass = 2;
    dataCallCaps2.iTotalRxTxTimeSlots = 6;
    dataCallCaps2.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
    
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV1, RMobileCall::TMobileCallDataCapsV1> hscsdData(1, RMobilePhone::ECircuitDataService, hscsdInfo, dataCallCaps);
    TMockLtsyCallData2<RMobileCall::TMobileCallHscsdInfoV1, RMobileCall::TMobileCallDataCapsV1> hscsdData2(2, RMobilePhone::ECircuitDataService, hscsdInfo2, dataCallCaps2);
    
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::NotifyHscsdInfoChange
 	//-------------------------------------------------------------------------
	
	call.NotifyHscsdInfoChange(reqStatus, infoPckg);
	call2.NotifyHscsdInfoChange(reqStatus2, infoPckg2);
	call3.NotifyHscsdInfoChange(reqStatus3, infoPckg3);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
    hscsdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	data.Close();
    hscsdData2.SerialiseL(data);
	iMockLTSY.CompleteL(EMobileCallNotifyHscsdInfoChange, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	ASSERT_EQUALS(hscsdInfo.iAiur, info.iAiur)
	ASSERT_EQUALS(hscsdInfo.iRxTimeSlots, info.iRxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iTxTimeSlots, info.iTxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iCodings, info.iCodings)
	
 	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())
	ASSERT_EQUALS(hscsdInfo2.iAiur, info2.iAiur)
	ASSERT_EQUALS(hscsdInfo2.iRxTimeSlots, info2.iRxTimeSlots)
	ASSERT_EQUALS(hscsdInfo2.iTxTimeSlots, info2.iTxTimeSlots)
	ASSERT_EQUALS(hscsdInfo2.iCodings, info2.iCodings)

	User::WaitForRequest(reqStatus3);
	ASSERT_EQUALS(KErrNone, reqStatus3.Int())
	// same data as for 1st call because 3rd call object
	// is opened from existing call
	ASSERT_EQUALS(hscsdInfo.iAiur, info3.iAiur)
	ASSERT_EQUALS(hscsdInfo.iRxTimeSlots, info3.iRxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iTxTimeSlots, info3.iTxTimeSlots)
	ASSERT_EQUALS(hscsdInfo.iCodings, info3.iCodings)

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(9, this); // call3, call2, line2, call, line, phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNHIC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyHscsdInfoChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyHscsdInfoChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyHscsdInfoChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyHscsdInfoChange is not supported for fax calls
 	//-------------------------------------------------------------------------

	RMobileCall::TMobileCallHscsdInfoV1 info;
	RMobileCall::TMobileCallHscsdInfoV1Pckg infoPckg(info);
	
	call.NotifyHscsdInfoChange(reqStatus, infoPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetDynamicHscsdParams for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetDynamicHscsdParams for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::SetDynamicHscsdParams is not supported for voice calls
 	//-------------------------------------------------------------------------

	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	call.SetDynamicHscsdParams(reqStatus, RMobileCall::EAiurBps38400, 2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);
	
	call.SetDynamicHscsdParams(reqStatus, RMobileCall::EAiurBps38400, 2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}

/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;
	
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
		
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData(KCallId, mobileService, hscsdParams);
	expLtsyData.SerialiseL(expData);
	
	TMockLtsyCallData0 completeLtsyData(KCallId, mobileService);
	completeLtsyData.SerialiseL(completeData);
	
 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNotSupported);
	
	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int())
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrGeneral, completeData);

	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::SetDynamicHscsdParams.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData);

	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::SetDynamicHscsdParams
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call, line, completeData, expData, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;
	
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData(KCallId, mobileService, hscsdParams);
	expLtsyData.SerialiseL(expData);
	
	TMockLtsyCallData0 completeLtsyData(KCallId, mobileService);
	completeLtsyData.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::SetDynamicHscsdParams
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData);

	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	
	call.CancelAsyncRequest(EMobileCallSetDynamicHscsdParams);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(5, this); // call, line, completeData, expData, this
	
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::SetDynamicHscsdParams for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);
	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	TName callName;
	TInt ret = call.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;
	
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData(KCallId, mobileService, hscsdParams);
	expLtsyData.SerialiseL(expData);
	
	TMockLtsyCallData0 completeLtsyData(KCallId, mobileService);
	completeLtsyData.SerialiseL(completeData);

	// Open second client
	RTelServer telServer2;
	ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RBuf8 expData2;
	CleanupClosePushL(expData2);
	RBuf8 completeData2;
	CleanupClosePushL(completeData2);

	RLine line2;
	RMobileCall call2;
	
	const TInt KCallId2 = 2;

	ret = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, ret)
   	CleanupClosePushL(line2);

	OpenNewCallLC(line2, call2);
	DialL(call2, KCallId2, RMobilePhone::ECircuitDataService);

	TRequestStatus reqStatus2;
	
	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams2;
	hscsdParams2.iWantedAiur = RMobileCall::EAiurBps43200;
	hscsdParams2.iWantedRxTimeSlots = 3;
	
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData2(KCallId2, mobileService, hscsdParams2);
	expLtsyData2.SerialiseL(expData2);
	
	TMockLtsyCallData0 completeLtsyData2(KCallId2, mobileService);
	completeLtsyData2.SerialiseL(completeData2);

	// Open third call
	RBuf8 expData3;
	CleanupClosePushL(expData3);

	RMobileCall call3;
	ret = call3.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);

	TRequestStatus reqStatus3;

	RMobileCall::TMobileHscsdCallParamsV8 hscsdParams3;
	hscsdParams3.iWantedAiur = RMobileCall::EAiurBps57600;
	hscsdParams3.iWantedRxTimeSlots = 4;
	
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData3(KCallId, mobileService, hscsdParams3);
	expLtsyData3.SerialiseL(expData3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::SetDynamicHscsdParams
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData);
	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	
	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData2, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData2);
	call2.SetDynamicHscsdParams(reqStatus2, hscsdParams2.iWantedAiur, hscsdParams2.iWantedRxTimeSlots);
	
	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData3, KErrNone);
	iMockLTSY.CompleteL(EMobileCallSetDynamicHscsdParams, KErrNone, completeData);
	call3.SetDynamicHscsdParams(reqStatus3, hscsdParams3.iWantedAiur, hscsdParams3.iWantedRxTimeSlots);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int())

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int())

	User::WaitForRequest(reqStatus3);
	ASSERT_EQUALS(KErrNone, reqStatus3.Int())

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(8); // call3, expData3, call2, line2, completeData2, expData2, phone2, telServer2
	CleanupStack::PopAndDestroy(5, this); // call, line, completeData, expData, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-00010
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetDynamicHscsdParams with timeout for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetDynamicHscsdParams and tests for timeout for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams00010L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 expData;
	CleanupClosePushL(expData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	TRequestStatus reqStatus;
	
	RMobileCall::TMobileHscsdCallParamsV1 hscsdParams;
	hscsdParams.iWantedAiur = RMobileCall::EAiurBps38400;
	hscsdParams.iWantedRxTimeSlots = 2;
	
	RMobilePhone::TMobileService mobileService = RMobilePhone::ECircuitDataService;
	TMockLtsyCallData1<RMobileCall::TMobileHscsdCallParamsV1>
	                 expLtsyData(KCallId, mobileService, hscsdParams);
	expLtsyData.SerialiseL(expData);
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobileCall::SetDynamicHscsdParams
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobileCallSetDynamicHscsdParams, expData, KErrNone);
	call.SetDynamicHscsdParams(reqStatus, hscsdParams.iWantedAiur, hscsdParams.iWantedRxTimeSlots);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, expData, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CSDHP-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::SetDynamicHscsdParams for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::SetDynamicHscsdParams for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestSetDynamicHscsdParams00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::SetDynamicHscsdParams is not supported for fax calls
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	call.SetDynamicHscsdParams(reqStatus, RMobileCall::EAiurBps38400, 2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCRR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallRLPRange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallRLPRange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallRLPRange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetMobileDataCallRLPRange is not supported
 	//-------------------------------------------------------------------------

	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileDataRLPRangesV1 ranges;
	RMobileCall::TMobileDataRLPRangesV1Pckg rangesPckg(ranges);
	call.GetMobileDataCallRLPRange(reqStatus, 0, rangesPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);
	
	call.GetMobileDataCallRLPRange(reqStatus, 0, rangesPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCRR-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallRLPRange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallRLPRange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallRLPRange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileDataRLPRangesV1 ranges;
	RMobileCall::TMobileDataRLPRangesV1Pckg rangesPckg(ranges);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetMobileDataCallRLPRange is not supported
 	//-------------------------------------------------------------------------

	call.GetMobileDataCallRLPRange(reqStatus, 0, rangesPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CGMDCRR-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::GetMobileDataCallRLPRange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::GetMobileDataCallRLPRange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestGetMobileDataCallRLPRange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileDataRLPRangesV1 ranges;
	RMobileCall::TMobileDataRLPRangesV1Pckg rangesPckg(ranges);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::GetMobileDataCallRLPRange is not supported
 	//-------------------------------------------------------------------------

	call.GetMobileDataCallRLPRange(reqStatus, 0, rangesPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileDataCallCapsChange for voice calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileDataCallCapsChange for voice calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyMobileDataCallCapsChange is not supported for voice calls
 	//-------------------------------------------------------------------------

	OpenLineLC(line, KMmTsyVoice1LineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())

	CleanupStack::PopAndDestroy(2); // call, line

	// same test for voice call from auxiliary voice line
	OpenLineLC(line, KMmTsyVoice2LineName);
	OpenNewCallLC(line, call);
	
	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-0006
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange0006L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	RMobileCall::TMobileCallDataCapsV1 completeCallCaps;
	completeCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed48000;
	completeCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV34;
	completeCallCaps.iServiceCaps = RMobileCall::KCapsPacketAccessSyncUDI;
	completeCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
	completeCallCaps.iHscsdSupport = ETrue;
	completeCallCaps.iMClass = 2;
	completeCallCaps.iMaxRxTimeSlots = 4;
	completeCallCaps.iMaxTxTimeSlots = 8;
	completeCallCaps.iTotalRxTxTimeSlots = 10;
	completeCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding96;
	completeCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
	completeCallCaps.iUserInitUpgrade = ETrue;
	completeCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
	completeCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
	                     completeLtsyData(KCallId, RMobilePhone::ECircuitDataService, completeCallCaps);
	completeLtsyData.SerialiseL(completeData);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobileCall::NotifyMobileDataCallCapsChange when result is not cached.
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps, callCaps));

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobileCall::NotifyMobileDataCallCapsChange
	// from LTSY.
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // call, line, completeData, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-0007
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange0007L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobileCall::NotifyMobileDataCallCapsChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	
	call.CancelAsyncRequest(EMobileCallNotifyMobileDataCallCapsChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // line, call, this
	
	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-0008
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileDataCallCapsChange with bad parameter data for data calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileDataCallCapsChange with bad parameter data for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange0008L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	OpenNewCallLC(line, call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	RMobileCall::TMobileCallDataCapsV1 completeCallCaps;
	completeCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed48000;
	completeCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV34;
	completeCallCaps.iServiceCaps = RMobileCall::KCapsPacketAccessSyncUDI;
	completeCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
	completeCallCaps.iHscsdSupport = ETrue;
	completeCallCaps.iMClass = 2;
	completeCallCaps.iMaxRxTimeSlots = 4;
	completeCallCaps.iMaxTxTimeSlots = 8;
	completeCallCaps.iTotalRxTxTimeSlots = 10;
	completeCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding96;
	completeCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
	completeCallCaps.iUserInitUpgrade = ETrue;
	completeCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
	completeCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
	                     completeLtsyData(KCallId, RMobilePhone::ECircuitDataService, completeCallCaps);
	completeLtsyData.SerialiseL(completeData);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobileCall::NotifyMobileDataCallCapsChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TBuf8<1> wrongCallCaps(KNullDesC8);
	call.NotifyMobileDataCallCapsChange(reqStatus, wrongCallCaps);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	
	//-------------------------------------------------------------------------
	// Test B: Test passing wrong parameter to
	// RMobileCall::NotifyMobileDataCallCapsChange method
 	//-------------------------------------------------------------------------	

    const TInt bufSize = sizeof ( RMobileCall::TMobileCallDataCapsV1 );
	TBuf8<bufSize> wrongCallCaps2;
	wrongCallCaps2.FillZ(); // Fill buffer with zero
	call.NotifyMobileDataCallCapsChange(reqStatus, wrongCallCaps2);
	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
		
	CleanupStack::PopAndDestroy(4, this); // call, line, completeData, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-0009
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobileCall::NotifyMobileDataCallCapsChange for data calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange0009L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 completeData;
	CleanupClosePushL(completeData);

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyDataLineName);
	const TInt KCallId = 1;
	TName callName;
	TInt ret = call.OpenNewCall(line, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call);
	DialL(call, KCallId, RMobilePhone::ECircuitDataService);
	
	RMobileCall::TMobileCallDataCapsV1 completeCallCaps;
	completeCallCaps.iSpeedCaps = RMobileCall::KCapsSpeed48000;
	completeCallCaps.iProtocolCaps = RMobileCall::KCapsProtocolV34;
	completeCallCaps.iServiceCaps = RMobileCall::KCapsPacketAccessSyncUDI;
	completeCallCaps.iQoSCaps = RMobileCall::KCapsTransparentPreferred;
	completeCallCaps.iHscsdSupport = ETrue;
	completeCallCaps.iMClass = 2;
	completeCallCaps.iMaxRxTimeSlots = 4;
	completeCallCaps.iMaxTxTimeSlots = 8;
	completeCallCaps.iTotalRxTxTimeSlots = 10;
	completeCallCaps.iCodingCaps = RMobileCall::KCapsAiurCoding96;
	completeCallCaps.iAsymmetryCaps = RMobileCall::KCapsAsymmetryDownlink;
	completeCallCaps.iUserInitUpgrade = ETrue;
	completeCallCaps.iRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion1;
	completeCallCaps.iV42bisCaps = RMobileCall::KCapsV42bisRxDirection;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
	                     completeLtsyData(KCallId, RMobilePhone::ECircuitDataService, completeCallCaps);
	completeLtsyData.SerialiseL(completeData);

	// Open second client
	RTelServer telServer2;
	ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	RLine line2;
	RMobileCall call2;

	const TInt KCallId2 = 2;

	ret = line2.Open(phone2, KMmTsyDataLineName);
	ASSERT_EQUALS(KErrNone, ret)
   	CleanupClosePushL(line2);

	OpenNewCallLC(line2, call2);
	DialL(call2, KCallId2, RMobilePhone::ECircuitDataService);

	RBuf8 completeData2;
	CleanupClosePushL(completeData2);

	RMobileCall::TMobileCallDataCapsV1 completeCallCaps2;
	completeCallCaps2.iSpeedCaps = RMobileCall::KCapsSpeed56000;
	completeCallCaps2.iProtocolCaps = RMobileCall::KCapsProtocolV32;
	completeCallCaps2.iServiceCaps = RMobileCall::KCapsPacketAccessSyncRDI;
	completeCallCaps2.iQoSCaps = RMobileCall::KCapsTransparent;
	completeCallCaps2.iHscsdSupport = EFalse;
	completeCallCaps2.iMClass = 1;
	completeCallCaps2.iMaxRxTimeSlots = 2;
	completeCallCaps2.iMaxTxTimeSlots = 4;
	completeCallCaps2.iTotalRxTxTimeSlots = 6;
	completeCallCaps2.iCodingCaps = RMobileCall::KCapsAiurCoding144;
	completeCallCaps2.iAsymmetryCaps = RMobileCall::KCapsAsymmetryUplink;
	completeCallCaps2.iUserInitUpgrade = EFalse;
	completeCallCaps2.iRLPVersionCaps = RMobileCall::KCapsRLPMultiLinkVersion2;
	completeCallCaps2.iV42bisCaps = RMobileCall::KCapsV42bisBothDirections;
	
	TMockLtsyCallData1<RMobileCall::TMobileCallDataCapsV1>
	                     completeLtsyData2(KCallId2, RMobilePhone::ECircuitDataService, completeCallCaps2);
	completeLtsyData2.SerialiseL(completeData2);

	// open third call
	RMobileCall call3;
	ret = call3.OpenExistingCall(line2, callName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(call3);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobileCall::NotifyMobileDataCallCapsChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);
	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	
	TRequestStatus reqStatus2;
	RMobileCall::TMobileCallDataCapsV1 callCaps2;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg2(callCaps2);
	call2.NotifyMobileDataCallCapsChange(reqStatus2, callCapsPckg2);
	
	TRequestStatus reqStatus3;
	RMobileCall::TMobileCallDataCapsV1 callCaps3;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg3(callCaps3);
	call3.NotifyMobileDataCallCapsChange(reqStatus3, callCapsPckg3);
	
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobileCallNotifyMobileDataCallCapsChange, KErrNone, completeData2);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int())

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps, callCaps));

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps2, callCaps2));

	User::WaitForRequest(reqStatus3);
	ASSERT_EQUALS(KErrNone, reqStatus3.Int());
	ASSERT_TRUE(TComparator<RMobileCall::TMobileCallDataCapsV1>::IsEqual(completeCallCaps, callCaps3));

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(10, this); // call3, completeData2, call2, line2, phone2, telServer2, call, line, completeData, this

	}


/**
@SYMTestCaseID BA-CTSY-DATC-CNMDCCC-00011
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobileCall::NotifyMobileDataCallCapsChange for fax calls
@SYMTestPriority High
@SYMTestActions Invokes RMobileCall::NotifyMobileDataCallCapsChange for fax calls
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyDataCallControlFU::TestNotifyMobileDataCallCapsChange00011L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RLine line;
	RMobileCall call;
	
	OpenLineLC(line, KMmTsyFaxLineName);
	OpenNewCallLC(line, call);
	
	TRequestStatus reqStatus;
	RMobileCall::TMobileCallDataCapsV1 callCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg callCapsPckg(callCaps);

	//-------------------------------------------------------------------------
	// TEST: failure to dispatch request to CTSY
	// because RMobileCall::NotifyMobileDataCallCapsChange is not supported for fax calls
 	//-------------------------------------------------------------------------

	call.NotifyMobileDataCallCapsChange(reqStatus, callCapsPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int())
	
	CleanupStack::PopAndDestroy(3, this); // call, line, this
	
	}


