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
// Test step definitions for the DataCallControl functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestdatacallcontrol.h"
#include "cctsyinidata.h"
#include "tcallcontroltsytesthelper.h"

_LIT8(KWriteTestData, "Some data");

	
CCTSYIntegrationTestDataCallControlBase::CCTSYIntegrationTestDataCallControlBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
		iCallControlTestHelper(*this),
		iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestDataCallControlBase::~CCTSYIntegrationTestDataCallControlBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestDataCallControl0001::CCTSYIntegrationTestDataCallControl0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0001::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0001::~CCTSYIntegrationTestDataCallControl0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0001
 * @SYMFssID BA/CTSY/DATC-0001
 * @SYMTestCaseDesc Make a circuit switched data call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::GetCaps, RMobileCall::GetMobileCallCaps, RCall::HangUp, RMobileCall::NotifyMobileCallCapsChange, RCall::RecoverDataPort, RMobileCall::GetMobileCallStatus, RCall::GetCallParams, RMobileCall::GetMobileCallInfo, RCall::GetInfo, RCall::NotifyStatusChange, RMobileCall::NotifyMobileCallStatusChange, RCall::NotifyHookChange, RCall::NotifyCapsChange, RCall::LoanDataPort, RCall::GetStatus
 * @SYMTestExpectedResults Pass - Data call can be made. Data can be sent.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call status, call caps and call info are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure phone supports KCapsData caps. 
	RMobilePhone::TCaps mobilephonecaps;
	CHECK_EQUALS_L(phone.GetCaps(mobilephonecaps), KErrNone, _L("RMobilePhone::GetCaps"));
	TUint32 expectedmobilephoneCaps = RMobilePhone::KCapsData | RMobilePhone::KCapsVoice;
	TUint32 unwantedmobilephoneCaps = KNoUnwantedBits;
	CHECK_BITS_SET_L(mobilephonecaps.iFlags, expectedmobilephoneCaps, unwantedmobilephoneCaps,
					_L("RMobilePhone::GetCaps returned wrong dynamic caps"));
	
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get Call 1.
	RMobileCall& call1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Check call caps ===

	// Check RCall::GetCaps supports KCapsDial | KCapsData.
	RCall::TCaps callCaps;
	TUint32 wantedCallCaps = RCall::KCapsDial | RCall::KCapsData;
	ASSERT_EQUALS(call1.GetCaps(callCaps),KErrNone, _L("RCall::GetCaps returned an error"));
	ASSERT_BITS_SET(callCaps.iFlags, wantedCallCaps, KNoUnwantedBits, _L("RCall::GetCaps returned wrong caps"));

	// Check RMobileCall::GetMobileCallCaps returns iCallControlCaps in set of KCapsData | KCapsDial.
	TUint32 wantedCallControlCaps = RMobileCall::KCapsData | RMobileCall::KCapsDial;
	TUint32 wantedCallEventCaps = 0;
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg mobileCallCapsPckg(mobileCallCaps);
	ASSERT_EQUALS(call1.GetMobileCallCaps(mobileCallCapsPckg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned an error"));
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, wantedCallControlCaps, KNoUnwantedBits,   
                    _L("RMobileCall::GetMobileCallCaps returned wrong caps"));

	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps in set of 0.
	ASSERT_BITS_SET(mobileCallCaps.iCallEventCaps, wantedCallEventCaps, KNoUnwantedBits,
			      _L("RMobileCall::GetMobileCallCaps returned unexpected iCallEventCaps"));

	// ===  Check call status ===

	// Check RCall::GetStatus returns EStatusIdle.
	RCall::TStatus callStatus;
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an Error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusIdle, _L("RCall::GetStatus did not return EStatusIdle"));

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusIdle.
	RMobileCall::TMobileCallStatus mobileCallStatus;
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone,    
			_L("RMobileCall::GetMobileCallStatus returned an Error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusIdle,    
			_L("RMobileCall::GetMobileCallStatus did not set the status to EStatusIdle"));

	// ===  Check call info ===

	// Check RCall::GetInfo returns valid call name.
	RCall::TCallInfo callInfo;
	ASSERT_EQUALS(call1.GetInfo(callInfo), KErrNone, _L("RCall::GetInfo returned an error"));
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, _L("RCall::GetInfo returned an invalid call name"));
	
	// Check RCall::GetInfo returns valid data line name.
	ASSERT_TRUE(callInfo.iLineName.Length() > 0,_L("RCall::GetInfo returned an invalid  line name"));
	
	// Check RCall::GetInfo returns hook status of EHookStatusOn.
	// $CTSYProblem CMmCallTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aCallInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RCall::GetInfo returned wrong status, should be EHookStatusUnknown"));

	// Check RCall::GetInfo returns call status of EStatusIdle.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusIdle, _L("RCall::GetInfo returned wrong status, should be EStatusIdle"));

	// Check RCall::GetInfo returns call duration of 0.
	ASSERT_EQUALS(callInfo.iDuration.Int(), 0, _L("RCall::GetInfo returned duration > 0, should be 0"));

	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg mobileCallInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(call1.GetMobileCallInfo(mobileCallInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iValid > 0, _L("RMobileCall::GetMobileCallInfo returned iValid <= 0 flags, should be > 0"));
	
	// Check RMobileCall::GetMobileCallInfo returns valid call name.
	ASSERT_TRUE(mobileCallInfo.iCallName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid call name"));

	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));

	// post notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	RCall::TStatus notifyCallStatus;
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyCallStatus);

	// Post notifiers RMobileCall::NotifyMobileCallStatusChange
    RMobileCall::TMobileCallStatus notifyMobileCallStatus;
    TExtEtelRequestStatus notifyMobileCallChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
    CleanupStack::PushL(notifyMobileCallChangeStatus);
    call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, notifyMobileCallStatus);

	// Post Notifier for RCall::NotifyCapsChange
	TCoreEtelRequestStatus<RCall> notifyCapsChangeStatus (call1,&RCall::NotifyCapsChangeCancel);	
	CleanupStack::PushL(notifyCapsChangeStatus);
	RCall::TCaps caps;
	call1.NotifyCapsChange(notifyCapsChangeStatus,caps);	

	// post a notifier for RMobileCall::NotifyMobileCallCapsChange
	TExtEtelRequestStatus notifyMobileCallCapsChangeStatus(call1, EMobileCallNotifyMobileCallCapsChange);
	CleanupStack::PushL(notifyMobileCallCapsChangeStatus);
	call1.NotifyMobileCallCapsChange(notifyMobileCallCapsChangeStatus, mobileCallCapsPckg);


	// post notifier for RCall::NotifyHookChange
	// @CTSYProblem NotifyHookChange has never been called
	TCoreEtelRequestStatus<RCall> notifyHookChangeStatus(call1, &RCall::NotifyHookChangeCancel);
	CleanupStack::PushL(notifyHookChangeStatus);
	RCall::THookStatus hookStatus;
	call1.NotifyHookChange(notifyHookChangeStatus, hookStatus);

	// Dial a number that answers. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	DisplayUserInteractionPromptL(_L("Board now dials to DataNumber1 (press any key to start dialling) "), ETimeLong);
	call1.Dial(dialStatus, number);
    DEBUG_PRINTF2(_L("Dialling data call: %S"), &number);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusDialling;
	const TInt wantedStatus = KErrNone;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyCallStatus,
														 expectedCallStatus,
														 wantedStatus);
														 
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyCallStatus);
	expectedCallStatus = RCall::EStatusConnecting;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyCallStatus,
														 expectedCallStatus,
														 wantedStatus);
														 
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyCallStatus);
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyCallStatus,
														 expectedCallStatus,
														 wantedStatus);

	// Check RCall::GetStatus returns EStatusConnected.
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return EStatusConnected"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileCallChangeStatus,
																		 notifyMobileCallStatus,
																		 expectedMobileCallStatus,
																	  	 wantedStatus);
														 
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, notifyMobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
    iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileCallChangeStatus,
																		 notifyMobileCallStatus,
																		 expectedMobileCallStatus,
																	  	 wantedStatus);
														 
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, notifyMobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileCallChangeStatus,
																		 notifyMobileCallStatus,
																		 expectedMobileCallStatus,
																	  	 wantedStatus);
    call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, notifyMobileCallStatus);


	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone, 
			_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, 
			_L("RMobileCall::GetMobileCallStatus did not return a status EStatusConnected"));

	// ===  Get call parameters ===
	RMobileCall::TMobileDataCallParamsV1 checkParams;
	RMobileCall::TMobileDataCallParamsV1Pckg checkParamsPkg(checkParams);
	call1.GetCallParams(checkParamsPkg);
	
	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iSpeed!=RMobileCall::ESpeedUnspecified
	ASSERT_TRUE(checkParams.iSpeed != RMobileCall::ESpeedUnspecified,
	            _L("RCall::GetCallParams returned wrongly RMobileCall::ESpeedUnspecified"));
	
	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iService != RMobileCall::EServiceUnspecified
	ASSERT_TRUE(checkParams.iService != RMobileCall::EServiceUnspecified,
	            _L("RCall::GetCallParams returned wrongly RMobileCall::EServiceUnspecified"));
	
	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iProtocol != RMobileCall::EProtocolUnspecified
	// @CTSYProblem RCall::GetCallParams returns EProtocolUnspecified always, it comes from LTSY in CMmCallGsmWcdmaExt::ReceivedCallParams
	ASSERT_EQUALS(checkParams.iProtocol,  RMobileCall::EProtocolUnspecified,
	            _L("RCall::GetCallParams returned wrongly RMobileCall::EProtocolUnspecified"));
	
	// ===  Check call caps ===

	// Check RCall::NotifyCapsChange completes with caps in set of KCapsData | KCapsHangUp | KCapsLoanDataPort.
	// Check RCall::NotifyCapsChange completes with caps NOT in set of KCapsDial | KCapsConnect
	RCall::TCaps wantedCaps, unwantedCaps;
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp | RCall::KCapsLoanDataPort;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsConnect;
	iCallControlTestHelper.WaitForCallNotifyCapsChange(call1,
													   notifyCapsChangeStatus,
													   caps,
													   wantedCaps,
													   unwantedCaps,
													   wantedStatus);

	// Check RCall::GetCaps supports caps in set of KCapsHangUp | KCapsData | KCapsLoanDataPort.
	// Check RCall::GetCaps supports caps NOT in set of KCapsDial | KCapsConnect
	ASSERT_EQUALS(call1.GetCaps(caps), KErrNone, _L("RCall::GetCaps returned an Error"));
	wantedCaps.iFlags = RCall::KCapsHangUp | RCall::KCapsData | RCall::KCapsLoanDataPort;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsConnect;
	ASSERT_BITS_SET(caps.iFlags,
					wantedCaps.iFlags,
					unwantedCaps.iFlags,
			        _L("RCall::GetCaps returned wrong caps"));

	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps in set of KCapsData | KCapsHangUp | KCapsTransfer | KCapsLoanDataPort
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps NOT in set of KCapsDial | KCapsConnect
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallEventCaps in set of KCapsLocalTransfer | KCapsRemoteTerminate
	RMobileCall::TMobileCallCapsV1 wantedMobileCaps;
	RMobileCall::TMobileCallCapsV1 unwantedMobileCaps;
    // @CTSYProblem No KCapsTransfer is sent. 
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsData | RMobileCall::KCapsHangUp | RMobileCall::KCapsLoanDataPort /* | RMobileCall::KCapsTransfer */;
	unwantedMobileCaps.iCallControlCaps = RMobileCall::KCapsDial | RMobileCall::KCapsConnect;
	wantedMobileCaps.iCallEventCaps = RMobileCall::KCapsLocalTransfer | RMobileCall::KCapsRemoteTerminate;
	TCmpRMobileCallTMobileCallCaps mobileCapsComp(wantedMobileCaps, mobileCallCaps, *this);
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallCapsChange(call1,
																	   notifyMobileCallCapsChangeStatus,
																	   mobileCapsComp,
																	   wantedStatus);
	call1.NotifyMobileCallCapsChange(notifyMobileCallCapsChangeStatus, mobileCallCapsPckg);

	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps in set of KCapsData | KCapsHangUp | KCapsTransfer | KCapsLoanDataPort
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsDial | KCapsConnect
	ASSERT_EQUALS(call1.GetMobileCallCaps(mobileCallCapsPckg), KErrNone,  
			      _L("RMobileCall::GetMobileCallCaps returned an error"));
    // @CTSYProblem No KCapsTransfer is set. 
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsData | RMobileCall::KCapsHangUp | RMobileCall::KCapsLoanDataPort /* | RMobileCall::KCapsTransfer */;
	unwantedMobileCaps.iCallControlCaps = RMobileCall::KCapsDial | RMobileCall::KCapsConnect;
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps,
                    wantedMobileCaps.iCallControlCaps,
                    unwantedMobileCaps.iCallControlCaps,
                    _L("RMobileCall::GetMobileCallCaps returned unexpected iCallControlCaps"));

	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps in set of KCapsLocalTransfer | KCapsRemoteTerminate
	wantedMobileCaps.iCallEventCaps = RMobileCall::KCapsLocalTransfer | RMobileCall::KCapsRemoteTerminate;
    ASSERT_BITS_SET(mobileCallCaps.iCallEventCaps, wantedMobileCaps.iCallEventCaps,
                    KNoUnwantedBits, 
                    _L("RMobileCall::GetMobileCallCaps returned unexpected iCallEventCaps"));
	
	// ===  Check call info ===

	// Check RCall::GetInfo returns valid call name.
	ASSERT_EQUALS(call1.GetInfo(callInfo), KErrNone, _L("RCall::GetInfo returned an error"));
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, _L("RCall::GetInfo returned an invalid call name"));
	
	// Check RCall::GetInfo returns valid voice line name.
	ASSERT_TRUE(callInfo.iLineName.Length() > 0,_L("RCall::GetInfo returned an invalid  line name"));
	
	// Check RCall::GetInfo returns hook status of EHookStatusOff.
	// $CTSYProblem CMmCallTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aCallInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RCall::GetInfo returned wrong status, should be EHookStatusUnknown"));
	
	// Check RCall::GetInfo returns call status of EStatusConnected.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusConnected, _L("RCall::GetInfo returned wrong status, should be EStatusConnected"));

	// Check RCall::GetInfo returns call duration of > 0.
	ASSERT_TRUE(callInfo.iDuration.Int() > 0, _L("RCall::GetInfo returned duration <= 0, should be > 0"));

	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes makred as valid in iValid.
	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_EQUALS(call1.GetMobileCallInfo(mobileCallInfoPckg), KErrNone,  
				  _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0,  
				_L("RMobileCall::GetMobileCallInfo returned an invalid line name"));
	ASSERT_TRUE(mobileCallInfo.iValid > 0,  
				_L("RMobileCall::GetMobileCallInfo returned unexpected flags in iValid"));
	
	iCallControlTestHelper.CheckForValidCallInfo(mobileCallInfo);

	// ===  Check hook status ===

	// Check RCall::NotifyHookChange completes with EHookStatusOff.
	// $CTSYProblem adaptation does not support RCall::NotifyHookChange, it gives status -5 (KErrNotSupported).
	RCall::THookStatus expectedHookStatus = RCall::EHookStatusOff;
	iCallControlTestHelper.WaitForCallNotifyHookChange(call1,
													notifyHookChangeStatus,
													hookStatus, 
													expectedHookStatus,
													wantedStatus);

	// ===  Loan data port ===

	// Declare a RCall::TCommPort 
	RCall::TCommPort commPort;

	// Loan the data port with RCall::LoanDataPort passing in the TCommPort 
	ASSERT_EQUALS(call1.LoanDataPort(commPort), KErrNone,  
				  _L("RCall::LoanDataPort returned an error"));

	// ===  Check call caps ===

	// Check RCall::NotifyCapsChange completes with caps in set of KCapsData | KCapsHangUp | KCapsRecoverDataPort.
	// Check RCall::NotifyCapsChange completes with caps NOT in set of KCapsDial | KCapsConnect
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp | RCall::KCapsRecoverDataPort;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsConnect;
	iCallControlTestHelper.WaitForCallNotifyCapsChange(call1,
													   notifyCapsChangeStatus,
													   caps,
													   wantedCaps,
													   unwantedCaps,
													   wantedStatus);

	// Check RCall::GetCaps supports caps in set of KCapsHangUp | KCapsData | KCapsRemoteTerminate.
	// Check RCall::GetCaps supports caps NOT in set of KCapsDial | KCapsConnect
    // @CTSYProblem KCapsRemoteTerminate is not set
	ASSERT_EQUALS(call1.GetCaps(caps), KErrNone, _L("RCall::GetCaps returned an Error"));
	wantedCaps.iFlags = RCall::KCapsHangUp | RCall::KCapsData /* | RMobileCall::KCapsRemoteTerminate */;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsConnect;
	ASSERT_BITS_SET(caps.iFlags,
					wantedCaps.iFlags,
					unwantedCaps.iFlags,
			        _L("RCall::GetCaps returned wrong caps"));


	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps in set of KCapsData | KCapsHangUp | KCapsTransfer | KCapsRemoteTerminate
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsDial | KCapsConnect
	ASSERT_EQUALS(call1.GetMobileCallCaps(mobileCallCapsPckg), KErrNone,  
			      _L("RMobileCall::GetMobileCallCaps returned an error"));
	wantedCaps.iFlags = RMobileCall::KCapsData | RMobileCall::KCapsHangUp;
	unwantedCaps.iFlags = RMobileCall::KCapsDial | RMobileCall::KCapsConnect;
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps,
					wantedCaps.iFlags,
					unwantedCaps.iFlags,
					_L("RMobileCall::GetMobileCallCaps returned unexpected iCallControlCaps"));

	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps in set of KCapsLocalTransfer | KCapsRemoteTerminate
	wantedCaps.iFlags = RMobileCall::KCapsLocalTransfer | RMobileCall::KCapsRemoteTerminate;
	ASSERT_BITS_SET(mobileCallCaps.iCallEventCaps, wantedCaps.iFlags,
                    KNoUnwantedBits,
                    _L("RMobileCall::GetMobileCallCaps returned unexpected iCallEventCaps"));

	// ===  Write some data to the port ===

	// RCommServ cs; 
	RCommServ cs;

	// cs.Connect(); 
	ASSERT_EQUALS(cs.Connect(), KErrNone,  _L("RRCommServ::Connect returned an error"));

	// RComm port; 
	RComm port;
	
	// port.Open(cs,commPort.iPort,ECommShared); 
	port.Open(cs, commPort.iPort, ECommShared);
	
	// port.Write(stat,KWriteTestData);
	TRequestStatus writeStatus;
	port.Write(writeStatus, KWriteTestData); 

	// port.Close();
	port.Close();

	// cs.Close();
	cs.Close();

	// ===  Recover the data port ===

	// Recover data port with RCall::RecoverDataPort 
	ASSERT_EQUALS(call1.RecoverDataPort(), KErrNone, _L("RMobileCall::RecoverDataPort() returned an error"));

	// Hang up with RCall::HangUp 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	

	// ===  Check call caps ===

	// Check RCall::NotifyCapsChange completes with caps in set of KCapsData | KCapsDial.
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsDial;
	unwantedCaps.iFlags = 0;	
	iCallControlTestHelper.WaitForCallNotifyCapsChange(call1,
													   notifyCapsChangeStatus,
													   caps,
													   wantedCaps,
													   unwantedCaps,
													   wantedStatus);

	// Check RCall::GetCaps supports KCapsData | KCapsHangUp.
	// @CTSYProblem KCapsHangUp is not set
	ASSERT_EQUALS(call1.GetCaps(caps), KErrNone, _L("RCall::GetCaps returned an Error"));
    wantedCaps.iFlags = RCall::KCapsData;
	ASSERT_BITS_SET(caps.iFlags, wantedCaps.iFlags, KNoUnwantedBits,
			        _L("RCall::GetCaps returned wrong caps"));

	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps in set of KCapsData | KCapsDial.
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallEventCaps in set of 0.
    // @CTSYProblem NotifyMobileCallCapsChange is not called

	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps in set of KCapsData | KCapsDial.
	ASSERT_EQUALS(call1.GetMobileCallCaps(mobileCallCapsPckg), KErrNone,  
			      _L("RMobileCall::GetMobileCallCaps returned an error"));
    wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsDial | RMobileCall::KCapsData;
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, wantedMobileCaps.iCallControlCaps, KNoUnwantedBits,
			      _L("RMobileCall::GetMobileCallCaps returned unexpected iCallControlCaps"));

	// Check RMobileCall::GetMobileCallCaps includes iCallEventCaps in set of 0.
	wantedMobileCaps.iCallEventCaps = 0;
	ASSERT_BITS_SET(mobileCallCaps.iCallEventCaps, 0, KNoUnwantedBits,
			      _L("RMobileCall::GetMobileCallCaps returned unexpected iCallEventCaps"));

	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusHangingUp -> EStatusIdle.
	expectedCallStatus = RCall::EStatusHangingUp;
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyCallStatus);
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyCallStatus,
														 expectedCallStatus,
														 wantedStatus);
														 
	call1.NotifyStatusChange(notifyStatusChangeStatus, notifyCallStatus);
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 notifyCallStatus,
														 expectedCallStatus,
														 wantedStatus);
														 

	// Check RCall::GetStatus returns EStatusIdle.
	ASSERT_EQUALS(call1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusIdle, _L("RCall::GetStatus did not return EStatusIdle"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileCallChangeStatus,
																		 notifyMobileCallStatus,
																		 expectedMobileCallStatus,
																	  	 wantedStatus);

	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, notifyMobileCallStatus);
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
    iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileCallChangeStatus,
																		 notifyMobileCallStatus,
																		 expectedMobileCallStatus,
																	  	 wantedStatus);

	// Check RMobileCall::GetMobileCallStatus returns status of EStatusIdle.
	ASSERT_EQUALS(call1.GetMobileCallStatus(mobileCallStatus), KErrNone, 
			_L("RMobileCall::GetMobileCallStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusIdle, 
			_L("RMobileCall::GetMobileCallStatus did not return a status EStatusIdle"));

	// ===  Check call info ===

	// Check RCall::GetInfo returns valid call name.
	ASSERT_EQUALS(call1.GetInfo(callInfo), KErrNone, _L("RCall::GetInfo returned an error"));
	ASSERT_TRUE(callInfo.iCallName.Length() > 0, _L("RCall::GetInfo returned an invalid call name"));

	// Check RCall::GetInfo returns valid voice line name.
	ASSERT_TRUE(callInfo.iLineName.Length() > 0,_L("RCall::GetInfo returned an invalid  line name"));

	// Check RCall::GetInfo returns hook status of EHookStatusOn.
	// $CTSYProblem CMmCallTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aCallInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RCall::GetInfo returned wrong status, should be EHookStatusUnknown"));

	// Check RCall::GetInfo returns call status of EStatusIdle.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusIdle, _L("RCall::GetInfo returned wrong status, should be EStatusIdle"));

	// Check RCall::GetInfo returns call duration of > 0.
	ASSERT_TRUE(callInfo.iDuration.Int() > 0, _L("RCall::GetInfo returned duration <= 0, should be > 0"));

	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes makred as valid in iValid.
	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_EQUALS(call1.GetMobileCallInfo(mobileCallInfoPckg), KErrNone,  
				  _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() > 0,  
				_L("RMobileCall::GetMobileCallInfo returned an invalid line name"));
	ASSERT_TRUE(mobileCallInfo.iValid > 0,  
				_L("RMobileCall::GetMobileCallInfo returned unexpected flags in iValid"));

	// ===  Check hook status ===

	// Check RCall::NotifyHookChange completes with EHookStatusOn.
	// $CTSYProblem adaptation does not support RCall::NotifyHookChange, it gives status -5 (KErrNotSupported).
	expectedHookStatus = RCall::EHookStatusOn;
	iCallControlTestHelper.WaitForCallNotifyHookChange(call1,
													notifyHookChangeStatus,
													hookStatus, 
													expectedHookStatus,
													wantedStatus);

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyStatusChangeStatus
	// notifyMobileCallChangeStatus
	// notifyCapsChangeStatus
	// notifyMobileCallCapsChangeStatus
	// notifyHookChangeStatus
	// dialStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(7, &notifyStatusChangeStatus);

	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0001");
	}



CCTSYIntegrationTestDataCallControl0002::CCTSYIntegrationTestDataCallControl0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0002::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0002::~CCTSYIntegrationTestDataCallControl0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0002
 * @SYMFssID BA/CTSY/DATC-0002
 * @SYMTestCaseDesc Make a circuit switched data call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RMobileCall::NotifyMobileDataCallCapsChange, RCall::NotifyStatusChange, RMobileCall::GetMobileDataCallCaps, RCall::Dial, RCall::GetBearerServiceInfo
 * @SYMTestExpectedResults Pass - Data call caps and bearer service info correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify data call caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get Call 1.
	RMobileCall& call1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Check default caps set by CTSY ===
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg CallCapsPckg(dataCallCaps);
	ASSERT_EQUALS(call1.GetMobileDataCallCaps(CallCapsPckg), KErrNone, _L("RMobileCall::GetMobileDataCallCaps returned an errror"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps in set of RMobileCall::KCapsSpeedAutobauding
	//         | RMobileCall::KCapsSpeed2400 
	//         | RMobileCall::KCapsSpeed4800 
	//         | RMobileCall::KCapsSpeed9600 
	//         | RMobileCall::KCapsSpeed14400 
	//         | RMobileCall::KCapsSpeed19200 
	//         | RMobileCall::KCapsSpeed28800 
	//         | RMobileCall::KCapsSpeed38400 
	//         | RMobileCall::KCapsSpeed43200 
	//         | RMobileCall::KCapsSpeed56000 
	TUint32 defaultSpeedCaps = RMobileCall::KCapsSpeedAutobauding |
                               RMobileCall::KCapsSpeed2400 |
                               RMobileCall::KCapsSpeed4800 |
                               RMobileCall::KCapsSpeed9600 |
                               RMobileCall::KCapsSpeed14400 |
                               RMobileCall::KCapsSpeed19200 |
                               RMobileCall::KCapsSpeed28800 |
                               RMobileCall::KCapsSpeed38400 |
                               RMobileCall::KCapsSpeed43200 |
                               RMobileCall::KCapsSpeed56000;
	ASSERT_BITS_SET(dataCallCaps.iSpeedCaps, defaultSpeedCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default speed caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps in set of 
	//			 RMobileCall::KCapsDataCircuitAsynchronous
	//         | RMobileCall::KCapsDataCircuitAsynchronousRDI 
	//         | RMobileCall::KCapsPADAsyncUDI 
	//         | RMobileCall::KCapsPADAsyncRDI 
	//         | RMobileCall::KCapsPacketAccessSyncUDI 
	//         | RMobileCall::KCapsPacketAccessSyncRDI 
	TUint32 defaultServiceCaps = RMobileCall::KCapsDataCircuitAsynchronous
						         | RMobileCall::KCapsDataCircuitAsynchronousRDI 
						         | RMobileCall::KCapsPADAsyncUDI 
						         | RMobileCall::KCapsPADAsyncRDI 
						         | RMobileCall::KCapsPacketAccessSyncUDI 
						         | RMobileCall::KCapsPacketAccessSyncRDI;
	ASSERT_BITS_SET(dataCallCaps.iServiceCaps, defaultServiceCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default service caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps in set of 
	//			RMobileCall::KCapsProtocolV22bis
	//         | RMobileCall::KCapsProtocolV32 
	//         | RMobileCall::KCapsProtocolV34 
	//         | RMobileCall::KCapsProtocolV110 
	//         | RMobileCall::KCapsProtocolV120 
	//         | RMobileCall::KCapsProtocolX31FlagStuffing 
	//         | RMobileCall::KCapsProtocolPIAFS 
	TUint32 defaultProtocolCaps = 	RMobileCall::KCapsProtocolV22bis
									| RMobileCall::KCapsProtocolV32 
									| RMobileCall::KCapsProtocolV34 
									| RMobileCall::KCapsProtocolV110 
									| RMobileCall::KCapsProtocolV120 
									| RMobileCall::KCapsProtocolX31FlagStuffing 
									| RMobileCall::KCapsProtocolPIAFS;
	ASSERT_BITS_SET(dataCallCaps.iProtocolCaps, defaultProtocolCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default protocol caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps in set of 
	//			RMobileCall::KCapsTransparent
	//         | RMobileCall::KCapsNonTransparent 
	//         | RMobileCall::KCapsTransparentPreferred 
	//         | RMobileCall::KCapsNonTransparentPreferred 
	TUint32 defaultQoSCaps =  RMobileCall::KCapsTransparent
							| RMobileCall::KCapsNonTransparent 
							| RMobileCall::KCapsTransparentPreferred 
							| RMobileCall::KCapsNonTransparentPreferred;
	ASSERT_BITS_SET(dataCallCaps.iQoSCaps, defaultQoSCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default QoS caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iHscsdSupport=ETrue
	ASSERT_TRUE(dataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::GetMobileDataCallCaps returned hscsd support EFalse"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iRLPVersionCaps in set of 
	//			 RMobileCall::KCapsRLPSingleLinkVersion0
	//         | RMobileCall::KCapsRLPSingleLinkVersion1 
	//         | RMobileCall::KCapsRLPMultiLinkVersion2 
	TUint32 defaultRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion0
									| RMobileCall::KCapsRLPSingleLinkVersion1 
									| RMobileCall::KCapsRLPMultiLinkVersion2;
	ASSERT_BITS_SET(dataCallCaps.iRLPVersionCaps, defaultRLPVersionCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default RLPVersion caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iV42bisCaps in set of 
	//			RMobileCall::KCapsV42bisTxDirection
	//         | RMobileCall::KCapsV42bisRxDirection 
	//         | RMobileCall::KCapsV42bisBothDirections 
	TUint32 defaultV42bisCaps = RMobileCall::KCapsV42bisTxDirection
								| RMobileCall::KCapsV42bisRxDirection 
								| RMobileCall::KCapsV42bisBothDirections;
	ASSERT_BITS_SET(dataCallCaps.iV42bisCaps, defaultV42bisCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default V42bis caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iAsymmetryCaps in set of 
	// 			RMobileCall::KCapsAsymmetryNoPreference
	//         | RMobileCall::KCapsAsymmetryDownlink 
	//         | RMobileCall::KCapsAsymmetryUplink 
	TUint32 defaultAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference
									| RMobileCall::KCapsAsymmetryDownlink 
									| RMobileCall::KCapsAsymmetryUplink;
	ASSERT_BITS_SET(dataCallCaps.iAsymmetryCaps, defaultAsymmetryCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default asymmetry caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iCodingCaps in set of 
	//			RMobileCall::KCapsTchCoding48
	//         | RMobileCall::KCapsTchCoding96 
	//         | RMobileCall::KCapsTchCoding144 
	TUint32 defaultCodingCaps = RMobileCall::KCapsTchCoding48
								| RMobileCall::KCapsTchCoding96 
								| RMobileCall::KCapsTchCoding144;
	ASSERT_BITS_SET(dataCallCaps.iCodingCaps, defaultCodingCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default coding caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMClass = 0x6
	ASSERT_EQUALS(dataCallCaps.iMClass, 0x6, _L("RMobileCall::GetMobileDataCallCaps.iMClass did not return 0x6"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxRxTimeSlots = 0x3
	ASSERT_EQUALS(dataCallCaps.iMaxRxTimeSlots, 0x3, _L("RMobileCall::GetMobileDataCallCaps.iMaxRxTimeSlots did not return 0x3"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxTxTimeSlots = 0x2
	ASSERT_EQUALS(dataCallCaps.iMaxTxTimeSlots, 0x2, _L("RMobileCall::GetMobileDataCallCaps.iMaxTxTimeSlots did not return 0x2"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iTotalRxTxTimeSlots = 0x4
	ASSERT_EQUALS(dataCallCaps.iTotalRxTxTimeSlots, 0x4, _L("RMobileCall::GetMobileDataCallCaps.iTotalRxTxTimeSlots did not return 0x4"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iUserInitUpgrade=EFalse
	ASSERT_TRUE(dataCallCaps.iUserInitUpgrade == EFalse, _L("RMobileCall::GetMobileDataCallCaps.iUserInitUpgrade did not return EFalse"));

	// ===  Check bearer service with no call connected ===
	RCall::TBearerService bearerService;

	// Check RCall::GetBearerServiceInfo returns KErrNotFound
	ASSERT_EQUALS(call1.GetBearerServiceInfo(bearerService), KErrNotFound, 
				  _L("RCall::GetBearerServiceInfo didn't return KErrNotFound"));

	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerCaps = RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown
	TUint bearerCaps = RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown;
	ASSERT_BITS_SET(bearerService.iBearerCaps, bearerCaps, KNoUnwantedBits,
					_L("RCall::GetBearerServiceInfo returned wrong bearer caps"));
	
	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerSpeed=RCall::EBearerDataUnknown
    TUint bearerSpeed = RCall::EBearerDataUnknown;
	ASSERT_BITS_SET(bearerService.iBearerSpeed, bearerSpeed, KNoUnwantedBits,
					_L("RCall::GetBearerServiceInfo returned wrong bearer speed"));
	
	// ===  Dial a call with call params ===

	// Set up a RMobileCall::TMobileDataCallParamsV1 with 
	RMobileCall::TMobileDataCallParamsV1 mobileDataCallParams;
	// iService = RMobileCall::EServiceDataCircuitAsync 
	mobileDataCallParams.iService = RMobileCall::EServiceDataCircuitAsync;
	// iSpeed = RMobileCall::ESpeed9600 
	mobileDataCallParams.iSpeed = RMobileCall::ESpeed9600;
	// iProtocol = RMobileCall::EProtocolV32 
	mobileDataCallParams.iProtocol = RMobileCall::EProtocolV32;
	// iQoS = RMobileCall::EQoSNonTransparent 
	mobileDataCallParams.iQoS = RMobileCall::EQoSNonTransparent;

	RMobileCall::TMobileDataCallParamsV1Pckg mobileDataCallParamsPkg(mobileDataCallParams);

	// post notifier for RCall::NotifyStatusChange
	TCoreEtelRequestStatus<RCall> notifyCallStatusChangeStatus (call1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	RCall::TStatus callStatus;
	call1.NotifyStatusChange(notifyCallStatusChangeStatus,callStatus);

	// Post notify RMobileCall::NotifyMobileDataCallCapsChange
	RMobileCall::TMobileCallDataCapsV1 notifydataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg notifydataCallCapsPckg(notifydataCallCaps);
	TExtEtelRequestStatus notifydataCallCapsStatus(call1, EMobileCallNotifyMobileDataCallCapsChange);
	CleanupStack::PushL(notifydataCallCapsStatus);
	call1.NotifyMobileDataCallCapsChange(notifydataCallCapsStatus, notifydataCallCapsPckg);

	// Dial a number that answers using the data call params 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);		
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse,  
			_L("GetStringFromConfig did not complete as expected"));
	call1.Dial(dialStatus,mobileDataCallParamsPkg, number);
	// Wait for completion. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, 
			_L("RCall::Dial returned an error"));

	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	const TInt expectedStatus = KErrNone;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);

	// ===  Check mobile data call caps ===
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifydataCallCapsStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyMobileDataCallCapsChange timed-out"));

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iHscsdSupport=EFalse
	ASSERT_TRUE(notifydataCallCaps.iHscsdSupport == EFalse, _L("RMobileCall::NotifyMobileDataCallCapsChange returned hscsd did not support ETrue"));
	
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iSpeedCaps=RMobileCall::KCapsSpeed9600
	ASSERT_BITS_SET(notifydataCallCaps.iSpeedCaps, RMobileCall::KCapsSpeed9600, KNoUnwantedBits,
					_L("RMobileCall::NotifyMobileDataCallCapsChange returned wrong speed caps"));
	
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iProtocolCaps=RMobileCall::KCapsProtocolV32
	ASSERT_BITS_SET(notifydataCallCaps.iProtocolCaps, RMobileCall::KCapsProtocolV32, KNoUnwantedBits,
					_L("RMobileCall::NotifyMobileDataCallCapsChange returned wrong protocol caps"));

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iQoSCaps=RMobileCall::KCapsNonTransparent
	ASSERT_BITS_SET(notifydataCallCaps.iQoSCaps, RMobileCall::KCapsNonTransparent, KNoUnwantedBits,
					_L("RMobileCall::NotifyMobileDataCallCapsChange returned wrong QoS caps"));

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iServiceCaps=RMobileCall::KCapsDataCircuitAsynchronous
	ASSERT_BITS_SET(notifydataCallCaps.iServiceCaps, RMobileCall::KCapsDataCircuitAsynchronous, KNoUnwantedBits,
					_L("RMobileCall::NotifyMobileDataCallCapsChange returned wrong service caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iHscsdSupport=ETrue
	ASSERT_TRUE(dataCallCaps.iHscsdSupport, _L("RMobileCall::GetMobileDataCallCaps returned hscsd support ETrue"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps=RMobileCall::KCapsSpeed9600
	ASSERT_EQUALS(call1.GetMobileDataCallCaps(CallCapsPckg), KErrNone, _L("RMobileCall::GetMobileDataCallCaps returned an errror "));
	ASSERT_BITS_SET(dataCallCaps.iSpeedCaps, RMobileCall::KCapsSpeed9600, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong speed caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps=RMobileCall::KCapsProtocolV32
	ASSERT_BITS_SET(dataCallCaps.iProtocolCaps, RMobileCall::KCapsProtocolV32, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong protocol caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps=RMobileCall::KCapsNonTransparent
	ASSERT_BITS_SET(dataCallCaps.iQoSCaps, RMobileCall::KCapsNonTransparent, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong QoS caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps=RMobileCall::KCapsDataCircuitAsynchronous
	ASSERT_BITS_SET(dataCallCaps.iServiceCaps, RMobileCall::KCapsDataCircuitAsynchronous, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong service caps"));

	// ===  Get bearer service info with call connected ===

	// Check RCall::GetBearerServiceInfo returns KErrNone
	ASSERT_EQUALS(call1.GetBearerServiceInfo(bearerService), KErrNone, 
				  _L("RCall::GetBearerServiceInfo didn't return KErrNone"));

	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerCaps !=RCall::KBearerCapsCompressionUnknown
	ASSERT_TRUE(bearerService.iBearerCaps != RCall::KBearerCapsCompressionUnknown, _L("RCall::GetBearerServiceInfo returned wrong bearer caps compression"));

	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerCaps != RCall::KBearerCapsProtocolUnknown
	ASSERT_TRUE(bearerService.iBearerCaps != RCall::KBearerCapsProtocolUnknown, _L("RCall::GetBearerServiceInfo returned wrong bearer caps protocol"));

	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerSpeed !=RCall::EBearerDataUnknown
	ASSERT_TRUE(bearerService.iBearerSpeed != RCall::EBearerDataUnknown, _L("RCall::GetBearerServiceInfo returned wrong bearer speed"));

	// ===  Hang up call ===
	call1.NotifyStatusChange(notifyCallStatusChangeStatus,callStatus);

	// Hang up call with RCall::HangUp 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));

	// Check RCall::NotifyStatusChange completes with EStatusIdle.
	expectedCallStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyCallStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 expectedStatus);

	// ===  Check caps reset ===
	ASSERT_EQUALS(call1.GetMobileDataCallCaps(CallCapsPckg), KErrNone, _L("RMobileCall::GetMobileDataCallCaps returned an errror"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps in set of 
	//			 RMobileCall::KCapsSpeedAutobauding
	//         | RMobileCall::KCapsSpeed2400 
	//         | RMobileCall::KCapsSpeed4800 
	//         | RMobileCall::KCapsSpeed9600 
	//         | RMobileCall::KCapsSpeed14400 
	//         | RMobileCall::KCapsSpeed19200 
	//         | RMobileCall::KCapsSpeed28800 
	//         | RMobileCall::KCapsSpeed38400 
	//         | RMobileCall::KCapsSpeed43200 
	//         | RMobileCall::KCapsSpeed56000 
	defaultSpeedCaps = RMobileCall::KCapsSpeedAutobauding |
						 RMobileCall::KCapsSpeed2400 |
						 RMobileCall::KCapsSpeed4800 |
						 RMobileCall::KCapsSpeed9600 |
						 RMobileCall::KCapsSpeed14400 |
						 RMobileCall::KCapsSpeed19200 |
						 RMobileCall::KCapsSpeed28800 |
						 RMobileCall::KCapsSpeed38400 |
						 RMobileCall::KCapsSpeed43200 |
						 RMobileCall::KCapsSpeed56000;
	ASSERT_BITS_SET(dataCallCaps.iSpeedCaps, defaultSpeedCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default speed caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps in set of 
	//RMobileCall::KCapsDataCircuitAsynchronous
	//         | RMobileCall::KCapsDataCircuitAsynchronousRDI 
	//         | RMobileCall::KCapsPADAsyncUDI 
	//         | RMobileCall::KCapsPADAsyncRDI 
	//         | RMobileCall::KCapsPacketAccessSyncUDI 
	//         | RMobileCall::KCapsPacketAccessSyncRDI 
	defaultServiceCaps = RMobileCall::KCapsDataCircuitAsynchronous
				         | RMobileCall::KCapsDataCircuitAsynchronousRDI 
				         | RMobileCall::KCapsPADAsyncUDI 
				         | RMobileCall::KCapsPADAsyncRDI 
				         | RMobileCall::KCapsPacketAccessSyncUDI 
				         | RMobileCall::KCapsPacketAccessSyncRDI;
	ASSERT_BITS_SET(dataCallCaps.iServiceCaps, defaultServiceCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default service caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps in set of 
	//			RMobileCall::KCapsProtocolV22bis
	//         | RMobileCall::KCapsProtocolV32 
	//         | RMobileCall::KCapsProtocolV34 
	//         | RMobileCall::KCapsProtocolV110 
	//         | RMobileCall::KCapsProtocolV120 
	//         | RMobileCall::KCapsProtocolX31FlagStuffing 
	//         | RMobileCall::KCapsProtocolPIAFS 
	defaultProtocolCaps = 	RMobileCall::KCapsProtocolV22bis
							| RMobileCall::KCapsProtocolV32 
							| RMobileCall::KCapsProtocolV34 
							| RMobileCall::KCapsProtocolV110 
							| RMobileCall::KCapsProtocolV120 
							| RMobileCall::KCapsProtocolX31FlagStuffing 
							| RMobileCall::KCapsProtocolPIAFS;
	ASSERT_BITS_SET(dataCallCaps.iProtocolCaps, defaultProtocolCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default protocol caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps in set of 
	// 			RMobileCall::KCapsTransparent
	//         | RMobileCall::KCapsNonTransparent 
	//         | RMobileCall::KCapsTransparentPreferred 
	//         | RMobileCall::KCapsNonTransparentPreferred 
	defaultQoSCaps =  RMobileCall::KCapsTransparent
					| RMobileCall::KCapsNonTransparent 
					| RMobileCall::KCapsTransparentPreferred 
					| RMobileCall::KCapsNonTransparentPreferred;
	ASSERT_BITS_SET(dataCallCaps.iQoSCaps, defaultQoSCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default QoS caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iHscsdSupport=ETrue
	ASSERT_TRUE(dataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::GetMobileDataCallCaps returned hscsd support EFalse"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iRLPVersionCaps in set of 
	//			 RMobileCall::KCapsRLPSingleLinkVersion0
	//         | RMobileCall::KCapsRLPSingleLinkVersion1 
	//         | RMobileCall::KCapsRLPMultiLinkVersion2 
	defaultRLPVersionCaps = RMobileCall::KCapsRLPSingleLinkVersion0
							| RMobileCall::KCapsRLPSingleLinkVersion1 
							| RMobileCall::KCapsRLPMultiLinkVersion2;
	ASSERT_BITS_SET(dataCallCaps.iRLPVersionCaps, defaultRLPVersionCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default RLPVersion caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iV42bisCaps in set of 
	//			RMobileCall::KCapsV42bisTxDirection
	//         | RMobileCall::KCapsV42bisRxDirection 
	//         | RMobileCall::KCapsV42bisBothDirections 
	defaultV42bisCaps = RMobileCall::KCapsV42bisTxDirection
						| RMobileCall::KCapsV42bisRxDirection 
						| RMobileCall::KCapsV42bisBothDirections;
	ASSERT_BITS_SET(dataCallCaps.iV42bisCaps, defaultV42bisCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default V42bis caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iAsymmetryCaps in set of 
	// 			RMobileCall::KCapsAsymmetryNoPreference
	//         | RMobileCall::KCapsAsymmetryDownlink 
	//         | RMobileCall::KCapsAsymmetryUplink 
	defaultAsymmetryCaps = RMobileCall::KCapsAsymmetryNoPreference
							| RMobileCall::KCapsAsymmetryDownlink 
							| RMobileCall::KCapsAsymmetryUplink;
	ASSERT_BITS_SET(dataCallCaps.iAsymmetryCaps, defaultAsymmetryCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default asymmetry caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iCodingCaps in set of 
	//			RMobileCall::KCapsTchCoding48
	//         | RMobileCall::KCapsTchCoding96 
	//         | RMobileCall::KCapsTchCoding144 
	defaultCodingCaps = RMobileCall::KCapsTchCoding48
						| RMobileCall::KCapsTchCoding96 
						| RMobileCall::KCapsTchCoding144;
	ASSERT_BITS_SET(dataCallCaps.iCodingCaps, defaultCodingCaps, KNoUnwantedBits,
					_L("RMobileCall::GetMobileDataCallCaps returned wrong default coding caps"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMClass = 0x6
	ASSERT_EQUALS(dataCallCaps.iMClass, 0x6, _L("RMobileCall::GetMobileDataCallCaps.iMClass did not return 0x6"));

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxRxTimeSlots = 0x3
	ASSERT_EQUALS(dataCallCaps.iMaxRxTimeSlots, 0x3, _L("RMobileCall::GetMobileDataCallCaps.iMaxRxTimeSlots did not return 0x3"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxTxTimeSlots = 0x2
	ASSERT_EQUALS(dataCallCaps.iMaxTxTimeSlots, 0x2, _L("RMobileCall::GetMobileDataCallCaps.iMaxTxTimeSlots did not return 0x2"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iTotalRxTxTimeSlots = 0x4
	ASSERT_EQUALS(dataCallCaps.iTotalRxTxTimeSlots, 0x4, _L("RMobileCall::GetMobileDataCallCaps.iTotalRxTxTimeSlots did not return 0x4"));
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iUserInitUpgrade=EFalse
	ASSERT_TRUE(dataCallCaps.iUserInitUpgrade == EFalse, _L("RMobileCall::GetMobileDataCallCaps.iUserInitUpgrade did not return EFalse"));

	// ===  Check bearer service with no call connected ===

	// Check RCall::GetBearerServiceInfo returns KErrNotFound
	ASSERT_EQUALS(call1.GetBearerServiceInfo(bearerService), KErrNotFound, 
				  _L("RCall::GetBearerServiceInfo didn't return KErrNotFound"));

	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerCaps=RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown
	bearerCaps = RCall::KBearerCapsCompressionNone | RCall::KBearerCapsProtocolUnknown;
	ASSERT_BITS_SET(bearerService.iBearerCaps, bearerCaps, KNoUnwantedBits,
					_L("RCall::GetBearerServiceInfo returned wrong bearer caps"));
	// Check RCall::GetBearerServiceInfo returns aBearerService.iBearerSpeed=RCall::EBearerDataUnknown
	ASSERT_BITS_SET(bearerService.iBearerSpeed, RCall::EBearerDataUnknown, KNoUnwantedBits,
					_L("RCall::GetBearerServiceInfo returned wrong bearer speed"));


	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyCallStatusChangeStatus
	// notifydataCallCapsStatus
	// dialStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(4, &notifyCallStatusChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0002");
	}



CCTSYIntegrationTestDataCallControl0003::CCTSYIntegrationTestDataCallControl0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0003::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0003::~CCTSYIntegrationTestDataCallControl0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0003
 * @SYMFssID BA/CTSY/DATC-0003
 * @SYMTestCaseDesc Make a circuit switched data call.
 * @SYMTestPriority High
 * @SYMTestActions RCall::HangUp, RMobileLine::NotifyMobileLineStatusChange, RLine::GetCallInfo, RCall::Dial, RLine::GetCaps, RLine::GetStatus, RMobileLine::GetMobileLineStatus, RLine::NotifyHookChange, RLine::GetHookStatus, RLine::GetInfo
 * @SYMTestExpectedResults Pass - Line caps, status and call info correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify line status, call info and caps.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Get data line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get Call 1.
	RMobileCall& call1= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Check line caps ===

	// Check RLine::GetCaps supports caps in set of KCapsData.
	RLine::TCaps lineCaps;
	ASSERT_EQUALS(mobileLine.GetCaps(lineCaps), KErrNone,  
			_L("RMobileLine::GetCaps returned an error"));
	ASSERT_BITS_SET(lineCaps.iFlags, RLine::KCapsData, KNoUnwantedBits, 
			_L("RLine::GetCaps returned wrong caps"));

	// ===  Check line status ===

	// Check RLine::GetStatus returns EStatusIdle.
	RCall::TStatus lineStatus;
	ASSERT_EQUALS(mobileLine.GetStatus(lineStatus), KErrNone,  
			_L("RLine::GetStatus returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusIdle, 
			_L("RLine::GetStatus returned wrong status, should be EStatusIdle"));

	// Check RMobileLine::GetMobileLineStatus returns EStatusIdle.
	RMobileCall::TMobileCallStatus mobileLineStatus;
	ASSERT_EQUALS(mobileLine.GetMobileLineStatus(mobileLineStatus), KErrNone, 
			_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusIdle,
			_L("RMobileLine::GetMobileLineStatus did not return EStatusIdle as expected"));

	// ===  Check line info ===
	RLine::TCallInfo callInfo;
	TInt dataLineIndex = 0;
	ASSERT_EQUALS(mobileLine.GetCallInfo(dataLineIndex, callInfo), KErrNone, 
			_L("RLine::GetCallInfo returned an error"));

	// Check RLine::GetCallInfo returns valid name.
	ASSERT_TRUE(callInfo.iCallName.Length() >0, 
			_L("RLine::GetCallInfo returned an invalid  name"));

	// Check RLine::GetCallInfo returns caps in set of KCapsData | KCapsDial | KCapsAnswer.
    // @CTSYProblem KCapsAnswer is not set
	TUint32 wantedCallCaps = RCall::KCapsData | RCall::KCapsDial;
	ASSERT_BITS_SET(callInfo.iCallCapsFlags, wantedCallCaps, KNoUnwantedBits , 
			_L("RLine::GetCallInfo returned wrong caps"));	

	// Check RLine::GetCallInfo returns status of EStatusIdle.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusIdle, 
			_L("RLine::GetCallInfo wrong iStatus, should be EStatusIdle"));
	
	// Check RLine::GetInfo returns hook status of EHookStatusOn.
	// $CTSYProblem CMmLineTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aLineInfo->iHookStatus = RCall::EHookStatusUnknown;)
	RLine::TLineInfo lineInfo;
	ASSERT_EQUALS(mobileLine.GetInfo(lineInfo),KErrNone, _L("RLine::GetInfo returned an error")); 
	ASSERT_EQUALS(lineInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RLine::GetInfo did not return iHookStatus EHookStatusUnknown"));
	
	// Check RLine::GetInfo returns status of EStatusIdle.
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle, _L("RLine::GetInfo did not return iStatus EStatusIdle"));

	// @CTSYProblem NotifyHookChange has never been called
	// post notifier for RLine::NotifyHookChange
	TCoreEtelRequestStatus<RLine> notifyHookChangeStatus(mobileLine, &RLine::NotifyHookChangeCancel);
	CleanupStack::PushL(notifyHookChangeStatus);
	RCall::THookStatus hookStatus;
	mobileLine.NotifyHookChange(notifyHookChangeStatus, hookStatus);

	// post notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyStatusChangeStatus (mobileLine, &RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, lineStatus);

	// Post notifiers RMobileLine::NotifyMobileLineStatusChange
    TExtEtelRequestStatus notifyMobileLineChangeStatus(mobileLine, EMobileLineNotifyMobileLineStatusChange);
    CleanupStack::PushL(notifyMobileLineChangeStatus);
    mobileLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);

	// Dial a number that answers with RCall::Dial 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	DisplayUserInteractionPromptL(_L("Board now dials to DataNumber1 (press any key to start dialling) "), ETimeLong);
	call1.Dial(dialStatus, number);
    DEBUG_PRINTF2(_L("Dialling data call: %S"), &number);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// ===  Check hook status ===

	// Check RLine::NotifyHookChange completes with EHookStatusOff.
    // @CTSYProblem NotifyHookChange is not supported

	// Check RLine::GetHookStatus returns EHookStatusOff.
    // @CTSYProblem GetHookStatus is not supported

	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	TInt  wantedStatus=KErrNone;
	RCall::TStatus expectedLineStatus = RCall::EStatusDialling;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 wantedStatus);
														 
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, lineStatus);
	expectedLineStatus = RCall::EStatusConnecting;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 wantedStatus);

	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, lineStatus);
	expectedLineStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 wantedStatus);
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, lineStatus);

	// Check RLine::GetStatus returns EStatusConnected
	ASSERT_EQUALS(mobileLine.GetStatus(lineStatus), KErrNone, _L("RLine::GetHookStatus returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusConnected, _L("RLine::GetStatus did not return EStatusConnected"));

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileLineStatus = RMobileCall::EStatusDialling;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(mobileLine,
																		 notifyMobileLineChangeStatus,
																		 mobileLineStatus,
																		 expectedMobileLineStatus,
																	  	 wantedStatus);
														 
    mobileLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusConnecting;
    iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileLineChangeStatus,
																		 mobileLineStatus,
																		 expectedMobileLineStatus,
																	  	 wantedStatus);
														 
    mobileLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		 notifyMobileLineChangeStatus,
																		 mobileLineStatus,
																		 expectedMobileLineStatus,
																	  	 wantedStatus);


	// Check RMobileLine::GetMobileLineStatus returns EStatusConnected
	ASSERT_EQUALS(mobileLine.GetMobileLineStatus(mobileLineStatus), KErrNone, 
			_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusConnected,
			_L("RMobileLine::GetMobileLineStatus did not return EStatusConnected as expected"));

	// ===  Check line info ===

	ASSERT_EQUALS(mobileLine.GetCallInfo(dataLineIndex, callInfo), KErrNone, 
			_L("RLine::GetCallInfo returned an error"));

	// Check RLine::GetCallInfo returns valid name.
	ASSERT_TRUE(callInfo.iCallName.Length() >0, 
			_L("RLine::GetCallInfo returned an invalid  name"));

	// Check RLine::GetCallInfo returns caps in set of KCapsHangUp.
	ASSERT_BITS_SET(callInfo.iCallCapsFlags, RCall::KCapsHangUp, KNoUnwantedBits , 
			_L("RLine::GetCallInfo returned wrong caps"));	

	// Check RLine::GetCallInfo returns status of EStatusConnected.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusConnected, 
			_L("RLine::GetCallInfo wrong iStatus, should be EStatusConnected"));

	// Check RLine::GetInfo returns hook status of EHookStatusOff.
	// $CTSYProblem CMmLineTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aLineInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(mobileLine.GetInfo(lineInfo),KErrNone, _L("RLine::GetInfo returned an error")); 
	ASSERT_EQUALS(lineInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RLine::GetInfo did not return iHookStatus EHookStatusUnknown"));
	
	// Check RLine::GetInfo returns status of EStatusConnected.
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusConnected, _L("RLine::GetInfo did not return iStatus EStatusConnected"));

	// Hangup call with RCall::HangUp 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));

	// ===  Check hook status ===

	// Check RLine::NotifyHookChange completes with EHookStatusOn.
    // @CTSYProblem NotifyHookChange is not supported


	// Check RLine::GetHookStatus returns EHookStatusOn.
    // @CTSYProblem GetHookStatus is not supported

	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusHangingUp -> EStatusIdle
	expectedLineStatus = RCall::EStatusHangingUp;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 wantedStatus);
														 
	mobileLine.NotifyStatusChange(notifyStatusChangeStatus, lineStatus);

	expectedLineStatus = RCall::EStatusIdle;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyStatusChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 wantedStatus);

	// Check RLine::GetStatus returns EStatusIdle
	ASSERT_EQUALS(mobileLine.GetStatus(lineStatus), KErrNone, _L("RLine::GetHookStatus returned an error"));
	ASSERT_EQUALS(lineStatus, RCall::EStatusIdle, _L("RLine::GetStatus did not return EStatusIdle"));


	// Check RMobileLine::NotifyMobileLineStatusChange completes with EStatusDisconnecting -> EStatusIdle
	expectedMobileLineStatus = RMobileCall::EStatusDisconnecting;
    mobileLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(mobileLine,
																		 notifyMobileLineChangeStatus,
																		 mobileLineStatus,
																		 expectedMobileLineStatus,
																	  	 wantedStatus);
														 
    mobileLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileLineStatus);
	expectedMobileLineStatus = RMobileCall::EStatusIdle;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(mobileLine,
																		 notifyMobileLineChangeStatus,
																		 mobileLineStatus,
																		 expectedMobileLineStatus,
																	  	 wantedStatus);
														 
	// Check RMobileLine::GetMobileLineStatus returns EStatusIdle
	ASSERT_EQUALS(mobileLine.GetMobileLineStatus(mobileLineStatus), KErrNone, 
			_L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileLineStatus, RMobileCall::EStatusIdle,
			_L("RMobileLine::GetMobileLineStatus did not return EStatusIdle as expected"));

	// ===  Check line info ===
	ASSERT_EQUALS(mobileLine.GetCallInfo(dataLineIndex, callInfo), KErrNone, 
			_L("RLine::GetCallInfo returned an error"));

	// Check RLine::GetCallInfo returns valid name.
	ASSERT_TRUE(callInfo.iCallName.Length() >0, 
			_L("RLine::GetCallInfo returned an invalid  name"));

	// Check RLine::GetCallInfo returns caps in set of KCapsData | KCapsDial | KCapsAnswer.
    // @CTSYProblem KCapsAnswer is not set
	wantedCallCaps = RCall::KCapsData | RCall::KCapsDial;
	ASSERT_BITS_SET(callInfo.iCallCapsFlags, wantedCallCaps, KNoUnwantedBits , 
			_L("RLine::GetCallInfo returned wrong caps"));	

	// Check RLine::GetCallInfo returns status of EStatusIdle.
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusIdle, 
			_L("RLine::GetCallInfo wrong iStatus, should be EStatusIdle"));

	// Check RLine::GetInfo returns hook status of EHookStatusOn.
	// $CTSYProblem CMmLineTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aLineInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(mobileLine.GetInfo(lineInfo),KErrNone, _L("RLine::GetInfo returned an error")); 
	ASSERT_EQUALS(lineInfo.iHookStatus, RCall::EHookStatusUnknown, _L("RLine::GetInfo did not return iHookStatus EHookStatusUnknown"));
	
	// Check RLine::GetInfo returns status of EStatusIdle.
	ASSERT_EQUALS(lineInfo.iStatus, RCall::EStatusIdle, _L("RLine::GetInfo did not return iStatus EStatusIdle"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyHookChangeStatus
	// notifyStatusChangeStatus
	// notifyMobileLineChangeStatus
	// dialStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(5, &notifyHookChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0003");
	}



CCTSYIntegrationTestDataCallControl0004::CCTSYIntegrationTestDataCallControl0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0004::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0004::~CCTSYIntegrationTestDataCallControl0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0004
 * @SYMFssID BA/CTSY/DATC-0004
 * @SYMTestCaseDesc Answer an incoming circuit switched data call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileLine::NotifyMobileLineStatusChange, RMobileCall::NotifyMobileCallCapsChange, RMobileCall::GetMobileCallStatus, RLine::NotifyIncomingCall, RCall::GetInfo, RCall::NotifyStatusChange, RCall::NotifyCapsChange, RLine::GetInfo, RMobileCall::GetMobileCallCaps, RLine::NotifyStatusChange, RCall::RecoverDataPort, RLine::GetCallInfo, RMobileCall::GetMobileCallInfo, RMobileCall::NotifyMobileCallStatusChange, RCall::AnswerIncomingCall
 * @SYMTestExpectedResults Pass - Data call can be answered. Data can be received.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call status, call caps and call info are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Get data line 1. 
    RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get call 1. 
	RMobileCall& dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for incoming call from RPS
	TName name;
	TPtrC number;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (dataLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	dataLine.NotifyIncomingCall(notifyIncomingCallStatus,name);
	
	// Request incoming data call from RPS 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes with a call name with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeLong),KErrNone,_L("RLine::NotifyIncomingCall timed out"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"));
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"));
	
    //Get the incoming call
	TCallId incomingCallId;
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// post notifier for RCall::NotifyCapsChange
	RCall::TCaps callCaps;
	TCoreEtelRequestStatus<RCall> notifyCapsChangeStatus (dataCall1,&RCall::NotifyCapsChangeCancel);	
	CleanupStack::PushL(notifyCapsChangeStatus);
	dataCall1.NotifyCapsChange(notifyCapsChangeStatus,callCaps);
	
	// Post notifier for RMobileCall::NotifyMobileCallCapsChange
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg capsPckg(mobileCallCaps);
	TExtEtelRequestStatus notifyMobileCallCapsChangeStatus(dataCall1, EMobileCallNotifyMobileCallCapsChange);
	CleanupStack::PushL(notifyMobileCallCapsChangeStatus);
	dataCall1.NotifyMobileCallCapsChange(notifyMobileCallCapsChangeStatus,capsPckg);
	
	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (dataCall1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	dataCall1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	
	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(dataCall1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	dataCall1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// post notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyRLineChangeStatus (dataLine,&RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyRLineChangeStatus);
	dataLine.NotifyStatusChange(notifyRLineChangeStatus,callStatus);
	
	// post a notifier for RMobileLine::NotifyMobileLineStatusChange
	TExtEtelRequestStatus notifyMobileLineChangeStatus(dataLine, EMobileLineNotifyMobileLineStatusChange);
	CleanupStack::PushL(notifyMobileLineChangeStatus);
	dataLine.NotifyMobileLineStatusChange(notifyMobileLineChangeStatus, mobileCallStatus);
	
	// ===  Check call caps ===

	// Check RCall::NotifyCapsChange completes with caps in set of KCapsData | KCapsHangUp | KCapsAnswer
	// Check RCall::NotifyCapsChange completes with caps NOT in set of KCapsDial
	RCall::TCaps wantedCaps, unwantedCaps;
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp | RCall::KCapsAnswer;
	unwantedCaps.iFlags = RCall::KCapsDial;
	const TInt wantedStatus = KErrNone;
	
	iCallControlTestHelper.WaitForCallNotifyCapsChange(dataCall1,
													   notifyCapsChangeStatus,
													   callCaps,
													   wantedCaps,
													   unwantedCaps,
													   wantedStatus);
			
	// Check RCall::GetCaps supports caps in set of KCapsData | KCapsHangUp | KCapsAnswer
	// Check RCall::GetCaps supports NOT in set of KCapsDial
	ASSERT_EQUALS(dataCall1.GetCaps(callCaps), KErrNone, _L("RCall::GetCaps returned an Error"));
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp;
	unwantedCaps.iFlags = RCall::KCapsAnswer;
	ASSERT_BITS_SET(callCaps.iFlags, wantedCaps.iFlags, unwantedCaps.iFlags, _L("RCall::GetCaps returned wrong caps"));
			
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps in set of KCapsData | KCapsHangUp | KCapsAnswer
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps NOT in set of KCapsDial
	RMobileCall::TMobileCallCapsV1 wantedMobileCaps;
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsData | RMobileCall::KCapsHangUp | RMobileCall::KCapsAnswer;																	  
	TCmpRMobileCallTMobileCallCaps mobileCapsComp(wantedMobileCaps,mobileCallCaps, *this);
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallCapsChange(dataCall1, 
																	   notifyMobileCallCapsChangeStatus,
																	   mobileCapsComp,
																	   wantedStatus);
																	   
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps in set of KCapsVoice | KCapsHangUp | KCapsAnswer
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsDial
	ASSERT_EQUALS(dataCall1.GetMobileCallCaps(capsPckg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned an error"));
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsVoice | RMobileCall::KCapsHangUp | RMobileCall::KCapsAnswer;									   
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, wantedMobileCaps.iCallControlCaps, KNoUnwantedBits, _L("RMobileCall::GetMobileCallCaps returned unexpected iCallControlCaps"));
			
	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusRinging
	RCall::TStatus expectedCallStatus = RCall::EStatusRinging;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
	// Check RCall::GetStatus returns EStatusRinging
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus),KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusRinging, _L("RCall::GetStatus did not return a status EStatusRinging"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusRinging
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusRinging;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);	
																											
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusRinging
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusRinging, _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
	
	// ===  Check call info ===

	// Check RCall::GetInfo returns valid call name.
	// Check RCall::GetInfo returns valid voice line name.
	// Check RCall::GetInfo returns hook status of EHookStatusOn
	// Check RCall::GetInfo returns call status of EStatusRinging.
	// Check RCall::GetInfo returns call duration of 0.
	RCall::TCallInfo callInfo;
	ASSERT_EQUALS(dataCall1.GetInfo(callInfo), KErrNone, _L("RCall::GetInfo returned an error"));
	ASSERT_TRUE(callInfo.iCallName.Length() >0, _L("RCall::GetInfo returned an invalid  name"));
	ASSERT_TRUE(callInfo.iLineName.Length() >0, _L("RCall::GetInfo returned an invalid  Line name"));
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusOn, _L("RCall::GetInfo wrong iHookStatus, should be EHookStatusOn"));
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusRinging, _L("RCall::GetInfo wrong iStatus, should be EStatusConnected"));
	ASSERT_TRUE(callInfo.iDuration.Int() == 0, _L("RCall::GetInfo wrong iDuration, should be >0"));
	
	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes makred as valid in iValid.
	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	RMobileCall::TMobileCallInfoV1 mobileCallInfo;
	RMobileCall::TMobileCallInfoV1Pckg callInfoPckg(mobileCallInfo);
	ASSERT_EQUALS(dataCall1.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));
	ASSERT_TRUE(mobileCallInfo.iValid > 0, _L("RMobileCall::GetMobileCallInfo returned unexpected flags in iValid"));
	iCallControlTestHelper.CheckForValidCallInfo(mobileCallInfo, number);
	
	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusRinging 
	expectedCallStatus = RCall::EStatusRinging;	
	iCallControlTestHelper.WaitForLineNotifyStatusChange(dataLine,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
	// Check RLine::GetStatus returns EStatusRinging
	ASSERT_EQUALS(dataLine.GetStatus(callStatus),KErrNone, _L("RLine::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusRinging, _L("RLine::GetStatus returned wrong status, should be EStatusRinging"));
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusRinging
	expectedMobileCallStatus = RMobileCall::EStatusRinging;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(dataLine,
																		notifyMobileLineChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		wantedStatus);
	// Check RMobileLine::GetMobileLineStatus returns EStatusRinging
	ASSERT_EQUALS(dataLine.GetMobileLineStatus(mobileCallStatus), KErrNone, _L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusRinging, _L("RMobileLine::GetMobileLineStatus did not return EStatusIdle as expected"));

	// ===  Check line info ===

	// Check RLine::GetCallInfo returns valid name
	// Check RLine::GetCallInfo returns caps in set of KCapsHangUp | KCapsAnswer
	// Check RLine::GetCallInfo returns caps NOT in set of KCapsDial
	// Check RLine::GetCallInfo returns status of EStatusRinging.
 	RLine::TCallInfo RLinecallInfo;
	ASSERT_EQUALS(dataLine.GetCallInfo(0, RLinecallInfo), KErrNone, _L("RLine::GetCallInfo returned an error"));
 	ASSERT_TRUE(RLinecallInfo.iCallName.Length() >0, _L("RLine::GetCallInfo returned an invalid  name"));
 	wantedCaps.iFlags = (RCall::KCapsHangUp | RCall::KCapsAnswer);
 	unwantedCaps.iFlags = RCall::KCapsDial;
 	ASSERT_BITS_SET(RLinecallInfo.iCallCapsFlags, wantedCaps.iFlags, unwantedCaps.iFlags, _L("RLine::GetCallInfo returned wrong caps"));
 	ASSERT_EQUALS(RLinecallInfo.iStatus, RCall::EStatusRinging, _L("RLine::GetCallInfo wrong iStatus, should be EStatusRinging"));
 	
	// Check RLine::GetInfo returns hook status of EHookStatusOn
	// Check RLine::GetInfo returns status of EStatusRinging
	RLine::TLineInfo RLineInfo;
	// $CTSYProblem CMmCallTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aCallInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(RLineInfo.iHookStatus, RCall::EHookStatusOn, _L("RCall::GetInfo wrong iHookStatus, should be EHookStatusUnknown"));
	ASSERT_EQUALS(RLineInfo.iStatus, RCall::EStatusRinging, _L("RLine::GetCallInfo wrong iStatus, should be EStatusRinging"));
	
	// ===  Answer call === 

	// Answer the incoming call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	dataCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));
    
	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusAnswering->EStatusConnected
	expectedCallStatus = RCall::EStatusAnswering;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
    dataCall1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
														 
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);	
														 											
	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus),KErrNone,  _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return a status EStatusConnected"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusAnswering->EStatusConnected
	expectedMobileCallStatus = RMobileCall::EStatusAnswering;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
															
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);
														
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
			
	// ===  Check call caps ===

	// Check RCall::NotifyCapsChange completes with caps in set of KCapsData | KCapsHangUp
	// Check RCall::NotifyCapsChange completes with caps NOT in set of KCapsDial | KCapsAnswer
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsAnswer;
	iCallControlTestHelper.WaitForCallNotifyCapsChange(dataCall1,
													   notifyCapsChangeStatus,
													   callCaps,
													   wantedCaps,
													   unwantedCaps,
													   wantedStatus);
													   
	// Check RCall::GetCaps supports caps in set of KCapsData | KCapsHangUp
	// Check RCall::GetCaps supports NOT in set of KCapsDial | KCapsAnswer
	ASSERT_EQUALS(dataCall1.GetCaps(callCaps), KErrNone, _L("RCall::GetCaps returned an Error"));
	wantedCaps.iFlags = RCall::KCapsData | RCall::KCapsHangUp;
	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsAnswer;
	ASSERT_BITS_SET(callCaps.iFlags, wantedCaps.iFlags, unwantedCaps.iFlags, _L("RCall::GetCaps returned wrong caps"));
			
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps in set of KCapsData | KCapsHangUp
	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps NOT in set of KCapsDial | KCapsAnswer
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsData | RMobileCall::KCapsHangUp;																	  
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallCapsChange(dataCall1, 
																	   notifyMobileCallCapsChangeStatus,
																	   mobileCapsComp,
																	   wantedStatus);
																	   
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps in set of KCapsVoice | KCapsHangUp
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsDial | KCapsAnswer
	ASSERT_EQUALS(dataCall1.GetMobileCallCaps(capsPckg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned an error"));
	wantedMobileCaps.iCallControlCaps = RMobileCall::KCapsVoice | RMobileCall::KCapsHangUp;									   
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, wantedMobileCaps.iCallControlCaps, KNoUnwantedBits, _L("RMobileCall::GetMobileCallCaps returned unexpected iCallControlCaps"));
			
	// ===  Check call info ===

	// Check RCall::GetInfo returns valid call name.
	// Check RCall::GetInfo returns valid voice line name.
	// Check RCall::GetInfo returns hook status of EHookStatusOff
	// Check RCall::GetInfo returns call status of EStatusConnected.
	// Check RCall::GetInfo returns call duration of 0.
	ASSERT_EQUALS(dataCall1.GetInfo(callInfo), KErrNone, _L("RCall::GetInfo returned an error"));
	ASSERT_TRUE(callInfo.iCallName.Length() >0, _L("RCall::GetInfo returned an invalid  name"));
	ASSERT_TRUE(callInfo.iLineName.Length() >0, _L("RCall::GetInfo returned an invalid  Line name"));
	ASSERT_EQUALS(callInfo.iHookStatus, RCall::EHookStatusOff, _L("RCall::GetInfo wrong iHookStatus, should be EHookStatusOn"));
	ASSERT_EQUALS(callInfo.iStatus, RCall::EStatusConnected, _L("RCall::GetInfo wrong iStatus, should be EStatusConnected"));
	ASSERT_TRUE(callInfo.iDuration.Int() == 0, _L("RCall::GetInfo wrong iDuration, should be >0"));
	
	// Check RMobileCall::GetMobileCallInfo returns iValid > 0.
	// Check RMobileCall::GetMobileCallInfo returns data for attributes makred as valid in iValid.
	// Check RMobileCall::GetMobileCallInfo returns valid line name.
	ASSERT_EQUALS(dataCall1.GetMobileCallInfo(callInfoPckg), KErrNone, _L("RMobileCall::GetMobileCallInfo returned an error"));
	ASSERT_TRUE(mobileCallInfo.iLineName.Length() >0, _L("RMobileCall::GetMobileCallInfo returned an invalid line name"));
	ASSERT_TRUE(mobileCallInfo.iValid > 0, _L("RMobileCall::GetMobileCallInfo returned unexpected flags in iValid"));
	iCallControlTestHelper.CheckForValidCallInfo(mobileCallInfo, number);
	
	// ===  Check line status ===

	// Check RLine::NotifyStatusChange completes with EStatusConnected
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(dataLine,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
														 
	// Check RLine::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataLine.GetStatus(callStatus),KErrNone, _L("RLine::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RLine::GetStatus returned wrong status, should be EStatusRinging"));
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusConnected
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTestHelper.WaitForMobileLineNotifyMobileLineStatusChange(dataLine,
																		notifyMobileLineChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		wantedStatus);
																		
	// Check RMobileLine::GetMobileLineStatus returns EStatusConnected
	ASSERT_EQUALS(dataLine.GetMobileLineStatus(mobileCallStatus), KErrNone, _L("RMobileLine::GetMobileLineStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileLine::GetMobileLineStatus did not return EStatusIdle as expected"));

	// ===  Check line info ===

	// Check RLine::GetCallInfo returns valid name
	// Check RLine::GetCallInfo returns caps in set of KCapsHangUp
	// Check RLine::GetCallInfo returns caps NOT in set of KCapsDial | KCapsAnswer
	// Check RLine::GetCallInfo returns status of EStatusConnected
	ASSERT_EQUALS(dataLine.GetCallInfo(0, RLinecallInfo), KErrNone, _L("RLine::GetCallInfo returned an error"));
 	ASSERT_TRUE(RLinecallInfo.iCallName.Length() >0, _L("RLine::GetCallInfo returned an invalid  name"));
 	wantedCaps.iFlags = RCall::KCapsHangUp;
 	unwantedCaps.iFlags = RCall::KCapsDial | RCall::KCapsAnswer;
 	ASSERT_BITS_SET(RLinecallInfo.iCallCapsFlags, wantedCaps.iFlags, unwantedCaps.iFlags, _L("RLine::GetCallInfo returned wrong caps"));
 	ASSERT_EQUALS(RLinecallInfo.iStatus, RCall::EStatusConnected, _L("RLine::GetCallInfo wrong iStatus, should be EStatusRinging"));
 	
	// Check RLine::GetInfo returns hook status of EHookStatusOff
	// Check RLine::GetInfo returns status of EStatusConnected
	// $CTSYProblem CMmCallTsy::GetInfo() returns EHookStatusUnknown always,
	// it is hard coded (aCallInfo->iHookStatus = RCall::EHookStatusUnknown;)
	ASSERT_EQUALS(RLineInfo.iHookStatus, RCall::EHookStatusOff, _L("RCall::GetInfo wrong iHookStatus, should be EHookStatusUnknown"));
	ASSERT_EQUALS(RLineInfo.iStatus, RCall::EStatusConnected, _L("RLine::GetCallInfo wrong iStatus, should be EStatusRinging"));
	
	// ===  Write some data to the port ===

	// RCommServ cs; 
	RCommServ cs;
	
	// cs.Connect(); 
	ASSERT_EQUALS(cs.Connect(), KErrNone,  _L("RRCommServ::Connect returned an error"));
	
	// RComm port; 
	RComm port;
	
	// port.Open(cs,commPort.iPort,ECommShared); 
	RCall::TCommPort commPort;
	port.Open(cs, commPort.iPort,ECommShared);
	
	// port.Write(stat,KWriteTestData);
	TRequestStatus stat;		
	port.Write(stat, KWriteTestData);
	
	// ===  Check call is still connected ===

	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus),KErrNone,  _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return a status EStatusConnected"));
	
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
			
	// port.Close(); 
	port.Close();
	
	// cs.Close(); 
	cs.Close();
	
	// ===  Recover the data port ===

	// Recover data port with RCall::RecoverDataPort 
	ASSERT_EQUALS(dataCall1.RecoverDataPort(), KErrNone, _L("RCall::RecoverDataPort returned an error"));	

	// ===  Check call is still connected ===

	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus),KErrNone,  _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return a status EStatusConnected"));
	
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusConnected
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileStatus did not return a status EStatusConnected"));
			
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyIncomingCallStatus
	// notifyCapsChangeStatus
	// notifyMobileCallCapsChangeStatus
	// notifyStatusChangeStatus
	// notifyMobileCallStatusChangeStatus
	// notifyRLineChangeStatus
	// notifyMobileLineChangeStatus
	// answerIncomingStatus
	CleanupStack::PopAndDestroy(8,&notifyIncomingCallStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0004");
	}



CCTSYIntegrationTestDataCallControl0005::CCTSYIntegrationTestDataCallControl0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0005::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0005::~CCTSYIntegrationTestDataCallControl0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0005
 * @SYMFssID BA/CTSY/DATC-0005
 * @SYMTestCaseDesc Answer an incoming circuit switched data call.
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RCall::AnswerIncomingCall, RMobileCall::NotifyMobileDataCallCapsChange, RMobileCall::GetMobileCallStatus, RCall::GetCallParams, RCall::GetStatus, RLine::NotifyIncomingCall, RMobileCall::GetMobileDataCallCaps
 * @SYMTestExpectedResults Pass - Incoming call answered with correct caps.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify data call caps and call info are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
				   
	// Get data line 1. 
    RLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	// Get call 1. 
	RMobileCall& dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for incoming call from RPS
	TName name;
	TPtrC number;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (dataLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	dataLine.NotifyIncomingCall(notifyIncomingCallStatus,name);
		
	// Request incoming data call from RPS 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));
	
	// Check RLine::NotifyIncomingCall completes with a call name with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"));
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"));
	
    //Get the incoming call
	TCallId incomingCallId;
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(dataCall1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	dataCall1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// post notifier for NotifyMobileDataCallCapsChange
	RMobileCall::TMobileCallDataCapsV1 dataCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg datacapsPckg(dataCaps);
	TExtEtelRequestStatus NotifyMobileDataCallCapsChangeStatus(dataCall1, EMobileCallNotifyMobileDataCallCapsChange);
	CleanupStack::PushL(NotifyMobileDataCallCapsChangeStatus);
	dataCall1.NotifyMobileDataCallCapsChange(NotifyMobileDataCallCapsChangeStatus, datacapsPckg);
	
	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (dataCall1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	dataCall1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	// ===  Check call status ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusRinging
	const TInt wantedStatus = KErrNone;
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusRinging;
	// $CTSYProblem: NotifyMobileCallStatusChange coming too fast. CMmDataCallTsy::CompleteNotifyStatusChange coming earlier than notify request. 
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);	
															
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusRinging
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileStatus returned an error"));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusRinging, _L("RMobileCall::GetMobileStatus did not return a status EStatusRinging"));
	
	// ===  Check mobile data call caps when line is ringing ===

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iProtocolCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iServiceCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iQoSCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iHscsdSupport = EFalse;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iMaxRxTimeSlots = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iMaxTxTimeSlots = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iCodingCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iAsymmetryCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iUserInitUpgrade = EFalse;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iRLPVersionCaps = 0x00;
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iV42bisCaps = 0x00;
 	RMobileCall::TMobileCallDataCapsV1 	wantedcaps;
 	wantedcaps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding;
    wantedcaps.iProtocolCaps = 0x00;
    wantedcaps.iServiceCaps = 0x00;
    wantedcaps.iQoSCaps = 0x00;
    wantedcaps.iHscsdSupport = EFalse;
    wantedcaps.iMaxRxTimeSlots = 0x00;
    wantedcaps.iMaxTxTimeSlots = 0x00;
    wantedcaps.iCodingCaps = 0x00;
    wantedcaps.iAsymmetryCaps = 0x00;
    wantedcaps.iUserInitUpgrade = EFalse;
    wantedcaps.iRLPVersionCaps = 0x00;
    wantedcaps.iV42bisCaps = 0x00;
	TCmpBase<RMobileCall::TMobileCallDataCapsV1> compare(dataCaps, wantedcaps, *this);														
	iCallControlTestHelper.WaitForMobileCallNotifyMobileDataCallCapsChange(dataCall1,
															NotifyMobileDataCallCapsChangeStatus,
															compare,
															wantedStatus);	
	
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iHscsdSupport = EFalse;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxRxTimeSlots = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iMaxTxTimeSlots = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iCodingCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iAsymmetryCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iUserInitUpgrade = EFalse;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iRLPVersionCaps = 0x00;
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iV42bisCaps = 0x00;

	ASSERT_EQUALS(dataCall1.GetMobileCallCaps(datacapsPckg), KErrNone,   _L("RMobileCall::GetMobileCallCaps returned an error"));
	ASSERT_TRUE(wantedcaps.iSpeedCaps = RMobileCall::KCapsSpeedAutobauding, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iProtocolCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iServiceCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iQoSCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returns returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iHscsdSupport = EFalse, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iMaxRxTimeSlots = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iMaxTxTimeSlots = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iCodingCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iAsymmetryCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iUserInitUpgrade = EFalse, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iRLPVersionCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iV42bisCaps = 0x00, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
		
	// ===  Answer call ===

	// Answer the incoming call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	dataCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));
    	
	// ===  Check call status ===

	// Check RCall::NotifyStatusChange completes with EStatusAnswering->EStatusConnected
	RCall::TStatus expectedCallStatus = RCall::EStatusAnswering;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);
    dataCall1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
														 
	expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(dataCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 wantedStatus);	
														 
	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus),KErrNone,  _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return a status EStatusConnected"));
	
	// ===  Check mobile data call caps ===

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iHscsdSupport=EFalse
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iSpeedCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iProtocolCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iQoSCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iServiceCaps!=0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(NotifyMobileDataCallCapsChangeStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyMobileDataCallCapsChange timed-out"));
	ASSERT_TRUE(wantedcaps.iHscsdSupport != EFalse, _L("RMobileCall::NotifyMobileDataCallCapsChange returned hscsd support EFalse"));
	ASSERT_TRUE(wantedcaps.iSpeedCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned speed caps 0, should have been other than 0"));
	ASSERT_TRUE(wantedcaps.iProtocolCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned protocol caps 0, should be other than 0"));	
	ASSERT_TRUE(wantedcaps.iQoSCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned QoS caps 0, should be other than 0"));	
	ASSERT_TRUE(wantedcaps.iServiceCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned service caps 0, should be other than 0"));	
																
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps!=0
	ASSERT_TRUE(wantedcaps.iHscsdSupport = EFalse, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iProtocolCaps!=0, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iQoSCaps!=0, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	ASSERT_TRUE(wantedcaps.iServiceCaps!=0, _L("RMobileCall::GetMobileDataCallCaps returned wrong caps"));
	
	// ===  Get call parameters ===

	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iSpeed!=RMobileCall::ESpeedUnspecified
	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iService != RMobileCall::EServiceUnspecified
	// Check RCall::GetCallParams with aParams of type TMobileDataCallParamsV1 returns aParams.iProtocol != RMobileCall::EProtocolUnspecified
	RMobileCall::TMobileDataCallParamsV1 checkParams;
	RMobileCall::TMobileDataCallParamsV1Pckg checkParamsPkg(checkParams);
	dataCall1.GetCallParams(checkParamsPkg);
	ASSERT_TRUE(checkParams.iSpeed!=RMobileCall::ESpeedUnspecified, _L("RCall::GetCallParams returned wrong caps"));
	ASSERT_TRUE(checkParams.iService != RMobileCall::EServiceUnspecified, _L("RCall::GetCallParams returned wrong caps"));
	ASSERT_TRUE(checkParams.iProtocol != RMobileCall::EProtocolUnspecified, _L("RCall::GetCallParams returned wrong caps"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyIncomingCallStatus
	// notifyMobileCallStatusChangeStatus
	// NotifyMobileDataCallCapsChangeStatus
	// notifyStatusChangeStatus
	// answerIncomingStatus
	CleanupStack::PopAndDestroy(5,&notifyIncomingCallStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0005");
	}



CCTSYIntegrationTestDataCallControl0006::CCTSYIntegrationTestDataCallControl0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0006::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0006::~CCTSYIntegrationTestDataCallControl0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0006
 * @SYMFssID BA/CTSY/DATC-0006
 * @SYMTestCaseDesc Dial an HSCSD data call
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::NotifyMobileCallStatusChange, RCall::AnswerIncomingCall, RMobileCall::NotifyMobileDataCallCapsChange, RMobileCall::GetMobileCallStatus, RCall::GetCallParams, RCall::GetStatus, RLine::NotifyIncomingCall, RMobileCall::GetMobileDataCallCaps, RCall::HangUp
 * @SYMTestExpectedResults Pass - HSCSD call made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify HSCSD info, call params and data call caps.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get data line 1. 
	RMobileLine& dataline = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);	
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	DoTest0006L(call1, RMobileCall::ESpeed28800);
	
	// Repeat test with call at different speeds, 9.6kbits/s. 14.4kbits/s, 56kbits/s 
	DoTest0006L(call1, RMobileCall::ESpeed9600 );
	DoTest0006L(call1, RMobileCall::ESpeed14400 );
	DoTest0006L(call1, RMobileCall::ESpeed56000 );
	
	

	
	//
	// TEST END
	//

    StartCleanup();
	

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0006");
	}

void CCTSYIntegrationTestDataCallControl0006::DoTest0006L(RMobileCall& aCall1, RMobileCall::TMobileCallDataSpeed aSpeed)
	{
	// ===  Dial an HSCSD call with call params ===

	// Set up a RMobileCall::TMobileHscsdCallParamsV1 with 
	RMobileCall::TMobileHscsdCallParamsV1 dataparams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg dataparamsPckg(dataparams);

	// valid parameters 
	dataparams.iIdRestrict = RMobileCall::ESendMyId;
	dataparams.iAckTimer = 20;
	dataparams.iMSToModemWindowSize = 2;
	dataparams.iModemToMSWindowSize = 2;	
	dataparams.iProtocol = RMobileCall::EProtocolV34;
	dataparams.iQoS = RMobileCall::EQoSNonTransparent;
	dataparams.iRLPVersion = RMobileCall::ERLPSingleLinkVersion0;
	dataparams.iResequencingPeriod = 1;
	dataparams.iRetransmissionAttempts = 2;
	dataparams.iService = RMobileCall::EServiceDataCircuitAsync;
	dataparams.iSpeed = aSpeed;
	dataparams.iUseEdge = EFalse;
	dataparams.iV42bisCodewordsNum = 0;
    dataparams.iV42bisMaxStringLength = 0;
    dataparams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataparams.iAsymmetry = RMobileCall::EAsymmetryDownlink;
	dataparams.iCodings = RMobileCall::KCapsAiurCoding144;
	dataparams.iUserInitUpgrade = EFalse;

	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (aCall1,&RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	aCall1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);

	// Post notify RMobileCall::NotifyHscsdInfoChange
	RMobileCall::TMobileCallHscsdInfoV1 notifyHscsdParams;
	RMobileCall::TMobileCallHscsdInfoV1Pckg notifyHscsdParamsPckg(notifyHscsdParams);
	TExtEtelRequestStatus notifyHscsdInfoStatus(aCall1, EMobileCallNotifyHscsdInfoChange);
	CleanupStack::PushL(notifyHscsdInfoStatus);
	aCall1.NotifyHscsdInfoChange(notifyHscsdInfoStatus, notifyHscsdParamsPckg);

	// Post notify RMobileCall::NotifyMobileDataCallCapsChange
	RMobileCall::TMobileCallDataCapsV1 notifydataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg notifydataCallCapsPckg(notifydataCallCaps);
	TExtEtelRequestStatus notifydataCallCapsStatus(aCall1, EMobileCallNotifyMobileDataCallCapsChange);
	CleanupStack::PushL(notifydataCallCapsStatus);
	aCall1.NotifyMobileDataCallCapsChange(notifydataCallCapsStatus, notifydataCallCapsPckg);

    
 	// Set the dynamic HSCSD params with RMobileCall::SetDynamicHscsdParams with aAiur=EAiurBps28800 and aRxTimeslots=15 
    TExtEtelRequestStatus setDynamicHscsdParamsStatus(aCall1, EMobileCallSetDynamicHscsdParams);
	CleanupStack::PushL(setDynamicHscsdParamsStatus);
	RMobileCall::TMobileCallAiur aiur = RMobileCall::EAiurBps28800;
	TInt rxTimeslots = 15;
	aCall1.SetDynamicHscsdParams(setDynamicHscsdParamsStatus, (RMobileCall::TMobileCallAiur)aiur, rxTimeslots);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setDynamicHscsdParamsStatus, ETimeVeryLong), KErrNone, _L("RMobileCall::setDynamicHscsdParamsStatus timed-out"));
	// @CTSYProblem RMobileCall::SetDynamicHscsdParams alwaysy return -33, it seems like this is LTSY answer
	ASSERT_EQUALS(setDynamicHscsdParamsStatus.Int(), KErrTimedOut, _L("RMobileCall::setDynamicHscsdParamsStatus returned with an error"));

	// Dial a number that answers using the HSCSD call params 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(aCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	aCall1.Dial(dialStatus, dataparamsPckg, number);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RCall::Dial returned with an error"));
	TEST_CHECK_POINT_L(_L("Couldn't dial a call, no point continuing."));

	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(aCall1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);
	// ===  Check HSCSD Info ===

	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iAiur!=EAiurBpsUnspecified
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iCodings!=ETchCodingUnspecified
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iRxTimeSlots>0
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iTxTimeSlots>0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyHscsdInfoStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyHscsdInfoChange timed-out"));
	ASSERT_TRUE(notifyHscsdParams.iAiur != RMobileCall::EAiurBpsUnspecified, _L("RMobileCall::NotifyHscsdInfoChange returned aiur EAiurBpsUnspecified"));
	ASSERT_TRUE(notifyHscsdParams.iCodings != RMobileCall::ETchCodingUnspecified, _L("RMobileCall::NotifyHscsdInfoChange returned codings ETchCodingUnspecified"));
	ASSERT_TRUE(notifyHscsdParams.iRxTimeSlots > 0, _L("RMobileCall::NotifyHscsdInfoChange returned rxTimeSlots 0, should be > 0"));	
	ASSERT_TRUE(notifyHscsdParams.iTxTimeSlots > 0, _L("RMobileCall::NotifyHscsdInfoChange returned txTimeSlots 0, should be > 0"));	

	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iAiur!=EAiurBpsUnspecified
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iCodings!=ETchCodingUnspecified
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iRxTimeSlots>0
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iTxTimeSlots>0
	RMobileCall::TMobileCallHscsdInfoV1 hscsdparams;
	RMobileCall::TMobileCallHscsdInfoV1Pckg hscsdparamsPckg(hscsdparams); 
	ASSERT_EQUALS(aCall1.GetCurrentHscsdInfo(hscsdparamsPckg), KErrNone, _L("RMobileCall::GetCurrentHscsdInfo returned an error"));
	ASSERT_TRUE(hscsdparams.iAiur != RMobileCall::EAiurBpsUnspecified, _L("RMobileCall::GetCurrentHscsdInfo returned aiur EAiurBpsUnspecified"));
	ASSERT_TRUE(hscsdparams.iCodings != RMobileCall::ETchCodingUnspecified, _L("RMobileCall::GetCurrentHscsdInfo returned codings ETchCodingUnspecified"));
	ASSERT_TRUE(hscsdparams.iRxTimeSlots > 0, _L("RMobileCall::GetCurrentHscsdInfo returned rxTimeSlots 0, should be > 0"));	
	ASSERT_TRUE(hscsdparams.iTxTimeSlots > 0, _L("RMobileCall::GetCurrentHscsdInfo returned txTimeSlots 0, should be > 0"));	

	// ===  Check mobile data call caps ===

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iHscsdSupport=ETrue
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iSpeedCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iProtocolCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iQoSCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iServiceCaps!=0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifydataCallCapsStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyMobileDataCallCapsChange timed-out"));
	ASSERT_TRUE(notifydataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::NotifyMobileDataCallCapsChange returned hscsd support EFalse"));
	ASSERT_TRUE(notifydataCallCaps.iSpeedCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned speed caps 0, should have been other than 0"));
	ASSERT_TRUE(notifydataCallCaps.iProtocolCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned protocol caps 0, should be other than 0"));	
	ASSERT_TRUE(notifydataCallCaps.iQoSCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned QoS caps 0, should be other than 0"));	
	ASSERT_TRUE(notifydataCallCaps.iServiceCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned service caps 0, should be other than 0"));	

	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iHscsdSupport=ETrue
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps!=0
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg CallCapsPckg(dataCallCaps);
	ASSERT_EQUALS(aCall1.GetMobileDataCallCaps(CallCapsPckg), KErrNone, _L("RMobileCall::GetMobileDataCallCaps returned an errro "));
	ASSERT_TRUE(dataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::GetMobileDataCallCaps returned hscsd support EFalse"));
	ASSERT_TRUE(dataCallCaps.iSpeedCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned speed caps 0, should have been other than 0"));
	ASSERT_TRUE(dataCallCaps.iProtocolCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned protocol caps 0, should be other than 0"));	
	ASSERT_TRUE(dataCallCaps.iQoSCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned QoS caps 0, should be other than 0"));	
	ASSERT_TRUE(dataCallCaps.iServiceCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned service caps 0, should be other than 0"));	

	
	// ===  Get call params ===

	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iAsymmetry=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iCodings=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iMaxTimeSlots > 0
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iUserInitUpgrade=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iWantedAiur=EAiurBps28800
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iWantedRxTimeSlots=15
	ASSERT_EQUALS(aCall1.GetCallParams(dataparamsPckg), KErrNone, _L("RCall::GetCallParams returned an error"));
	ASSERT_EQUALS((TInt)dataparams.iCodings, (TInt)RMobileCall::KCapsAiurCoding144, _L("RCall::GetCallParams codings was not the same that used in dial"));
	ASSERT_TRUE(dataparams.iMaxTimeSlots > 0, _L("RCall::GetCallParams max time slot was 0, should have been bigger than 0"));
 	ASSERT_EQUALS((TInt)dataparams.iUserInitUpgrade, (TInt)EFalse, _L("RCall::GetCallParams user init upgrade was not the same that used in dial"));
 	ASSERT_EQUALS(dataparams.iWantedAiur, RMobileCall::EAiurBps28800, _L("RCall::GetCallParams wanted Aiur was not the same that used in dial"));
 	ASSERT_EQUALS(dataparams.iWantedRxTimeSlots, 15, _L("RCall::GetCallParams wanted Rx timeslot was not the same that used in dial"));
 
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iProtocol=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iQoS=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iService=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iSpeed=same as that used in dial
 	ASSERT_EQUALS(dataparams.iProtocol, RMobileCall::EProtocolV34, _L("RCall::GetCallParams protocol was not the same that used in dial"));
 	ASSERT_EQUALS(dataparams.iQoS,  RMobileCall::EQoSNonTransparent, _L("RCall::GetCallParams QoS was not the same that used in dial"));
 	ASSERT_EQUALS(dataparams.iService,  RMobileCall::EServiceDataCircuitAsync, _L("RCall::GetCallParams service was not the same that used in dial"));
 	ASSERT_EQUALS(dataparams.iSpeed,  aSpeed, _L("RCall::GetCallParams speed was not the same that used in dial"));

	// ===  Write some data to the port ===

	// RCommServ cs; 
	RCommServ cs;

	// cs.Connect();
	ASSERT_EQUALS(cs.Connect(), KErrNone,  _L("RRCommServ::Connect returned an error"));

	// RComm port; 
	RComm port;

	// port.Open(cs,commPort.iPort,ECommShared); 
	RCall::TCommPort commPort;
	port.Open(cs, commPort.iPort,ECommShared); 

	// ===  Check call is still connected ===

	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(aCall1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected,  _L("RCall::GetStatus did not return a status EStatusConnected"));

	// port.Write(stat,KWriteTestData); 
	TRequestStatus stat;
	port.Write(stat, KWriteTestData);
	
	// port.Close(); 
	port.Close();
	
	// cs.Close(); 
	cs.Close(); 

	// ===  Recover the data port ===

	// Recover data port with RCall::RecoverDataPort 
	ASSERT_EQUALS(aCall1.RecoverDataPort(), KErrNone, _L("RCall::RecoverDataPort returned an error"));

	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(aCall1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected,  _L("RCall::GetStatus did not return a status EStatusConnected"));
		
	// Check RMobileCall::GetMobileDataCallRLPRange with aRLPVersion=0 completes with KErrNotSupported
	TInt rplVersion = 0;
	RMobileCall::TMobileDataRLPRangesV1 range;
	RMobileCall::TMobileDataRLPRangesV1Pckg rangePckg(range);
	TExtEtelRequestStatus getMobileDataCallRLPRangeStatus(aCall1, EMobileCallGetMobileDataCallRLPRange);
	CleanupStack::PushL(getMobileDataCallRLPRangeStatus);
	aCall1.GetMobileDataCallRLPRange(getMobileDataCallRLPRangeStatus, rplVersion, rangePckg);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMobileDataCallRLPRangeStatus, ETimeMedium), KErrNotSupported,
		_L("RMobileCall::GetMobileDataCallRLPRange returned other than KErrNotSupported"));
	
	// Hang up call 
	TCoreEtelRequestStatus<RCall> hangUpStatus (aCall1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	aCall1.HangUp(hangUpStatus);
	
	// Pop
	// setDynamicHscsdParamsStatus
	// notifyStatusChangeStatus
	// notifyHscsdInfoStatus
	// notifydataCallCapsStatus
	// dialStatus
	// getMobileDataCallRLPRangeStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(7, &setDynamicHscsdParamsStatus);
	
	}


CCTSYIntegrationTestDataCallControl0007::CCTSYIntegrationTestDataCallControl0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0007::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0007::~CCTSYIntegrationTestDataCallControl0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0007
 * @SYMFssID BA/CTSY/DATC-0007
 * @SYMTestCaseDesc Answer an HSCSD data call
 * @SYMTestPriority High
 * @SYMTestActions RCall::RecoverDataPort, RMobileCall::SetDynamicHscsdParams, RCall::GetCallParams, RLine::NotifyIncomingCall, RMobileCall::GetMobileDataCallRLPRange, RCall::NotifyStatusChange, RCall::AnswerIncomingCall, RCall::GetStatus, RMobileCall::GetMobileDataCallCaps, RMobileCall::NotifyHscsdInfoChange
 * @SYMTestExpectedResults Pass - HSCSD call answered.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify HSCSD info, call params and data call caps.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get data line 1. 
	RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	
	DoTest0007L(dataLine);
	
	// Repeat with call at different speeds, 9.6kbits/s. 14.4kbits/s, 56kbits/s 
	DisplayUserInteractionPromptL(_L("Tester needs about to Dial in with CS call speed of 9.6kbits/s after pressing Any key"), ETimeMedium);
	DoTest0007L(dataLine);
	DisplayUserInteractionPromptL(_L("Tester needs about to Dial in with CS call speed of 14.4kbits/s after pressing Any key"), ETimeMedium);
	DoTest0007L(dataLine);
	DisplayUserInteractionPromptL(_L("Tester needs about to Dial in with CS call speed of 56kbits/s after pressing Any key"), ETimeMedium);
	DoTest0007L(dataLine);
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0007");
	}

void CCTSYIntegrationTestDataCallControl0007::DoTest0007L(RMobileLine& aDataLine)
	{

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	// ===  Answer an incoming HSCSD call ===

	// Post notification for incoming call from RPS
	TName name;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (aDataLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	aDataLine.NotifyIncomingCall(notifyIncomingCallStatus, name);

	// Request an incoming HSCSD call with RPS. 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"));
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"));
	
	// Get the incoming call
	TCallId incomingCallId;
	call1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// Post noitfy RMobileCall::NotifyMobileDataCallCapsChange
	RMobileCall::TMobileCallDataCapsV1 notifydataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg notifydataCallCapsPckg(notifydataCallCaps);
	TExtEtelRequestStatus notifydataCallCapsStatus(call1, EMobileCallNotifyMobileDataCallCapsChange);
	CleanupStack::PushL(notifydataCallCapsStatus);
	call1.NotifyMobileDataCallCapsChange(notifydataCallCapsStatus, notifydataCallCapsPckg);

	// Post notify RMobileCall::NotifyHscsdInfoChange
	RMobileCall::TMobileCallHscsdInfoV1 notifyHscsdParams;
	RMobileCall::TMobileCallHscsdInfoV1Pckg notifyHscsdParamsPckg(notifyHscsdParams);
	TExtEtelRequestStatus notifyHscsdInfoStatus(call1, EMobileCallNotifyHscsdInfoChange);
	CleanupStack::PushL(notifyHscsdInfoStatus);
	call1.NotifyHscsdInfoChange(notifyHscsdInfoStatus, notifyHscsdParamsPckg);

	// post notifier for RCall::NotifyStatusChange
	RCall::TStatus callStatus;
	TCoreEtelRequestStatus<RCall> notifyStatusChangeStatus (call1, &RCall::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyStatusChangeStatus);
	call1.NotifyStatusChange(notifyStatusChangeStatus,callStatus);
	
	// Answer the call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(call1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	call1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Check RCall::NotifyStatusChange completes with EStatusConnected.
	RCall::TStatus expectedCallStatus = RCall::EStatusConnected;
	iCallControlTestHelper.WaitForCallNotifyStatusChange(call1,
														 notifyStatusChangeStatus,
														 callStatus,
														 expectedCallStatus,
														 KErrNone);

	// ===  Check mobile data call caps ===

	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iHscsdSupport=ETrue
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iSpeedCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iProtocolCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iQoSCaps!=0
	// Check RMobileCall::NotifyMobileDataCallCapsChange completes with aCaps.iServiceCaps!=0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifydataCallCapsStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyMobileDataCallCapsChange timed-out"));
	ASSERT_TRUE(notifydataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::NotifyMobileDataCallCapsChange returned hscsd support EFalse"));
	ASSERT_TRUE(notifydataCallCaps.iSpeedCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned speed caps 0, should have been other than 0"));
	ASSERT_TRUE(notifydataCallCaps.iProtocolCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned protocol caps 0, should be other than 0"));	
	ASSERT_TRUE(notifydataCallCaps.iQoSCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned QoS caps 0, should be other than 0"));	
	ASSERT_TRUE(notifydataCallCaps.iServiceCaps != 0, _L("RMobileCall::NotifyMobileDataCallCapsChange returned service caps 0, should be other than 0"));	

	// Check RMobileCall::GetMobileDataCallCaps completes with aCaps.iHscsdSupport=ETrue
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iSpeedCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iProtocolCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iQoSCaps!=0
	// Check RMobileCall::GetMobileDataCallCaps returns aCaps.iServiceCaps!=0
	RMobileCall::TMobileCallDataCapsV1 dataCallCaps;
	RMobileCall::TMobileCallDataCapsV1Pckg CallCapsPckg(dataCallCaps);
	ASSERT_EQUALS(call1.GetMobileDataCallCaps(CallCapsPckg), KErrNone, _L("RMobileCall::GetMobileDataCallCaps returned an errro "));
	ASSERT_TRUE(dataCallCaps.iHscsdSupport != EFalse, _L("RMobileCall::GetMobileDataCallCaps returned hscsd support EFalse"));
	ASSERT_TRUE(dataCallCaps.iSpeedCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned speed caps 0, should have been other than 0"));
	ASSERT_TRUE(dataCallCaps.iProtocolCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned protocol caps 0, should be other than 0"));	
	ASSERT_TRUE(dataCallCaps.iQoSCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned QoS caps 0, should be other than 0"));	
	ASSERT_TRUE(dataCallCaps.iServiceCaps != 0, _L("RMobileCall::GetMobileDataCallCaps returned service caps 0, should be other than 0"));	

	// ===  Check HSCSD Info ===

	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iAiur!=EAiurBpsUnspecified
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iCodings!=ETchCodingUnspecified
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iRxTimeSlots>0
	// Check RMobileCall::NotifyHscsdInfoChange with parameter of type TMobileCallHscsdInfoV1 returns iTxTimeSlots>0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyHscsdInfoStatus, ETimeMedium), KErrNone, _L("RMobileCall::NotifyHscsdInfoChange timed-out"));
	ASSERT_TRUE(notifyHscsdParams.iAiur != RMobileCall::EAiurBpsUnspecified, _L("RMobileCall::NotifyHscsdInfoChange returned aiur EAiurBpsUnspecified"));
	ASSERT_TRUE(notifyHscsdParams.iCodings != RMobileCall::ETchCodingUnspecified, _L("RMobileCall::NotifyHscsdInfoChange returned codings ETchCodingUnspecified"));
	ASSERT_TRUE(notifyHscsdParams.iRxTimeSlots > 0, _L("RMobileCall::NotifyHscsdInfoChange returned rxTimeSlots 0, should be > 0"));	
	ASSERT_TRUE(notifyHscsdParams.iTxTimeSlots > 0, _L("RMobileCall::NotifyHscsdInfoChange returned txTimeSlots 0, should be > 0"));	

	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iAiur!=EAiurBpsUnspecified
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iCodings!=ETchCodingUnspecified
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iRxTimeSlots>0
	// Check RMobileCall::GetCurrentHscsdInfo with parameter of type TMobileCallHscsdInfoV1 returns iTxTimeSlots>0
	RMobileCall::TMobileCallHscsdInfoV1 hscsdparams;
	RMobileCall::TMobileCallHscsdInfoV1Pckg hscsdparamsPckg(hscsdparams); 
	ASSERT_EQUALS(call1.GetCurrentHscsdInfo(hscsdparamsPckg), KErrNone, _L("RMobileCall::GetCurrentHscsdInfo returned an error"));
	ASSERT_TRUE(hscsdparams.iAiur != RMobileCall::EAiurBpsUnspecified, _L("RMobileCall::GetCurrentHscsdInfo returned aiur EAiurBpsUnspecified"));
	ASSERT_TRUE(hscsdparams.iCodings != RMobileCall::ETchCodingUnspecified, _L("RMobileCall::GetCurrentHscsdInfo returned codings ETchCodingUnspecified"));
	ASSERT_TRUE(hscsdparams.iRxTimeSlots > 0, _L("RMobileCall::GetCurrentHscsdInfo returned rxTimeSlots 0, should be > 0"));	
	ASSERT_TRUE(hscsdparams.iTxTimeSlots > 0, _L("RMobileCall::GetCurrentHscsdInfo returned txTimeSlots 0, should be > 0"));	

	// ===  Get call params ===

	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iAsymmetry=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iCodings=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iMaxTimeSlots > 0
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iUserInitUpgrade=same as that used in dial
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iProtocol!=EProtocolUnspecified
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iQoS!=EQoSUnspecified
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iService!=EServiceUnspecified
	// Check RCall::GetCallParams with aParams of type TMobileHscsdCallParamsV1 returns iSpeed!=ESpeedUnspecified
	// NOTE: some of parameters are checked due test does not make a Dial and we cannot compare "same as that used in dial"
	RMobileCall::TMobileHscsdCallParamsV1 dataparams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg dataparamsPckg(dataparams);
	ASSERT_EQUALS(call1.GetCallParams(dataparamsPckg), KErrNone, _L("RCall::GetCallParams returned an error"));
	ASSERT_TRUE(dataparams.iMaxTimeSlots > 0, _L("RCall::GetCallParams max time slot was 0, should have been bigger than 0"));
	ASSERT_TRUE(dataparams.iProtocol != RMobileCall::EProtocolUnspecified, _L("RCall::GetCallParams returned protocol RMobileCall::EProtocolUnspecified, should have been other"));
	ASSERT_TRUE(dataparams.iQoS != RMobileCall::EQoSUnspecified, _L("RCall::GetCallParams returned QoS RMobileCall::EProtocoEQoSUnspecifiedl, should have been other")); 	
	ASSERT_TRUE(dataparams.iSpeed != RMobileCall::ESpeedUnspecified, _L("RCall::GetCallParams returned iSpeed RMobileCall::ESpeedUnspecified, should have been other")); 	

	// Hang up call. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	
	// Pop
	// notifyIncomingCallStatus
	// notifyStatusChangeStatus
	// notifydataCallCapsStatus
	// notifyHscsdInfoStatus
	// answerIncomingStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(6, &notifyIncomingCallStatus);

	}


CCTSYIntegrationTestDataCallControl0008::CCTSYIntegrationTestDataCallControl0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0008::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0008::~CCTSYIntegrationTestDataCallControl0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0008
 * @SYMFssID BA/CTSY/DATC-0008
 * @SYMTestCaseDesc Check that a data call cannot be put on hold (GSM limitation)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrNotSupported returned
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
 	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone,
				   _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1. 
	 RLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
    
	// Get call 1. 
	RMobileCall& dataCall1= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KDataLine, KCall1);

	// Ensure there is a data call connected.
	TPtrC dataNumber; 
    CHECK_TRUE_L(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber), _L("Failed to obtain KIniDataNumber1") ); 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(dataCall1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	dataCall1.Dial(dialStatus, dataNumber);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeVeryLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial did not return KErrNone"));
	
	// Check RCall::GetStatus returns EStatusConnected
	RCall::TStatus callStatus;
	CHECK_EQUALS_L(dataCall1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	CHECK_EQUALS_L(callStatus, RCall::EStatusConnected,  _L("RCall::GetStatus did not return a status EStatusConnected"));


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Check data call does not have hold caps ===

	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsHold
	RMobileCall::TMobileCallCapsV1 unwantedMobileCaps;
	unwantedMobileCaps.iCallControlCaps = RMobileCall::KCapsHold;
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg capsPckg(mobileCallCaps);
	ASSERT_EQUALS(dataCall1.GetMobileCallCaps(capsPckg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned an error"));								
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, mobileCallCaps.iCallControlCaps, unwantedMobileCaps.iCallControlCaps, _L("RMobileCall::GetMobileCallCaps containsed unwanted caps KCapsHold"));
	
	// Attempt to hold the call with RMobileCall::Hold  
	TExtEtelRequestStatus holdStatus(dataCall1, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	dataCall1.Hold(holdStatus);
	
	// Check this completes with KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out."));
	ASSERT_EQUALS(holdStatus.Int(), KErrNotSupported, _L("RMobileCall::Hold did not return KErrNotSupported."));

	// ===  Check call is still connected ===

	// Check RCall::GetStatus returns EStatusConnected
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected,  _L("RCall::GetStatus did not return a status EStatusConnected"));

	
	//
	// TEST END
	//


    StartCleanup();
	
	// Pop
	// dialStatus
	// holdStatus
	CleanupStack::PopAndDestroy(2, &dialStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0008");
	}



CCTSYIntegrationTestDataCallControl0009::CCTSYIntegrationTestDataCallControl0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0009::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0009::~CCTSYIntegrationTestDataCallControl0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0009
 * @SYMFssID BA/CTSY/DATC-0009
 * @SYMTestCaseDesc Deflect data call (currently unsupported)
 * @SYMTestPriority High
 * @SYMTestActions RMobileCall::GetMobileCallStatus, RMobileCall::NotifyMobileCallCapsChange, RMobileCall::GetMobileCallCaps, RMobileCall::Deflect
 * @SYMTestExpectedResults Pass - KErrNotSupported returned
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotSupported returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Get data line 1. 
    RLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
    
	// Get call 1. 
	RMobileCall& dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Request for an incoming call ===

	// Post notification for incoming call from RPS
	TName name;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (dataLine,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	dataLine.NotifyIncomingCall(notifyIncomingCallStatus,name);
	
	// Request incoming data call from RPS. 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));
	
	// Check RLine::NotifyIncomingCall completes with a valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"));
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"));
	
	//Get the incoming call
	TCallId incomingCallId;
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	RMobileCall::TMobileCallStatus mobileCallStatus;
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(dataCall1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	dataCall1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Post notifier for RMobileCall::NotifyMobileCallCapsChange
	RMobileCall::TMobileCallCapsV1 mobileCallCaps;
	RMobileCall::TMobileCallCapsV1Pckg capsPckg(mobileCallCaps);
	TExtEtelRequestStatus notifyMobileCallCapsChangeStatus(dataCall1, EMobileCallNotifyMobileCallCapsChange);
	CleanupStack::PushL(notifyMobileCallCapsChangeStatus);
	dataCall1.NotifyMobileCallCapsChange(notifyMobileCallCapsChangeStatus,capsPckg);
	
	// ===  Check call is in correct state to be deflected ===

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusRinging
	const TInt wantedStatus = KErrNone;
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusRinging;
	// $CTSYProblem: NotifyMobileCallStatusChange coming too fast. CMmDataCallTsy::CompleteNotifyStatusChange coming earlier than notify request. 
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(dataCall1,
															notifyMobileCallStatusChangeStatus,
															mobileCallStatus,
															expectedMobileCallStatus,
															wantedStatus);	
																												
	// Check RMobileCall::GetMobileCallStatus returns status of EStatusRinging
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(mobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned error."));
	ASSERT_EQUALS(mobileCallStatus, RMobileCall::EStatusRinging, _L("RMobileCall::GetMobileCallStatus returned error."));
	
	// ===  Check data call does not have deflect caps ===

	// Check RMobileCall::NotifyMobileCallCapsChange completes with iCallControlCaps NOT in set of KCapsDeflect
	RMobileCall::TMobileCallCapsV1 wantedMobileCaps, unwantedMobileCaps;
	wantedMobileCaps.iCallEventCaps = KNoWantedBits;
	unwantedMobileCaps.iCallControlCaps = RMobileCall::KCapsDeflect;
	TCmpRMobileCallTMobileCallCaps mobileCapsComp(wantedMobileCaps,mobileCallCaps, *this);
	iCallControlTestHelper.WaitForMobileCallNotifyMobileCallCapsChange(dataCall1, 
																	   notifyMobileCallCapsChangeStatus,
																	   mobileCapsComp,
																	   wantedStatus);
						
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, wantedMobileCaps.iCallControlCaps, unwantedMobileCaps.iCallControlCaps,  
			_L("RMobileCall::NotifyMobileCallCapsChange did not set expected iCallControlCaps"));
														   
	// Check RMobileCall::GetMobileCallCaps includes iCallControlCaps NOT in set of KCapsDeflect
	unwantedMobileCaps.iCallControlCaps = RMobileCall::KCapsDeflect;
	ASSERT_EQUALS(dataCall1.GetMobileCallCaps(capsPckg), KErrNone, _L("RMobileCall::GetMobileCallCaps returned an error"));								
	ASSERT_BITS_SET(mobileCallCaps.iCallControlCaps, KNoWantedBits, unwantedMobileCaps.iCallControlCaps, _L("RMobileCall::GetMobileCallCaps contains unwanted caps KCapsTransfer"));
	
	// Attempt to deflect the call to any number with RMobileCall::Deflect 
	DEBUG_PRINTF1(_L("Deflecting call... "));
    TExtEtelRequestStatus setDeflectStatus(dataLine, EMobileCallDeflect);
    CleanupStack::PushL(setDeflectStatus);
    
    RMobilePhone::TMobileAddress destination;
    dataCall1.Deflect(setDeflectStatus, RMobileCall::EDeflectVoicemail, destination);
    ASSERT_EQUALS(setDeflectStatus.Int(), KErrNone, _L("RMobileCall::Deflect returned with an error"));
    
	// Check this completes with KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setDeflectStatus, ETimeLong), KErrNone, _L("RCall::Deflect timed-out"));
	ASSERT_TRUE((setDeflectStatus.Int() == KErrNotSupported), _L("RMobileCall::Deflect did not return KErrNotSupported"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyIncomingCallStatus
	// notifyMobileCallStatusChangeStatus
	// notifyMobileCallCapsChangeStatus
	// setDeflectStatus
	CleanupStack::PopAndDestroy(4,&notifyIncomingCallStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0009");
	}



CCTSYIntegrationTestDataCallControl0010::CCTSYIntegrationTestDataCallControl0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0010::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0010::~CCTSYIntegrationTestDataCallControl0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0010
 * @SYMFssID BA/CTSY/DATC-0010
 * @SYMTestCaseDesc CSD call cannot be made when a voice call is active or held
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrEtelCallAlreadyActive returned on attempt to dial a data call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrEtelCallAlreadyActive returned
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
    // Fetch used phone numbers
    TPtrC voiceNumber; 
    CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, voiceNumber), _L("Failed to obtain KIniVoiceNumber1") );
    TPtrC dataNumber; 
    CHECK_TRUE_L(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber), _L("Failed to obtain KIniDataNumber1") );
	
	// Get data line 1. 
	RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get call 1 with data line 1. 
	RMobileCall& dataCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	// Get voice line 2 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 2 with voice line 2 
	RMobileCall& voiceCall = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);

	// Ensure there is a connected voice call on the voice line. 
	TCoreEtelRequestStatus<RMobileCall> dialStatus(voiceCall, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	voiceCall.Dial(dialStatus, voiceNumber);
    DEBUG_PRINTF2(_L("Dialling call 2(voice):%S"), &voiceNumber);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Dial a number on call 1. 
	TCoreEtelRequestStatus<RMobileCall> dialStatus2(dataCall, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus2);
	dataCall.Dial(dialStatus2, dataNumber);
    DEBUG_PRINTF2(_L("Dialling call 1(data):%S"), &dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus2, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));

	// Check dial completes with KErrEtelCallAlreadyActive
	ASSERT_EQUALS(dialStatus2.Int(), KErrEtelCallAlreadyActive, _L("RMobileCall::Dial did not return KErrEtelCallAlreadyActive as expected"));

	// Hold the voice call. 
	TExtEtelRequestStatus holdStatus(voiceCall, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	voiceCall.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out."));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold Returned error."));
    
	// Dial a number on call 1. 
	dataCall.Dial(dialStatus2, dataNumber);
    DEBUG_PRINTF2(_L("Dialling call 1(data):%S"), &dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus2, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
		
	// Check dial completes with KErrEtelCallAlreadyActive
	// $CTSYProblem CTSY returns KErrServerBusy instead of KErrEtelCallAlreadyActive.This is because the first data call doesn't complete.
	// CMmDataCallTsy::Dial() set the dialFlag to ETrue in the first data call and when second call is dialled it is still ETrue.
	// When CMmDataCallTsy::Dial() returns KErrEtelCallAlreadyActive it should also set dialFlag back to EFalse.	
	ASSERT_EQUALS(dialStatus2.Int(), KErrEtelCallAlreadyActive, _L("RMobileCall::Dial did not return KErrEtelCallAlreadyActive as expected"));

	//
	// TEST END
	//

    StartCleanup();
	
	voiceCall.HangUp();
    dataCall.HangUp();	
	
	// Pop
	// dialStatus
	// dialStatus2
	// holdStatus
	CleanupStack::PopAndDestroy(3,&dialStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0010");
	}



CCTSYIntegrationTestDataCallControl0011::CCTSYIntegrationTestDataCallControl0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestDataCallControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestDataCallControl0011::GetTestStepName());
	}

CCTSYIntegrationTestDataCallControl0011::~CCTSYIntegrationTestDataCallControl0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestDataCallControl0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-DATC-0011
 * @SYMFssID BA/CTSY/DATC-0011
 * @SYMTestCaseDesc CSD call can't be made when conference call is active or held
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrEtelCallAlreadyActive returned on attempt to dial a data call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrEtelCallAlreadyActive returned
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
    
    // Fetch used phone numbers
    TPtrC voiceNumber1; 
    CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, voiceNumber1), _L("Failed to obtain KIniVoiceNumber1") );
    TPtrC voiceNumber2; 
    CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, voiceNumber2), _L("Failed to obtain KIniVoiceNumber2") );
    TPtrC dataNumber1; 
    CHECK_TRUE_L(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber1), _L("Failed to obtain KIniDataNumber1") );
    	
	// Get data line 1. 
	RMobileLine& line = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);

	// Get call 1 with data line 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);
	
	// Ensure there is an active 3 party conference call. 
	RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);
	RMobileCall& call3 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall3);
	RMobileConferenceCall& confCall = iEtelSessionMgr.GetConferenceCallL(KMainServer,KMainPhone,KMainConferenceCall);
	
	TCoreEtelRequestStatus<RMobileCall> dialStatus1(call2, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus1);
	call2.Dial(dialStatus1, voiceNumber1);
    DEBUG_PRINTF2(_L("Dialling call 2(voice):%S"), &voiceNumber1);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus1, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus1.Int(), KErrNone, _L("RMobileCall::Dial returned an error"));
	
	TExtEtelRequestStatus holdStatus(call2, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	call2.Hold(holdStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(holdStatus, ETimeMedium), KErrNone, _L("RMobileCall::Hold timed-out."));
	CHECK_EQUALS_L(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold Returned error."));
		
	TCoreEtelRequestStatus<RMobileCall> dialStatus2(call3, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus2);
	call3.Dial(dialStatus2, voiceNumber2);
    DEBUG_PRINTF2(_L("Dialling call 3(voice):%S"), &voiceNumber2);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(dialStatus2, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	CHECK_EQUALS_L(dialStatus2.Int(), KErrNone, _L("RMobileCall::Dial returned an error"));
		
	TExtEtelRequestStatus createConferenceStatus(phone, EMobileConferenceCallCreateConference);
	CleanupStack::PushL(createConferenceStatus);
	confCall.CreateConference(createConferenceStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(createConferenceStatus, ETimeMedium), KErrNone, _L("RMobileConferenceCall::CreateConference timed out"));
	CHECK_EQUALS_L(createConferenceStatus.Int(), KErrNone, _L("RMobileConferenceCall::CreateConference returned an error"));
    
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Dial a number on call 1. 
	TCoreEtelRequestStatus<RMobileCall> dialStatus3(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus3);
	call1.Dial(dialStatus3, dataNumber1);
    DEBUG_PRINTF2(_L("Dialling call 3(data):%S"), &dataNumber1);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus3, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	
	// Check dial completes with KErrEtelCallAlreadyActive
	ASSERT_EQUALS(dialStatus3.Int(), KErrEtelCallAlreadyActive, _L("RMobileCall::Dial did not return KErrEtelCallAlreadyActive as expected"));

	// Put the conference call on hold using RMobileConferenceCall::Swap 
	TExtEtelRequestStatus swapConferenceStatus(confCall, EMobileConferenceCallSwap);
	CleanupStack::PushL(swapConferenceStatus);
	confCall.Swap(swapConferenceStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(swapConferenceStatus,ETimeMedium), KErrNone, _L("RMobileConferenceCall::Swap timed out"));
	ASSERT_EQUALS(swapConferenceStatus.Int(),KErrNone, _L("RMobileConferenceCall::Swap returned error"));

	// Dial a number on call 1. 
	call1.Dial(dialStatus3, dataNumber1);
    DEBUG_PRINTF2(_L("Dialling call 3(data):%S"), &dataNumber1);
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus3, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));

	// Check dial completes with KErrEtelCallAlreadyActive
	// $CTSYProblem CTSY returns KErrServerBusy instead of KErrEtelCallAlreadyActive
    ASSERT_EQUALS(dialStatus3.Int(), KErrEtelCallAlreadyActive, _L("RMobileCall::Dial did not return KErrEtelCallAlreadyActive as expected"));
	
	//
	// TEST END
	//

    StartCleanup();
		
    call1.HangUp();
    call2.HangUp();
    call3.HangUp();	
	
	// Pop
	// dialStatus1
	// holdStatus
	// dialStatus2
	// createConferenceStatus
	// dialStatus3
	// swapConferenceStatus
	CleanupStack::PopAndDestroy(6,&dialStatus1);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestDataCallControl0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestDataCallControl0011");
	}



