// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Unit tests for SPUD FSM
// 
//

/**
 @file
 @internalComponent
*/

#include <e32def.h>
#include <test/testexecutelog.h>

#include "SpudUnitTestStep.h"
#include "meteldriverstrategy.h"
#include "ceteldrivercontext.h"
#include "CompareContextParameters.h"
#include "DefaultContextParameters.h"

using namespace EtelDriver;

CSpudFsmUnitNormalOperation::~CSpudFsmUnitNormalOperation()
	{
	}

// configure to use test step 1 from simtsy config file
CSpudFsmUnitNormalOperation::CSpudFsmUnitNormalOperation() : CSpudFsmUnitTestStepBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmUnitNormalOperation);
	}
	
TVerdict CSpudFsmUnitNormalOperation::doTestStepL()
/**
 Test normal operation
 This test step goes through all of the states in the SPUD FSM
 */
	{
	INFO_PRINTF1(_L("Starting Spud Driver Test normal operation"));
	
	TRequestStatus status;
	RPacketService::TContextInfo contextInfo;

	RPacketQoS::TQoSR99_R4Negotiated qos;
	TQoSR99_R4NegotiatedPckg qosPckg(qos);
	RPacketQoS::TQoSR99_R4Negotiated expectedTestQoSR99_R4Negotiated;

	InitPhoneAndPacketServiceL();
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	FsmObjectCreate();
	TContextId ci0 = WaitForFsmObjectCreateResponse(0);
	FsmObjectCreate();
	TContextId ci1 = WaitForFsmObjectCreateResponse(1);
	FsmObjectCreate();
	TContextId ci2 = WaitForFsmObjectCreateResponse(2);

	
	// Set up context config and QoS parameters

	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci1, contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci2, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);
	iListener->iPdpFsmInterface->Set(ci1, qosRequested);
	iListener->iPdpFsmInterface->Set(ci2, qosRequested);

	
	//
	// Test creating first primary context
	// This tests state transitions:
	// Initialised -> OpeningPhone -> CreatingPrimary -> SettingQoSPrimary -> ActivatingPrimary -> Open
	//
	INFO_PRINTF1(_L("Test creating first primary context"));
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, 0);
	// Check that the context was created
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	// Check that the QoS was created for the primary context
	RPacketQoS primaryPacketQos;
	TEST(primaryPacketQos.OpenExistingQoS(iPrimaryPacketContext, KFirstQosName) == KErrNone);

	// Check that the QoS parameters were set
	primaryPacketQos.GetProfileParameters(status, qosPckg);
	User::WaitForRequest(status);
	SecondQoSR99_R4Negotiated(expectedTestQoSR99_R4Negotiated);
	TEST(CompareUMTSQoSNeg(expectedTestQoSR99_R4Negotiated, qos));

	//
	// Test creating second primary context - since the phone is already open, there is no transition to OpeningPhone state
	// This tests state transitions:
	// Initialised -> CreatingPrimary -> SettingQoSPrimary -> ActivatingPrimary -> Open
	//
	INFO_PRINTF1(_L("Test creating second primary context"));	
	FsmInputRequest(ci1, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci1, KPrimaryContextCreated, 0);
	RPacketContext secondPrimaryContext;
	// Test that the context was created
	TEST(secondPrimaryContext.OpenExistingContext(iPacketService, KSecondPrimaryContextName) == KErrNone);

	
	//
	// Test creating secondary context
	// This tests state transitions:
	// Initialised -> CreatingSecondary -> CreatedSecondary
	//
	INFO_PRINTF1(_L("Test creating secondary contexts"));
	FsmInputRequest(ci2, SpudMan::ECreateSecondaryPDPContext, 0);
	WaitForFsmInputResponse(ci2, KSecondaryContextCreated, 0);
	// Test that the context was created
	TEST(iSecondaryPacketContext.OpenExistingContext(iPacketService, KFirstSecondaryContextName) == KErrNone);

	
	//
	// Test setting QOS on secondary
	// This tests state transitions:
	// CreatedSecondary -> SettingQoS -> CreatedSecondary
	// 
	INFO_PRINTF1(_L("Test setting QoS on secondary context"));
	FsmInputRequest(ci2, SpudMan::EContextQoSSet, 0);
	WaitForFsmInputResponse(ci2, KContextQoSSetEvent, 0);
	// Check that the QoS was created for the secondary
	RPacketQoS secondaryPacketQos;
	TEST(secondaryPacketQos.OpenExistingQoS(iSecondaryPacketContext, KFirstQosName) == KErrNone);

	// Check that the QoS parameters were set
	secondaryPacketQos.GetProfileParameters(status, qosPckg);
	User::WaitForRequest(status);
	SecondQoSR99_R4Negotiated(expectedTestQoSR99_R4Negotiated);
	TEST(CompareUMTSQoSNeg(expectedTestQoSR99_R4Negotiated, qos));

	// 
	// Test setting TFT on secondary
	// This tests state transitions:
	// CreatedSecondary -> SettingTFT -> CreatedSecondary
	// 
	INFO_PRINTF1(_L("Test setting TFT on secondary context"));
	TTFTOperationCode tftOperationCode = KAddFilters;
	iListener->iPdpFsmInterface->Set(ci2, tftOperationCode);
	TTFTInfo tftInfo;
	DefaultTftInfo(tftInfo);
	iListener->iPdpFsmInterface->Set(ci2, tftInfo);
	FsmInputRequest(ci2, SpudMan::EContextTFTModify, 0);
	WaitForFsmInputResponse(ci2, KContextTFTModifiedEvent, 0);
	// check that TFT was set correctly
	TEST(CompareTFTInfo(tftInfo, iSecondaryPacketContext));
	
	
	// 
	// Test activating secondary
	// This tests state transitions:
	// CreatedSecondary -> ActivatingSecondary -> Open
	// 
	INFO_PRINTF1(_L("Test activating secondary context"));
	FsmInputRequest(ci2, SpudMan::EContextActivate, 0);
	WaitForFsmInputResponse(ci2, KContextActivateEvent, 0);
	// Check that the context is active
	iPacketService.GetContextInfo(status, ci2, contextInfo);
	User::WaitForRequest(status);
	TEST(status == KErrNone && contextInfo.iStatus == RPacketContext::EStatusActive);
	
	
	INFO_PRINTF1(_L("Test Retrieving Negotiated QoSt"));
	FsmInputRequest(ci2, SpudMan::EGetNegQoS, 0);
	WaitForFsmInputResponse(ci2, KGetNegQoSEvent, 0);
	
	
	// 
	// Test network suspending/resuming context
	// This tests state transitions:
	// Open -> Suspend -> Open
	// 
	INFO_PRINTF1(_L("Test network suspend/resume context"));
	EtelRequest(EContextStatusChange, 0);
	WaitForFsmSuspendedNotification(ci0);

	EtelRequest(EContextStatusChange, 1);
	WaitForFsmResumedNotification(ci0);

	
	// 
	// Test SpudMan suspending/resuming context
	// This tests state transitions:
	// Open -> Suspend -> Open
	// 
	INFO_PRINTF1(_L("Test SpudMan suspend/resume context"));
	FsmInputRequest(ci0, SpudMan::ESuspend, 0);
	WaitForFsmSuspendedNotification(ci0);

	FsmInputRequest(ci0, SpudMan::EResume, 0);
	WaitForFsmResumedNotification(ci0);

	
	// 
	// Test changing TFT
	// This tests state transitions:
	// Open -> ChangingTFT -> Open
	// 
	INFO_PRINTF1(_L("Test changing TFT"));
	TTFTInfo secondTftInfo;
	SecondDefaultTftInfo(secondTftInfo);
	iListener->iPdpFsmInterface->Set(ci2, KAddFilters);
	iListener->iPdpFsmInterface->Set(ci2, secondTftInfo);
	// The second TFT should be added to the existing TFT
	FsmInputRequest(ci2, SpudMan::EContextTFTModify, 0);
	WaitForFsmInputResponse(ci2, KContextTFTModifiedEvent, 0);
	// check that TFT was changed
	TTFTInfo combinedTftInfo;
	CombinedDefaultTftInfo(combinedTftInfo);
	TEST(CompareTFTInfo(combinedTftInfo, iSecondaryPacketContext));

	
	// 
	// Test changing QOS
	// This tests state transitions:
	// Open -> ChangingQoS -> Open
	// 
	INFO_PRINTF1(_L("Test changing QoS"));

	SecondQoSR99_R4Requested(qosRequested);

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);
	FsmInputRequest(ci0, SpudMan::EContextQoSSet, 0);
	WaitForFsmInputResponse(ci0, KContextQoSSetEvent, 0);

	// check that QOS was changed
	DefaultQoSR99_R4Negotiated(expectedTestQoSR99_R4Negotiated);
	primaryPacketQos.GetProfileParameters(status, qosPckg);
	User::WaitForRequest(status);
	TEST(CompareUMTSQoSNeg(expectedTestQoSR99_R4Negotiated, qos));

	// 
	// Test modifying active
	// This tests state transitions:
	// Open -> ModifyingActive -> Open
	// 
	INFO_PRINTF1(_L("Test modifying active context"));
	FsmInputRequest(ci0, SpudMan::EContextModifyActive, 0);
	WaitForFsmInputResponse(ci0, KContextModifyActiveEvent, 0);

	
	secondaryPacketQos.Close();
	primaryPacketQos.Close();
	iSecondaryPacketContext.Close();
	secondPrimaryContext.Close();
	iPrimaryPacketContext.Close();
	
	
	// 
	// Test deleting contexts
	// This tests state transitions:
	// Open -> Closing -> Initialised
	// 
	INFO_PRINTF1(_L("Test deleting contexts"));
	for (TContextId deleteIndex = 0; deleteIndex < 3; deleteIndex++)
		{
		FsmInputRequest(deleteIndex, SpudMan::EContextDelete, 0);
		WaitForFsmInputResponse(deleteIndex, KContextDeleteEvent, 0);
		}
	// Check that the contexts were deleted
	for (TInt contextInfoIndex = 0; contextInfoIndex < 3; contextInfoIndex++)
		{
		iPacketService.GetContextInfo(status, contextInfoIndex, contextInfo);
		User::WaitForRequest(status);
		TEST(status == KErrNone && contextInfo.iStatus == RPacketContext::EStatusDeleted);
		}
	
	
	// 
	// Test that we can still use a context that has been deleted
	// 
	INFO_PRINTF1(_L("Test re-using deleted contexts"));
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, 0);
	// Test that the context was created
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KThirdPrimaryContextName) == KErrNone);

	// Clean-up
	iPrimaryPacketContext.Close();
	FsmInputRequest(ci0, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci0, KContextDeleteEvent, 0);
	
	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudFsmUnitCreatePrimaryError1::~CSpudFsmUnitCreatePrimaryError1()
	{}

// configure to use test step 102 from simtsy config file
CSpudFsmUnitCreatePrimaryError1::CSpudFsmUnitCreatePrimaryError1() : CSpudFsmUnitTestStepBase(102)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmCreatePrimaryError1);
	}
	
TVerdict CSpudFsmUnitCreatePrimaryError1::doTestStepL()
/**
 Test errors creating primary context
 */
	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test error creating primary context 1"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	TContextId ci0 = FsmObjectCreate();
	WaitForFsmObjectCreateResponse(ci0);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// RPacketContext::SetConfig is configured to fail with error KErrGeneral
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrGeneral);
	
	InitPhoneAndPacketServiceL();

	// The primary context should have been cleaned up after the failure
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	WaitForGivenContextStatus(iPrimaryPacketContext, RPacketContext::EStatusDeleted);
	iPrimaryPacketContext.Close();
	
	DestroyPhoneAndPacketService();
	
	return TestStepResult();
	}

CSpudFsmUnitCreatePrimaryError2::~CSpudFsmUnitCreatePrimaryError2()
	{}

// configure to use test step 103 from simtsy config file
CSpudFsmUnitCreatePrimaryError2::CSpudFsmUnitCreatePrimaryError2() : CSpudFsmUnitTestStepBase(103)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmCreatePrimaryError2);
	}
	
TVerdict CSpudFsmUnitCreatePrimaryError2::doTestStepL()
/**
 Test errors setting QOS on primary context
 */
	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test error creating primary context 2"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	TContextId ci0 = FsmObjectCreate();
	WaitForFsmObjectCreateResponse(ci0);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// RPacketQoS::SetProfileParameters is configured to fail
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrGeneral);
	
	InitPhoneAndPacketServiceL();

	// The primary context should be cleaned up after the failure
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	WaitForGivenContextStatus(iPrimaryPacketContext, RPacketContext::EStatusDeleted);
	iPrimaryPacketContext.Close();

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudFsmUnitCreatePrimaryError3::~CSpudFsmUnitCreatePrimaryError3()
	{}

// configure to use test step 104 from simtsy config file
CSpudFsmUnitCreatePrimaryError3::CSpudFsmUnitCreatePrimaryError3() : CSpudFsmUnitTestStepBase(104)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmCreatePrimaryError3);
	}
	
TVerdict CSpudFsmUnitCreatePrimaryError3::doTestStepL()
/**
 Test errors activating primary context
 */
	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test error creating primary context 3"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	TContextId ci0 = FsmObjectCreate();
	WaitForFsmObjectCreateResponse(ci0);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// Activiating the context is configured to fail with KErrGeneral
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrGeneral);
	
	InitPhoneAndPacketServiceL();

	// The primary context should have been cleaned up after the failure
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	WaitForGivenContextStatus(iPrimaryPacketContext, RPacketContext::EStatusDeleted);
	iPrimaryPacketContext.Close();

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudFsmUnitSetQosAndTftError::~CSpudFsmUnitSetQosAndTftError()
	{}

// configure to use test step 106 from simtsy config file
CSpudFsmUnitSetQosAndTftError::CSpudFsmUnitSetQosAndTftError() : CSpudFsmUnitTestStepBase(106)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmSetQosAndTftError);
	}
	
TVerdict CSpudFsmUnitSetQosAndTftError::doTestStepL()
/**
 Test errors setting QoS and setting TFT and activating secondary
 */
 	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test error creating QOS, setting TFT and activating"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	FsmObjectCreate();
	TContextId ci0 = WaitForFsmObjectCreateResponse(0);
	FsmObjectCreate();
	TContextId ci1 = WaitForFsmObjectCreateResponse(1);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// Test creating primary
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrNone);

	// Test creating secondary
	FsmInputRequest(ci1, SpudMan::ECreateSecondaryPDPContext, 0);
	WaitForFsmInputResponse(ci1, KSecondaryContextCreated, KErrNone);

	// RPacketQoS::SetProfileParameters is configured to fail for the secondary context
	iListener->iPdpFsmInterface->Set(ci1, qosRequested);
	FsmInputRequest(ci1, SpudMan::EContextQoSSet, 0);
	WaitForFsmInputResponse(ci1, KContextQoSSetEvent, KErrGeneral);
	
	// RPacketQoS::CreateNewTFT is configured to fail with KErrGeneral for the secondary context
	TTFTOperationCode tftOperationCode = KAddFilters;
	iListener->iPdpFsmInterface->Set(ci1, tftOperationCode);
	TTFTInfo tftInfo;
	DefaultTftInfo(tftInfo);
	iListener->iPdpFsmInterface->Set(ci1, tftInfo);
	FsmInputRequest(ci1, SpudMan::EContextTFTModify, 0);
	WaitForFsmInputResponse(ci1, KContextTFTModifiedEvent, KErrGeneral);
	
	// Activating the secondary context is configured to fail with KErrGeneral
	FsmInputRequest(ci1, SpudMan::EContextActivate, 0);
	WaitForFsmInputResponse(ci1, KContextActivateEvent, KErrGeneral);
	
	// Clean-up
	FsmInputRequest(ci0, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(0, KContextDeleteEvent, 0);

	FsmInputRequest(ci1, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci1, KContextDeleteEvent, 0);

	return TestStepResult();
	}

CSpudFsmUnitChangeQosAndTftError::~CSpudFsmUnitChangeQosAndTftError()
	{}

// configure to use test step 107 from simtsy config file
CSpudFsmUnitChangeQosAndTftError::CSpudFsmUnitChangeQosAndTftError() : CSpudFsmUnitTestStepBase(107)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmChangeQosAndTftError);
	}
	
TVerdict CSpudFsmUnitChangeQosAndTftError::doTestStepL()
/**
 Test errors changing QoS and changing TFT for secondary
 */
 	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test error changing QOS and TFT"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	FsmObjectCreate();
	TContextId ci0 = WaitForFsmObjectCreateResponse(0);
	FsmObjectCreate();
	TContextId ci1 = WaitForFsmObjectCreateResponse(1);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// Test creating primary
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrNone);
	
	// Test creating and activating secondary
	FsmInputRequest(ci1, SpudMan::ECreateSecondaryPDPContext, 0);
	WaitForFsmInputResponse(ci1, KSecondaryContextCreated, KErrNone);
	FsmInputRequest(ci1, SpudMan::EContextActivate, 0);
	WaitForFsmInputResponse(ci1, KContextActivateEvent, KErrNone);
	
	FsmInputRequest(ci1, SpudMan::EGetNegQoS, 0);
	WaitForFsmInputResponse(ci1, KGetNegQoSEvent, KErrNone);
	
	
	
	// RPacketQoS::SetProfileParameters is configured to fail for the secondary context
	FsmInputRequest(ci1, SpudMan::EContextQoSSet, 0);
	WaitForFsmInputResponse(ci1, KContextQoSSetEvent, KErrGeneral);
	
	// RPacketQoS::CreateNewTFT is configured to fail with KErrGeneral for the secondary context
	TTFTOperationCode tftOperationCode = KAddFilters;
	iListener->iPdpFsmInterface->Set(ci1, tftOperationCode);
	TTFTInfo tftInfo;
	DefaultTftInfo(tftInfo);
	iListener->iPdpFsmInterface->Set(ci1, tftInfo);
	FsmInputRequest(ci1, SpudMan::EContextTFTModify, 0);
	WaitForFsmInputResponse(ci1, KContextTFTModifiedEvent, KErrGeneral);
	
	// Because the TFT and QoS have not changed, RPacketContext::ModifyActiveContext will fail
	FsmInputRequest(ci1, SpudMan::EContextModifyActive, 0);
	WaitForFsmInputResponse(ci1, KContextModifyActiveEvent, KErrNotReady);

	// Clean-up
	FsmInputRequest(ci0, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci0, KContextDeleteEvent, 0);

	FsmInputRequest(ci1, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci1, KContextDeleteEvent, 0);

	return TestStepResult();
	}

CSpudFsmUnitNotifications::~CSpudFsmUnitNotifications()
	{}

// configure to use test step 1 from simtsy config file
CSpudFsmUnitNotifications::CSpudFsmUnitNotifications() : CSpudFsmUnitTestStepBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmTestNotifications);
	}
	
TVerdict CSpudFsmUnitNotifications::doTestStepL()
/**
 Test that notifications are passed from etel driver
 */
 	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test notifications"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	TContextId ci0 = FsmObjectCreate();
	WaitForFsmObjectCreateResponse(ci0);

	TRequestStatus status;
	
	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// Test creating primary
	INFO_PRINTF1(_L("Create primary context"));
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrNone);

	RPacketQoS::TQoSR99_R4Negotiated qosNegotiated;

	//
	// Test service status notification
	//
	// Creating/activating a primary context should change the network status
	INFO_PRINTF1(_L("Wait for service status change notification"));
	WaitForFsmServiceNotificationStatus();


	InitPhoneAndPacketServiceL();
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);


	//
	// Test context config notifications
	//
	// Calling SetConfig to change the context config and trigger a context config notification
	INFO_PRINTF1(_L("Trigger context config change notification"));
	RPacketContext::TContextConfigGPRS contextConfig;
	DefaultContextConfigGPRS(contextConfig);
	TContextConfigGPRSPckg contextConfigPckg(contextConfig);
	iPrimaryPacketContext.SetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);
	// Wait for the notification
	INFO_PRINTF1(_L("Wait for context config change notification"));
	WaitForFsmContextConfigNotification(ci0, contextConfig);


	//
	// Test QoS notifications
	//
	// Calling SetProfileParameters to change the QoS and trigger a QoS change notification
	INFO_PRINTF1(_L("Trigger QoS change notification"));
	RPacketQoS primaryPacketQos;
	TEST(primaryPacketQos.OpenExistingQoS(iPrimaryPacketContext, KFirstQosName) == KErrNone);

	RPacketQoS::TQoSR99_R4Requested qosChangeRequested;
	SecondQoSR99_R4Requested(qosChangeRequested);
	TQoSR99_R4RequestedPckg qosChangeRequestedPckg(qosChangeRequested);
	primaryPacketQos.SetProfileParameters(status, qosChangeRequestedPckg);
	User::WaitForRequest(status);
	// Wait for the notification
	INFO_PRINTF1(_L("Wait for QoS change notification"));
	DefaultQoSR99_R4Negotiated(qosNegotiated);
	WaitForFsmQosChangeNotification(ci0, qosNegotiated);

	//
	// Clean-up
	//
	primaryPacketQos.Close();
	iPrimaryPacketContext.Close();

	FsmInputRequest(ci0, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci0, KContextDeleteEvent, 0);

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudFsmUnitNetworkDelete::~CSpudFsmUnitNetworkDelete()
	{}

// configure to use test step 108 from simtsy config file
CSpudFsmUnitNetworkDelete::CSpudFsmUnitNetworkDelete() : CSpudFsmUnitTestStepBase(108)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmNetworkDelete);
	}
	
TVerdict CSpudFsmUnitNetworkDelete::doTestStepL()
/**
 Test context delete from network
 */
 	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test delete context from network"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	FsmObjectCreate();
	TContextId ci0 = WaitForFsmObjectCreateResponse(0);
	FsmObjectCreate();
	TContextId ci1 = WaitForFsmObjectCreateResponse(1);

	// Set up context config and QoS parameters
	RPacketContext::TContextConfigGPRS contextConfigGPRS;
	DefaultContextConfigGPRS(contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigGPRS);
	iListener->iPdpFsmInterface->Set(ci1, contextConfigGPRS);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);
	iListener->iPdpFsmInterface->Set(ci1, qosRequested);

	// 
	// Test context delete while an operation is pending
	// 
	INFO_PRINTF1(_L("Test network delete context with pending operation"));
	// Activating the primary context is configured to take a very long time
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	// The packet service status will become attached almost immediately, but the create
	// primary context won't finish until it is active (which will take a long time)
	WaitForFsmServiceNotificationStatus();
	
	// This delete from the network will occur while the create primary is activating
	EtelRequest(EContextStatusChange, 0);


	// 
	// Test context delete while an operation is not pending
	// 
	INFO_PRINTF1(_L("Test network delete context from open state"));
	// Activating this context is configured to take a short period of time
	FsmInputRequest(ci1, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci1, KPrimaryContextCreated, KErrNone);
	
	EtelRequest(EContextStatusChange, 1);
	WaitForFsmInputResponse(ci1, KContextDeleteEvent, KErrNotReady);	

	// context should be deleted (and resources freed), so no need to delete here

	return TestStepResult();
	}

CSpudFsmUnitNotificationsR5OrR99R4::~CSpudFsmUnitNotificationsR5OrR99R4()
	{}

// configure to use test step 109 from simtsy config file
CSpudFsmUnitNotificationsR5OrR99R4::CSpudFsmUnitNotificationsR5OrR99R4() : CSpudFsmUnitTestStepBase(109)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudFsmTestNotificationsR5OrR99R4);
	}
	
TVerdict CSpudFsmUnitNotificationsR5OrR99R4::doTestStepL()
/**
 Test that notifications are passed from etel driver
 */
 	{
	INFO_PRINTF1(_L("Starting Spud Driver Unit test notifications Release 5/ Release 4"));
	
	// Start by creating all the Spud FSM objects.
	// They have to be created in the test's Active Scheduler Thread
	// (terminology from the test's design document).
	//
	TContextId ci0 = FsmObjectCreate();
	WaitForFsmObjectCreateResponse(ci0);

	TRequestStatus status;
	
	// Set up context config and QoS parameters
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketContext::TContextConfig_R5 contextConfig_R5;
	User::LeaveIfError(DefaultContextConfig_R5(contextConfig_R5));
	iListener->iPdpFsmInterface->Set(ci0, contextConfig_R5);
	
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
#else
    RPacketContext::TContextConfigR99_R4 contextConfigR99_R4;
	DefaultContextConfigR99_R4(contextConfigR99_R4);
	iListener->iPdpFsmInterface->Set(ci0, contextConfigR99_R4);

	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
#endif

	iListener->iPdpFsmInterface->Set(ci0, qosRequested);

	// Test creating primary
	INFO_PRINTF1(_L("Create primary context"));
	FsmInputRequest(ci0, SpudMan::ECreatePrimaryPDPContext, 0);
	WaitForFsmInputResponse(ci0, KPrimaryContextCreated, KErrNone);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Negotiated qosNegotiated;
#else
    RPacketQoS::TQoSR99_R4Negotiated qosNegotiated;
#endif
	//
	// Test service status notification
	//
	// Creating/activating a primary context should change the network status
	INFO_PRINTF1(_L("Wait for service status change notification"));
	WaitForFsmServiceNotificationStatus();


	InitPhoneAndPacketServiceL();
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);


	//
	// Test context config notifications
	//
	// Calling SetConfig to change the context config and trigger a context config notification
	INFO_PRINTF1(_L("Trigger context config change notification"));

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketContext::TContextConfig_R5 contextConfig;
	User::LeaveIfError(DefaultContextConfig_R5(contextConfig));
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckg(contextConfig);
#else
	RPacketContext::TContextConfigR99_R4 contextConfig;
	DefaultContextConfigR99_R4(contextConfig);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckg(contextConfig);
#endif

	iPrimaryPacketContext.SetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);
	// Wait for the notification
	INFO_PRINTF1(_L("Wait for context config change notification"));
	WaitForFsmContextConfigNotification(ci0, contextConfig);

	//
	// Test QoS notifications
	//
	// Calling SetProfileParameters to change the QoS and trigger a QoS change notification
	INFO_PRINTF1(_L("Trigger QoS change notification"));
	RPacketQoS primaryPacketQos;
	TEST(primaryPacketQos.OpenExistingQoS(iPrimaryPacketContext, KFirstQosName) == KErrNone);

#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosChangeRequested;
	SecondQoSR5Requested(qosChangeRequested);
	TQoSR5RequestedPckg qosChangeRequestedPckg(qosChangeRequested);
	primaryPacketQos.SetProfileParameters(status, qosChangeRequestedPckg);
	User::WaitForRequest(status);
	// Wait for the notification
	INFO_PRINTF1(_L("Wait for QoS change notification"));
	DefaultQoSR5Negotiated(qosNegotiated);
#else
    RPacketQoS::TQoSR99_R4Requested qosChangeRequested;
	SecondQoSR99_R4Requested(qosChangeRequested);
	TQoSR99_R4RequestedPckg qosChangeRequestedPckg(qosChangeRequested);
	primaryPacketQos.SetProfileParameters(status, qosChangeRequestedPckg);
	User::WaitForRequest(status);
	// Wait for the notification
	INFO_PRINTF1(_L("Wait for QoS change notification"));
	DefaultQoSR99_R4Negotiated(qosNegotiated);
#endif

	WaitForFsmQosChangeNotification(ci0, qosNegotiated);

	//
	// Clean-up
	//
	primaryPacketQos.Close();
	iPrimaryPacketContext.Close();

	FsmInputRequest(ci0, SpudMan::EContextDelete, 0);
	WaitForFsmInputResponse(ci0, KContextDeleteEvent, 0);

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

