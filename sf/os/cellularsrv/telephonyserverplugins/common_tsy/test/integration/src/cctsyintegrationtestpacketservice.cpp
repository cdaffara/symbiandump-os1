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
// Test step definitions for the PacketService functional unit.
// 
//

/**
 @internalTechnology
*/

#include <etelpckt.h>
#include <pcktcs.h>

#include "cctsyintegrationtestpacketservice.h"

#include "cetelsessionmgr.h"
#include "tetelrequeststatus.h"
#include "tsmsstoretsytesthelper.h"

_LIT(KPrompt, "This test requires network simulation,Press a key if simualted network is ready");
CCTSYIntegrationTestPacketServiceBase::CCTSYIntegrationTestPacketServiceBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iPacketServiceTestHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPacketServiceBase::~CCTSYIntegrationTestPacketServiceBase()
/*
 * Destructor
 */
	{
	}
	
/**
 * Ensures that the correct attach mode is set and leaves if not
 @param aPacketService RPacketService service
 @param aModeAttach give information about the desired mode. The function leaves if the required mode is not there.
 */
 void CCTSYIntegrationTestPacketServiceBase::EnsureAttachModeL( RPacketService &aPacketService, RPacketService::TAttachMode &aModeAttach)
 	{

 	// Get attach mode to check whether it is already RPacketService::EAttachWhenNeeded		
	TExtEtelRequestStatus getAttachModeStatus (aPacketService,EPacketGetAttachMode);
	CleanupStack::PushL(getAttachModeStatus);	
	RPacketService::TAttachMode attachMode;
	aPacketService.GetAttachMode(getAttachModeStatus, attachMode);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetAttachMode timed out"));
	CHECK_EQUALS_L(getAttachModeStatus.Int(), KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));

	// If the attach mode is not RPacketService::EAttachWhenNeeded, then set it
	TExtEtelRequestStatus setAttachModeStatus(aPacketService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);	
	if (attachMode != aModeAttach)	
		{
		// Set attach mode to RPacketService::EAttachWhenNeeded with RPacketService::SetAttachMode
		aPacketService.SetAttachMode(setAttachModeStatus, aModeAttach);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,	
						_L("PacketService::SetAttachMode timed out."));								
		CHECK_EQUALS_L(setAttachModeStatus.Int(), KErrNone, 
						_L("PacketService::SetAttachMode returned an error"));		
		
		// Check that it was set
		aPacketService.GetAttachMode(getAttachModeStatus,attachMode);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
						_L("PacketService::GetAttachMode timed out"));
		CHECK_EQUALS_L(getAttachModeStatus.Int(), KErrNone, 
						_L("PacketService::GetAttachMode returned an error"));			
		}		
	
	CHECK_EQUALS_L(attachMode, aModeAttach, _L("PacketService::GetAttachMode did not return the desired attach mode"));
	
	// getAttachModeStatus
	// setAttachModeStatus
	CleanupStack::PopAndDestroy(2, &getAttachModeStatus);
 	}
 //*************************************************************************	



CCTSYIntegrationTestPacketService0001::CCTSYIntegrationTestPacketService0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0001::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0001::~CCTSYIntegrationTestPacketService0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0001
 * @SYMFssID BA/CTSY/PKTS-0001
 * @SYMTestCaseDesc Get static caps.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Static caps returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Get static caps for each protocol type.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get packet service 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, 
																	  KMainPhone,
																	  KMainPacketService);
	
	// Check static caps are 
	// RPacketService::KCapsSuspendSupported |
	// RPacketService::KCapsNetworkAvailabilitySupported | 
	// RPacketService::KCapsSetDefaultContextSupported | RPacketService::KCapsChangeAttachModeSupported | 
	// RPacketService::KCapsGetDataTransferredSupported | RPacketService::KCapsPreferredBearerSupported | 
	// RPacketService::KCapsPdpDataCompSupported | RPacketService::KCapsPdpHeaderCompSupported | 
	// RPacketService::KCapsNotifyMSClassSupported
	TUint staticCaps = 0;
	TUint expectedCaps = 	RPacketService::KCapsSuspendSupported |
							RPacketService::KCapsNetworkAvailabilitySupported | 
							RPacketService::KCapsSetDefaultContextSupported | 
							RPacketService::KCapsChangeAttachModeSupported | 
							RPacketService::KCapsGetDataTransferredSupported | 
							RPacketService::KCapsPreferredBearerSupported | 
							RPacketService::KCapsPdpDataCompSupported | 
							RPacketService::KCapsPdpHeaderCompSupported | 
							RPacketService::KCapsNotifyMSClassSupported;
	ASSERT_EQUALS(packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4), KErrNone, _L("RPacketService::GetStaticCaps returned an error"));
	ASSERT_BITS_SET(staticCaps, expectedCaps, KNoUnwantedBits,
			_L("RPacketService::GetStaticCaps did not return correct static caps"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0001");
	}



CCTSYIntegrationTestPacketService0002::CCTSYIntegrationTestPacketService0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0002::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0002::~CCTSYIntegrationTestPacketService0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0002
 * @SYMFssID BA/CTSY/PKTS-0002
 * @SYMTestCaseDesc Automatic attach to the packet service when attach mode changed to EAttachWhenPossible.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyAttachModeChange, RPacketService::GetNtwkRegStatus, RPacketService::GetStatus, RPacketService::GetAttachMode, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyStatusChange, RPacketService::GetStaticCaps
 * @SYMTestExpectedResults Pass - Phone automatically attached to packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify that packet service attaches automatically when attach mode is changed to EAttachWhenPossible.
 * Verify static caps, packet network registration status,  dynamic caps are correct. 
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone,
					_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Ensure packet service status is EStatusUnattached
	RPacketService::TStatus setUpStatus;
	CHECK_EQUALS_L(packetService.GetStatus(setUpStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	TExtEtelRequestStatus detachStatus(packetService, EPacketDetach);
	CleanupStack::PushL(detachStatus);
	if (setUpStatus != RPacketService::EStatusUnattached)
		{
		packetService.Detach(detachStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone, 	
						_L("PacketService::Detach timed out"));
		CHECK_TRUE_L(detachStatus.Int()== KErrNone|| detachStatus.Int()== KErrAlreadyExists, 
						_L("PacketService::Detach returned an error"));										
		}
	User::After(5*KOneSecond); // Time delay is needed to detach properly
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached
	RPacketService::TStatus serviceStatus; 
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("RPacketService::GetStatus did not return RPacketService::EStatusUnattached."));
		
	// Check RPacketService::GetDynamicCaps returns caps in set of 
	// RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of 
	// RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | 
	// RPacketService::KCapsRxContextActivationReq
	RPacketService::TDynamicCapsFlags dynCaps;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("RPacketService::GetDynamicCaps returned with an error"));
	RPacketService::TDynamicCapsFlags expectedCaps = 
					RPacketService::KCapsManualAttach | 
					RPacketService::KCapsRxCSCall;
	RPacketService::TDynamicCapsFlags unwantedCaps = 
					RPacketService::KCapsSMSTransfer | 
					RPacketService::KCapsManualDetach | 
					RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,
		_L("RPacketService::GetDynamicCaps did not return correct dynamic caps")); // NOTE: Use ASSERT_BITS_SET for wanted and unwanted bits, name the variables expectedCaps and unwantedCaps or similar

	// Post notification for RPacketService::NotifyAttachModeChange   
	TExtEtelRequestStatus notifyAttachModeChangeStatus(packetService, EPacketNotifyAttachModeChange); // NOTE: Name the request status object so that it is possible to identify which API it is for since they cannot be used for a different API
	CleanupStack::PushL(notifyAttachModeChangeStatus); // NOTE: Always push EVERY TExtEtelRequestStatus and TCoreEtelRequestStatus onto the CleanupStack
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
	
	// Post notification for RPacketService::NotifyStatusChange
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post notification for RPacketService::NotifyDynamicCapsChange
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
		
	// Set attach mode to RPacketService::EAttachWhenPossible with RPacketService::SetAttachMode
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus); 
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone, 
					_L("RPacketService::SetAttachMode timed-out")); 
	ASSERT_EQUALS(setAttachModeStatus.Int(), KErrNone, 
					_L("RPacketService::SetAttachMode returned with an error"));
	User::After(10*KOneSecond); // This time delay is required to complete the implicit attach operation caused by EAttachWhenPossible attach mode
	
	// Check RPacketService::NotifyAttachModeChange completes with RPacketService::EAttachWhenPossible  
	RPacketService::TAttachMode expectedAttachMode = RPacketService::EAttachWhenPossible;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService, 
																		notifyAttachModeChangeStatus, 
																		attachMode, 
																		expectedAttachMode, 
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenPossible
	TExtEtelRequestStatus getAttachModeStatus(packetService, EPacketGetAttachMode);
	CleanupStack::PushL(getAttachModeStatus);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeShort), KErrNone,
					_L("RPacketService::GetAttachMode timed-out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(), KErrNone, 
					_L("RPacketService::GetAttachMode returned with an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("RPacketService::GetAttachMode did not return RPacketService::EAttachWhenPossible"));
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
	expectedCaps = 	RPacketService::KCapsRxCSCall | 
					RPacketService::KCapsRxContextActivationReq | 
					RPacketService::KCapsManualDetach | 
					RPacketService::KCapsSMSTransfer;
	unwantedCaps =  RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
																		 notifyDynamicCapsChangeStatus,
																		 dynCaps, 
																		 expectedCaps,
																		 unwantedCaps,
																		 KErrNone);

	// Check RPacketService::GetStatus returns RPacketService::EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("RPacketService::GetStatus returned with an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("RPacketService::GetStatus did not return RPacketService::EStatusAttached"));

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::GetDynamicCaps completes with caps NOT in set of RPacketService::KCapsManualAttach
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone,
    				_L("RPacketService::GetDynamicCaps returned with an error"));
    expectedCaps = 	RPacketService::KCapsRxCSCall | 
    				RPacketService::KCapsRxContextActivationReq | 
    				RPacketService::KCapsManualDetach | 
    				RPacketService::KCapsSMSTransfer;
    unwantedCaps = 	RPacketService::KCapsManualAttach;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
    		_L("RPacketService::GetDynamicCaps did not return correct dynamic caps"));

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork
    TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
    CleanupStack::PushL(regStatus);
    RPacketService::TRegistrationStatus registrationStatus;
    packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium), KErrNone,
					_L("RPacketService::GetNtwkRegStatus timed-out."));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
					_L("RPacketService::GetNtwkRegStatus returned with an error"));
    ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
    				_L("RPacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork"));

    //
    // Test end
    //
    		
    StartCleanup();
	
    // Pop:
	// detachStatus
	// notifyAttachModeChangeStatus
    // notifyStatusStatus
	// notifyDynamicCapsChangeStatus
	// setAttachModeStatus
	// getAttachModeStatus
	// regStatus
	CleanupStack::PopAndDestroy(7, &detachStatus); 
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0002");
	}



CCTSYIntegrationTestPacketService0003::CCTSYIntegrationTestPacketService0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0003::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0003::~CCTSYIntegrationTestPacketService0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0003
 * @SYMFssID BA/CTSY/PKTS-0003
 * @SYMTestCaseDesc Automatic attach to the packet service, when Context Activated and attach mode is EAttachWhenNeeded.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetNtwkRegStatus, RPacketService::GetStatus, RPacketService::GetAttachMode, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyStatusChange, RPacketService::NotifyContextAdded, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Packet service is active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify that automatic attach occurs when a packet context is activated.

Verify packet network registration status,  dynamic caps, number of active contexts, context status are correct.
 *
 * @return - TVerdict code
 */
	{
	
	
	// Attach to the packet service with attach mode set to EAttachWhenNeeded by Activating a context
	//
	// SET UP
	//
	
	// Start with attach mode in RPacketService::EAttachWhenNeeded.
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Get packet context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone,
					_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Ensure packet service status is EStatusUnattached
	RPacketService::TStatus setUpStatus;
	CHECK_EQUALS_L(packetService.GetStatus(setUpStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	TExtEtelRequestStatus detachStatus(packetService, EPacketDetach);
	CleanupStack::PushL(detachStatus);
	if (setUpStatus != RPacketService::EStatusUnattached)
		{
		packetService.Detach(detachStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone, 	
						_L("PacketService::Detach timed out"));
		CHECK_TRUE_L(detachStatus.Int()== KErrNone|| detachStatus.Int()== KErrAlreadyExists, 
						_L("PacketService::Detach returned an error"));										
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached."));
				
	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
    RPacketService::TDynamicCapsFlags dynCaps = 0;		
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	//$CTSYProblem - Caps returned is KCapsActive, caps not in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall 
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsActivate;
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer | 
													 RPacketService::KCapsManualDetach | 
													 RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,          
					_L("PacketService::GetDynamicCaps Wrong dynamic caps"));

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post notification for dynamic caps change
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
		
	// Activate a new context
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
		
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached->EStausActive
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus, 
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Check RPacketService::NotifyStatusChange completes with EStausActive
	expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus, 
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);

	//  Check RPacketService::NotifyDynamicCapsChange completes with caps in set of  KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer
	//	Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
	expectedCaps = RPacketService::KCapsRxCSCall | 
				   RPacketService::KCapsRxContextActivationReq | 
				   RPacketService::KCapsManualDetach | 
				   RPacketService::KCapsSMSTransfer;
	unwantedCaps = RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
																		 notifyDynamicCapsChangeStatus,
																		 dynCaps,
																		 expectedCaps,
																		 unwantedCaps,
																		 KErrNone);

	// Check RPacketService::GetStatus returns RPacketService::EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusActive, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusActive."));	
	
	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
	    			_L("PacketService::GetNtwkRegStatus Wrong registration status"));

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
					_L("PacketService::GetDynamicCaps Wrong dynamic caps"));
    
	// Check RPacketService::EnumerateContexts returns aCount, 1 and aMaxAllowed, 11 (KMmMaxNumberOfContexts in CTSY)	
	TInt count = 0;
    TInt maxAllowed = 0;
	TExtEtelRequestStatus enumContextStatus(packetService, EPacketEnumerateContexts);
    CleanupStack::PushL(enumContextStatus);
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, COUNT_ONE, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
	ASSERT_EQUALS(maxAllowed, MAX_CONTEXTS, 
				_L("PacketService::EnumerateContexts, wrong max count returned"));

	// deactivate the context and detach from packet service
	TExtEtelRequestStatus deactivateStatus(packetService, EPacketContextDeactivate);
	CleanupStack::PushL(deactivateStatus);
	packetContext.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium),KErrNone, 
					_L("PacketContext::Deactivate timed out"));	
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("PacketContext::Deactivate returned an error"));		
	
	//
    // Test end
    //
    		
    StartCleanup();
		
    // Pop:
   	// detachStatus
	// notifyStausStaus
	// notifyDynamicCapsChangeStatus
	// regStatus
	// enumContextStatus
	// deactivateStatus
	CleanupStack::PopAndDestroy(6, &detachStatus); 
	return TestStepResult();
	}
	
TPtrC CCTSYIntegrationTestPacketService0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0003");
	}



CCTSYIntegrationTestPacketService0004::CCTSYIntegrationTestPacketService0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0004::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0004::~CCTSYIntegrationTestPacketService0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0004
 * @SYMFssID BA/CTSY/PKTS-0004
 * @SYMTestCaseDesc Automatic attach and detach to the packet service.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyAttachModeChange, RPacketService::GetNtwkRegStatus, RPacketService::GetStatus, RPacketService::GetAttachMode, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyStatusChange
 * @SYMTestExpectedResults Pass - Phone automatically attaches to then detaches from packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify that automatic attach occurs when attach mode is changed to RPacketService::EAttachWhenPossible and automatic detach occurs when attach mode is change to RPacketService::EAttachWhenNeeded. 

Verify packet network registration status and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	CHECK_EQUALS_L(ret, KErrNone, 
					_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	
	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	
	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,	
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached."));
				
	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	RPacketService::TDynamicCapsFlags dynCaps;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsManualAttach | 
													 RPacketService::KCapsRxCSCall;
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer | 
													 RPacketService::KCapsManualDetach | 
													 RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
					_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));
	
	// Post notification for attach mode change
	TExtEtelRequestStatus notifyAttachModeChangeStatus(packetService, EPacketNotifyAttachModeChange);
	CleanupStack::PushL(notifyAttachModeChangeStatus);
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post notification for dynamic caps change
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
		
	// Set attach mode to RPacketService::EAttachWhenPossible with RPacketService::SetAttachMode
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);	
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone, 
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));
		
	// Check RPacketService::NotifyAttachModeChange completes with RPacketService::EAttachWhenPossible
	RPacketService::TAttachMode expectedAttachMode = RPacketService::EAttachWhenPossible;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenPossible
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);
	CleanupStack::PushL(getAttachModeStatus);	
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("PacketService::GetAttachMode Wrong attach mode"));

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
			
	// Check RPacketService::NotifyDynamicCapsChange completes with KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
	expectedCaps = 	RPacketService::KCapsRxCSCall | 
					RPacketService::KCapsRxContextActivationReq | 
					RPacketService::KCapsManualDetach | 
					RPacketService::KCapsSMSTransfer;
	unwantedCaps =  RPacketService::KCapsManualAttach;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
																		 notifyDynamicCapsChangeStatus,
																		 dynCaps,
																		 expectedCaps,
																		 unwantedCaps,
																		 KErrNone);

	// Check RPacketService::GetStatus returns RPacketService::EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,	
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));
	
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::GetDynamicCaps completes with caps NOT in set of RPacketService::KCapsManualAttach
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
    expectedCaps = 	RPacketService::KCapsRxCSCall | 
    				RPacketService::KCapsRxContextActivationReq | 
    				RPacketService::KCapsManualDetach | 
    				RPacketService::KCapsSMSTransfer;
    unwantedCaps = 	RPacketService::KCapsManualAttach;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
					_L("PacketService::GetDynamicCaps Wrong dynamic caps"));
    
	// Check RPacketService::GetNtwkRegStatus returns ENotRegisteredOnHomeNetwork
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork,
					_L("PacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork registration status"));
	// $CTSYProblem: After EStatusUnattached staus, ENotRegisteredButAvailable registration status of network was not achieved as expected.
	// This problem occurs when once you register with the network then it remains registered even when we expect not registered.
	
	// Post notification for RPacketService::NotifyAttachModeChange 
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);

	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Post notification for dynamic caps change
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
	    		
	// Set attach mode to RPacketService::EAttachWhenNeeded with RPacketService::SetAttachMode
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetAttachMode timed out."));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));
		
	// Check RPacketService::NotifyAttachModeChange completes with RPacketService::EAttachWhenNeeded
	expectedAttachMode = RPacketService::EAttachWhenNeeded;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error."));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached
	expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);	
			
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	expectedCaps = 	RPacketService::KCapsManualAttach | 
					RPacketService::KCapsRxCSCall;
	unwantedCaps =  RPacketService::KCapsSMSTransfer | 
				    RPacketService::KCapsManualDetach | 
				    RPacketService::KCapsRxContextActivationReq;
   	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
																		 notifyDynamicCapsChangeStatus,
																		 dynCaps,
																		 expectedCaps,
																		 unwantedCaps,
																		 KErrNone);

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,	
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));
	
	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));	
    expectedCaps = 	RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall;
    unwantedCaps = 	RPacketService::KCapsSMSTransfer | 
    			    RPacketService::KCapsManualDetach | 
    			    RPacketService::KCapsRxContextActivationReq;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, _L("PacketService::GetDynamicCaps returned wrong dynamic caps"));
    
    DEBUG_PRINTF1(_L("<font color=Orange>$CTSYProblem: After EStatusUnattached staus, ENotRegisteredButAvailable registration status of network was not achieved\n as expected. This problem occurs when once you register with the network then it remains registered even when we expect not registered.</font> "));
	// Check RPacketService::GetNtwkRegStatus returns ENotRegisteredOnHomeNetwork
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeLong),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
	    			_L("PacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork registration status"));
	
	//
	// TEST END
	//

	StartCleanup();
	
	// Pop:
	// notifyAttachModeChangeStatus
    // notifyStatusStatus, 
    // notifyDynamicCapsChangeStatus
	// setAttachModeStatus
	// getAttachModeStatus
	// regStatus
	CleanupStack::PopAndDestroy(6,&notifyAttachModeChangeStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0004");
	}



CCTSYIntegrationTestPacketService0005::CCTSYIntegrationTestPacketService0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0005::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0005::~CCTSYIntegrationTestPacketService0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0005
 * @SYMFssID BA/CTSY/PKTS-0005
 * @SYMTestCaseDesc Attach to the packet service manually.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetNtwkRegStatus, RPacketService::GetStatus, RPacketService::GetAttachMode, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyStatusChange
 * @SYMTestExpectedResults Pass - Phone attached to packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet service can be manually attached and phone is registered on packet network when attach mode is EAttachWhenNeeded.

Verify packet network registration status, attach mode, dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{
	
	//
	// SET UP
	//

	
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded	
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post notification for dynamic caps change
	RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);	
	CleanupStack::PushL(getAttachModeStatus);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetAttachMode timed out."));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));
	
	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached."));
				
	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ENotRegisteredButAvailable
    TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
    RPacketService::TRegistrationStatus registrationStatus;
    packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeLong), KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out."));
    ASSERT_EQUALS(regStatus.Int(),KErrNone, 
					_L("PacketService::GetNtwkRegStatus returned an error"));
    ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
    				_L("PacketService::GetNtwkRegStatus did not return RPacketService::ENotRegisteredButAvailable"));
    DEBUG_PRINTF1(_L("<font color=Orange>$CTSYProblem: After EStatusUnattached staus, ENotRegisteredButAvailable registration status of network was not achieved\n as expected. This problem occurs when once you register with the network then it remains registered even when we expect not registered.</font> "));	
    
	// Registered with the network
	TInt ret = iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone);
	ASSERT_EQUALS(ret, KErrNone, 
					_L("RMobilePhone::NotifyNetworkRegistrationStatusChange network registration failed"));
			
	// Check RPacketService::GetDynamicCaps returns caps RPacketService::KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps = 
					RPacketService::KCapsManualAttach | 
					RPacketService::KCapsRxCSCall;
	RPacketService::TDynamicCapsFlags unwantedCaps = 
					RPacketService::KCapsSMSTransfer | 
					RPacketService::KCapsManualDetach | 
					RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
				_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	// Attach to packet network
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,
					_L("PacketService::Attach timed out."));
	ASSERT_EQUALS(attachStatus.Int(), KErrNone, 
					_L("PacketService::Attach returned an error"));
 
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached.
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);	
	
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach
	expectedCaps = 	RPacketService::KCapsRxCSCall |
	 				RPacketService::KCapsRxContextActivationReq |
	 				RPacketService::KCapsManualDetach |
	 				RPacketService::KCapsSMSTransfer;
	unwantedCaps = RPacketService::KCapsManualAttach;
   	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
   																		 notifyDynamicCapsChangeStatus,
   																		 dynCaps,
   																		 expectedCaps,
   																		 unwantedCaps,
   																		 KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusAttached.
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached."));
				
	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork.
    packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetNtwkRegStatus timed out."));
    ASSERT_EQUALS(regStatus.Int(),KErrNone, 
					_L("PacketService::GetNtwkRegStatus returned an error"));
    ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork,	
					_L("PacketService::GetNtwkRegStatus returned wrong registration status"));
    			
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
    expectedCaps = 	RPacketService::KCapsRxCSCall | 
    				RPacketService::KCapsRxContextActivationReq | 
    				RPacketService::KCapsManualDetach | 
    				RPacketService::KCapsSMSTransfer;
    unwantedCaps = 	RPacketService::KCapsManualAttach;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
				_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));
 
    //
    // Test end
    //
    		
    StartCleanup();

    // Pop:
    // notifyStatusStatus
    // notifyDynamicCapsChangeStatus
    // getAttachModeStatus
    // regStatus
    // attachStatus
    CleanupStack::PopAndDestroy(5, &notifyStatusStatus); 
	
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0005");
	}



CCTSYIntegrationTestPacketService0006::CCTSYIntegrationTestPacketService0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0006::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0006::~CCTSYIntegrationTestPacketService0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0006
 * @SYMFssID BA/CTSY/PKTS-0006
 * @SYMTestCaseDesc Attach to and detach from the packet service manually (roaming).
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetNtwkRegStatus, RPacketService::GetStatus, RPacketService::GetAttachMode, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyStatusChange, RPacketService::Detach
 * @SYMTestExpectedResults Pass - Phone attached to packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify packet service is attached to and detached from and phone is registered on packet network.

Verify packet network registration status, attach mode, dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get packet service. 

	// Check RPacketService::GetAttachMode returns PacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ERegisteredRoaming.

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	// Attach to packet network 

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached.

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsManualAttach

	// Check RPacketService::GetStatus returns RPacketService::EStatusAttached.

	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ERegisteredRoaming.

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsManualDetach | KCapsSMSTransfer.
	// Check RPacketService::GetDynamicCaps completes with caps NOT in set of RPacketService::KCapsManualAttach

	// Detach from packet network. 

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached.

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredRoaming.

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0006");
	}



CCTSYIntegrationTestPacketService0007::CCTSYIntegrationTestPacketService0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0007::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0007::~CCTSYIntegrationTestPacketService0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0007
 * @SYMFssID BA/CTSY/PKTS-0007
 * @SYMTestCaseDesc Attach to packet service failure.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::GetDynamicCaps, RPacketService::SetAttachMode, RPacketService::GetAttachMode, RPacketService::GetNtwkRegStatus
 * @SYMTestExpectedResults Pass - Attach fails with KErrPacketNetworkFailure
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify attach attempt fails when attach rejected by network with unknown IMSI.
Verify packet network registration status, attach mode, dynamic caps are correctly showing detached state.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10087);
	User::Leave(KErrNone);	

	// Ensure attach mode is RPacketService::EAttachWhenNeeded	
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
		
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(5*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);	
	CleanupStack::PushL(getAttachModeStatus);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeShort), KErrNone,
					_L("RPacketService::GetAttachMode timed-out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(), KErrNone, 
					_L("RPacketService::GetAttachMode returned with an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("RPacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));
	
	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached."));
	
	// Check RPacketService::GetNtwkRegStatus returns one of ENotRegisteredButAvailable | ENotRegisteredNotSearching | ENotRegisteredSearching | ERegistrationDenied
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_TRUE((registrationStatus== RPacketService::ENotRegisteredButAvailable)|| (registrationStatus== RPacketService::ENotRegisteredNotSearching)
				|| (registrationStatus== RPacketService::ENotRegisteredSearching)|| (registrationStatus== RPacketService::ERegistrationDenied),
					_L("PacketService::GetNtwkRegStatus did not return ENotRegisteredButAvailable registration status"));

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	RPacketService::TDynamicCapsFlags dynCaps = 0;	
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsManualAttach | 
			    									 RPacketService::KCapsRxCSCall;
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer |
													 RPacketService::KCapsManualDetach |
													 RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
			_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	// Check RPacketService::Attach returns KErrPacketNetworkFailure 
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,
					_L("PacketService::Attach timed out."));
//	ASSERT_EQUALS(attachStatus.Int(), RPacketService::KErrPacketNetworkFailure,
//					_L("PacketService::Attach did not return  KErrPacketNetworkFailure"));


	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeShort), KErrNone,
					_L("RPacketService::GetAttachMode timed-out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(), KErrNone, 
					_L("RPacketService::GetAttachMode returned with an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("RPacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));
	
	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached."));

	// Check RPacketService::GetNtwkRegStatus returns one of ENotRegisteredButAvailable | ENotRegisteredNotSearching | ENotRegisteredSearching | ERegistrationDenied.
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_TRUE((registrationStatus== RPacketService::ENotRegisteredButAvailable)|| (registrationStatus== RPacketService::ENotRegisteredNotSearching)
				|| (registrationStatus== RPacketService::ENotRegisteredSearching)|| (registrationStatus== RPacketService::ERegistrationDenied),
					_L("PacketService::GetNtwkRegStatus did not return ENotRegisteredButAvailable registration status"));

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	expectedCaps = RPacketService::KCapsManualAttach | 
				   RPacketService::KCapsRxCSCall;
	unwantedCaps = RPacketService::KCapsSMSTransfer |
	               RPacketService::KCapsManualDetach |
				   RPacketService::KCapsRxContextActivationReq;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
					_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// getAttachModeStatus
	// regStatus
	// attachStatus
	CleanupStack::PopAndDestroy(3, &getAttachModeStatus);
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0007");
	}



CCTSYIntegrationTestPacketService0008::CCTSYIntegrationTestPacketService0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0008::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0008::~CCTSYIntegrationTestPacketService0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0008
 * @SYMFssID BA/CTSY/PKTS-0008
 * @SYMTestCaseDesc Attach to packet service failure.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::GetDynamicCaps, RPacketService::SetAttachMode, RPacketService::GetAttachMode, RPacketService::GetNtwkRegStatus
 * @SYMTestExpectedResults Pass - Attach fails with KErrPacketNetworkFailure
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify attach attempt fails and packet service is detached when packet network roaming not available in cell.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get packet service. 

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ENotRegisteredButAvailable.

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	// Check RPacketService::Attach returns KErrPacketNetworkFailure 

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ENotRegisteredButAvailable.

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0008");
	}



CCTSYIntegrationTestPacketService0009::CCTSYIntegrationTestPacketService0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0009::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0009::~CCTSYIntegrationTestPacketService0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0009
 * @SYMFssID BA/CTSY/PKTS-0009
 * @SYMTestCaseDesc Attach to packet service failure.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::GetDynamicCaps, RPacketService::SetAttachMode, RPacketService::GetAttachMode, RPacketService::GetNtwkRegStatus
 * @SYMTestExpectedResults Pass - Attach fails with KErrPacketNetworkFailure
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify attach attempt fails and packet service is detached when attach rejected by network because packet services not allowed error returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get packet service. 

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns one of RPacketService::ENotRegisteredButAvailable | RPacketService::ERegistrationDenied.

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	// Check RPacketService::Attach returns KErrPacketNetworkFailure 

	// Check RPacketService::GetAttachMode returns RPacketService::EAttachWhenNeeded.

	// Check RPacketService::GetStatus returns RPacketService::EStatusUnattached.

	// Check RPacketService::GetNtwkRegStatus returns RPacketService::ENotRegisteredButAvailable | RPacketService::ERegistrationDenied.

	// Check RPacketService::GetDynamicCaps returns caps KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0009");
	}



CCTSYIntegrationTestPacketService0010::CCTSYIntegrationTestPacketService0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0010::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0010::~CCTSYIntegrationTestPacketService0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0010
 * @SYMFssID BA/CTSY/PKTS-0010
 * @SYMTestCaseDesc Attach to packet service when already attached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::Attach, RPacketService::GetStatus, RPacketService::NotifyStatusChange
 * @SYMTestExpectedResults Pass - Attach fails with KErrAlreadyExists
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify attempt to re-attach fails but device remains attached.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Set attach mode to RPacketService::EAttachWhenPossible
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
				
	// Post request RPacketService::NotifyStatusChange
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
		
	// Attach to the packet service. 
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	
	// Check KErrAlreadyExists returned
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,	
					_L("PacketService::Attach timed out."));	
	ASSERT_EQUALS(attachStatus.Int(), KErrAlreadyExists, 
					_L("PacketService::Attach returned wrong error code."));
			
	// Check RPacketService::GetStatus returns RPacketService::EStatusAttached
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not retun RPacketService::EStatusAttached."));	

	// Check RPacketService::NotifyStatusChange is still KRequestPending	
	ASSERT_EQUALS(notifyStatusStatus.Int(), KRequestPending,
					_L("PacketService::NotifyStatusChange has changed state unexpectedly."));	
    //
    // Test end
    //
    		
    StartCleanup();
		
    // Pop:
    // notifyStatusStatus  
    // setAttachModeStatus
	// attachStatus
    CleanupStack::PopAndDestroy(3, &notifyStatusStatus); 
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0010");
	}



CCTSYIntegrationTestPacketService0011::CCTSYIntegrationTestPacketService0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0011::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0011::~CCTSYIntegrationTestPacketService0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0011
 * @SYMFssID BA/CTSY/PKTS-0011
 * @SYMTestCaseDesc Attach to packet service when active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::Attach, RPacketService::GetStatus, RPacketContext::GetStatus, RPacketService::EnumerateContexts
 * @SYMTestExpectedResults Pass - Attach fails with KErrAlreadyExists
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify attach fails. Verify contexts not disrupted
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get packet service 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Set attach mode to RPacketService::EAttachWhenPossible 
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));

	// Wait for 10 seconds. 
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
			
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Open a new context.
	// Set valid context config 
	// Activate context 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusActive
	expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::EnumerateContexts returns 1
	TInt count = 0;
	TInt maxAllowed = 0;
    TExtEtelRequestStatus enumContextStatus(packetService, EPacketEnumerateContexts);
    CleanupStack::PushL(enumContextStatus);
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, COUNT_ONE, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
		
	// Check RPacketContext::GetStatus returns EStatusActive
	RPacketContext::TContextStatus packetContextStatus;
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// Post request for RPacketService::NotifyStatusChange 
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Attach to the packet service again.  
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	
	// Check KErrAlredyExists returned
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,	
					_L("PacketService::Attach timed out."));	
	ASSERT_EQUALS(attachStatus.Int(), KErrAlreadyExists, 
					_L("PacketService::Attach returned wrong error code."));	

	// Check RPacketService::GetStatus returns RPacketService::EStatusActive
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned with an error."));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusActive, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusActive."));	

	// Check RPacketService::EnumerateContexts returns 1
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, COUNT_ONE, 
					_L("PacketService::EnumerateContexts returned wrong count"));

	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// Check RPacketService::NotifyStatusChange is still KRequestPending
	ASSERT_EQUALS(notifyStatusStatus.Int(), KRequestPending,
					_L("PacketService::NotifyStatusChange did not return KRequestPending."));	
	
	//
	// TEST END
	//

    StartCleanup();
	
    // deactivate the context and detach from packet service
	TExtEtelRequestStatus deactivateStatus(packetService, EPacketContextDeactivate);
	CleanupStack::PushL(deactivateStatus);
	packetContext.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium), KErrNone, 
					_L("PacketContext::Deactivate timed out"));
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("PacketContext::Deactivate returned an error"));
    
	// Pop:
	// notifyStatusStatus
	// setAttachModeStatus
	// enumContextStatus
	// attachStatus
	// deactivateStatus
	CleanupStack::PopAndDestroy(5, &notifyStatusStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0011");
	}



CCTSYIntegrationTestPacketService0012::CCTSYIntegrationTestPacketService0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0012::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0012::~CCTSYIntegrationTestPacketService0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0012
 * @SYMFssID BA/CTSY/PKTS-0012
 * @SYMTestCaseDesc Attach to packet service when suspended.
 * @SYMTestPriority High
 * @SYMTestActions RPacketContext::SetConfig, RPacketContext::Activate, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RCall::Dial, RPacketService::SetMSClass, RPacketService::GetMSClass, RPacketService::NotifyMSClassChange
 * @SYMTestExpectedResults Pass - Attach fails with KErrAlreadyExists
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify attach fails.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get packet service 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, 
																	  KMainPhone,
																	  KMainPacketService);
	
	// Open a new context. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RPacketService::GetStaticCaps returns in set of caps KCapsMSClassSupported 
	// For IPv4 type gateway (PdpType) we wish to receive static capabilities. 
	TUint staticCaps = 0;
	TUint expectedCaps = 	RPacketService::KCapsMSClassSupported;
	ASSERT_EQUALS(packetService.GetStaticCaps(staticCaps, RPacketContext::EPdpTypeIPv4), KErrNone,
					_L("RPacketService::GetStaticCaps returned an error"));
	ASSERT_BITS_SET(staticCaps, expectedCaps, KNoUnwantedBits,
					_L("RPacketService::GetStaticCaps did not return correct static caps"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Get Voice line 1 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Set attach mode to RPacketService::EAttachWhenPossible 
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								

	// Wait for 10 seconds. 
	User::After(10*KOneSecond);// This time delay is required to complete the implicit attach operation caused by EAttachWhenPossible attach mode
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);

	// Set the MS class to EMSClassSuspensionRequired 
	RPacketService::TMSClass newClass=RPacketService::EMSClassSuspensionRequired;
	TExtEtelRequestStatus setMSClassStatus(packetService, EPacketSetMSClass);
	CleanupStack::PushL(setMSClassStatus);
	packetService.SetMSClass(setMSClassStatus,newClass);     
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetMSClass timed out."));        
	ASSERT_EQUALS(setMSClassStatus.Int(), KErrNone, 
					_L("PacketService::SetMSClassStatus returned an error"));
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10050);
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentionally as CTSY currently does not\n support the SetMSClass operation so it is not possible to attach to packet network "));		
				
	// Check RPacketService::GetMSClass returns aCurrentClass, EMSClassSuspensionRequired and aMaxClass, EMSClassDualMode
	TExtEtelRequestStatus getMSClassStatus(packetService, EPacketGetMSClass);
	CleanupStack::PushL(getMSClassStatus);
	RPacketService::TMSClass msClass;
	RPacketService::TMSClass msMax;
	packetService.GetMSClass(getMSClassStatus, msClass, msMax);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone, 
	            	_L("PacketService::GetMSClass timedout"));
	ASSERT_EQUALS(getMSClassStatus.Int(), KErrNone, 
	            	_L("packetService::GetMSClass returned an error"));     
	ASSERT_EQUALS(msClass, RPacketService::EMSClassSuspensionRequired, 
					_L("RPacketService::GetMSClass did not return EMSClassSuspensionRequired as aCurrentClass"));
	ASSERT_EQUALS(msMax, RPacketService::EMSClassDualMode, 
					_L("RPacketService::GetMSClass did not return EMSClassDualMode as aMaxClass"));
	
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Activate context 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusActive
	expectedPacketServiceStatus = RPacketService::EStatusActive;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Dial a number that answers 
	TPtrC number; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,			  
			_L("CTestStep::GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, number); 
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
					_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone,
					_L("RCall::Dial returned with an error"));
	
	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusSuspended
	expectedPacketServiceStatus = RPacketService::EStatusSuspended;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Attach to the packet service again.  
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	
	// Check KErrAlredyExists returned
	ASSERT_EQUALS(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,	
					_L("PacketService::Attach timed out."));	
	ASSERT_EQUALS(attachStatus.Int(), KErrAlreadyExists, 
					_L("PacketService::Attach returned wrong error code."));	

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// notifyStatusStatus
	// setAttachModeStatus
	// setMSClassStatus
	// getMSClassStatus
	// dialStatus
	// attachStatus
	CleanupStack::PopAndDestroy(6, &notifyStatusStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0012");
	}



CCTSYIntegrationTestPacketService0013::CCTSYIntegrationTestPacketService0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0013::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0013::~CCTSYIntegrationTestPacketService0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0013
 * @SYMFssID BA/CTSY/PKTS-0013
 * @SYMTestCaseDesc Detach from packet service when network initiated detach received when attached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyChangeOfNtwkRegStatus, RPacketService::GetNtwkRegStatus, RPacketService::NotifyReleaseModeChange, RPacketService::GetCurrentReleaseMode
 * @SYMTestExpectedResults Pass - Phone detached from packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify that automatic detach occurs when network request to detach received and there are no contexts active.

Verify packet network registration status, release mode and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	//
	
	// Get Packet Service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer,
																	  KMainPhone,
																	  KMainPacketService);

	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone,
					_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); //This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Attach to packet service 
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone,
					_L("PacketService::Attach timed out."));
	
	if(attachStatus.Int() !=  KErrAlreadyExists)
		{
		CHECK_EQUALS_L(attachStatus.Int(), KErrNone, _L("PacketService::Attach returned an error"));
		}
 
	// Ensure RPacketService::GetStatus returns EStatusAttached 
	RPacketService::TStatus currentStatus;
	CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusAttached, _L("SET UP should be attached at this point"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post a notification for RPacketService::NotifyChangeOfNtwkRegStatus
	TExtEtelRequestStatus changeOfNtwkRegStatusSt(packetService, EPacketNotifyChangeOfNtwkRegStatus);
	CleanupStack::PushL(changeOfNtwkRegStatusSt); 
	RPacketService::TRegistrationStatus ntwkRegStatus;
	packetService.NotifyChangeOfNtwkRegStatus(changeOfNtwkRegStatusSt, ntwkRegStatus);
			
	// Post notification for dynamic caps change
	RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);

	// Simulate network initiated detach 
	DisplayUserInteractionPromptL(KPrompt); 

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached.
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::NotifyChangeOfNtwkRegStatus completes with RPacketService::ENotRegisteredButAvailable.
	RPacketService::TRegistrationStatus expectedRegistrationStatus= RPacketService::ENotRegisteredButAvailable;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyChangeOfNtwkRegStatus(packetService,
																			 changeOfNtwkRegStatusSt,
																			 ntwkRegStatus,
																			 expectedRegistrationStatus,
																			 KErrNone);	
				
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::NotifyDynamicCapsChange completes with cap NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsManualAttach | 
													 RPacketService::KCapsRxCSCall;
	
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer | 
													 RPacketService::KCapsManualDetach | 
													 RPacketService::KCapsRxContextActivationReq  ;
   	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
   																		 notifyDynamicCapsChangeStatus,
   																		 dynCaps,
   																		 expectedCaps,
   																		 unwantedCaps,
   																		 KErrNone);

	// Check RPacketService::GetStatus returns EStatusUnattached.
   	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus did not return EStatusUnattached "));

	// Check RPacketService::GetNtwkRegStatus returns ENotRegisteredButAvailable.
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, expectedRegistrationStatus,
					_L("PacketService::GetNtwkRegStatus did not return ENotRegisteredButAvailable registration status"));

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
					_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// attachStatus
	// notifyStatusStatus
	// changeOfNtwkRegStatusSt
	// notifyDynamicCapsChangeStatus
	// regStatus
	CleanupStack::PopAndDestroy(5, &attachStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0013");
	}



CCTSYIntegrationTestPacketService0014::CCTSYIntegrationTestPacketService0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0014::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0014::~CCTSYIntegrationTestPacketService0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0014
 * @SYMFssID BA/CTSY/PKTS-0014
 * @SYMTestCaseDesc Detach from packet service when network initiated detach received when active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetDynamicCaps, RPacketService::NotifyChangeOfNtwkRegStatus, RPacketService::GetNtwkRegStatus, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::NotifyReleaseModeChange, RPacketService::GetCurrentReleaseMode
 * @SYMTestExpectedResults Pass - Phone detached from packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify that automatic detach occurs when network request to detach received and there are active contexts.

Verify packet network registration status, number of contexts, context status, release mode and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYProblem: defect id = %d</font>"), 10088);
	User::Leave(KErrNone);	
	

	// Ensure attach mode is RPacketService::EAttachWhenNeeded 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone,
					_L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(5*KOneSecond); //This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Attach to packet service 
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone, _L("PacketService::Attach timed out."));
	if(attachStatus.Int() != KErrAlreadyExists)
		{
		CHECK_EQUALS_L(attachStatus.Int(), KErrNone, _L("PacketService::Attach returned an error"));
		}
 
	// Activate a context with a valid config 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
   	
	// Ensure RPacketService::GetStatus returns EStatusActive 
    RPacketService::TStatus currentStatus;
    CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post a notification for RPacketService::NotifyChangeOfNtwkRegStatus
	TExtEtelRequestStatus changeOfNtwkRegStatusSt(packetService, EPacketNotifyChangeOfNtwkRegStatus);
	CleanupStack::PushL(changeOfNtwkRegStatusSt); 
	RPacketService::TRegistrationStatus ntwkRegStatus;
	packetService.NotifyChangeOfNtwkRegStatus(changeOfNtwkRegStatusSt, ntwkRegStatus);
			
	// Post notification for dynamic caps change
	RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);

	// Simulate network initiated detach 
	DisplayUserInteractionPromptL(KPrompt); 

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached.
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::NotifyChangeOfNtwkRegStatus completes with RPacketService::ENotRegisteredButAvailable.
	RPacketService::TRegistrationStatus expectedRegistrationStatus= RPacketService::ENotRegisteredButAvailable;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyChangeOfNtwkRegStatus(packetService,
																			 changeOfNtwkRegStatusSt,
																			 ntwkRegStatus,
																			 expectedRegistrationStatus,
																			 KErrNone);	
	
	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsManualAttach | 
													 RPacketService::KCapsRxCSCall;
	
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer | 
													 RPacketService::KCapsManualDetach | 
													 RPacketService::KCapsRxContextActivationReq  ;
   	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
   																		 notifyDynamicCapsChangeStatus,
   																		 dynCaps,
   																		 expectedCaps,
   																		 unwantedCaps,
   																		 KErrNone);

  	// Check RPacketService::GetStatus returns EStatusUnattached.
   	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus did not return EStatusUnattached "));

	// Check RPacketService::GetNtwkRegStatus returns ENotRegisteredButAvailable.
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, expectedRegistrationStatus,
					_L("PacketService::GetNtwkRegStatus did not return ENotRegisteredButAvailable registration status"));

	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsManualAttach | KCapsRxCSCall.
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
			_L("PacketService::GetDynamicCaps returned an error"));
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
			_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

   	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// attachStatus
	// notifyStatusStatus
	// changeOfNtwkRegStatusSt
	// notifyDynamicCapsChangeStatus
	// regStatus
	CleanupStack::PopAndDestroy(5, &attachStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0014");
	}



CCTSYIntegrationTestPacketService0015::CCTSYIntegrationTestPacketService0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0015::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0015::~CCTSYIntegrationTestPacketService0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0015
 * @SYMFssID BA/CTSY/PKTS-0015
 * @SYMTestCaseDesc Detach from packet service when network initiated detach received when active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::EnumerateNifs, RPacketService::EnumerateContextsInNif, RPacketService::GetContextNameInNif, RPacketService::GetNifInfo, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Phone detached from packet service and contexts deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify NIF related info correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure attach mode is RPacketService::EAttachWhenNeeded 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Post notification to get a ContextName
	TName contextName;
	TExtEtelRequestStatus notifyContextAddedStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus);	
	packetService.NotifyContextAdded(notifyContextAddedStatus,contextName);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Wait to complete the NotifyContextAdded
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeShort), KErrNone,
					_L("PacketService;:NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(), KErrNone, 
					_L("PacketService::NotifyContextAdded returned an error"));
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Attach to packet service 
	TExtEtelRequestStatus attachStatus(packetService, EPacketAttach);
	CleanupStack::PushL(attachStatus);
	packetService.Attach(attachStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(attachStatus, ETimeLong), KErrNone, _L("PacketService::Attach timed out."));
	CHECK_EQUALS_L(attachStatus.Int(), KErrNone, _L("PacketService::Attach returned an error"));
 
	// Activate a context with a valid config 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
   	
	// Ensure RPacketService::GetStatus returns EStatusActive
    RPacketService::TStatus currentStatus;
    CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Post notification for context's status change
	TExtEtelRequestStatus notifyContextStChStatus(packetContext, EPacketContextNotifyStatusChange);
	CleanupStack::PushL(notifyContextStChStatus);
	RPacketContext::TContextStatus packetContextStatus;
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus);
	
	// Simulate network initiated detach  
	DisplayUserInteractionPromptL(KPrompt); // 30 seconds

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached.
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::GetStatus returns EStatusUnattached
   	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus did not return EStatusUnattached "));

	// Check RPacketContext::NotifyStatusChange completes with EStatusDeactivating -> EStatusInactive
	RPacketContext::TContextStatus expectedPacketContextStatus = RPacketContext::EStatusDeactivating;
	iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(packetContext,
																	notifyContextStChStatus,
																	packetContextStatus, 
																	expectedPacketContextStatus,
																	KErrNone);
 
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusActivating, _L("Packet Context's status expected to be EStatusActivating but is not."));
	packetContext.NotifyStatusChange(notifyContextStChStatus, packetContextStatus); 

    expectedPacketContextStatus = RPacketContext::EStatusInactive;
    iPacketServiceTestHelper.WaitForPacketContextNotifyStatusChange(packetContext,
    																notifyContextStChStatus,
    																packetContextStatus, 
    																expectedPacketContextStatus,
    																KErrNone);

	// Check RPacketContext::GetStatus returns EStatusInactive
    ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, _L("RPacketContext::GetStatus returned with an error."));
    ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, _L("RPacketContext::GetStatus returns incorrect status."));
  
	// Check RPacketService::EnumerateNifs returns 0
	TInt numNif=0;
	TExtEtelRequestStatus enumerateNifsSt(packetService,EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsSt);
	packetService.EnumerateNifs(enumerateNifsSt,numNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateNifs timed out."));   
	ASSERT_EQUALS(enumerateNifsSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateNifs returned an error"));
	ASSERT_EQUALS(numNif,0, 
					_L("RPacketService::EnumerateNifs did not return count as 0"));
	
	// Check RPacketService::EnumerateContextsInNif returns 0 when aExistingContextName, name of context activated in set up
	TInt numContexts=0;
	TExtEtelRequestStatus enumerateContextsInNifSt(packetService,EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifSt);
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt, contextName, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt.Int(), KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts, 0, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 0"));

	// Check RPacketService::GetContextNameInNif returns KErrArgument
	TName nameInNif;
	TExtEtelRequestStatus getContextNameInNifSt(packetService,EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifSt);
	packetService.GetContextNameInNif(getContextNameInNifSt, contextName, 0, nameInNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt.Int(),KErrArgument, 
					_L("RPacketService::GetContextNameInNif did not return KErrArgument"));
	
	// Check RPacketService::GetNifInfo returns KErrArgument
	RPacketService::TNifInfoV2 nifInfo;
	TPckg<RPacketService::TNifInfoV2> nifInfoPk(nifInfo);
	TExtEtelRequestStatus getNifInfoSt(packetService,EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoSt);
	packetService.GetNifInfo(getNifInfoSt, 0, nifInfoPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo timed out."));   
	ASSERT_EQUALS(getNifInfoSt.Int(),KErrArgument, 
					_L("RPacketService::GetNifInfo did not return KErrArgument"));
			
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // notifyContextAddedStatus
    // attachStatus
    // notifyStatusStatus
    // notifyContextStChStatus
    // enumerateNifsSt
    // enumerateContextsInNifSt
    // getContextNameInNifSt
    // getNifInfoSt
    CleanupStack::PopAndDestroy(8, &notifyContextAddedStatus);
    
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0015");
	}



CCTSYIntegrationTestPacketService0016::CCTSYIntegrationTestPacketService0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0016::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0016::~CCTSYIntegrationTestPacketService0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0016
 * @SYMFssID BA/CTSY/PKTS-0016
 * @SYMTestCaseDesc Detach from packet service manually when attached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetNtwkRegStatus, RPacketService::GetDynamicCaps
 * @SYMTestExpectedResults Pass - Phone detached from packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet service detach succeeds when there are no contexts active.

Verify packet network registration status and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{
    
	//
	// SET UP
	//

	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
    // Attach to packet service
    TExtEtelRequestStatus attachStatus (packetService,EPacketAttach);	
    CleanupStack::PushL(attachStatus);
    packetService.Attach(attachStatus);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(attachStatus, ETimeVeryLong), KErrNone, 
					_L("PacketService::Attach timed out"));
   	CHECK_EQUALS_L(attachStatus.Int(), KErrNone, 
					_L("PacketService::Attach returned an error"));
   	
	// Ensure RPacketService::GetStatus returns EStatusAttached
    RPacketService::TStatus currentStatus;
	CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusAttached, _L("SET UP should be attached at this point"));
	
	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus (packetService,EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus);
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Post notification for Dynamic Caps Change
	RPacketService::TDynamicCapsFlags dynCaps;
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
	
	// Detach from packet service
	TExtEtelRequestStatus detachStatus(packetService,EPacketDetach);
	CleanupStack::PushL(detachStatus);
	packetService.Detach(detachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone,
					_L("PacketService::Detach timed out."));   
	ASSERT_EQUALS(detachStatus.Int(),KErrNone, 
					_L("RPacketService::Detach returned an error"));

	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus did not return EStatusUnattached"));

	// Check RPacketService::NotifyStatusChange completes with RPacketService::EStatusUnattached.   
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	currentStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	 
    // Check RPacketService::NotifyDynamicCapsChange completes with caps in set of KCapsManualAttach | KCapsRxCSCall.
    // Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq     	
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsManualAttach | 
													 RPacketService::KCapsRxCSCall;
	
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsSMSTransfer | 
													 RPacketService::KCapsManualDetach | 
													 RPacketService::KCapsRxContextActivationReq  ;
   	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService,
   																		 notifyDynamicCapsChangeStatus,
   																		 dynCaps,
   																		 expectedCaps,
   																		 unwantedCaps,
   																		 KErrNone);

    // Check RPacketService::GetStatus returns EStatusUnattached.
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
				_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, expectedPacketServiceStatus, 
				_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));
       
    // Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork.
    TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
    CleanupStack::PushL(regStatus);
    RPacketService::TRegistrationStatus registrationStatus;
    packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetNtwkRegStatus timed out."));    
    ASSERT_EQUALS(regStatus.Int(),KErrNone, 
					_L("RPacketService::GetNtwkRegStatus returned an error"));
    ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
					_L("RPacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork"));
      
    // Check RPacketService::GetDynamicCaps returns caps in set of KCapsManualAttach | KCapsRxCSCall.
    // Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach | RPacketService::KCapsRxContextActivationReq	
    ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
    expectedCaps = 	RPacketService::KCapsManualAttach | 
        		    RPacketService::KCapsRxCSCall;
    unwantedCaps =  RPacketService::KCapsSMSTransfer |
    				RPacketService::KCapsManualDetach |
    				RPacketService::KCapsRxContextActivationReq;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps, 
				_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));
 
	//
	// TEST END
	//

	StartCleanup();
    
    // Pop:
    // attachStatus
    // notifyStatusStatus
    // notifyDynamicCapsChangeStatus
    // detachStatus
    // regStatus
    
	CleanupStack::PopAndDestroy(5, &attachStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0016");
	}



CCTSYIntegrationTestPacketService0017::CCTSYIntegrationTestPacketService0017(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0017::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0017::~CCTSYIntegrationTestPacketService0017()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0017::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0017
 * @SYMFssID BA/CTSY/PKTS-0017
 * @SYMTestCaseDesc Detach from packet service when active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetDynamicCaps, RPacketService::GetStatus, RPacketService::EnumerateContexts, RPacketService::GetNtwkRegStatus, RPacketContext::GetStatus
 * @SYMTestExpectedResults Pass - Packet service and contexts are still active.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify packet service detach fails when there are contexts active.

Verify packet network registration status,  number of contexts, context status,  and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Open a new context, set a valid config and activate it. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
   	
	// Ensure packet service status is EStatusActive
    RPacketService::TStatus currentStatus;
    CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Detach from packet service. Check RPacketService::Detach returns KErrInUse 
	TExtEtelRequestStatus detachStatus(packetService,EPacketDetach);
	CleanupStack::PushL(detachStatus);
	packetService.Detach(detachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone,
					_L("PacketService::Detach timed out."));    
	ASSERT_EQUALS(detachStatus.Int(),KErrInUse, 
					_L("RPacketService::Detach did not return KErrInUse"));

	// Check RPacketService::GetStatus returns RPacketService::EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus did not return EStatusActive"));
     
	// Check RPacketService::GetDynamicCaps returns caps in set of KCapsActivate | KCapsRxCSCall | KCapsRxContextActivationReq | KCapsSMSTransfer.
	// Check RPacketService::GetDynamicCaps completes with caps NOT in set of RPacketService::KCapsManualAttach | RPacketService::KCapsManualDetach
	RPacketService::TDynamicCapsFlags dynCaps;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps =RPacketService::KCapsManualDetach | 
        		    								RPacketService::KCapsRxCSCall |
        		    								RPacketService::KCapsRxContextActivationReq |
        		    								RPacketService::KCapsSMSTransfer;
	RPacketService::TDynamicCapsFlags unwantedCaps =RPacketService::KCapsManualAttach;
    ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));
    // $CTSYProblem: RPacketService::KCapsManualDetach was included in unwantedCaps list in the procedure
    // whereas CTSY gets this capability in response to GetDynamicCaps. Procedure is correct but CTSY behaviour is different.
   
	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetNtwkRegStatus timed out."));    
	ASSERT_EQUALS(regStatus.Int(),KErrNone, 
					_L("RPacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
					_L("RPacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork"));
				
	// Check RPacketService::EnumerateContexts returns aCount, 1 and aMaxAllowed, 11
	TInt count = 0;
	TInt maxAllowed = 0;
	TExtEtelRequestStatus enumContextStatus(packetService, EPacketEnumerateContexts);
	CleanupStack::PushL(enumContextStatus);
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, COUNT_ONE, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
	ASSERT_EQUALS(maxAllowed, MAX_CONTEXTS, 
					_L("PacketService::EnumerateContexts, wrong max count returned"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// detachStatus
	// regStatus
	// enumContextStatus
	CleanupStack::PopAndDestroy(3, &detachStatus);	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0017::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0017");
	}



CCTSYIntegrationTestPacketService0018::CCTSYIntegrationTestPacketService0018(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0018::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0018::~CCTSYIntegrationTestPacketService0018()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0018::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0018
 * @SYMFssID BA/CTSY/PKTS-0018
 * @SYMTestCaseDesc Detach from packet service when active.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::EnumerateContextsInNif, RPacketService::EnumerateNifs, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif
 * @SYMTestExpectedResults Pass - Packet service and contexts are still active and correct NIF related info is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify NIF related info correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Get Packet Service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Post notification to get a ContextName
	TName contextName;
	TExtEtelRequestStatus notifyContextAddedStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus);	
	packetService.NotifyContextAdded(notifyContextAddedStatus,contextName);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Wait to complete the NotifyContextAdded
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeShort), KErrNone,
					_L("PacketService;:NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(), KErrNone, 
					_L("PacketService::NotifyContextAdded returned an error"));

	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded 
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); //This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode

	// Open a new context, set a valid config and activate it.
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	   	
	// Ensure packet service status is EStatusActive
    RPacketService::TStatus currentStatus;
    CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Check RPacketService::Detach returns KErrInUse 
	TExtEtelRequestStatus detachStatus(packetService,EPacketDetach);
	CleanupStack::PushL(detachStatus);
	packetService.Detach(detachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone,
					_L("PacketService::Detach timed out."));   
	ASSERT_EQUALS(detachStatus.Int(), KErrInUse, 
					_L("RPacketService::Detach did not return an KErrInUse"));
	
	// Check RPacketContext::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus did not return EStatusActive"));

	// Check RPacketService::EnumerateNifs returns 1
	TInt numNif=0;
	TExtEtelRequestStatus enumerateNifsSt(packetService,EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsSt);
	packetService.EnumerateNifs(enumerateNifsSt,numNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateNifs timed out."));   
	ASSERT_EQUALS(enumerateNifsSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateNifs returned an error"));
	ASSERT_EQUALS(numNif,COUNT_ONE, 
					_L("RPacketService::EnumerateNifs did not return count as 1"));
	
	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context activated in set up
	TInt numContexts=0;
	TExtEtelRequestStatus enumerateContextsInNifSt(packetService,EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifSt);
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt, contextName, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts,COUNT_ONE, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 1"));

	// Check RPacketService::GetContextNameInNif returns valid context name
	TName nameInNif;
	TExtEtelRequestStatus getContextNameInNifSt(packetService,EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifSt);
	packetService.GetContextNameInNif(getContextNameInNifSt, contextName, 0, nameInNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt.Int(),KErrNone, 
					_L("RPacketService::GetContextNameInNif returned an error"));
	ASSERT_TRUE(nameInNif.Length()>0, 
					_L("RPacketService::GetContextNameInNif did not return a valid context name"));
	
	// Check RPacketService::GetNifInfo with aIndex=0 returns context name = name of context activated in set up, iNumberOfContexts, 1 iNifStatus, EStatusActive iPdpAddress, valid IP address iContextType, RPacketService::EInternalContext
	RPacketService::TNifInfoV2 nifInfo;
	TPckg<RPacketService::TNifInfoV2> nifInfoPk(nifInfo);
	TExtEtelRequestStatus getNifInfoSt(packetService,EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoSt);
	packetService.GetNifInfo(getNifInfoSt, 0, nifInfoPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo timed out."));   
	ASSERT_EQUALS(getNifInfoSt.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, contextName, _L("RPacketService::GetNifInfo did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo.iNumberOfContexts , COUNT_ONE, _L("RPacketService::GetNifInfo did not return the number of contexts equal to one"));
	ASSERT_EQUALS(nifInfo.iNifStatus ,RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo did not return EStatusActive"));
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()> 0, _L("RPacketService::GetNifInfo did not return valis pdp address"));
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo did not return EInternalContext context type"));

	//
	// TEST END
	//

    StartCleanup();

    // Pop:
    // notifyContextAddedStatus
    // detachStatus
    // enumerateNifsSt
    // enumerateContextsInNifSt
    // getContextNameInNifSt
    // getNifInfoSt
    CleanupStack::PopAndDestroy(6, &notifyContextAddedStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0018::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0018");
	}



CCTSYIntegrationTestPacketService0019::CCTSYIntegrationTestPacketService0019(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0019::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0019::~CCTSYIntegrationTestPacketService0019()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0019::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0019
 * @SYMFssID BA/CTSY/PKTS-0019
 * @SYMTestCaseDesc Detach from packet service when last context is deactivated.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::NotifyDynamicCapsChange, RPacketService::GetNtwkRegStatus, RPacketService::EnumerateContexts, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RPacketService::GetDynamicCaps
 * @SYMTestExpectedResults Pass - Phone detached from packet service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify automatic detach from packet service succeeds when the last context is deactivated and attach mode is RPacketService::EAttachWhenNeeded

Verify packet network registration status, number of contexts, context status and dynamic caps are correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Get Packet Context
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	// Get second packet context
	RPacketContext& packetContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
									 										  KMainPhone,
									 										  KMainPacketService,
									 										  KPrimaryPacketContext2); 
	
	// Get Mobile Phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Register with the home network
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(5*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Activate two primary contexts with a valid config. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext2 );
	
	// Ensure status of both context is EStatusActive 
	RPacketContext::TContextStatus packetContextStatus;
	CHECK_EQUALS_L(packetContext.GetStatus(packetContextStatus), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	CHECK_EQUALS_L(packetContextStatus, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));
	
	// Staus of second packet context
	RPacketContext::TContextStatus packetContextStatus2;
	CHECK_EQUALS_L(packetContext2.GetStatus(packetContextStatus2), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	CHECK_EQUALS_L(packetContextStatus2, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// Ensure RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus currentStatus;
	CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RPacketService::EnumerateContexts returns aCount, 2 aMaxAllowed, 11
	TInt count = 0;
    TInt maxAllowed = 0;
	TExtEtelRequestStatus enumContextStatus(packetService, EPacketEnumerateContexts);
    CleanupStack::PushL(enumContextStatus);
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, 2, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
	ASSERT_EQUALS(maxAllowed, MAX_CONTEXTS, 
				_L("PacketService::EnumerateContexts, wrong max count returned"));

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	RPacketService::TDynamicCapsFlags dynCaps;
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps =RPacketService::KCapsSMSTransfer | 
	    		    								RPacketService::KCapsRxContextActivationReq |
	    		    								RPacketService::KCapsRxCSCall;
	RPacketService::TDynamicCapsFlags unwantedCaps =RPacketService::KCapsManualAttach;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	// Request notification RPacketService::NotifyStatusChange 
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Request notification RPacketService::NotifyDynamicCapsChange 
	TExtEtelRequestStatus notifyDynamicCapsChangeStatus(packetService, EPacketNotifyDynamicCapsChange);
	CleanupStack::PushL(notifyDynamicCapsChangeStatus);
	packetService.NotifyDynamicCapsChange(notifyDynamicCapsChangeStatus, dynCaps);
		
	// Deactivate context 1. 
	TExtEtelRequestStatus deactivateStatus(packetService, EPacketContextDeactivate);
	CleanupStack::PushL(deactivateStatus);
	packetContext.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium),	KErrNone, 
					_L("RPacketContext::Deactivate timed out"));	
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("RPacketContext::Deactivate returned an error"));		
		
	// Check RPacketService::EnumerateContexts returns aCount, 2 aMaxAllowed, 11
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, 2, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
	ASSERT_EQUALS(maxAllowed, MAX_CONTEXTS, 
					_L("PacketService::EnumerateContexts, wrong max count returned"));

	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	// Check RPacketContext::GetStatus for context 1 returns EStatusInactive
	ASSERT_EQUALS(packetContext.GetStatus(packetContextStatus), KErrNone, 
			_L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus, RPacketContext::EStatusInactive, 
			_L("RPacketContext::GetStatus did not return RPacketContext::EStatusInactive."));
	
	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	expectedCaps =RPacketService::KCapsSMSTransfer | 
	    		  RPacketService::KCapsRxContextActivationReq |
	    		  RPacketService::KCapsRxCSCall;
	unwantedCaps =RPacketService::KCapsManualAttach;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	// Check RPacketService::NotifyStatusChange is still KRequestPending
	ASSERT_EQUALS(notifyStatusStatus.Int(), KRequestPending, 
					_L("RPacketContext::NotifyStatusChange did not return KRequestPending"));
	
	// Check RPacketService::NotifyDynamicCapsChange is still KRequestPending
	ASSERT_EQUALS(notifyDynamicCapsChangeStatus.Int(), KRequestPending, 
					_L("RPacketContext::NotifyStatusChange did not return KRequestPending"));	

	// Deactivate context 2. 
	packetContext2.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium),	KErrNone, 
					_L("RPacketContext::Deactivate timed out"));
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("RPacketContext::Deactivate returned an error"));		
		
	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);

	// Check RPacketService::NotifyDynamicCapsChange completes with caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::NotifyDynamicCapsChange completes with caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq
	expectedCaps = RPacketService::KCapsManualAttach |
				   RPacketService::KCapsRxCSCall;
	unwantedCaps = RPacketService::KCapsSMSTransfer |
				   RPacketService::KCapsManualDetach|
				   RPacketService::KCapsRxContextActivationReq ;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyDynamicCapsChange(packetService, 
																		 notifyDynamicCapsChangeStatus,
																		 dynCaps,
																		 expectedCaps,
																		 unwantedCaps,
																		 KErrNone);

	// Check RPacketContext::GetStatus for context 2 returns EStatusInactive
	ASSERT_EQUALS(packetContext2.GetStatus(packetContextStatus2), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	ASSERT_EQUALS(packetContextStatus2, RPacketContext::EStatusInactive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusInactive."));

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsManualAttach | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsManualDetach |  RPacketService::KCapsRxContextActivationReq
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	expectedCaps = 	RPacketService::KCapsManualAttach |
					RPacketService::KCapsRxCSCall;
	unwantedCaps =  RPacketService::KCapsSMSTransfer |
				    RPacketService::KCapsManualDetach|
				    RPacketService::KCapsRxContextActivationReq ;
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,_L("PacketService::GetDynamicCaps returned wrong dynamic caps"));

	// Check RPacketService::EnumerateContexts returns aCount, 2 aMaxAllowed, 11
	packetService.EnumerateContexts(enumContextStatus,count,maxAllowed);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumContextStatus, ETimeMedium),KErrNone, 
					_L("PacketService::EnumerateContexts timed out"));
	ASSERT_EQUALS(enumContextStatus.Int(), KErrNone, 
					_L("PacketService::EnumerateContexts returned an error"));
	ASSERT_EQUALS(count, 2, 
					_L("PacketService::EnumerateContexts, wrong count returned"));
	ASSERT_EQUALS(maxAllowed, MAX_CONTEXTS, 
					_L("PacketService::EnumerateContexts, wrong max count returned"));

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium), KErrNone,
					_L("RPacketService::GetNtwkRegStatus timed-out."));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
					_L("RPacketService::GetNtwkRegStatus returned with an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
					_L("RPacketService::GetNtwkRegStatus did not return RPacketService::ERegisteredOnHomeNetwork"));

	//
	// TEST END
	//

	
    StartCleanup();
	// Pop:
	// enumContextStatus
	// notifyStatusStatus
	// notifyDynamicCapsChangeStatus
	// deactivateStatus
	// regStatus
	CleanupStack::PopAndDestroy(5, &enumContextStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0019::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0019");
	}



CCTSYIntegrationTestPacketService0020::CCTSYIntegrationTestPacketService0020(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0020::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0020::~CCTSYIntegrationTestPacketService0020()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0020::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0020
 * @SYMFssID BA/CTSY/PKTS-0020
 * @SYMTestCaseDesc Detach from packet service when last context is deactivated.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::EnumerateContextsInNif, RPacketService::EnumerateNifs, RPacketService::GetNifInfo, RPacketService::GetContextNameInNif
 * @SYMTestExpectedResults Pass - Phone detached and contexts deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify automatic detach from packet service succeeds when the last context is deactivated and attach mode is RPacketService::EAttachWhenNeeded

Verify NIF related info correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Post notification to get a ContextName
	TName contextName;
	TExtEtelRequestStatus notifyContextAddedStatus(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus);	
	packetService.NotifyContextAdded(notifyContextAddedStatus,contextName);
	
	// Open Primary context 1
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	
	// Wait to complete the NotifyContextAdded
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus, ETimeShort), KErrNone,
					_L("PacketService;:NotifyContextAdded timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus.Int(), KErrNone, 
					_L("PacketService::NotifyContextAdded returned an error"));
	
	// Post notification to get a ContextName
	TName contextName2;
	TExtEtelRequestStatus notifyContextAddedStatus2(packetService, EPacketNotifyContextAdded);
	CleanupStack::PushL(notifyContextAddedStatus2);	
	packetService.NotifyContextAdded(notifyContextAddedStatus2,contextName2);

	// Open primary context 2
	RPacketContext& packetContext2 = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
									 										  KMainPhone,
									 										  KMainPacketService,
									 										  KPrimaryPacketContext2); 
	
	// Wait to complete the NotifyContextAdded
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(notifyContextAddedStatus2, ETimeShort), KErrNone,
					_L("PacketService;:NotifyContextAdded(second) timed out"));
	CHECK_EQUALS_L(notifyContextAddedStatus2.Int(), KErrNone, 
					_L("PacketService::NotifyContextAdded(second) returned an error"));
	
	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Register with the home network
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
		
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);
	User::After(10*KOneSecond); // This time delay is required to complete the implicit detach operation caused by EAttachWhenNeeded attach mode
	
	// Activate two primary contexts with a valid config. 
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext2 );
	
	// Ensure status of both context is EStatusActive 
	RPacketContext::TContextStatus packetContextStatus;
	CHECK_EQUALS_L(packetContext.GetStatus(packetContextStatus), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	CHECK_EQUALS_L(packetContextStatus, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));
	
	// Staus of second packet context
	RPacketContext::TContextStatus packetContextStatus2;
	CHECK_EQUALS_L(packetContext2.GetStatus(packetContextStatus2), KErrNone, 
					_L("RPacketContext::GetStatus returned with an error."));
	CHECK_EQUALS_L(packetContextStatus2, RPacketContext::EStatusActive, 
					_L("RPacketContext::GetStatus did not return RPacketContext::EStatusActive."));

	// Ensure RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus currentStatus;
	CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RPacketService::EnumerateNifs returns aCount, 2
	TInt numNif=0;
	TExtEtelRequestStatus enumerateNifsSt(packetService,EPacketEnumerateNifs);
	CleanupStack::PushL(enumerateNifsSt);
	packetService.EnumerateNifs(enumerateNifsSt,numNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateNifs timed out."));   
	ASSERT_EQUALS(enumerateNifsSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateNifs returned an error"));
	ASSERT_EQUALS(numNif ,2 , 
					_L("RPacketService::EnumerateNifs did not return count as 2"));

	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 1
	TInt numContexts=0;
	TExtEtelRequestStatus enumerateContextsInNifSt(packetService,EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifSt);
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt, contextName, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts,COUNT_ONE, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 1"));

	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 2
	TExtEtelRequestStatus enumerateContextsInNifSt2(packetService,EPacketEnumerateContextsInNif);
	CleanupStack::PushL(enumerateContextsInNifSt2);
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt2, contextName2, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt2, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt2.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts, COUNT_ONE, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 1"));

	// Check RPacketService::GetContextNameInNif returns context 1 name when aExistingContextName=name of context 1 aIndex=0
	TName nameInNif;
	TExtEtelRequestStatus getContextNameInNifSt(packetService,EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifSt);
	packetService.GetContextNameInNif(getContextNameInNifSt, contextName, 0, nameInNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt.Int(),KErrNone, 
					_L("RPacketService::GetContextNameInNif returned an error"));
	ASSERT_TRUE(nameInNif.Length()>0, 
					_L("RPacketService::GetContextNameInNif did not return a valid context name"));

	// Check RPacketService::GetContextNameInNif returns context 1 name when aExistingContextName=name of context 2 aIndex=0
	TName nameInNif2;
	TExtEtelRequestStatus getContextNameInNifSt2(packetService,EPacketGetContextNameInNif);
	CleanupStack::PushL(getContextNameInNifSt2);
	packetService.GetContextNameInNif(getContextNameInNifSt2, contextName2, 0, nameInNif2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt2, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif(second) timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt2.Int(),KErrNone, 
					_L("RPacketService::GetContextNameInNif(second) returned an error"));
	ASSERT_TRUE(nameInNif2.Length()>0, 
					_L("RPacketService::GetContextNameInNif did not return a valid context name"));

	// Check RPacketService::GetNifInfo with aIndex=0 returns context name = name of context 1, iNumberOfContexts, 1 iNifStatus, EStatusActive iPdpAddress, valid IP address iContextType, RPacketService::EInternalContext
	RPacketService::TNifInfoV2 nifInfo;
	TPckg<RPacketService::TNifInfoV2> nifInfoPk(nifInfo);
	TExtEtelRequestStatus getNifInfoSt(packetService,EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoSt);
	packetService.GetNifInfo(getNifInfoSt, 0, nifInfoPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo timed out."));   
	ASSERT_EQUALS(getNifInfoSt.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, contextName, _L("RPacketService::GetNifInfo did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo.iNumberOfContexts , COUNT_ONE, _L("RPacketService::GetNifInfo did not return the number of contexts equal to one"));
	ASSERT_EQUALS(nifInfo.iNifStatus ,RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo did not return EStatusActive"));
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()> 0, _L("RPacketService::GetNifInfo did not return valis pdp address"));
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo did not return EInternalContext context type"));

	// Check RPacketService::GetNifInfo with aIndex=1 and context name = name of context 2 returns iNumberOfContexts, 1 iNifStatus, EStatusActive iPdpAddress, valid IP address iContextType, RPacketService::EInternalContext
	RPacketService::TNifInfoV2 nifInfo2;
	TPckg<RPacketService::TNifInfoV2> nifInfoPk2(nifInfo2);
	TExtEtelRequestStatus getNifInfoSt2(packetService,EPacketGetNifInfo);
	CleanupStack::PushL(getNifInfoSt2);
	packetService.GetNifInfo(getNifInfoSt2, 1, nifInfoPk2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt2, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo(second) timed out."));   
	ASSERT_EQUALS(getNifInfoSt2.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo(second) returned an error"));
	ASSERT_EQUALS_DES16(nifInfo2.iContextName, contextName2, _L("RPacketService::GetNifInfo(second) did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo2.iNumberOfContexts , COUNT_ONE, _L("RPacketService::GetNifInfo(second) did not return the number of contexts equal to one"));
	ASSERT_EQUALS(nifInfo2.iNifStatus ,RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo(second) did not return EStatusActive"));
	ASSERT_TRUE(nifInfo2.iPdpAddress.Length()> 0, _L("RPacketService::GetNifInfo(second) did not return valis pdp address"));
	ASSERT_EQUALS(nifInfo2.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo(second) did not return EInternalContext context type"));

	// Deactivate context 1 
	TExtEtelRequestStatus deactivateStatus(packetService, EPacketContextDeactivate);
	CleanupStack::PushL(deactivateStatus);
	packetContext.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium),	KErrNone, 
					_L("RPacketContext::Deactivate timed out"));
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("RPacketContext::Deactivate returned an error"));		
	
	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	// Check RPacketService::EnumerateNifs returns aCount, 2
	packetService.EnumerateNifs(enumerateNifsSt,numNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateNifs timed out."));   
	ASSERT_EQUALS(enumerateNifsSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateNifs returned an error"));
	ASSERT_EQUALS(numNif,2, 
					_L("RPacketService::EnumerateNifs did not return count as 2"));

	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 1
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt, contextName, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts,1, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 1"));

	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 2
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt2, contextName2, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt2, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif(second) timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt2.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif(second) returned an error"));
	ASSERT_EQUALS(numContexts,1, 
					_L("RPacketService::EnumerateContextsInNif(second) did not return count as 1"));

	// Check RPacketService::GetContextNameInNif returns context 1 name when aExistingContextName=name of context 1 aIndex=0
	packetService.GetContextNameInNif(getContextNameInNifSt, contextName, 0, nameInNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt.Int(),KErrNone, 
					_L("RPacketService::GetContextNameInNif returned an error"));
	ASSERT_TRUE(nameInNif.Length()>0, 
					_L("RPacketService::GetContextNameInNif did not return a valid context name"));
	
	// Check RPacketService::GetContextNameInNif returns context 2 name when aExistingContextName=name of context 2 aIndex=0
	packetService.GetContextNameInNif(getContextNameInNifSt2, contextName2, 0, nameInNif2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getContextNameInNifSt2, ETimeMedium), KErrNone,
					_L("PacketService::GetContextNameInNif(second) timed out."));   
	ASSERT_EQUALS(getContextNameInNifSt2.Int(),KErrNone, 
					_L("RPacketService::GetContextNameInNif(second) returned an error"));
	ASSERT_TRUE(nameInNif2.Length()>0, 
					_L("RPacketService::GetContextNameInNif did not return a valid context name"));

	// Check RPacketService::GetNifInfo with aIndex=0 returns context name = name of context 1, iNumberOfContexts, 1 iNifStatus, EStatusInactive iPdpAddress, valid IP address iContextType, RPacketService::EInternalContext
	packetService.GetNifInfo(getNifInfoSt, 0, nifInfoPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo timed out."));   
	ASSERT_EQUALS(getNifInfoSt.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, contextName, _L("RPacketService::GetNifInfo did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo.iNumberOfContexts , COUNT_ONE, _L("RPacketService::GetNifInfo did not return the number of contexts equal to one"));
	ASSERT_EQUALS(nifInfo.iNifStatus ,RPacketContext::EStatusInactive, _L("RPacketService::GetNifInfo did not return EStatusInactive"));
	ASSERT_TRUE(nifInfo.iPdpAddress.Length()> 0, _L("RPacketService::GetNifInfo did not return valis pdp address"));
	ASSERT_EQUALS(nifInfo.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo did not return EInternalContext context type"));

	// Check RPacketService::GetNifInfo with aIndex=1 and context name = name of context 2 returns iNumberOfContexts, 1 iNifStatus, EStatusActive iPdpAddress, valid IP address iContextType, RPacketService::EInternalContext
	packetService.GetNifInfo(getNifInfoSt2, 1, nifInfoPk2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt2, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo(second) timed out."));   
	ASSERT_EQUALS(getNifInfoSt2.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo(second) returned an error"));
	ASSERT_EQUALS_DES16(nifInfo2.iContextName, contextName2, _L("RPacketService::GetNifInfo(second) did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo2.iNumberOfContexts , COUNT_ONE, _L("RPacketService::GetNifInfo(second) did not return the number of contexts equal to one"));
	ASSERT_EQUALS(nifInfo2.iNifStatus ,RPacketContext::EStatusActive, _L("RPacketService::GetNifInfo(second) did not return EStatusActive"));
	ASSERT_TRUE(nifInfo2.iPdpAddress.Length()> 0, _L("RPacketService::GetNifInfo(second) did not return valis pdp address"));
	ASSERT_EQUALS(nifInfo2.iContextType, RPacketService::EInternalContext, _L("RPacketService::GetNifInfo(second) did not return EInternalContext context type"));

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Deactivate context 2 
	packetContext2.Deactivate(deactivateStatus);			
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deactivateStatus, ETimeMedium),	KErrNone, 
					_L("RPacketContext::Deactivate timed out"));	
	ASSERT_EQUALS(deactivateStatus.Int(), KErrNone, 
					_L("RPacketContext::Deactivate returned an error"));		
	
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached -> EStatusInactive(it should be EStatusUnattached )
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);

	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus  did not return EStautusActive"));

	// Check RPacketService::EnumerateNifs returns aCount, 2
	packetService.EnumerateNifs(enumerateNifsSt,numNif);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateNifsSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateNifs timed out."));   
	ASSERT_EQUALS(enumerateNifsSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateNifs returned an error"));
	ASSERT_EQUALS(numNif,2, 
					_L("RPacketService::EnumerateNifs did not return count as 2"));

	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 1
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt, contextName, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif returned an error"));
	ASSERT_EQUALS(numContexts,1, 
					_L("RPacketService::EnumerateContextsInNif did not return count as 1"));
	
	// Check RPacketService::EnumerateContextsInNif returns aCount, 1 when aExistingContextName, name of context 2
	packetService.EnumerateContextsInNif(enumerateContextsInNifSt2, contextName2, numContexts);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(enumerateContextsInNifSt2, ETimeMedium), KErrNone,
					_L("PacketService::EnumerateContextsInNif(second) timed out."));   
	ASSERT_EQUALS(enumerateContextsInNifSt2.Int(),KErrNone, 
					_L("RPacketService::EnumerateContextsInNif(second) returned an error"));
	ASSERT_EQUALS(numContexts,1, 
					_L("RPacketService::EnumerateContextsInNif(second) did not return count as 1"));

	// Check RPacketService::GetNifInfo with aIndex=0 returns context name = name of context 1, iNifStatus, EStatusInactive 
	packetService.GetNifInfo(getNifInfoSt, 0, nifInfoPk);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo timed out."));   
	ASSERT_EQUALS(getNifInfoSt.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo returned an error"));
	ASSERT_EQUALS_DES16(nifInfo.iContextName, contextName, _L("RPacketService::GetNifInfo did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo.iNifStatus ,RPacketContext::EStatusInactive, _L("RPacketService::GetNifInfo did not return EStatusInactive"));
	
	// Check RPacketService::GetNifInfo with aIndex=1 and context name = name of context 2 returns iNifStatus, EStatusInActive
	packetService.GetNifInfo(getNifInfoSt2, 1, nifInfoPk2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getNifInfoSt2, ETimeMedium), KErrNone,
					_L("PacketService::GetNifInfo(second) timed out."));   
	ASSERT_EQUALS(getNifInfoSt2.Int(),KErrNone, 
					_L("RPacketService::GetNifInfo(second) returned an error"));
	ASSERT_EQUALS_DES16(nifInfo2.iContextName, contextName2, _L("RPacketService::GetNifInfo(second) did not return the desired Context Name"));
	ASSERT_EQUALS(nifInfo2.iNifStatus ,RPacketContext::EStatusInactive, _L("RPacketService::GetNifInfo(second) did not return EStatusInactive"));
		
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
    // notifyContextAddedStatus
    // notifyContextAddedStatus2
    // enumerateNifsSt
    // enumerateContextsInNifSt
    // enumerateContextsInNifSt2
    // getContextNameInNifSt
    // getContextNameInNifSt2
    // getNifInfoSt
    // getNifInfoSt2
    // deactivateStatus
    // notifyStatusStatus
	CleanupStack::PopAndDestroy(11,&notifyContextAddedStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0020::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0020");
	}



CCTSYIntegrationTestPacketService0021::CCTSYIntegrationTestPacketService0021(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0021::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0021::~CCTSYIntegrationTestPacketService0021()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0021::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0021
 * @SYMFssID BA/CTSY/PKTS-0021
 * @SYMTestCaseDesc Detach from packet service when already detached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::SetAttachMode, RPacketService::GetAttachMode
 * @SYMTestExpectedResults Pass - Detach fails.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify detach fails.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	
	// Get packet service 
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Set attach mode to EAttachWhenNeeded 
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);	
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS (WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone, 
					_L("PacketService::SetAttachMode timed out."));								
	ASSERT_EQUALS(setAttachModeStatus.Int(), KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));		
	
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);	
	CleanupStack::PushL(getAttachModeStatus);
	RPacketService::TAttachMode attachMode;
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService;:GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(), KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return an EAttachWhenNeeded"));

	// Check RPacketService::GetStatus returns EStatusUnattached
	RPacketService::TStatus currentStatus;
    ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus  did not return EStatusUnattached"));
	
	// Check RPacketService::Detach returns KErrAlreadyExists
	TExtEtelRequestStatus detachStatus(packetService,EPacketDetach);
	CleanupStack::PushL(detachStatus);
	packetService.Detach(detachStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone,
					_L("PacketService::Detach timed out."));    
	ASSERT_EQUALS(detachStatus.Int(),KErrAlreadyExists, 
					_L("RPacketService::Detach did not returned KErrAlreadyExists"));

	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusUnattached, _L("RPacketService::GetStatus did not return EStatusUnattached"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// setAttachModeStatus
	// getAttachModeStatus
	// detachStatus
	CleanupStack::PopAndDestroy(3,&setAttachModeStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0021::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0021");
	}



CCTSYIntegrationTestPacketService0022::CCTSYIntegrationTestPacketService0022(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0022::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0022::~CCTSYIntegrationTestPacketService0022()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0022::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0022
 * @SYMFssID BA/CTSY/PKTS-0022
 * @SYMTestCaseDesc Automatic attach and detach when attach mode is changed.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyAttachModeChange, RPacketService::GetAttachMode, RPacketService::NotifyStatusChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Attach mode set successfully. Packet service automatically attaches and detaches depending on mode.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify when attach mode is set to RPacketService::EAttachWhenNeeded, packet service detaches when there are no contexts active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Ensure packet service status is EStatusUnattached
	RPacketService::TStatus setUpStatus;
	CHECK_EQUALS_L(packetService.GetStatus(setUpStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	TExtEtelRequestStatus detachStatus(packetService, EPacketDetach);
	CleanupStack::PushL(detachStatus);
	TExtEtelRequestStatus serviceStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(serviceStatus);
	
	if (setUpStatus != RPacketService::EStatusUnattached)
		{
		packetService.NotifyStatusChange(serviceStatus, setUpStatus);
		User::After(2*KOneSecond); // With TRP, the notify isn't getting through before the detach req
		packetService.Detach(detachStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone, 	
						_L("PacketService::Detach timed out"));
		CHECK_TRUE_L(detachStatus.Int()== KErrNone|| KErrAlreadyExists, 
						_L("PacketService::Detach returned an error"));										
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(serviceStatus, ETimeMedium), KErrNone, 
						_L("PacketService::NotifyStatusChange timed out"));
		CHECK_EQUALS_L(serviceStatus.Int(), KErrNone, 
						_L("PacketService::NotifyStatusChange returned an error"));										
		}
		
	CHECK_EQUALS_L(setUpStatus, RPacketService::EStatusUnattached, _L("SET UP Packet service should be detached at this point"));
	
	// Ensure attach mode is RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenNeeded; 
	EnsureAttachModeL(packetService,attachMode);

	
	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
			
	// Set attach mode to EAttachWhenPossible
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);	
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetAttachMode timed out."));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));	
	
	// Check RPacketService::GetAttachMode returns EAttachWhenPossible
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);	
	CleanupStack::PushL(getAttachModeStatus);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out."));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenPossible"));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
			
	// Check RPacketService::GetStatus completes with EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));

	// Post notification for status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);	
				
	// Set attach mode to EAttachWhenNeeded
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetAttachMode Timed out"));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));	
		
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode Timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error."));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));
	
	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached
	expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
				
	// Check RPacketService::GetStatus completes with EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(packetServiceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(packetServiceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));

	// 
	// TEST END
	//

	StartCleanup();    
    
    // Pop:
    // detachStatus
	// serviceStatus
 	// notifyStatusStatus
	// setAttachModeStatus 
	// getAttachModeStatus
	CleanupStack::PopAndDestroy(5, &detachStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0022::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0022");
	}



CCTSYIntegrationTestPacketService0023::CCTSYIntegrationTestPacketService0023(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0023::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0023::~CCTSYIntegrationTestPacketService0023()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0023::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0023
 * @SYMFssID BA/CTSY/PKTS-0023
 * @SYMTestCaseDesc Set attach mode to RPacketService::EAttachWhenNeeded when attached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyAttachModeChange, RPacketService::GetAttachMode, RPacketService::GetNtwkRegStatus, RPacketService::GetDynamicCaps, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Attach mode set successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify attach mode is set correctly when there is at least one context active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Get Packet Service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Get mobile phone
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Register with the home network
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	// Ensure there is one context active. 
	RPacketContext& packetContext = iEtelSessionMgr.GetPrimaryPacketContextL(KMainServer,
																			 KMainPhone,
																			 KMainPacketService,
																			 KPrimaryPacketContext1); 
	// Activate primary Context
	iPacketServiceTestHelper.ActivatePrimaryPacketContextL(mobilePhone, packetContext );
	
	// Ensure RPacketService::GetStatus returns EStatusActive
	RPacketService::TStatus currentStatus;
	CHECK_EQUALS_L(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));
	
	// Ensure RPacketService::GetAttachMode is EAttachWhenPossible 
	RPacketService::TAttachMode attachMode= RPacketService::EAttachWhenPossible; 
	EnsureAttachModeL(packetService,attachMode);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	
	// Check RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	RPacketService::TDynamicCapsFlags dynCaps = 0;		
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	RPacketService::TDynamicCapsFlags expectedCaps = RPacketService::KCapsSMSTransfer| 
													RPacketService::KCapsRxContextActivationReq |
													RPacketService::KCapsRxCSCall ;
	RPacketService::TDynamicCapsFlags unwantedCaps = RPacketService::KCapsManualAttach; 
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,          
					_L("PacketService::GetDynamicCaps Wrong dynamic caps"));

	// RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork 
	TExtEtelRequestStatus regStatus(packetService, EPacketGetNtwkRegStatus);
	CleanupStack::PushL(regStatus);
	RPacketService::TRegistrationStatus registrationStatus;
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
	    			_L("PacketService::GetNtwkRegStatus Wrong registration status"));

	// Post notification for attach mode change
	TExtEtelRequestStatus notifyAttachModeChangeStatus(packetService, EPacketNotifyAttachModeChange);
	CleanupStack::PushL(notifyAttachModeChangeStatus);
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);

	// Set attach mode to RPacketService::EAttachWhenNeeded
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);	
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetAttachMode timed out."));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));
		
	// Check RPacketService::NotifyAttachModeChange completes with RPacketService::EAttachWhenNeeded
	RPacketService::TAttachMode expectedAttachMode = RPacketService::EAttachWhenNeeded;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded 
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);	
	CleanupStack::PushL(getAttachModeStatus);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService;:GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(), KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return EAttachWhenNeeded"));
		
	// Chek RPacketService::GetStatus returns EStatusActive
	ASSERT_EQUALS(packetService.GetStatus(currentStatus), KErrNone, _L("RPacketService::GetStatus returned an error"));
	ASSERT_EQUALS(currentStatus, RPacketService::EStatusActive, _L("RPacketService::GetStatus  did not return EStautusActive"));

	// Check RPacketService::GetDynamicCaps returns caps in set of RPacketService::KCapsSMSTransfer | RPacketService::KCapsRxContextActivationReq | RPacketService::KCapsRxCSCall
	// Check RPacketService::GetDynamicCaps returns caps NOT in set of RPacketService::KCapsManualAttach
	ASSERT_EQUALS(packetService.GetDynamicCaps(dynCaps), KErrNone, 
					_L("PacketService::GetDynamicCaps returned an error"));
	expectedCaps = RPacketService::KCapsSMSTransfer| 
				   RPacketService::KCapsRxContextActivationReq |
				   RPacketService::KCapsRxCSCall ;
	unwantedCaps = RPacketService::KCapsManualAttach; 
	ASSERT_BITS_SET(dynCaps, expectedCaps, unwantedCaps,          
					_L("PacketService::GetDynamicCaps Wrong dynamic caps"));

	// RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork 
	packetService.GetNtwkRegStatus(regStatus, registrationStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(regStatus, ETimeMedium),KErrNone, 
					_L("PacketService::GetNtwkRegStatus timed out"));
	ASSERT_EQUALS(regStatus.Int(), KErrNone, 
	    			_L("PacketService::GetNtwkRegStatus returned an error"));
	ASSERT_EQUALS(registrationStatus, RPacketService::ERegisteredOnHomeNetwork, 
	    			_L("PacketService::GetNtwkRegStatus Wrong registration status"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// regStatus
	// notifyAttachModeChangeStatus
    // setAttachModeStatus
    // getAttachModeStatus
	CleanupStack::PopAndDestroy(4, &regStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0023::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0023");
	}



CCTSYIntegrationTestPacketService0024::CCTSYIntegrationTestPacketService0024(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0024::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0024::~CCTSYIntegrationTestPacketService0024()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0024::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0024
 * @SYMFssID BA/CTSY/PKTS-0024
 * @SYMTestCaseDesc Register with packet network fail when roaming.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetStatus, RPacketService::NotifyChangeOfNtwkRegStatus, RPacketService::GetNtwkRegStatus
 * @SYMTestExpectedResults Pass - Phone not registered to packet network.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify packet service is detached and phone is not registered on packet network moving to from cell with packet network roaming to one without.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Simulate phone roaming in a 3G cell which supports packet network roaming. 

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredRoaming

	// Check RPacketService::GetStatus returns EStatusAttached

	// Simulate phone moving into a new 3G cell which does not allow packet network roaming. 

	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached

	// Check RPacketService::GetStatus returns EStatusUnattached

	// Check RPacketService::NotifyChangeOfNtwkRegStatus completes with one of ENotRegisteredNotSearching | ENotRegisteredSearching | ERegistrationDenied | ENotRegisteredAndNotAvailable

	// Check RPacketService::GetNtwkRegStatus returns one of ENotRegisteredNotSearching | ENotRegisteredSearching | ERegistrationDenied | ENotRegisteredAndNotAvailable

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0024::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0024");
	}



CCTSYIntegrationTestPacketService0025::CCTSYIntegrationTestPacketService0025(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0025::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0025::~CCTSYIntegrationTestPacketService0025()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0025::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0025
 * @SYMFssID BA/CTSY/PKTS-0025
 * @SYMTestCaseDesc Set preferred bearer to RPacketService::BearerPacketSwitched and RPacketService::EBearerCircuitSwitched when attach mode is RPacketService::EAttachWhenPossible in MS class C operation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMSClassChange, RPacketService::GetMSClass, RPacketService::SetAttachMode, RPacketService::NotifyAttachModeChange, RPacketService::GetAttachMode, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::SetPreferredBearer, RPacketService::GetPreferredBearer
 * @SYMTestExpectedResults Pass - In class C operation, packet service status depends on preferred bearer.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify when attach mode is RPacketService::EAttachWhenPossible and there are no contexts active, status of packet service depends on preferred bearer.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	// Note: please dont review this code as this test case is priority 4, so we are not dealing them any more
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
					
	// Ensure MS class is not RPacketService::EMSClassAlternateMode with RPacketService::GetMSClass		
	TExtEtelRequestStatus getMSClassStatus(packetService, EPacketGetMSClass);
	CleanupStack::PushL(getMSClassStatus);
	RPacketService::TMSClass msClass;
	RPacketService::TMSClass msMax;
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetMSClass timedout"));
	CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
					_L("packetService::GetMSClass returned an error"));											
				
	TExtEtelRequestStatus setMSClassStatus(packetService, EPacketSetMSClass);
	CleanupStack::PushL(setMSClassStatus);	
	if (msClass == RPacketService::EMSClassAlternateMode)	
		{
		msClass = RPacketService::EMSClassDualMode;
		packetService.SetMSClass(setMSClassStatus,msClass);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone, 
						_L("PacketService::SetMSClass timed out"));
		CHECK_EQUALS_L(setMSClassStatus.Int(), KErrNone, 
						_L("PacketService::SetMSClass returned an error"));												
		User::After(ETimeMedium); // we need this to get consistent responses from the H2		
		packetService.GetMSClass(getMSClassStatus,msClass,msMax);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone, 
						_L("PacketService::GetMSClass timed out"));
		CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
						_L("PacketService::GetMSClass returned an error"));												
		}
	
	CHECK_TRUE_L((msClass != RPacketService::EMSClassAlternateMode), 
			_L("RPacketService::EMSClassAlternateMode SET UP Wrong MS class"));
		
	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Post Notification for MS Class Change
	TExtEtelRequestStatus notifyMSClassChangeStatus(packetService, EPacketNotifyMSClassChange);
	CleanupStack::PushL(notifyMSClassChangeStatus);
	RPacketService::TMSClass newClass;
	packetService.NotifyMSClassChange(notifyMSClassChangeStatus,newClass);

	// Set MS class to RPacketService::EMSClassAlternateMode with RPacketService::SetMSClass
	msClass = RPacketService::EMSClassAlternateMode;
	packetService.SetMSClass(setMSClassStatus,msClass);		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone,
		 			_L("PacketService::SetMSClass timed out."));			
	ASSERT_EQUALS(setMSClassStatus.Int(), KErrNone, 
					_L("PacketService::SetMSClassStatus returned an error"));
	
	// Check RPacketService::NotifyMSClassChange completes with RPacketService::EMSClassAlternateMode
	RPacketService::TMSClass expectedNewClass= RPacketService::EMSClassAlternateMode;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyMSClassChange(packetService,
																	 notifyMSClassChangeStatus,
																	 newClass,
																	 expectedNewClass,
																	 KErrNone);

	// Check RPacketService::GetMSClass returns RPacketService::EMSClassAlternateMode
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetMSClass timedout"));
	ASSERT_EQUALS(getMSClassStatus.Int(), KErrNone, 
					_L("PacketService::GetMSClass returned and error"));
	ASSERT_EQUALS(newClass, RPacketService::EMSClassAlternateMode, 
					_L("PacketService::GetMSClass did not return RPacketService::EMSClassAlternateMode"));	

	// Post notification for attach mode change
	TExtEtelRequestStatus notifyAttachModeChangeStatus(packetService, EPacketNotifyAttachModeChange);
	CleanupStack::PushL(notifyAttachModeChangeStatus);
	RPacketService::TAttachMode attachMode;
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
		
	// Set attach mode to EAttachWhenPossible with RPacketService::SetAttachMode.
	TExtEtelRequestStatus setAttachModeStatus(packetService,EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetAttachMode timedout."));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));
		
	// Check RPacketService::NotifyAttachModeChange completes with EAttachWhenPossible
	RPacketService::TAttachMode expectedAttachMode = RPacketService::EAttachWhenPossible;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);

	// Check RPacketService::GetAttachMode returns EAttachWhenPossible
	TExtEtelRequestStatus getAttachModeStatus(packetService, EPacketGetAttachMode);
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenPossible"));

	// Check RPacketService::GetStatus returns EStatusAttached.
	RPacketService::TStatus serviceStatus;
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));

	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Set preferred bearer to EBearerCircuitSwitched.
	TExtEtelRequestStatus setBearerStatus(packetService, EPacketSetPrefBearer);
	
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timed out."));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketService::SetPreferredBearer returned an error."));	
	
	// Check RPacketService::GetPreferredBearer returns EBearerCircuitSwitched
	TExtEtelRequestStatus getBearerStatus(packetService, EPacketGetPrefBearer);	
	RPacketService::TPreferredBearer newBearer;
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetPreferredBearer timedout."));
	ASSERT_EQUALS(getBearerStatus.Int(), KErrNone, 
					_L("PacketService::GetPreferredBearer returned an error"));						 				
	ASSERT_EQUALS(newBearer, RPacketService::EBearerCircuitSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerCircuitSwitched"));	
		
	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																    notifyStatusStatus, 
																    serviceStatus, 
																    expectedPacketServiceStatus, 
																    KErrNone);	
			
	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone,	
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));

	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Set preferred bearer to EBearerPacketSwitched
	bearer = RPacketService::EBearerPacketSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timed out"));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketService::SetPreferredBearer returned an error"));	
	
	// Check RPacketService::GetPreferredBearer returns EBearerPacketSwitched
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetPreferredBearer timed out"));
	ASSERT_EQUALS(getBearerStatus.Int(), KErrNone, 
					_L("PacketService::GetPreferredBearer returned an error"));		 								
	ASSERT_EQUALS(newBearer, RPacketService::EBearerPacketSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerPacketSwitched"));	
		
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																    notifyStatusStatus, 
																    serviceStatus, 
																    expectedPacketServiceStatus, 
																    KErrNone);
					
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));
	
	// 
	// TEST END
	//

	StartCleanup();
    
    // Pop:
    // getMSClassStatus
    // setMSClassStatus
	// notifyMSClassChangeStatus
	// notifyAttachModeChangeStatus
	// setAttachModeStatus
	// notifyStatusStatus    
    
	CleanupStack::PopAndDestroy(6, &getMSClassStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0025::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0025");
	}



CCTSYIntegrationTestPacketService0026::CCTSYIntegrationTestPacketService0026(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0026::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0026::~CCTSYIntegrationTestPacketService0026()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0026::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0026
 * @SYMFssID BA/CTSY/PKTS-0026
 * @SYMTestCaseDesc Set preferred bearer to RPacketService::BearerPacketSwitched and RPacketService::EBearerCircuitSwitched when attach mode is RPacketService::EAttachWhenNeeded in MS class C operation.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::SetMSClass, RPacketService::NotifyMSClassChange, RPacketService::GetMSClass, RPacketService::SetAttachMode, RPacketService::GetAttachMode, RPacketService::NotifyStatusChange, RPacketService::GetStatus, RPacketService::SetPreferredBearer, RPacketService::GetPreferredBearer
 * @SYMTestExpectedResults Pass - In class C operation, packet service status depends on preferred bearer.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify when attach mode is RPacketService::EAttachWhenNeeded and there are no contexts active, packet service is detached regardless of preferred bearer.
 *
 * @return - TVerdict code
 */
	{
   
	//
	// SET UP
	//
// Note: please dont review this code as this test case is priority 4, so we are not dealing them any more
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);

	// Ensure packet service is EStatusUnattached with RPacketService::GetStatus
	TExtEtelRequestStatus serviceStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(serviceStatus);
	TExtEtelRequestStatus detachStatus(packetService, EPacketDetach);	
	CleanupStack::PushL(detachStatus);
	RPacketService::TStatus setUpStatus;
	CHECK_EQUALS_L(packetService.GetStatus(setUpStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	if (setUpStatus != RPacketService::EStatusUnattached)
		{
		packetService.NotifyStatusChange(serviceStatus, setUpStatus);
		User::After(2*KOneSecond); // With TRP, the notify isn't getting through before the detach req
		packetService.Detach(detachStatus);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(detachStatus, ETimeMedium), KErrNone,
						_L("PacketService::Detach timed out"));
		CHECK_EQUALS_L(detachStatus.Int(), KErrNone, 
						_L("PacketService::Detach returned an error"));										
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(serviceStatus, ETimeMedium), KErrNone, 
						_L("PacketService::NotifyStatusChange timed out"));
		CHECK_EQUALS_L(serviceStatus.Int(), KErrNone, 
						_L("PacketService::NotifyStatusChange returned an error"));
		}		
	CHECK_EQUALS_L(setUpStatus, RPacketService::EStatusUnattached, _L("SET UP Packet service should be detached"));

	// Ensure MS class is not RPacketService::EMSClassAlternateMode with RPacketService::GetMSClass
	TExtEtelRequestStatus getMSClassStatus(packetService, EPacketGetMSClass);
	CleanupStack::PushL(getMSClassStatus);
	RPacketService::TMSClass msClass;
	RPacketService::TMSClass msMax;
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetMSClass timed out"));
	CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
					_L("PacketService::GetMSClass returned an error"));
	TExtEtelRequestStatus setMSClassStatus(packetService, EPacketSetMSClass);
	CleanupStack::PushL(setMSClassStatus);
	if (msClass == RPacketService::EMSClassAlternateMode)	
		{
		msClass = RPacketService::EMSClassDualMode;
		packetService.SetMSClass(setMSClassStatus,msClass);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone, 
						_L("PacketService::SetMSClass timed out"));		
		CHECK_EQUALS_L(setMSClassStatus.Int(), KErrNone, 
						_L("PacketService::SetMSClass returned an error"));
		User::After(ETimeMedium); // we need this to get consistent responses from the reference board
		packetService.GetMSClass(getMSClassStatus,msClass,msMax);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone,
						_L("PacketService::GetMSClass timed out"));					
		CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
						_L("PacketService::GetMSClass returned an error"));
		}
	
	CHECK_TRUE_L((msClass != RPacketService::EMSClassAlternateMode), _L("SET UP Wrong MS class in set up"));		
		
	//
	// SET UP END
	//

	StartTest();

	//
	// TEST START
	//

	// Set attach mode to EAttachWhenNeeded.
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone, 
					_L("PacketService::SetAttachMode timed out."));
	ASSERT_EQUALS(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));	
	
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded
	TExtEtelRequestStatus getAttachModeStatus(packetService, EPacketGetAttachMode);
	CleanupStack::PushL(getAttachModeStatus);
	RPacketService::TAttachMode attachMode;
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out."));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode did not return RPacketService::EAttachWhenNeeded"));

	// Post Notification for MS Class Change
	TExtEtelRequestStatus notifyMSClassChangeStatus(packetService, EPacketNotifyMSClassChange);
	CleanupStack::PushL(notifyMSClassChangeStatus);
	RPacketService::TMSClass newClass;
	packetService.NotifyMSClassChange(notifyMSClassChangeStatus,newClass);
	
	// Set MS class to RPacketService::EMSClassAlternateMode with RPacketService::SetMSClass
	msClass = RPacketService::EMSClassAlternateMode;
	packetService.SetMSClass(setMSClassStatus,msClass);		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetMSClass timed out."));
	ASSERT_EQUALS(setMSClassStatus.Int(), KErrNone, 
					_L("PacketService::SetMSClass returned an error"));								
			
	// Check RPacketService::NotifyMSClassChange completes with RPacketService::EMSClassAlternateMode
	RPacketService::TMSClass expectedNewClass= RPacketService::EMSClassAlternateMode;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyMSClassChange(packetService,
																	 notifyMSClassChangeStatus,
																	 newClass,
																	 expectedNewClass,
																	 KErrNone);
	
	// Check RPacketService::GetMSClass returns RPacketService::EMSClassAlternateMode
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetMSClass timedout."));
	ASSERT_EQUALS(getMSClassStatus.Int(), KErrNone, 
					_L("PacketService::GetMSClass returned an error"));
	ASSERT_EQUALS(newClass, RPacketService::EMSClassAlternateMode, 
					_L("PacketService::GetMSClass did not return RPacketService::EMSClassAlternateMode"));	
			
	// Set preferred bearer to EBearerCircuitSwitched
	TExtEtelRequestStatus setBearerStatus(packetService, EPacketSetPrefBearer);
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timed out"));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketService::SetPreferredBearer returned an error"));		
	
	// Check RPacketService::GetPreferredBearer returns EBearerCircuitSwitched
	TExtEtelRequestStatus getBearerStatus(packetService, EPacketGetPrefBearer);	
	RPacketService::TPreferredBearer newBearer;
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone, 
				_L("PacketService::GetPreferredBearer timed out"))
	ASSERT_EQUALS(getBearerStatus.Int(),KErrNone, 
				_L("PacketService::GetPreferredBearer returned an error"));					 						
	ASSERT_EQUALS(newBearer, RPacketService::EBearerCircuitSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerCircuitSwitched"));	
		
	// Check RPacketService::GetStatus returns EStatusUnattached
	RPacketService::TStatus getStatus;
	ASSERT_EQUALS(packetService.GetStatus(getStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(getStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));	

	// Set preferred bearer to EBearerPacketSwitched
	bearer = RPacketService::EBearerPacketSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timedout"));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketSercice::SetPreferredBearer returned and error"));
		
	// Check RPacketService::GetPreferredBearer returns EBearerPacketSwitched
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetPreferredBearer timed out."));
	ASSERT_EQUALS(getBearerStatus.Int(), KErrNone, 
					_L("PacketService::GetPreferredBearer returned an error"));	 									
	ASSERT_EQUALS(newBearer, RPacketService::EBearerPacketSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerPacketSwitched"));
			
	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(getStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(getStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached "));

	// 
	// TEST END
	//

	StartCleanup();
    
    // Pop:
    // serviceStatus
    // detachStatus
    // getMSClassStatus 
    // setMSClassStatus   
    // setAttachModeStatus
    // setAttachModeStatus
    // notifyMSClassChangeStatus

	CleanupStack::PopAndDestroy(7, &serviceStatus);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0026::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0026");
	}



CCTSYIntegrationTestPacketService0027::CCTSYIntegrationTestPacketService0027(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0027::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0027::~CCTSYIntegrationTestPacketService0027()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0027::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0027
 * @SYMFssID BA/CTSY/PKTS-0027
 * @SYMTestCaseDesc Set MS class to class A.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyMSClassChange, RPacketService::GetMSClass, RPacketContext::SetConfig, RPacketContext::Activate, RPacketContext::NotifyStatusChange, RPacketContext::GetStatus, RCall::Dial, RCall::HangUp, RCall::GetStatus, RCall::NotifyStatusChange, RPacketService::NotifyStatusChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Packet service remains active when CS call is made.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify simultaneous GSM and packet operation possible.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure RPacketService::GetMSClass returns EMSClassDualMode 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get the packet service.  

	// Get a primary packet context 

	// Set a valid config for the context with RPacketContext::SetConfig 

	// Activate the primary context 

	// Check RPacketContext::NotifyStatusChange completes with EStatusActivating -> EStatusActive

	// Check RPacketContext::GetStatus returns RPacketContext::EStatusActive

	// Check RPacketService::GetStatus returns RPacketService::EStatusActive

	// Get voice line 1 

	// Dial a number that answers 

	// Check RCall::NotifyStatusChange completes with EStatusConnected

	// Check RCall::GetStatus returns EStatusConnected

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	// Hang up call. 

	// Check RPacketService::GetStatus returns EStatusActive

	// Check RPacketContext::GetStatus returns EStatusActive

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0027::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0027");
	}



CCTSYIntegrationTestPacketService0028::CCTSYIntegrationTestPacketService0028(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0028::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0028::~CCTSYIntegrationTestPacketService0028()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0028::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0028
 * @SYMFssID BA/CTSY/PKTS-0028
 * @SYMTestCaseDesc Check preferred bearer has no effect in class A and B.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::GetPreferredBearer, RPacketService::SetMSClass, RPacketService::NotifyMSClassChange, RPacketService::GetMSClass, RPacketService::SetAttachMode, RPacketService::NotifyAttachModeChange, RPacketService::GetAttachMode, RPacketService::NotifyStatusChange, RPacketService::GetStatus
 * @SYMTestExpectedResults Pass - Packet service status independent of preferred bearer.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify that when preferred bearer is CS, automatic attach still occurs when MS is in class A or B operation.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	
	// Get packet service
	RPacketService& packetService = iEtelSessionMgr.GetPacketServiceL(KMainServer, KMainPhone, KMainPacketService);
	
	// Ensure RPacketService::GetMSClass returns EMSClassDualMode 
	TExtEtelRequestStatus getMSClassStatus(packetService, EPacketGetMSClass);
	CleanupStack::PushL(getMSClassStatus);
	RPacketService::TMSClass msClass;
	RPacketService::TMSClass msMax;
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetMSClass timedout"));
	CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
					_L("packetService::GetMSClass returned an error"));											
				
	TExtEtelRequestStatus setMSClassStatus(packetService, EPacketSetMSClass);
	CleanupStack::PushL(setMSClassStatus);	
	if (msClass != RPacketService::EMSClassDualMode)	
		{
		msClass = RPacketService::EMSClassDualMode;
		packetService.SetMSClass(setMSClassStatus,msClass);	
		ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10050);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone, 
						_L("PacketService::SetMSClass timed out"));
		CHECK_EQUALS_L(setMSClassStatus.Int(), KErrNone, 
						_L("PacketService::SetMSClass returned an error"));												
		packetService.GetMSClass(getMSClassStatus,msClass,msMax);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone, 
						_L("PacketService::GetMSClass timed out"));
		CHECK_EQUALS_L(getMSClassStatus.Int(), KErrNone, 
						_L("PacketService::GetMSClass returned an error"));												
		}
	
	CHECK_EQUALS_L(msClass, RPacketService::EMSClassDualMode, 
					_L("RPacketService::GetMSClass did not return RPacketService::EMSClassDualMode MS class"));
		
	// Set attach mode to EAttachWhenNeeded.
	TExtEtelRequestStatus setAttachModeStatus(packetService, EPacketSetAttachMode);
	CleanupStack::PushL(setAttachModeStatus);
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeMedium), KErrNone, 
					_L("PacketService::SetAttachMode timed out."));
	CHECK_EQUALS_L(setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error"));	

	// Ensure RPacketService::GetStatus returns EStatusUnattached 
	RPacketService::TStatus serviceStatus;
	CHECK_EQUALS_L(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	CHECK_EQUALS_L(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));
   	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Set preferred bearer to EBearerCircuitSwitched.
	TExtEtelRequestStatus setBearerStatus(packetService, EPacketSetPrefBearer);
	CleanupStack::PushL(setBearerStatus);
	RPacketService::TPreferredBearer bearer = RPacketService::EBearerCircuitSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timed out."));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketService::SetPreferredBearer returned an error."));	
	
	// Check RPacketService::GetPreferredBearer returns EBearerCircuitSwitched
	TExtEtelRequestStatus getBearerStatus(packetService, EPacketGetPrefBearer);	
	CleanupStack::PushL(getBearerStatus);
	RPacketService::TPreferredBearer newBearer;
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetPreferredBearer timedout."));
	ASSERT_EQUALS(getBearerStatus.Int(), KErrNone, 
					_L("PacketService::GetPreferredBearer returned an error"));						 				
	ASSERT_EQUALS(newBearer, RPacketService::EBearerCircuitSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerCircuitSwitched"));	
		
	// Post notification for attach mode change
	RPacketService::TAttachMode attachMode;
	TExtEtelRequestStatus notifyAttachModeChangeStatus(packetService, EPacketNotifyAttachModeChange);
	CleanupStack::PushL(notifyAttachModeChangeStatus);
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
	
	// Post notification for packet service status change
	TExtEtelRequestStatus notifyStatusStatus(packetService, EPacketNotifyStatusChange);
	CleanupStack::PushL(notifyStatusStatus); 
	RPacketService::TStatus packetServiceStatus;
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);

	// Set attach mode to RPacketService::EAttachWhenPossible 
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								
	
	// Check RPacketService::NotifyAttachModeChange completes with EAttachWhenPossible
	RPacketService::TAttachMode expectedAttachMode = RPacketService::EAttachWhenPossible;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);

	// Check RPacketService::GetAttachMode returns EAttachWhenPossible
	TExtEtelRequestStatus getAttachModeStatus (packetService,EPacketGetAttachMode);
	CleanupStack::PushL(getAttachModeStatus);	
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("PacketService::GetAttachMode Wrong attach mode"));

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	RPacketService::TStatus expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));
   	
	// Post notification for attach mode change
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
	
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	
	// Set attach mode to RPacketService::EAttachWhenNeeded 
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								
	
	// Check RPacketService::NotifyAttachModeChange completes with EAttachWhenNeeded
	expectedAttachMode = RPacketService::EAttachWhenNeeded;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode Wrong attach mode"));
	
	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached
	expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));
	
	// Post Notification for MS Class Change
	TExtEtelRequestStatus notifyMSClassChangeStatus(packetService, EPacketNotifyMSClassChange);
	CleanupStack::PushL(notifyMSClassChangeStatus);
	RPacketService::TMSClass newClass;
	packetService.NotifyMSClassChange(notifyMSClassChangeStatus,newClass);
	
	// Set MS class to EMSClassSuspensionRequired 
	msClass = RPacketService::EMSClassSuspensionRequired;
	packetService.SetMSClass(setMSClassStatus,msClass);		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setMSClassStatus, ETimeMedium), KErrNone,
		 			_L("PacketService::SetMSClass timed out."));			
	ASSERT_EQUALS(setMSClassStatus.Int(), KErrNone, 
					_L("PacketService::SetMSClassStatus returned an error"));
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10050);
	CHECK_EQUALS_L(1, 2, _L("This test has been made to leave intentially as it is not possible now to move forward in the test"));		

				
	// Check RPacketService::NotifyMSClassChange completes with EMSClassSuspensionRequired
	RPacketService::TMSClass expectedNewClass= RPacketService::EMSClassSuspensionRequired;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyMSClassChange(packetService,
																	 notifyMSClassChangeStatus,
																	 newClass,
																	 expectedNewClass,
																	 KErrNone);
	
	// Check RPacketService::GetMSClass returns EMSClassSuspensionRequired
	packetService.GetMSClass(getMSClassStatus,msClass, msMax);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getMSClassStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetMSClass timed out"));
	ASSERT_EQUALS(getMSClassStatus.Int(), KErrNone, 
					_L("PacketService::GetMSClass returned and error"));
	ASSERT_EQUALS(newClass, RPacketService::EMSClassSuspensionRequired, 
					_L("PacketService::GetMSClass did not return RPacketService::EMSClassSuspensionRequired"));	
	
	// Set preferred bearer to EBearerCircuitSwitched.
	bearer = RPacketService::EBearerCircuitSwitched;
	packetService.SetPreferredBearer(setBearerStatus,bearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setBearerStatus, ETimeMedium), KErrNone,
					_L("PacketService::SetPreferredBearer timed out."));
	ASSERT_EQUALS(setBearerStatus.Int(), KErrNone, 
					_L("PacketService::SetPreferredBearer returned an error."));	
	
	// Check RPacketService::GetPreferredBearer returns EBearerCircuitSwitched
	packetService.GetPreferredBearer(getBearerStatus,newBearer);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getBearerStatus, ETimeMedium), KErrNone, 
					_L("PacketService::GetPreferredBearer timedout."));
	ASSERT_EQUALS(getBearerStatus.Int(), KErrNone, 
					_L("PacketService::GetPreferredBearer returned an error"));						 				
	ASSERT_EQUALS(newBearer, RPacketService::EBearerCircuitSwitched, 
					_L("PacketService::GetPreferredBearer did not return RPacketService::EBearerCircuitSwitched"));	
		
	// Post notification for attach mode change
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
	
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	// Set attach mode to RPacketService::EAttachWhenPossible 
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenPossible);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								
	
	// Check RPacketService::NotifyAttachModeChange completes with EAttachWhenPossible
	expectedAttachMode = RPacketService::EAttachWhenPossible;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns EAttachWhenPossible
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenPossible, 
					_L("PacketService::GetAttachMode Wrong attach mode"));
	
	// Check RPacketService::NotifyStatusChange completes with EStatusAttached
	expectedPacketServiceStatus = RPacketService::EStatusAttached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
	
	// Check RPacketService::GetStatus returns EStatusAttached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusAttached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusAttached"));
		
	// Post notification for attach mode change
	packetService.NotifyAttachModeChange(notifyAttachModeChangeStatus, attachMode);
	
	// Post notification for packet service status change
	packetService.NotifyStatusChange(notifyStatusStatus, packetServiceStatus);
	
	
	// Set attach mode to RPacketService::EAttachWhenNeeded 
	packetService.SetAttachMode(setAttachModeStatus, RPacketService::EAttachWhenNeeded);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setAttachModeStatus, ETimeLong), KErrNone,
					_L("PacketService::SetAttachMode timed out"));
	ASSERT_EQUALS (setAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::SetAttachMode returned an error."));								
	
	// Check RPacketService::NotifyAttachModeChange completes with EAttachWhenNeeded
	expectedAttachMode = RPacketService::EAttachWhenNeeded;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyAttachModeChange(packetService,
																		notifyAttachModeChangeStatus,
																		attachMode,
																		expectedAttachMode,
																		KErrNone);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10049);
	
	// Check RPacketService::GetAttachMode returns EAttachWhenNeeded
	packetService.GetAttachMode(getAttachModeStatus, attachMode);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getAttachModeStatus, ETimeMedium), KErrNone,
					_L("PacketService::GetAttachMode timed out"));
	ASSERT_EQUALS(getAttachModeStatus.Int(),KErrNone, 
					_L("PacketService::GetAttachMode returned an error"));
	ASSERT_EQUALS(attachMode, RPacketService::EAttachWhenNeeded, 
					_L("PacketService::GetAttachMode Wrong attach mode"));
	
	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached
	expectedPacketServiceStatus = RPacketService::EStatusUnattached;
	iPacketServiceTestHelper.WaitForPacketServiceNotifyStatusChange(packetService, 
																	notifyStatusStatus,
																	packetServiceStatus,
																	expectedPacketServiceStatus,
																	KErrNone);
				
	// Check RPacketService::GetStatus returns EStatusUnattached
	ASSERT_EQUALS(packetService.GetStatus(serviceStatus), KErrNone, 
					_L("PacketService::GetStatus returned an error"));
	ASSERT_EQUALS(serviceStatus, RPacketService::EStatusUnattached, 
					_L("PacketService::GetStatus did not return RPacketService::EStatusUnattached"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Pop:
	// getMSClassStatus
	// setMSClassStatus
	// setAttachModeStatus
	// setBearerStatus
	// getBearerStatus
	// notifyAttachModeChangeStatus
	// notifyStatusStatus
	// getAttachModeStatus
	// notifyMSClassChangeStatus
	CleanupStack::PopAndDestroy(9, &getMSClassStatus);
	
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0028::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0028");
	}



CCTSYIntegrationTestPacketService0029::CCTSYIntegrationTestPacketService0029(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0029::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0029::~CCTSYIntegrationTestPacketService0029()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0029::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0029
 * @SYMFssID BA/CTSY/PKTS-0029
 * @SYMTestCaseDesc Get release mode change notifications when attaching.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyReleaseModeChange, RPacketService::GetCurrentReleaseMode
 * @SYMTestExpectedResults Pass - Release mode change notification completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify release mode change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure phone is not registered on packet network 

	// RPacketService::GetNtwkRegStatus returns one of ENotRegisteredNotSearching | ENotRegisteredSearching | ENotRegisteredButAvailable | EUnknown  


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get packet service 

	// Check RPacketService::GetCurrentReleaseMode returns one of EReleaseModeUnregistered | EReleaseModeUnknown

	// Attach 

	// Check RPacketService::NotifyReleaseModeChange completes with one of EReleaseMode97_98 | EReleaseMode99 | EReleaseMode4

	// Check RPacketService::GetCurrentReleaseMode returns one of EReleaseMode97_98 | EReleaseMode99 | EReleaseMode4

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0029::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0029");
	}



CCTSYIntegrationTestPacketService0030::CCTSYIntegrationTestPacketService0030(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPacketServiceBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPacketService0030::GetTestStepName());
	}

CCTSYIntegrationTestPacketService0030::~CCTSYIntegrationTestPacketService0030()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPacketService0030::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PKTS-0030
 * @SYMFssID BA/CTSY/PKTS-0030
 * @SYMTestCaseDesc Get release mode and network registration status change notifications when moving between 2G and 3G cells when attached.
 * @SYMTestPriority High
 * @SYMTestActions RPacketService::NotifyReleaseModeChange, RPacketService::GetCurrentReleaseMode, RPacketService::GetStatus, RPacketService::NotifyChangeOfNtwkRegStatus, RPacketService::GetNtwkRegStatus
 * @SYMTestExpectedResults Pass - Release mode changes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies simulated/manual
 *
 * Reason for test: Verify release mode change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure attach mode is RPacketService::EAttachWhenPossible and RPacketService::GetStatus returns EStatusAttached 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Simulate a 3G cell supporting release mode 4. 

	// Check RPacketService::GetCurrentReleaseMode returns EReleaseMode4

	// Simulate moving to a 2G cell. 

	// Check RPacketService::NotifyStatusChange completes with EStatusUnattached

	// Check RPacketService::GetStatus returns EStatusUnattached

	// Check RPacketService::NotifyReleaseModeChange completes with EReleaseModeUnregistered

	// Check RPacketService::NotifyChangeOfNtwkRegStatus completes with one of  ENotRegisteredAndNotAvailable | ENotRegisteredSearching | ENotRegisteredNotSearching

	// Check RPacketService::GetNtwkRegStatus returns one of ENotRegisteredAndNotAvailable | ENotRegisteredSearching | ENotRegisteredNotSearching

	// Simulate moving into a 3G cell supporting release mode EReleaseMode97_98 

	// Check RPacketService::NotifyReleaseModeChange completes with EReleaseMode97_98

	// Check RPacketService::GetCurrentReleaseMode returns EReleaseMode97_98

	// Check RPacketService::NotifyChangeOfNtwkRegStatus completes with ERegisteredOnHomeNetwork

	// Check RPacketService::GetNtwkRegStatus returns ERegisteredOnHomeNetwork

	// Check RPacketService::NotifyStatusChange completes with EStatusAttached

	// Check RPacketService::GetStatus returns EStatusAttached

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPacketService0030::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPacketService0030");
	}



