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
// Unit tests for SPUD TEL
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

CSpudTelUnitNormalOperation::~CSpudTelUnitNormalOperation()
	{}

// configure to use test step 1 from simtsy config file
CSpudTelUnitNormalOperation::CSpudTelUnitNormalOperation() : CSpudTelUnitTestStepBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitNormalOperation);
	}
	
TVerdict CSpudTelUnitNormalOperation::doTestStepL()
/**
 Test normal operation - test normal setqos (before and after context created)
 					   - test normal settft (before and after context created)
 					   - test normal create primary context
 					   - test normal create secondary context
 */
	{
	INFO_PRINTF1(_L("Starting Spud Tel Test Normal Operation"));
	
	TRequestStatus status;
	RPacketService::TContextInfo contextInfo;

	InitPhoneAndPacketServiceL();

	//
	// Test creating primary context
	//
	INFO_PRINTF1(_L("Test creating primary context"));
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);
	
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first primary context with name 'PRIMARY1'
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);

	// check that context config for GPRS was set correctly
	RPacketContext::TContextConfigGPRS *contextConfig = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(contextConfig);
	TContextConfigGPRSPckg contextConfigPckg(*contextConfig);
	iPrimaryPacketContext.GetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);
	RPacketContext::TContextConfigGPRS *defaultContextConfigGPRS = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(defaultContextConfigGPRS);
	DefaultContextConfigGPRS(*defaultContextConfigGPRS);
	TEST(CompareContextConfigGPRS(*defaultContextConfigGPRS, *contextConfig));
	CleanupStack::PopAndDestroy(defaultContextConfigGPRS);
	CleanupStack::PopAndDestroy(contextConfig);


	//
	// Test creating secondary context
	//
	INFO_PRINTF1(_L("Test creating secondary context"));
	EtelDriverInputRequest(			KContextId1, 	ECreate2ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::E2ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first secondary context with name 'SECONDARY1'
	TEST(iSecondaryPacketContext.OpenExistingContext(iPacketService, KFirstSecondaryContextName) == KErrNone);
	

	//
	// Test setting and changing TFT
	//
	INFO_PRINTF1(_L("Test setting and changing TFT"));
	TTFTInfo *firstTftInfo = new(ELeave) TTFTInfo;
	CleanupStack::PushL(firstTftInfo);
	TTFTInfo *secondTftInfo = new(ELeave) TTFTInfo;
	CleanupStack::PushL(secondTftInfo);
	TTFTInfo *combinedTftInfo = new(ELeave) TTFTInfo;
	CleanupStack::PushL(combinedTftInfo);
	DefaultTftInfo(*firstTftInfo);
	SecondDefaultTftInfo(*secondTftInfo);
	CombinedDefaultTftInfo(*combinedTftInfo);

	// Test setting TFT
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, KAddFilters);
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, *firstTftInfo);
	EtelDriverInputRequest(KContextId0, ESetTft);
	WaitForEtelDriverInputResponse(KContextId0, PdpFsm::ETftSet);
	// check that TFT was set
	TTFTInfo expectedTftInfo;
	TEST(CompareTFTInfo(*firstTftInfo, iPrimaryPacketContext));
	
	// Test re-setting TFT
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, KAddFilters);
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, *secondTftInfo);
	EtelDriverInputRequest(KContextId0, ESetTft);
	WaitForEtelDriverInputResponse(KContextId0, PdpFsm::ETftSet);
	// check that TFT was set
	TEST(CompareTFTInfo(*secondTftInfo, iPrimaryPacketContext));
	
	// Test changing TFT (using KAddFilters)
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, KAddFilters);
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, *firstTftInfo);	
	EtelDriverInputRequest(KContextId0, EChangeTft);
	WaitForEtelDriverInputResponse(KContextId0, PdpFsm::ETftChanged);
	// check that TFT was set
	TEST(CompareTFTInfo(*combinedTftInfo, iPrimaryPacketContext));
	
	// Test setting TFT (using KRemoveFilters)
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, KRemoveFilters);
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, *firstTftInfo);
	EtelDriverInputRequest(KContextId0, EChangeTft);
	WaitForEtelDriverInputResponse(KContextId0, PdpFsm::ETftChanged);
	// check that TFT was set
	TEST(CompareTFTInfo(*secondTftInfo, iPrimaryPacketContext));

	// Test setting TFT (using KDeleteTFT)
	iListener->iPdpFsmInterfaceTest->Set(KContextId0, KDeleteTFT);
	EtelDriverInputRequest(KContextId0, EChangeTft);
	WaitForEtelDriverInputResponse(KContextId0, PdpFsm::ETftChanged);
	// check that TFT was set
	TInt count;
	iPrimaryPacketContext.EnumeratePacketFilters(status, count);
	User::WaitForRequest(status);
	TEST(status == KErrNone);
	TEST(count == 0);
	CleanupStack::PopAndDestroy(combinedTftInfo);
	CleanupStack::PopAndDestroy(secondTftInfo);
	CleanupStack::PopAndDestroy(firstTftInfo);


	//
	// Test setting QOS
	//
	INFO_PRINTF1(_L("Test setting QOS"));
	EtelDriverInputRequest(		KContextId1, 	ESetQoS);
	RPacketQoS::TQoSR99_R4Negotiated *defaultQoSR99_R4Negotiated = new(ELeave) RPacketQoS::TQoSR99_R4Negotiated;
	CleanupStack::PushL(defaultQoSR99_R4Negotiated);
	SecondQoSR99_R4Negotiated(*defaultQoSR99_R4Negotiated);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EQoSSet);
	RPacketQoS packetQos;
	// check that QOS was created - the simtsy creates the first QOS with name KFirstQosName
	TEST(packetQos.OpenExistingQoS(iSecondaryPacketContext, KFirstQosName) == KErrNone);
	// qos was created ok
	RPacketQoS::TQoSR99_R4Negotiated *qos = new(ELeave) RPacketQoS::TQoSR99_R4Negotiated;
	CleanupStack::PushL(qos);
	TQoSR99_R4NegotiatedPckg qosPckg(*qos);
	packetQos.GetProfileParameters(status, qosPckg);
	User::WaitForRequest(status);
	TEST(CompareUMTSQoSNeg(*defaultQoSR99_R4Negotiated, *qos));
	CleanupStack::PopAndDestroy(qos);
	CleanupStack::PopAndDestroy(defaultQoSR99_R4Negotiated);
	packetQos.Close();
	iSecondaryPacketContext.Close();
	iPrimaryPacketContext.Close();

	
	//
	// Test activating context
	//
	INFO_PRINTF1(_L("Test activating context"));
	EtelDriverInputRequest(			KContextId1, 	EActivatePdp);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EPdpActivated);
	
	// test that context is set to activated
	TEST(CompareContextInfo(iPacketService, KFirstSecondaryContextName, RPacketContext::EStatusActive));
	// make sure the data channel was set as well
	RPacketContext::TDataChannelV2 dataChannel;
	DefaultDataChannel(dataChannel);
	TEST(CompareDataChannel(iListener->iNotifyContextDataChannelData.iDataChannelV2, dataChannel));
	
	
	//
	// Test modifying active context
	//
	INFO_PRINTF1(_L("Test modifying activate context"));
	EtelDriverInputRequest(KContextId1, EChangeTft);
	WaitForEtelDriverInputResponse(KContextId1, PdpFsm::ETftChanged);
	EtelDriverInputRequest(			KContextId1, 	EModifyActive);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EPdpContextModified);
	
	
	//
	// Test deleting secondary context
	//
	INFO_PRINTF1(_L("Test deleting secondary context"));
	EtelDriverInputRequest(			KContextId1, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EContextDeleted);

	// make sure it is marked as deleted
	iPacketService.GetContextInfo(status, KContextId1, contextInfo);
	User::WaitForRequest(status);
	TEST(status == KErrNone && contextInfo.iStatus == RPacketContext::EStatusDeleted);
	

	//
	// Test deleting primary context
	//
	INFO_PRINTF1(_L("Test deleting primary context"));
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);
	
	// make sure it is marked as deleted
	iPacketService.GetContextInfo(status, KContextId0, contextInfo);
	User::WaitForRequest(status);
	TEST(status == KErrNone && contextInfo.iStatus == RPacketContext::EStatusDeleted);
	
	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitNotifications::~CSpudTelUnitNotifications()
	{}

// configure to use test step 1 from simtsy config file
CSpudTelUnitNotifications::CSpudTelUnitNotifications() : CSpudTelUnitTestStepBase(1)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitNotifications);
	}
	
TVerdict CSpudTelUnitNotifications::doTestStepL()
/**
 Test notifications
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Notifications"));
	
	TRequestStatus status;
	RPacketQoS packetQos;

	InitPhoneAndPacketServiceL();

	//
	// setup test - create primary context and start notifications
	//
	INFO_PRINTF1(_L("setup test - create primary context and start notifications"));
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	
	EtelDriverInputRequest(			KContextId0, 	EActivatePdp);	
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPdpActivated);
	
	// check that context was created - the simtsy creates the first primary context with name 'PRIMARY1'
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	
	//
	// Test service status notification
	//
	// the timing here may or may not allow us to recieve an 'activating' request
	WaitForNextServiceNotificationStatus(	RPacketService::EStatusActive);


	//
	// Test context config notification
	//
	INFO_PRINTF1(_L("Test context config notification"));
	RPacketContext::TContextConfigGPRS contextConfig;
	DefaultContextConfigGPRS(contextConfig);
	TContextConfigGPRSPckg contextConfigPckg(contextConfig);
	iPrimaryPacketContext.SetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);

	WaitForNextContextConfigNotification(KContextId0, contextConfig);
	
	// check that QOS was created - the simtsy creates the first QOS with name KFirstQosName
	TEST(packetQos.OpenExistingQoS(iPrimaryPacketContext, KFirstQosName) == KErrNone);


	//
	// Test QOS notification
	//

	INFO_PRINTF1(_L("Test QOS notification"));
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
	RPacketQoS::TQoSR99_R4Negotiated qosNegotiated;
	SecondQoSR99_R4Negotiated(qosNegotiated);
	TQoSR99_R4RequestedPckg qosRequestedPckg(qosRequested);
	packetQos.SetProfileParameters(status, qosRequestedPckg);
	User::WaitForRequest(status);
	WaitForNextQosNotification(KContextId0, qosNegotiated);

	// clean up
	packetQos.Close();
	iPrimaryPacketContext.Close();
	
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);
	WaitForNextServiceNotificationStatus(RPacketService::EStatusAttached);

	DestroyPhoneAndPacketService();
	
	return TestStepResult();
	}

CSpudTelUnitCreatePrimaryError::~CSpudTelUnitCreatePrimaryError()
	{
	}

// configure to use test step 3 from simtsy config file
CSpudTelUnitCreatePrimaryError::CSpudTelUnitCreatePrimaryError() : CSpudTelUnitTestStepBase(3)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitCreatePrimaryError);
	}
	
TVerdict CSpudTelUnitCreatePrimaryError::doTestStepL()
/**
 Errors with creating primary
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Create Primary Context Error with SetConfig"));
	
	InitPhoneAndPacketServiceL();

	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	// simtsy is configured to fail second RPacketContext::SetConfig
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreatedFailed, KErrGeneral);
	
	// clean up
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);	
	
	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitSetQosError::~CSpudTelUnitSetQosError()
	{}

// configure to use test step 6 from simtsy config file
CSpudTelUnitSetQosError::CSpudTelUnitSetQosError() : CSpudTelUnitTestStepBase(6)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitSetQosError);
	}
	
TVerdict CSpudTelUnitSetQosError::doTestStepL()
/**
 Errors with setting QoS
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Set QOS Error"));
	
	InitPhoneAndPacketServiceL();

	// set test up
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreatedFailed, KErrGeneral);

	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);	

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitSetTftError::~CSpudTelUnitSetTftError()
	{
	}

// configure to use test step 7 from simtsy config file
CSpudTelUnitSetTftError::CSpudTelUnitSetTftError() : CSpudTelUnitTestStepBase(7)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitSetTftError);
	}
	
TVerdict CSpudTelUnitSetTftError::doTestStepL()
/**
 Errors with setting TFT
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Set TFT Error"));
	
	InitPhoneAndPacketServiceL();

	// set test up
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);

	// test error in RPacketQoS::CreateNewTFT
	EtelDriverInputRequest(			KContextId0, 	ESetTft);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::ETftSetFailed, KErrGeneral);

	// test error in RPacketQoS::AddPacketFilter
	EtelDriverInputRequest(			KContextId0, 	ESetTft);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::ETftSetFailed, KErrGeneral);

	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);	

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitActivateError1::~CSpudTelUnitActivateError1()
	{
	}

// configure to use test step 8 from simtsy config file
CSpudTelUnitActivateError1::CSpudTelUnitActivateError1() : CSpudTelUnitTestStepBase(8)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitActivateError1);
	}
	
TVerdict CSpudTelUnitActivateError1::doTestStepL()
/**
 *	Errors with activating
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Activate Error 1"));
	
	InitPhoneAndPacketServiceL();

	// set test up
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);

	// test error in RPacketContext::Activate
	EtelDriverInputRequest(			KContextId0, 	EActivatePdp);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPdpActivatedFailed, KErrGeneral);

	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);	

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitActivateError2::~CSpudTelUnitActivateError2()
	{
	}

// configure to use test step 8 from simtsy config file
CSpudTelUnitActivateError2::CSpudTelUnitActivateError2() : CSpudTelUnitTestStepBase(14)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitActivateError2);
	}
	
TVerdict CSpudTelUnitActivateError2::doTestStepL()
/**
 *	Errors with activating
 */
    {
    INFO_PRINTF1(_L("Starting Spud Test Activate Error 2"));
    
    InitPhoneAndPacketServiceL();

    // set test up
    EtelDriverInputRequest(         KContextId0,    EOpenPhone);
    WaitForEtelDriverInputResponse( KContextId0,    PdpFsm::EPhoneOpened);

    EtelDriverInputRequest(         KContextId0,    ECreate1ryPdpContext);
    WaitForEtelDriverInputResponse( KContextId0,    PdpFsm::E1ryPdpContextCreatedFailed, KErrGeneral);

    EtelDriverInputRequest(         KContextId0,    EtelDriver::EContextDelete);
    WaitForEtelDriverInputResponse( KContextId0,    PdpFsm::EContextDeleted);   

    DestroyPhoneAndPacketService();

    return TestStepResult();
    }



CSpudTelUnitDeleteError1::~CSpudTelUnitDeleteError1()
	{
	}

// configure to use test step 9 from simtsy config file
CSpudTelUnitDeleteError1::CSpudTelUnitDeleteError1() : CSpudTelUnitTestStepBase(9)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitDeleteError1);
	}
	
TVerdict CSpudTelUnitDeleteError1::doTestStepL()
/**
 Errors with deleting
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Delete Error"));
	
	InitPhoneAndPacketServiceL();

	// set test up phone, primary context, it's QoS parameters, and activate the context
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	EtelDriverInputRequest(			KContextId0, 	ESetQoS);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EQoSSet);
	EtelDriverInputRequest(			KContextId0, 	EActivatePdp);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPdpActivated);

	// test error in RPacketContext::Deactivate
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeletedFailed, KErrGeneral);

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitDeleteError2::~CSpudTelUnitDeleteError2()
	{
	}

// configure to use test step 13 from simtsy config file
CSpudTelUnitDeleteError2::CSpudTelUnitDeleteError2() : CSpudTelUnitTestStepBase(10)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitDeleteError2);
	}
	
TVerdict CSpudTelUnitDeleteError2::doTestStepL()
/**
 Errors with deleting
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Delete Error 2"));
	
	InitPhoneAndPacketServiceL();

	// set test up
	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	EtelDriverInputRequest(			KContextId0, 	EActivatePdp);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPdpActivated);

	// test error in RPacketContext::Delete
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeletedFailed, KErrGeneral);

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitMultipleContexts::~CSpudTelUnitMultipleContexts()
	{
	}

// configure to use test step 10 from simtsy config file
CSpudTelUnitMultipleContexts::CSpudTelUnitMultipleContexts() : CSpudTelUnitTestStepBase(11)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitMultipleContexts);
	}
	
TVerdict CSpudTelUnitMultipleContexts::doTestStepL()
/**
 Test multiple primary contexts with multiple secondary contexts
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Multiple Contexts"));
	
	TRequestStatus status;

	InitPhoneAndPacketServiceL();
	RPacketContext primaryPacketContext2, secondaryPacketContext2;

	EtelDriverInputRequest(			KContextId0, 	EOpenPhone);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPhoneOpened);

	//
	// Test creating multiple primary contexts
	//
	INFO_PRINTF1(_L("Test create first primary context"));
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first primary context with name 'PRIMARY1'
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);

	// check that context config for GPRS was set correctly
	RPacketContext::TContextConfigGPRS *contextConfig = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(contextConfig);
	TContextConfigGPRSPckg contextConfigPckg(*contextConfig);
	iPrimaryPacketContext.GetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);
	RPacketContext::TContextConfigGPRS *defaultTestContextConfigGPRS = new(ELeave) RPacketContext::TContextConfigGPRS;
	CleanupStack::PushL(defaultTestContextConfigGPRS);
	DefaultContextConfigGPRS(*defaultTestContextConfigGPRS);
	TEST(CompareContextConfigGPRS(*defaultTestContextConfigGPRS, *contextConfig));

	INFO_PRINTF1(_L("Test create second primary context"));
	EtelDriverInputRequest(			KContextId1, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::E1ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first primary context with name 'PRIMARY2'
	TEST(primaryPacketContext2.OpenExistingContext(iPacketService, KSecondPrimaryContextName) == KErrNone);

	// check that context config for GPRS was set correctly
	primaryPacketContext2.GetConfig(status, contextConfigPckg);
	User::WaitForRequest(status);
	TEST(CompareContextConfigGPRS(*defaultTestContextConfigGPRS, *contextConfig));
	CleanupStack::PopAndDestroy(defaultTestContextConfigGPRS);
	CleanupStack::PopAndDestroy(contextConfig);

	
	//
	// Test creating secondary contexts on each primary context
	//
	INFO_PRINTF1(_L("Test create first secondary context"));
	EtelDriverInputRequest(			KContextId2, 	ECreate2ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId2, 	PdpFsm::E2ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first secondary context with name 'SECONDARY1'
	TEST(iSecondaryPacketContext.OpenExistingContext(iPacketService, KFirstSecondaryContextName) == KErrNone);
	
	INFO_PRINTF1(_L("Test create second secondary context"));
	EtelDriverInputRequest(			KContextId3, 	ECreate2ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId3, 	PdpFsm::E2ryPdpContextCreated);
	
	// check that context was created - the simtsy creates the first secondary context with name 'SECONDARY2'
	TEST(secondaryPacketContext2.OpenExistingContext(iPacketService, KSecondSecondaryContextName) == KErrNone);
	

	//
	// Test setting TFT (this time on secondary context)
	//
	INFO_PRINTF1(_L("Test setting TFT"));
	EtelDriverInputRequest(		KContextId3, 		ESetTft);
	WaitForEtelDriverInputResponse(	KContextId3, 	PdpFsm::ETftSet);
	
	// check that TFT was set
	TTFTInfo *expectedTftInfo = new(ELeave) TTFTInfo;
	CleanupStack::PushL(expectedTftInfo);
	DefaultTftInfo(*expectedTftInfo);
	TEST(CompareTFTInfo(*expectedTftInfo, secondaryPacketContext2));
	CleanupStack::PopAndDestroy(expectedTftInfo);
	

	//
	// Test setting QOS (this time on primary context)
	//
	INFO_PRINTF1(_L("Test setting QOS"));
	EtelDriverInputRequest(		KContextId1, 		ESetQoS);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EQoSSet);
	RPacketQoS packetQos;
	// check that QOS was created - the simtsy creates the first QOS with name KFirstQosName
	TEST(packetQos.OpenExistingQoS(primaryPacketContext2, KFirstQosName) == KErrNone);
	// qos was created ok
	RPacketQoS::TQoSR99_R4Negotiated *qos = new(ELeave) RPacketQoS::TQoSR99_R4Negotiated;
	CleanupStack::PushL(qos);
	TQoSR99_R4NegotiatedPckg qosPckg(*qos);
	packetQos.GetProfileParameters(status, qosPckg);
	User::WaitForRequest(status);
	RPacketQoS::TQoSR99_R4Negotiated *defaultTestTQoSR99_R4Negotiated = new(ELeave) RPacketQoS::TQoSR99_R4Negotiated;
	CleanupStack::PushL(defaultTestTQoSR99_R4Negotiated);
	SecondQoSR99_R4Negotiated(*defaultTestTQoSR99_R4Negotiated);
	TEST(CompareUMTSQoSNeg(*defaultTestTQoSR99_R4Negotiated, *qos));
	packetQos.Close();
	CleanupStack::PopAndDestroy(defaultTestTQoSR99_R4Negotiated);
	CleanupStack::PopAndDestroy(qos);
	primaryPacketContext2.Close();
	secondaryPacketContext2.Close();
	iPrimaryPacketContext.Close();
	iSecondaryPacketContext.Close();

	
	//
	// Test activating contexts
	//
	INFO_PRINTF1(_L("Test activating contexts"));
	EtelDriverInputRequest(			KContextId1, 	EActivatePdp);
	// The service is now attached
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EPdpActivated);
	
	// test that context is set to activated
	TEST(CompareContextInfo(iPacketService, KSecondPrimaryContextName, RPacketContext::EStatusActive));
	RPacketContext::TDataChannelV2 dataChannel;
	DefaultDataChannel(dataChannel);
	TEST(CompareDataChannel(iListener->iNotifyContextDataChannelData.iDataChannelV2, dataChannel));
	
	EtelDriverInputRequest(			KContextId2, 	EActivatePdp);
	WaitForEtelDriverInputResponse(	KContextId2, 	PdpFsm::EPdpActivated);
	
	// test that context is set to activated
	TEST(CompareContextInfo(iPacketService, KFirstSecondaryContextName, RPacketContext::EStatusActive));

	
	//
	// Test deleting primary context before secondary contexts
	//
	INFO_PRINTF1(_L("Test deleting primary contexts before secondary contexts"));
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);

	// make sure it is marked as deleted
	TEST(CompareContextInfo(iPacketService, KFirstPrimaryContextName, RPacketContext::EStatusDeleted));
	
	EtelDriverInputRequest(			KContextId1, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EContextDeleted);

	// make sure it is marked as deleted
	TEST(CompareContextInfo(iPacketService, KSecondPrimaryContextName, RPacketContext::EStatusDeleted));
	

	//
	// Test deleting secondary contexts
	//
	INFO_PRINTF1(_L("Test deleting secondary contexts"));
	EtelDriverInputRequest(			KContextId2, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId2, 	PdpFsm::EContextDeleted);
	
	// make sure it is marked as deleted
	TEST(CompareContextInfo(iPacketService, KFirstSecondaryContextName, RPacketContext::EStatusDeleted));
	
	EtelDriverInputRequest(			KContextId3, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId3, 	PdpFsm::EContextDeleted);
	
	// make sure it is marked as deleted
	TEST(CompareContextInfo(iPacketService, KSecondSecondaryContextName, RPacketContext::EStatusDeleted));

	DestroyPhoneAndPacketService();

	return TestStepResult();
	}

CSpudTelUnitCancel::~CSpudTelUnitCancel()
	{
	}

// configure to use test step 12 from simtsy config file
CSpudTelUnitCancel::CSpudTelUnitCancel() : CSpudTelUnitTestStepBase(12)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitCancel);
	}
	
TVerdict CSpudTelUnitCancel::doTestStepL()
/**
 Test cancelling operations
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Cancel on Input Operations"));
	
	TRequestStatus timerStatus;
	RTimer timer;
	timer.CreateLocal();

	InitPhoneAndPacketServiceL();

	//
	// Test cancelling create primary request
	//
	INFO_PRINTF1(_L("Test cancelling create primary request"));
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	CancelPdpRequest(KContextId0);
	timer.After(timerStatus, 1500000);
	User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	TEST(timerStatus == KErrNone && iListener->iEtelDriverInputResponseStatus == KRequestPending);
	

	//
	// Set up primary for upcoming tests
	//
	// 
	INFO_PRINTF1(_L("Create primary"));
	EtelDriverInputRequest(			KContextId1, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::E1ryPdpContextCreated);

	// since creating secondary contains no asyncronous requests, the request cannot be cancelled
	//EtelDriverInputRequest(			KContextId2, 	ECreate2ryPdpContext);
	//CancelPdpRequest(KContextId2);
	//timer.After(timerStatus, 1500000);
	//User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	//TEST(iListener->iEtelDriverInputResponseStatus == KErrNone);


	//
	// Test cancelling set TFT
	//
	INFO_PRINTF1(_L("Test cancelling set TFT"));
	EtelDriverInputRequest(	KContextId1, 	ESetTft);
	CancelPdpRequest(KContextId1);
	timer.After(timerStatus, 1500000);
	User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	TEST(timerStatus == KErrNone && iListener->iEtelDriverInputResponseStatus == KRequestPending);
	

	//
	// Test cancelling set QOS
	//
	//INFO_PRINTF1(_L("Test cancelling set QOS"));
	//EtelDriverInputRequest(	KContextId1, 	ESetQoS);
	//CancelPdpRequest(KContextId1);
	//timer.After(timerStatus, 1500000);
	//User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	//TEST(timerStatus == KErrNone && iListener->iEtelDriverInputResponseStatus == KRequestPending);
	

	//
	// Test cancelling activate PDP
	//
	INFO_PRINTF1(_L("Test cancelling create secondary request"));
	EtelDriverInputRequest(	KContextId1, 	EActivatePdp);
	CancelPdpRequest(KContextId1);
	timer.After(timerStatus, 1500000);
	User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	TEST(timerStatus == KErrNone && iListener->iEtelDriverInputResponseStatus == KRequestPending);


	//
	// Test cancelling context delete
	//
	INFO_PRINTF1(_L("Test cancelling context delete"));
	EtelDriverInputRequest(	KContextId1, 	EtelDriver::EContextDelete);
	CancelPdpRequest(KContextId1);
	timer.After(timerStatus, 1500000);
	User::WaitForRequest(iListener->iEtelDriverInputResponseStatus, timerStatus);
	TEST(timerStatus == KErrNone && iListener->iEtelDriverInputResponseStatus == KRequestPending);

	// clean up
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);	
	EtelDriverInputRequest(			KContextId1, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId1, 	PdpFsm::EContextDeleted);	

	DestroyPhoneAndPacketService();

	timer.Close();
	
	return TestStepResult();
	}

CSpudTelUnitNotificationsR5OrR99R4::~CSpudTelUnitNotificationsR5OrR99R4()
	{}

// configure to use test step 1 from simtsy config file
CSpudTelUnitNotificationsR5OrR99R4::CSpudTelUnitNotificationsR5OrR99R4() : CSpudTelUnitTestStepBase(109)
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSpudTelUnitNotificationsR5OrR99R4);
	}
	
TVerdict CSpudTelUnitNotificationsR5OrR99R4::doTestStepL()
/**
 Test notifications
 */
	{
	INFO_PRINTF1(_L("Starting Spud Test Notifications for Release 5"));
	
	TRequestStatus status;
	RPacketQoS packetQos;

	InitPhoneAndPacketServiceL();
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketContext::TContextConfig_R5 contextConfig;
	User::LeaveIfError(DefaultContextConfig_R5(contextConfig));
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckg(contextConfig);
	iPacketService.SetDefaultContextParams(status, contextConfigPckg);
#else
	RPacketContext::TContextConfigR99_R4 contextConfig;
	DefaultContextConfigR99_R4(contextConfig);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckg(contextConfig);
	iPacketService.SetDefaultContextParams(status, contextConfigPckg);
#endif
	
	User::WaitForRequest(status);
	
	//
	// setup test - create primary context and start notifications
	//

	INFO_PRINTF1(_L("setup test - create primary context and start notifications"));
	EtelDriverInputRequest(			KContextId0, 	ECreate1ryPdpContext);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::E1ryPdpContextCreated);
	
	EtelDriverInputRequest(			KContextId0, 	EActivatePdp);	
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EPdpActivated);
	
	// check that context was created - the simtsy creates the first primary context with name 'PRIMARY1'
	TEST(iPrimaryPacketContext.OpenExistingContext(iPacketService, KFirstPrimaryContextName) == KErrNone);
	
	// Test service status notification
	//
	// the timing here may or may not allow us to recieve an 'activating' request
	
	WaitForNextServiceNotificationStatus(	RPacketService::EStatusActive);


	//
	// Test context config notification
	//
	INFO_PRINTF1(_L("Test context config notification"));
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketContext::TContextConfig_R5 contextConfig2;
	User::LeaveIfError(DefaultContextConfig_R5(contextConfig2));
	TPckg<RPacketContext::TContextConfig_R5> contextConfigPckg2(contextConfig2);
#else
	RPacketContext::TContextConfigR99_R4 contextConfig2;
	DefaultContextConfigR99_R4(contextConfig2);
	TPckg<RPacketContext::TContextConfigR99_R4> contextConfigPckg2(contextConfig2);
#endif
	iPrimaryPacketContext.SetConfig(status, contextConfigPckg2);
	User::WaitForRequest(status);

	WaitForNextContextConfigNotification(KContextId0, contextConfig2);
	
	// check that QOS was created - the simtsy creates the first QOS with name KFirstQosName
	TEST(packetQos.OpenExistingQoS(iPrimaryPacketContext, KFirstQosName) == KErrNone);


	//
	// Test QOS notification
	//

	INFO_PRINTF1(_L("Test QOS notification"));
#ifdef SYMBIAN_NETWORKING_UMTSR5
	RPacketQoS::TQoSR5Requested qosRequested;
	DefaultQoSR5Requested(qosRequested);
	RPacketQoS::TQoSR5Negotiated qosNegotiated;
	SecondQoSR5Negotiated(qosNegotiated);
	TQoSR5RequestedPckg qosRequestedPckg(qosRequested);
#else
	RPacketQoS::TQoSR99_R4Requested qosRequested;
	DefaultQoSR99_R4Requested(qosRequested);
	RPacketQoS::TQoSR99_R4Negotiated qosNegotiated;
	SecondQoSR99_R4Negotiated(qosNegotiated);
	TQoSR99_R4RequestedPckg qosRequestedPckg(qosRequested);
#endif
	packetQos.SetProfileParameters(status, qosRequestedPckg);
	User::WaitForRequest(status);
	WaitForNextQosNotification(KContextId0, qosNegotiated);

	// clean up
	packetQos.Close();
	iPrimaryPacketContext.Close();
	
	EtelDriverInputRequest(			KContextId0, 	EtelDriver::EContextDelete);
	WaitForEtelDriverInputResponse(	KContextId0, 	PdpFsm::EContextDeleted);
	WaitForNextServiceNotificationStatus(RPacketService::EStatusAttached);

	DestroyPhoneAndPacketService();
	
	return TestStepResult();
	}
