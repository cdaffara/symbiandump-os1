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
// Test step definitions for the PhoneControl functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestphonecontrol.h"



	
CCTSYIntegrationTestPhoneControlBase::CCTSYIntegrationTestPhoneControlBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneControlBase::~CCTSYIntegrationTestPhoneControlBase()
/*
 * Destructor
 */
	{
	}
	

CCTSYIntegrationTestPhoneControl0001::CCTSYIntegrationTestPhoneControl0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneControl0001::GetTestStepName());
	}

CCTSYIntegrationTestPhoneControl0001::~CCTSYIntegrationTestPhoneControl0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneControl0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PCON-0001
 * @SYMFssID BA/CTSY/PCON-0001
 * @SYMTestCaseDesc Get the TSY's supported modules.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - ETrue returned for supported modules, EFalse otherwise.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify correct modules are supported.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Get the TelServer 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Check RTelServer::IsSupportedByModule with aMixin=KETelExtMultimodeV1 returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileIccAccess returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileNetwork returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileIdentity returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobilePower returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileSignal returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileDTMF returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileUserNetworkAccess returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileIdentityService returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileCallForwarding returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileCallBarring returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileCallWaiting returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileCallCompletion returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileSecurity returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileAlternateLineService returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileFixedDiallingNumbers returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileDataCall returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileEmergencyCall returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileSmsMessaging returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileBroadcastMessaging returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileUssdMessaging returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileConferenceCall returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobilePhonebook returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileSmsStore returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileOwnNumberStore returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileEmergencyNumberStore returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileAlternatingCall returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileMessageWaiting returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileNamStore returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileMultimediaCall returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileIndicator returns aResult=ETrue // Are supported by CTSY but CTSY returns false
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobilePrivacy returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileCost returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileMulticall returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileNextIncomingCall returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelFuncMobileUserSignalling returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KETelExtMultimodeV2 returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileNetworkSecurity returns aResult=ETrue // Are supported by CTSY but CTSY returns false
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileMultimediaCallSettings returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileUSIMApplications returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KETelExtMultimodeV3 returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileAPNControlList returns aResult=ETrue
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileMMSInformation returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileAirTime returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileAutoRedial returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobilePersonalisation returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileMailboxNumbers returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KETelExtMultimodeV4 returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileIMS returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelFuncMobileSmartCardApplications returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KEtelExtMultimodeV5 returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KEtelExtMultimodeV6 returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelSmartCardEap returns aResult=EFalse

	// Check RTelServer::IsSupportedByModule with aMixin=KEtelExtMultimodeV7 returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KEtelLocationControlServices returns aResult=EFalse
	// Check RTelServer::IsSupportedByModule with aMixin=KETelExt3rdPartyV1 returns aResult=EFalse

	//   

	//   

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneControl0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneControl0001");
	}



CCTSYIntegrationTestPhoneControl0002::CCTSYIntegrationTestPhoneControl0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneControlBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneControl0002::GetTestStepName());
	}

CCTSYIntegrationTestPhoneControl0002::~CCTSYIntegrationTestPhoneControl0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneControl0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PCON-0002
 * @SYMFssID BA/CTSY/PCON-0002
 * @SYMTestCaseDesc Reinitialise the phone.
 * @SYMTestPriority High
 * @SYMTestActions RPhone::InitialiseCancel
 * @SYMTestExpectedResults Pass - Reinitialisation completes with KErrNone. Cancel request completes with KErrCancel.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify initialise request completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	
	// Ensure TSY is loaded. 


	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Reinitialise with synchronous version ===

	// Open RPhone subsession. 

	// Reinitialise the phone with synchronous version of RPhone::Initialise  

	// Check KErrNone is returned

	// ===  Reinitialise with async version ===

	// Reinitialise the phone with asynchronous version of RPhone::Initialise 

	// Check request completes with KErrNone

	// ===  Cancel reinitialisation ===

	// Reinitialise the phone with asynchronous version of RPhone::Initialise 

	// Cancel initialisation with RPhone::InitialiseCancel 

	// Check request completes with KErrCancel

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Put any required test clean up here, then remove this comment
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneControl0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneControl0002");
	}



