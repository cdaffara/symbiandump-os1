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
// Test step definitions for the MultiBearerInteroperability functional unit.
// 
//

/**
 @internalTechnology
*/
#include <etelpckt.h>
#include <ctsy/rmmcustomapi.h>
#include <pcktcs.h>
#include "cctsyinidata.h" 
#include "cctsyintegrationtestmultibearerinteroperability.h"
#include "chttpdownload.h"


CCTSYIntegrationTestMultiBearerInteroperabilityBase::CCTSYIntegrationTestMultiBearerInteroperabilityBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr),
		iNetworkTestHelper(*this),
		iPacketServiceTestHelper(*this),
		iCallControlTestHelper(*this),
		iSmsStoreTsyTestHelper(*this)
			
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestMultiBearerInteroperabilityBase::~CCTSYIntegrationTestMultiBearerInteroperabilityBase()
/*
 * Destructor
 */
	{
	}
	
void CCTSYIntegrationTestMultiBearerInteroperabilityBase::SetAttachModeToWhenPossibleAndWaitForAttachL(RPacketService& aPacketService)
/**
 * Attaches the phone to the packet service by setting the
 * attach mode to RPacketService::EAttachWhenPossible. Requests
 * that the phone attaches to the packet service and waits for
 * attach to complete.
 * 
 * @param aPacketService Reference to packet service subsession.
 *
 * @return void
 *
 */
    {
    RPacketService::TAttachMode attachMode;
    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));

    TExtEtelRequestStatus notifyServiceStatusChStatus(aPacketService, EPacketNotifyStatusChange);
    CleanupStack::PushL(notifyServiceStatusChStatus);
    RPacketService::TStatus packetServiceStatus;
    aPacketService.NotifyStatusChange(notifyServiceStatusChStatus, packetServiceStatus);

    if(attachMode != RPacketService::EAttachWhenPossible)
		{
		CHECK_EQUALS_L(aPacketService.SetAttachMode(RPacketService::EAttachWhenPossible), KErrNone, _L("RPacketService::SetAttachMode returned with an error."));
		}

    CHECK_EQUALS_L(aPacketService.GetAttachMode(attachMode), KErrNone, _L("RPacketService::GetAttachMode returned with an error."));
    CHECK_EQUALS_L(attachMode, RPacketService::EAttachWhenPossible, _L("RPacketService::GetAttachMode did not return EAttachWhenPossible."));

    // Ensure phone is attached to the packet service (RPacketService::GetStatus returns EStatusAttached).
	RPacketService::TStatus packetStatus;
	CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
	
	// Wait for phone to attach to packet service if it is not attached.
	if (packetStatus == RPacketService::EStatusUnattached)
		{
        iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(
				aPacketService,
				notifyServiceStatusChStatus,
				packetServiceStatus, 
				RPacketService::EStatusAttached,
				KErrNone);
        CHECK_EQUALS_L(aPacketService.GetStatus(packetStatus), KErrNone, _L("RPacketService::GetStatus returned with an error"));
        CHECK_EQUALS_L(packetStatus, RPacketService::EStatusAttached, _L("RPacketService::GetStatus did not return EStatusAttached"));
		}
    
    // Pop
    // notifyServiceStatusChStatus
    CleanupStack::PopAndDestroy(1, &notifyServiceStatusChStatus);
    }



		
CCTSYIntegrationTestMultiBearerInteroperability0001::CCTSYIntegrationTestMultiBearerInteroperability0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultiBearerInteroperabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultiBearerInteroperability0001::GetTestStepName());
	}

CCTSYIntegrationTestMultiBearerInteroperability0001::~CCTSYIntegrationTestMultiBearerInteroperability0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultiBearerInteroperability0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBI-0001
 * @SYMFssID BA/CTSY/MBI-0001
 * @SYMTestCaseDesc Verify simultaneous PS connection and MO CS call
 * @SYMTestPriority High
 * @SYMTestActions RLine::NotifyStatusChange, RCall::HangUp, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RMobileSmsMessaging::SendMessage
 * @SYMTestExpectedResults Pass - MO call made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test:
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer,KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure phone is attached to packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    				
	// Get voice line 1. 
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Post a notifier for RLine::NotifyStatusChange
	TCoreEtelRequestStatus<RLine> notifyLineChangeStatus (mobileLine, &RLine::NotifyStatusChangeCancel);
	CleanupStack::PushL(notifyLineChangeStatus);
	RCall::TStatus lineStatus;
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);

	// Dial a number that answers. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(mobileCall, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, number);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));
	
	// Verify line rings and call is answered.
	RCall::TStatus expectedLineStatus = RCall::EStatusDialling;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
													     KErrNone);
	
    expectedLineStatus = RCall::EStatusConnecting;
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);

    expectedLineStatus = RCall::EStatusConnected;
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);

	// Hang up call. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (mobileCall, &RMobileCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	mobileCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));
	
	// Activate a PDP context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer, KMainPhone, KMainPacketService, KPrimaryPacketContext1);
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, packetContext);

    // Repost notifier for RLine::NotifyStatusChange
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);

	// Post notification for service's status change
	RPacketService::TStatus packetServiceStatus;
	TExtEtelRequestStatus notifyServiceStatusChange(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyServiceStatusChange);
	packetService.NotifyStatusChange(notifyServiceStatusChange, packetServiceStatus);

	// Post notification for context's status change
	RPacketContext::TContextStatus packetContextStatus;
	TExtEtelRequestStatus notifyContextStatusChange(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStatusChange);
	packetContext.NotifyStatusChange(notifyContextStatusChange, packetContextStatus);

	// Dial a number that answers. 
	mobileCall.Dial(dialStatus, number);

	// Verify line rings and call is answered.
	expectedLineStatus = RCall::EStatusDialling;
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
													     KErrNone);
	
    expectedLineStatus = RCall::EStatusConnecting;
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);

	expectedLineStatus = RCall::EStatusConnected;
	mobileLine.NotifyStatusChange(notifyLineChangeStatus,lineStatus);
	iCallControlTestHelper.WaitForLineNotifyStatusChange(mobileLine,
														 notifyLineChangeStatus,
														 lineStatus,
														 expectedLineStatus,
														 KErrNone);

    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Verify packet service and context are suspended. 
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusSuspended;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService,
	                                                                notifyServiceStatusChange,
	                                                                packetServiceStatus,
	                                                                expectedPacketServiceStatus,
	                                                                KErrNone);

	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusSuspended;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(packetContext,
	                                                                notifyContextStatusChange,
	                                                                packetContextStatus,
	                                                                expectedPacketContextStatus,
	                                                                KErrNone);

	// Repost notification for service's status change
	packetService.NotifyStatusChange(notifyServiceStatusChange, packetServiceStatus);

	// Repost notification for context's status change
	packetContext.NotifyStatusChange(notifyContextStatusChange, packetContextStatus);

	// Hang up. 
	mobileCall.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Verify packet service and context are active again. 
    expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService,
	                                                                notifyServiceStatusChange,
	                                                                packetServiceStatus,
	                                                                expectedPacketServiceStatus,
	                                                                KErrNone);

	expectedPacketContextStatus = RPacketContext::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(packetContext,
	                                                                notifyContextStatusChange,
	                                                                packetContextStatus,
	                                                                expectedPacketContextStatus,
	                                                                KErrNone);

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// notifyLineChangeStatus
	// dialStatus
	// hangUpStatus
	// notifyServiceStatusChange
	// notifyContextStatusChange
	CleanupStack::PopAndDestroy(5, &notifyLineChangeStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultiBearerInteroperability0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultiBearerInteroperability0001");
	}



CCTSYIntegrationTestMultiBearerInteroperability0002::CCTSYIntegrationTestMultiBearerInteroperability0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultiBearerInteroperabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultiBearerInteroperability0002::GetTestStepName());
	}

CCTSYIntegrationTestMultiBearerInteroperability0002::~CCTSYIntegrationTestMultiBearerInteroperability0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultiBearerInteroperability0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBI-0002
 * @SYMFssID BA/CTSY/MBI-0002
 * @SYMTestCaseDesc Verify simultaneous PS connection and MO SMS
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyStatusChange, RPacketService::NotifyStatusChange, RMobileSmsMessaging::SendMessage
 * @SYMTestExpectedResults Pass - MO SMS sent.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 

	// Ensure phone is attached to packet service. 

	// Get voice line 1. 

	// Get call 1. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Send an SMS.   

	// Verify SMS can be sent. 

	// Activate a PDP context. 

	// Send an SMS. 

	// Verify SMS can be sent. 

	// Verify packet service and context become suspended and the active again when message sent. 

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultiBearerInteroperability0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultiBearerInteroperability0002");
	}



CCTSYIntegrationTestMultiBearerInteroperability0003::CCTSYIntegrationTestMultiBearerInteroperability0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultiBearerInteroperabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultiBearerInteroperability0003::GetTestStepName());
	}

CCTSYIntegrationTestMultiBearerInteroperability0003::~CCTSYIntegrationTestMultiBearerInteroperability0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultiBearerInteroperability0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBI-0003
 * @SYMFssID BA/CTSY/MBI-0003
 * @SYMTestCaseDesc 3G-2G handover with PDP context active
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::NotifyDataTransferred, RPacketContext::GetDataVolumeTransferred, RMobilePhone::GetNetworkRegistrationStatus, RMobilePhone::NotifyNetworkRegistrationStatusChange
 * @SYMTestExpectedResults Pass - File continues downloading after each cell reselection, service is never lost.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10071);
	User::Leave(KErrNone);	
	

	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Ensure phone has KCapsNotifyRegistrationStatus and KCapsNotifyCurrentNetwork capabilities. 
	TUint32 caps;
	CHECK_EQUALS_L(phone.GetNetworkCaps(caps), KErrNone, _L("RMobilePhone::GetNetworkCaps returned an error"));
	TUint32 wantedCaps = RMobilePhone::KCapsNotifyRegistrationStatus | RMobilePhone::KCapsNotifyCurrentNetwork;
	CHECK_BITS_SET_L(caps, wantedCaps, KNoUnwantedBits, _L("RMobilePhone::GetNetworkCaps returned wrong caps"));

	// Ensure phone is attached to packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Get Voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// Start downloading a file via HTTP. 
	TBuf<140> host(_L("developer.symbian.org"));
	TBuf<140> page(_L("/wiki/images/1/12/Common_Design_Patterns_for_Symbian_OS_Sample_Chapter.pdf"));
    CHTTPDownload* download = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download);
    ASSERT_TRUE(download->StartDownloadL(host,page), _L("Download Failed - perhaps page has moved!"));
    
 	// Get context info	
	TExtEtelRequestStatus getContextInfo(packetService, EPacketGetContextInfo);
	CleanupStack::PushL(getContextInfo);
	RPacketService::TContextInfo contextInfo;
	TInt index = 0;
	packetService.GetContextInfo(getContextInfo, index, contextInfo);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextInfo, ETimeLong), KErrNone, _L("RPacketService::GetContextInfo timed out"));
	ASSERT_EQUALS(getContextInfo.Int(), KErrNone, _L("RPacketService::GetContextInfo returned an error"));
	ASSERT_TRUE(contextInfo.iName.Length() > 0, _L("RPacketService::GetContextInfo returned with zero length context name"));

	// Open a handle on an existing context with OpenExistingContext
	RPacketContext primaryPacketContext;										
    ASSERT_EQUALS(primaryPacketContext.OpenExistingContext(packetService, contextInfo.iName), KErrNone, _L("RPacketContext::OpenExistingContext returned with an error."));

	// Post notification for network change
	RMobilePhone::TMobilePhoneNetworkInfoV1 netInfo;
	RMobilePhone::TMobilePhoneLocationAreaV1 locationArea;
	TPckg<RMobilePhone::TMobilePhoneNetworkInfoV1> netInfoPckg(netInfo);
	TExtEtelRequestStatus notifyCurrentNetworkChangeStatus(phone, EMobilePhoneNotifyCurrentNetworkChange);
	CleanupStack::PushL(notifyCurrentNetworkChangeStatus);
	phone.NotifyCurrentNetworkChange(notifyCurrentNetworkChangeStatus, netInfoPckg, locationArea);	

	// Request status for RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (voiceLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName incomingCallName;

	// Get tranferred data for later comparison
    RPacketContext::TDataVolume dataVolume;	
	primaryPacketContext.GetDataVolumeTransferred(dataVolume);	
	TUint32 lastBytesSent = dataVolume.iBytesSent;
	TUint32 lastBytesReceived = dataVolume.iBytesReceived;

	// Move from one cell to another. At each cell: 
	const TInt KCellChange = 7;
	for(TInt index = 0; index < KCellChange; index++)
		{
		switch(index)
			{
			case 0:		// Release 99 3G cell To/From an EGSM 900 cell 
				{
				DisplayUserInteractionPromptL(_L("Release 99 3G cell To/From an EGSM 900 cell"));
				break;
				}
			case 1: 	// Release 99 3G cell To/From an GSM 900 cell 
				{
				DisplayUserInteractionPromptL(_L("Release 99 3G cell To/From an GSM 900 cell"));
				break;
				}
			case 2: 	// Release 99 3G cell To/From an GSM 1800 cell 
				{
				DisplayUserInteractionPromptL(_L("Release 99 3G cell To/From an GSM 1800 cell"));
				break;
				}
			case 3:		// HSDPA enabled Release 5 3G cell To/From an EGSM 900 cell 
				{
				DisplayUserInteractionPromptL(_L("HSDPA enabled Release 5 3G cell To/From an EGSM 900 cell"));
				break;
				}
			case 4:		// HSDPA enabled Release 5 3G cell To/From an GSM 900 cell 
				{
				DisplayUserInteractionPromptL(_L("HSDPA enabled Release 5 3G cell To/From an GSM 900 cell"));
				break;
				}
			case 5:		// HSDPA enabled Release 5 3G cell To/From an GSM 1800 cell 
				{
				DisplayUserInteractionPromptL(_L("HSDPA enabled Release 5 3G cell To/From an GSM 1800 cell"));
				break;
				}
			case 6:		// Cell with poor signal strength 
				{
				DisplayUserInteractionPromptL(_L("Move to a cell with poor signal strength"));
				break;
				}
			}

		// Verify cell reselection completes. 
		ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyCurrentNetworkChangeStatus, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange timed out"));
		ASSERT_EQUALS(notifyCurrentNetworkChangeStatus.Int(), KErrNone, _L("RMobilePhone::NotifyCurrentNetworkChange returned error"));

		// Verify service is not lost and phone remains registered. 
		ASSERT_EQUALS(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

		// Verify volume data transferred advances realistically. 
		primaryPacketContext.GetDataVolumeTransferred(dataVolume);	
		ASSERT_TRUE(dataVolume.iBytesSent > lastBytesSent, _L("RPacketContext::GetDataVolumeTransferred Data bytes send is not higher than at the previous round."));
		lastBytesSent = dataVolume.iBytesSent;
		ASSERT_TRUE(dataVolume.iBytesReceived > lastBytesReceived, _L("RPacketContext::GetDataVolumeTransferred Data bytes received is not higher than at the previous round."));
		lastBytesReceived = dataVolume.iBytesReceived;

		// Repost notification for network change
		phone.NotifyCurrentNetworkChange(notifyCurrentNetworkChangeStatus, netInfoPckg, locationArea);	

		// Receive an incoming call and verify it can be answered

		// post notify RLine::NotifyIncomingCall
		voiceLine.NotifyIncomingCall(notifyIncomingCallStatus, incomingCallName);

		// Request incoming call from RPS. 
		ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL returned an error"));

		// Check RLine::NotifyIncomingCall completes.
		ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus, ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
		ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned an error"));
		ASSERT_TRUE(incomingCallName.Length() > 0, _L("RLine::NotifyIncomingCall did not set a line name"));

		// Get the incoming call
		TCallId incomingCallId;
		RMobileCall& call1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, incomingCallName, incomingCallId);

		// Call RCall::AnswerIncomingCall
		TCoreEtelRequestStatus<RCall> answerIncomingStatus(call1, &RCall::AnswerIncomingCallCancel);
		CleanupStack::PushL(answerIncomingStatus);	
		call1.AnswerIncomingCall(answerIncomingStatus);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
		ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

		// Hang up
		call1.HangUp();
		
		// Release call
		iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KVoiceLine, incomingCallId);
		
		// Pop
		// answerIncomingStatus
		CleanupStack::PopAndDestroy(&answerIncomingStatus);
		}


	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// download
	// getContextInfo
	// notifyCurrentNetworkChangeStatus
	// notifyIncomingCallStatus
	CleanupStack::PopAndDestroy(4, &download);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultiBearerInteroperability0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultiBearerInteroperability0003");
	}



CCTSYIntegrationTestMultiBearerInteroperability0004::CCTSYIntegrationTestMultiBearerInteroperability0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultiBearerInteroperabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultiBearerInteroperability0004::GetTestStepName());
	}

CCTSYIntegrationTestMultiBearerInteroperability0004::~CCTSYIntegrationTestMultiBearerInteroperability0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultiBearerInteroperability0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBI-0004
 * @SYMFssID BA/CTSY/MBI-0004
 * @SYMTestCaseDesc Voice call & CS-data call
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetMulticallParams, RMobilePhone::NotifyMulticallParamsChange, RCall::Dial, RMobilePhone::NotifyMulticallIndicatorChange, RCall::HangUp, RMobileCall::Hold, RMobileCall::Resume
 * @SYMTestExpectedResults Pass - One CS call can be hung up without disturbing other call.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Get Voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& voiceCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Get data line 1. 
	RMobileLine& dataLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get data call 1. 
	RMobileCall& dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);	


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//


	// Get the multicall parameters. 
	RMobilePhone::TMobilePhoneMulticallSettingsV1 multiCallSettings;
	TPckg<RMobilePhone::TMobilePhoneMulticallSettingsV1> multiCallSettingsPkg(multiCallSettings);
	TExtEtelRequestStatus getMulticallParams(phone, EMobilePhoneGetMulticallParams);
	CleanupStack::PushL(getMulticallParams);
	phone.GetMulticallParams(getMulticallParams, multiCallSettingsPkg);
 	ERR_PRINTF1(_L("<font color=Orange>$CTSYProblem: RMobilePhone::GetMulticallParams always returns KErrNotSupported because there is no implementation for it. Check changed from KErrNone to KErrNotSupported.</font>"));
 	// $CTSYProblem:RMobilePhone::GetMulticallParams always returns KErrNotSupported
 	// because there is no implementation for it. Checking changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMulticallParams, ETimeLong), KErrNone, _L("RMobilePhone::GetMulticallParams timed out"));
	ASSERT_EQUALS(getMulticallParams.Int(), KErrNotSupported, _L("RMobilePhone::GetMulticallParams returned an error"));

	// Post notification for RMobilePhone::NotifyMulticallParamsChange
	RMobilePhone::TMobilePhoneMulticallSettingsV1 notifiedMultiCallSettings;
	TPckg<RMobilePhone::TMobilePhoneMulticallSettingsV1> notifiedMultiCallSettingsPkg(notifiedMultiCallSettings);
	TExtEtelRequestStatus notifyMulticallParamsChangeStatus(phone, EMobilePhoneNotifyMulticallParamsChange);
	CleanupStack::PushL(notifyMulticallParamsChangeStatus);
	phone.NotifyMulticallParamsChange(notifyMulticallParamsChangeStatus, notifiedMultiCallSettingsPkg);

	// Set the multicall parameters to something valid. 
	TInt userMaxBearers = 3;
	TExtEtelRequestStatus setMulticallParams(phone, EMobilePhoneSetMulticallParams);
	CleanupStack::PushL(setMulticallParams);
	phone.SetMulticallParams(setMulticallParams, userMaxBearers);
 	ERR_PRINTF1(_L("<font color=Orange>$CTSYProblem: RMobilePhone::SetMulticallParams always returns KErrNotSupported because there is no implementation for it. Check changed from KErrNone to KErrNotSupported.</font>"));
 	// $CTSYProblem:RMobilePhone::SetMulticallParams always returns KErrNotSupported
 	// because there is no implementation for it. Checking changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMulticallParams, ETimeLong), KErrNone, _L("RMobilePhone::SetMulticallParams timed out"));
	ASSERT_EQUALS(setMulticallParams.Int(), KErrNotSupported, _L("RMobilePhone::SetMulticallParams returned an error"));

	// Verify notification completes. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMulticallParamsChangeStatus, ETimeLong), KErrNone, _L("RMobilePhone::NotifyMulticallParamsChange timed out."));
 	ERR_PRINTF1(_L("<font color=Orange>$CTSYProblem: RMobilePhone::NotifyMulticallParamsChange always returns KErrNotSupported because there is no implementation for it. Check changed from KErrNone to KErrNotSupported.</font>"));
 	// $CTSYProblem:RMobilePhone::NotifyMulticallParamsChange always returns KErrNotSupported
 	// because there is no implementation for it. Checking changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(notifyMulticallParamsChangeStatus.Int(), KErrNotSupported, _L("RMobilePhone::NotifyMulticallParamsChange returned with error status."));

	// 1. Dial voice call (call 1). 
	TPtrC voiceNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, voiceNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(voiceCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	DisplayUserInteractionPromptL(_L("Board now dials to VoiceNumber1 (press any key to start dialling) "), ETimeLong);
	voiceCall1.Dial(dialStatus, voiceNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Hold call 1. 
	TExtEtelRequestStatus holdStatus(voiceCall1, EMobileCallHold);
	CleanupStack::PushL(holdStatus);
	voiceCall1.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium),KErrNone, _L("RMobileCall::Hold timed out"));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error"));

	// Dial data call (call 2). 
	RMobileCall::TMobileHscsdCallParamsV1 dataparams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg dataparamsPckg(dataparams);
	dataparams.iIdRestrict = RMobileCall::ESendMyId; 
	dataparams.iAckTimer = 0;
	dataparams.iMSToModemWindowSize = 0;
	dataparams.iModemToMSWindowSize = 0;	
	dataparams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;
	dataparams.iQoS = RMobileCall::EQoSTransparent;
	dataparams.iRLPVersion = RMobileCall::ERLPNotRequested;
	dataparams.iResequencingPeriod = 0;
	dataparams.iRetransmissionAttempts = 0;
	dataparams.iService = RMobileCall::EServiceDataCircuitSync;
	dataparams.iSpeed = RMobileCall::ESpeed64000;
	dataparams.iUseEdge = EFalse;
	dataparams.iV42bisCodewordsNum = 0;
	dataparams.iV42bisMaxStringLength = 0;
	dataparams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataparams.iAsymmetry = RMobileCall::EAsymmetryNoPreference; 
	dataparams.iCodings = 0; 
	dataparams.iUserInitUpgrade = EFalse; 

	TPtrC dataNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DisplayUserInteractionPromptL(_L("Board now dials to DataNumber1 (press any key to start dialling) "), ETimeLong);
	dataCall1.Dial(dialStatus, dataparamsPckg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Start downloading a file e.g. via HTTP. 
	TBuf<140> host(_L("developer.symbian.org"));
	TBuf<140> page(_L("/wiki/images/1/12/Common_Design_Patterns_for_Symbian_OS_Sample_Chapter.pdf"));
    CHTTPDownload* download = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download);
    ASSERT_TRUE(download->StartDownloadL(host,page), _L("Download Failed - perhaps page has moved!"));
    
	// Verify file is downloading. 
	ASSERT_TRUE(download->VerifyDownloading(), _L("CHTTPDownload::VerifyDownloading did not return ETrue as expected. File is not downloaded."));

	// Hang up call 1. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (voiceCall1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	voiceCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Resume call 2. 
	TExtEtelRequestStatus resumeStatus(dataCall1, EMobileCallResume);
	CleanupStack::PushL(resumeStatus);
	dataCall1.Resume(resumeStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeStatus, ETimeMedium),KErrNone, _L("RMobileCall::Resume timed out"));
	ASSERT_EQUALS(resumeStatus.Int(), KErrNone, _L("RMobileCall::Resume returned with an error"));

	// Verify call 2 is still active.
	RMobileCall::TMobileCallStatus getMobileCallStatus;
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with an error"));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus returned wrong status"));

	// Hang up call 2.
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Release calls
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KVoiceLine, KCall1);
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KDataLine, KCall1);
	

	// Get call 1. 
	voiceCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// 2. Dial voice call (call 1). 
	DisplayUserInteractionPromptL(_L("Board now dials to VoiceNumber1 (press any key to start dialling) "), ETimeLong);
	voiceCall1.Dial(dialStatus, voiceNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Hold call 1. 
	voiceCall1.Hold(holdStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdStatus, ETimeMedium),KErrNone, _L("RMobileCall::Hold timed out"));
	ASSERT_EQUALS(holdStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error"));

	// Post notification for incoming call
	TName name;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (dataLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	dataLine.NotifyIncomingCall(notifyIncomingCallStatus, name);

	// Request an incoming HSCSD call with RPS. 
	ASSERT_EQUALS(RPSRequestIncomingCallL(EDataLine), KErrNone, _L("RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes with valid call name.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"));
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"));
	ASSERT_TRUE(name.Length() > 0, _L("RLine::NotifyIncomingCall did not return a valid call name"));
	
	// Get the incoming call
	TCallId incomingCallId;
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// Answer incoming data call. 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	dataCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Start downloading a file e.g. via FTP. 
    ASSERT_TRUE(download->StartDownloadL(host,page),_L("CHTTPDownload::StartDownloadL failed"));

	// Verify file is downloading. 
	ASSERT_TRUE(download->VerifyDownloading(), _L("CHTTPDownload::VerifyDownloading did not return ETrue as expected. File is not downloaded."));

	// Hang up call 1. 
	voiceCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Resume call 2. 
	dataCall1.Resume(resumeStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeStatus, ETimeMedium),KErrNone, _L("RMobileCall::Resume timed out"));
	ASSERT_EQUALS(resumeStatus.Int(), KErrNone, _L("RMobileCall::Resume returned with an error"));

	// Verify call 2 is still active. 
	ASSERT_EQUALS(dataCall1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with an error."));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus returned wrong status"));

	// Hang up call 2. 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Release calls
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KVoiceLine, KCall1);
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KDataLine, incomingCallId);
	
	// 3. Get data call 1.
	dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	// Dial data call (call 1). 
	dataCall1.Dial(dialStatus, dataparamsPckg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Start downloading a file e.g. via FTP. 
    ASSERT_TRUE(download->StartDownloadL(host,page),_L("CHTTPDownload::StartDownloadL failed"));

	// Verify file is downloading. 
	ASSERT_TRUE(download->VerifyDownloading(), _L("CHTTPDownload::VerifyDownloading did not return ETrue as expected. File is not downloaded."));

	// Hold call 1. 
	TExtEtelRequestStatus holdDataCallStatus(dataCall1, EMobileCallHold);
	CleanupStack::PushL(holdDataCallStatus);
	dataCall1.Hold(holdDataCallStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdDataCallStatus, ETimeMedium),KErrNone, _L("RMobileCall::Hold timed out"));
	ASSERT_EQUALS(holdDataCallStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error"));

	// Get voice call.
	voiceCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	// Dial voice call (call 2). 
	DisplayUserInteractionPromptL(_L("Board now dials to VoiceNumber1 (press any key to start dialling) "), ETimeLong);
	voiceCall1.Dial(dialStatus, voiceNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Hang up call 1. 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Resume call 2. 
	voiceCall1.Resume(resumeStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeStatus, ETimeMedium),KErrNone, _L("RMobileCall::Resume timed out"));
	ASSERT_EQUALS(resumeStatus.Int(), KErrNone, _L("RMobileCall::Resume returned with an error"));

	// Verify call 2 still active. 
	ASSERT_EQUALS(voiceCall1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with an error."));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus returned wrong status"));

	// Hang up call 2. 
	voiceCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Release calls
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KVoiceLine, KCall1);
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KDataLine, KCall1);

	// Get data call 1.
	dataCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KDataLine, KCall1);

	// 4. Dial data call (call 1). 
	dataCall1.Dial(dialStatus, dataparamsPckg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));

	// Start downloading a file e.g. via FTP. 
    ASSERT_TRUE(download->StartDownloadL(host,page),_L("CHTTPDownload::StartDownloadL failed"));

	// Verify file is downloading. 
	ASSERT_TRUE(download->VerifyDownloading(), _L("CHTTPDownload::VerifyDownloading did not return ETrue as expected. File is not downloaded."));

	// Hold call 1. 
	dataCall1.Hold(holdDataCallStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(holdDataCallStatus, ETimeMedium),KErrNone, _L("RMobileCall::Hold timed out"));
	ASSERT_EQUALS(holdDataCallStatus.Int(), KErrNone, _L("RMobileCall::Hold returned with an error"));

	// Post notify RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingVoiceCallStatus (voiceLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingVoiceCallStatus);
	TName incomingCallName;
	voiceLine.NotifyIncomingCall(notifyIncomingVoiceCallStatus, incomingCallName);

	// Request incoming call from RPS. 
	ASSERT_EQUALS (RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingVoiceCallStatus, ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
	ASSERT_EQUALS(notifyIncomingVoiceCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned an error"));
	ASSERT_TRUE(incomingCallName.Length() > 0, _L("RLine::NotifyIncomingCall did not set a line name"));

    // Get the incoming call
	TCallId incomingVoiceCallId;
	voiceCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, incomingCallName, incomingVoiceCallId);

	// Answer incoming voice call (call 2). 
	TCoreEtelRequestStatus<RCall> answerIncomingVoiceCallStatus(voiceCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingVoiceCallStatus);	
	voiceCall1.AnswerIncomingCall(answerIncomingVoiceCallStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingVoiceCallStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingVoiceCallStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Hang up call 1. 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Resume call 2. 
	voiceCall1.Resume(resumeStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(resumeStatus, ETimeMedium),KErrNone, _L("RMobileCall::Resume timed out"));
	ASSERT_EQUALS(resumeStatus.Int(), KErrNone, _L("RMobileCall::Resume returned with an error"));
	
	// Verify call 2 still active. 
	ASSERT_EQUALS(voiceCall1.GetMobileCallStatus(getMobileCallStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus returned with an error."));
	ASSERT_EQUALS(getMobileCallStatus, RMobileCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus returned wrong status"));

	// Hang up call 2.
	voiceCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// Release calls
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KVoiceLine, incomingVoiceCallId);
	iEtelSessionMgr.ReleaseCall(KMainServer, KMainPhone, KDataLine, KCall1);


	//
	// TEST END
	//

    StartCleanup();
	
	// Pop
	// getMulticallParams
	// notifyMulticallParamsChangeStatus
	// setMulticallParams
	// dialStatus
	// holdStatus
	// download
	// hangUpStatus
	// resumeStatus
	// notifyIncomingCallStatus
	// answerIncomingStatus
	// holdDataCallStatus
	// notifyIncomingVoiceCallStatus
	// answerIncomingVoiceCallStatus
	CleanupStack::PopAndDestroy(13, &getMulticallParams);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultiBearerInteroperability0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultiBearerInteroperability0004");
	}



CCTSYIntegrationTestMultiBearerInteroperability0005::CCTSYIntegrationTestMultiBearerInteroperability0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestMultiBearerInteroperabilityBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestMultiBearerInteroperability0005::GetTestStepName());
	}

CCTSYIntegrationTestMultiBearerInteroperability0005::~CCTSYIntegrationTestMultiBearerInteroperability0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestMultiBearerInteroperability0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-MBI-0005
 * @SYMFssID BA/CTSY/MBI-0005
 * @SYMTestCaseDesc CS data call & PS-data call
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RPacketContext::GetDataVolumeTransferred, RPacketContext::NotifyStatusChange, RPacketContext::NotifyDataTransferred, RCall::HangUp, RCall::GetStatus
 * @SYMTestExpectedResults Pass - Simulatneous CS and PS data call can be made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
/*

	
	// Ensure phone is camped on a valid cell. 
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(phone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	// Ensure phone is attached to packet service. 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
    SetAttachModeToWhenPossibleAndWaitForAttachL(packetService);

	// Get data line 1. 
	RMobileLine& line1= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KDataLine);
	
	// Get call 1.
	RMobileCall& dataCall1= iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KDataLine, KCall1);


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Get the multicall parameters.
	RMobilePhone::TMobilePhoneMulticallSettingsV1 multiCallSettings;
	TPckg<RMobilePhone::TMobilePhoneMulticallSettingsV1> multiCallSettingsPkg(multiCallSettings);
	TExtEtelRequestStatus getMulticallParams(phone, EMobilePhoneGetMulticallParams);
	CleanupStack::PushL(getMulticallParams);
	phone.GetMulticallParams(getMulticallParams, multiCallSettingsPkg);
 	// @CTSYProblem:RMobilePhone::GetMulticallParams always returns KErrNotSupported
 	// because there is no implementation for it. Check changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMulticallParams, ETimeLong), KErrNone, _L("RMobilePhone::GetMulticallParams timed out"));
	ASSERT_EQUALS(getMulticallParams.Int(), KErrNotSupported, _L("RMobilePhone::GetMulticallParams returned an error")); 
	
	// Post notification for RMobilePhone::NotifyMulticallParamsChange
	RMobilePhone::TMobilePhoneMulticallSettingsV1 notifiedMultiCallSettings;
	TPckg<RMobilePhone::TMobilePhoneMulticallSettingsV1> notifiedMultiCallSettingsPkg(notifiedMultiCallSettings);
	TExtEtelRequestStatus notifyMulticallParamsChangeStatus(phone, EMobilePhoneNotifyMulticallParamsChange);
	CleanupStack::PushL(notifyMulticallParamsChangeStatus);
	phone.NotifyMulticallParamsChange(notifyMulticallParamsChangeStatus, notifiedMultiCallSettingsPkg);

	// Set the multicall parameters to something valid. 
	TInt userMaxBearers = 3;
	TExtEtelRequestStatus setMulticallParams(phone, EMobilePhoneSetMulticallParams);
	CleanupStack::PushL(setMulticallParams);
	phone.SetMulticallParams(getMulticallParams, userMaxBearers);
 	// @CTSYProblem:RMobilePhone::SetMulticallParams always returns KErrNotSupported
 	// because there is no implementation for it. Check changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMulticallParams, ETimeLong), KErrNone, _L("RMobilePhone::SetMulticallParams timed out"));
	ASSERT_EQUALS(setMulticallParams.Int(), KErrNotSupported, _L("RMobilePhone::SetMulticallParams returned an error"));

	// Verify notification completes. 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyMulticallParamsChangeStatus, ETimeLong), KErrNone, _L("RMobilePhone::NotifyMulticallParamsChange timed out."));
 	// @CTSYProblem:RMobilePhone::NotifyMulticallParamsChange always returns KErrNotSupported
 	// because there is no implementation for it. Check changed from KErrNone to KErrNotSupported
	ASSERT_EQUALS(notifyMulticallParamsChangeStatus.Int(), KErrNotSupported, _L("RMobilePhone::NotifyMulticallParamsChange returned with error status."));

	// 1. Dial a CS data call (call 1).
	RMobileCall::TMobileHscsdCallParamsV1 dataparams;
	RMobileCall::TMobileHscsdCallParamsV1Pckg dataparamsPckg(dataparams);
	dataparams.iIdRestrict = RMobileCall::ESendMyId; 
	dataparams.iAckTimer = 0;
	dataparams.iMSToModemWindowSize = 0;
	dataparams.iModemToMSWindowSize = 0;	
	dataparams.iProtocol = RMobileCall::EProtocolIsdnMultimedia;
	dataparams.iQoS = RMobileCall::EQoSTransparent;
	dataparams.iRLPVersion = RMobileCall::ERLPNotRequested;
	dataparams.iResequencingPeriod = 0;
	dataparams.iRetransmissionAttempts = 0;
	dataparams.iService = RMobileCall::EServiceDataCircuitSync;
	dataparams.iSpeed = RMobileCall::ESpeed64000;
	dataparams.iUseEdge = EFalse;
	dataparams.iV42bisCodewordsNum = 0;
	dataparams.iV42bisMaxStringLength = 0;
	dataparams.iV42bisReq = RMobileCall::EV42bisNeitherDirection;
	dataparams.iAsymmetry = RMobileCall::EAsymmetryNoPreference; 
	dataparams.iCodings = 0; 
	dataparams.iUserInitUpgrade = EFalse; 
	
	TPtrC dataNumber;
	TCoreEtelRequestStatus<RCall> dialStatus(dataCall1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	ASSERT_TRUE(GetStringFromConfig(KIniDataNumSection, KIniDataNumber1, dataNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DisplayUserInteractionPromptL(_L("Board now dials to DataNumber1 (press any key to start dialling) "), ETimeLong);
	dataCall1.Dial(dialStatus, dataparamsPckg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));
	
	// Start downloading a file on call 1.
	const TInt KPort = 80;
	TBuf<140> host(_L("developer.symbian.com"));
    TBuf<140> page(_L("/main/downloads/papers/IMS_Introduction_Part_1.pdf"));
    CHTTPDownload *download_CS = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download_CS);
    
    
   // ASSERT_TRUE(download_CS->StartDownloadL(host, page, KPort ,EFalse, ENifCSD), _L("Download Failed"));
   ASSERT_TRUE(download_CS->StartDownloadL(host, page, KPort), _L("Download Failed"));
   
	// Verify file is downloading. 
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	

	// Open a primary context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(
											KMainServer,
											KMainPhone,
											KMainPacketService,
										KPrimaryPacketContext1);
	// Activate a context. 										
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, packetContext);

	// Start downloading a file via packet service connection. 
	TBuf<140> page2(_L("/main/downloads/papers/Leaves%20and%20Exceptions.pdf"));
    CHTTPDownload *download_PS = new (ELeave) CHTTPDownload(this);
    CleanupStack::PushL(download_PS);
	//ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort, EFalse, ENifPSD),_L("Download Failed"));
	ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort),_L("Download Failed"));


	// Verify both files are still downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	
	
	// Stop download
	download_CS->StopDownload();
	
	// Hang up call 1. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (dataCall1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	
	// Verify file via packet service connection is still downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	
	// Stop download
	download_PS->StopDownload();
	
	//Deactivate context
	TExtEtelRequestStatus contextDeactivateStatus(packetContext, EPacketContextDeactivate);
	CleanupStack::PushL(contextDeactivateStatus);
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));


	// 2. Activate a context. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, packetContext);

	// Start downloading a file. 
	//ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort, EFalse, ENifPSD),_L("Download Failed"));
	ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort),_L("Download Failed"));


	// Verify file is downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));

	// Dial a data call. 
	dataCall1.Dial(dialStatus, dataparamsPckg, dataNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,  _L("RCall::Dial returned with an error"));
	
	// Start downloading a file via data call connection. 
    //ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort,EFalse, ENifCSD), _L("Download Failed"));
	ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort), _L("Download Failed"));


	// Verify both files are downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	

	// Stop download
	download_PS->StopDownload();
	
	// Deactivate context. 
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
					
	// Verify data call is still active and file is downloading. 
	RCall::TStatus callStatus;
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus), KErrNone, _L("RCall::GetStatus returned an Error"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RCall::GetStatus did not return EStatusConnected"));
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));

	// Hang up 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));	
	

	// 3. Activate a context. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, packetContext);

	// Start downloading a file. 
	//ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort, EFalse, ENifPSD),_L("Download Failed"));
	ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort),_L("Download Failed"));

	// Verify file is downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	
	// Post notification for incoming call from RPS
	TName name;
	TPtrC number;
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus (line1,&RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	line1.NotifyIncomingCall(notifyIncomingCallStatus,name);
	
	// Request incoming data call from RPS 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes with a call name with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"))
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"))
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"))
	
    //Get the incoming call
	TCallId incomingCallId;
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);
	
	// Answer an incoming data call. 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(dataCall1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	dataCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Start downloading a file via data call connection. 
    //ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort,EFalse, ENifCSD), _L("Download Failed"));
    ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort), _L("Download Failed"));

	// Verify both files are downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	

	// Deactivate context. 
	packetContext.Deactivate(contextDeactivateStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(contextDeactivateStatus, ETimeLong), KErrNone, 
					_L("RPacketContext::Deactivate timed out."));					
	ASSERT_EQUALS(contextDeactivateStatus.Int(), KErrNone,
					_L("RPacketContext::Deactivate returned with error status."));
					
	// Verify data call is still active and file is downloading. 
	ASSERT_EQUALS(dataCall1.GetStatus(callStatus), KErrNone, _L("RMobileCall::GetMobileCallStatus return code is not KErrNone"));
	ASSERT_EQUALS(callStatus, RCall::EStatusConnected, _L("RMobileCall::GetMobileCallStatus returned wrong status"));
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	

	// Hang up 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));

	// 4. Activate a context. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(phone, packetContext);

	// Start downloading a file. 
	//ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort, EFalse, ENifPSD),_L("Download Failed"));
	ASSERT_TRUE(download_PS->StartDownloadL(host,page2, KPort),_L("Download Failed"));
	
	// Verify file is downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));

	// Post notification for incoming call from RPS
	line1.NotifyIncomingCall(notifyIncomingCallStatus,name);

	// Request incoming data call from RPS 
	ASSERT_EQUALS (RPSRequestIncomingCallL( EDataLine ), KErrNone, _L("RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes with a call name with length > 0
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrNone,_L("RLine::NotifyIncomingCall timed out"))
	ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L("RLine::NotifyIncomingCall returned with an error"))
	ASSERT_TRUE(name.Length()>0, _L("RLine::NotifyIncomingCall did not return a call name"))
	
    //Get the incoming call
	dataCall1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KDataLine, name, incomingCallId);

	// Answer an incoming data call. 
	dataCall1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// Start downloading a file via data call connection. 
    //ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort,EFalse, ENifCSD), _L("Download Failed"));
    ASSERT_TRUE(download_CS->StartDownloadL(host,page,KPort), _L("Download Failed"));

	// Verify both files are downloading. 
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));
	ASSERT_TRUE(download_CS->VerifyDownloading(), _L("File is not downloading via CS data connection"));	

	// Hang up data call. 
	dataCall1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeLong), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::HangUp returned an error"));
	
	// Verify context is still active and file is downloading. 
	RPacketContext::TContextStatus contextStatus;
	ASSERT_EQUALS(packetContext.GetStatus(contextStatus), KErrNone, _L("RPacketContext::GetStatus returned an error"));
	ASSERT_EQUALS(contextStatus, RPacketContext::EStatusActive, _L("RPacketContext::GetStatus did not return EStatusActive"));
	ASSERT_TRUE(download_PS->VerifyDownloading(), _L("File is not downloading via PS data connection"));


	//
	// TEST END
	//

    StartCleanup();
	
	//Pop
	//getMulticallParams
	//notifyMulticallParamsChangeStatus
	//setMulticallParams
	//dialStatus
	//download_CS
	//download_PS
	//hangUpStatus
	//contextDeactivateStatus
	//notifyIncomingCallStatus
	//answerIncomingStatus
	
	CleanupStack::PopAndDestroy(10, &getMulticallParams);

	*/
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestMultiBearerInteroperability0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestMultiBearerInteroperability0005");
	}



