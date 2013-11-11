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
// Test step definitions for the CallForwarding functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallforwarding.h"

#include "cctsyinidata.h"

#include "listretrievalhelpers.h"


CCTSYIntegrationTestCallForwardingBase::CCTSYIntegrationTestCallForwardingBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iCallForwardingSupplementalHelper(*this), iCallForwardingControlHelper(*this), iNetworkTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallForwardingBase::~CCTSYIntegrationTestCallForwardingBase()
/*
 * Destructor
 */
	{
	}

TBool CCTSYIntegrationTestCallForwardingBase::SetCallForwardingStatusUtilL(RMobilePhone& aMobilePhone, RMobilePhone::TMobilePhoneCFChangeV1 aNewSetting, RMobilePhone::TMobilePhoneCFCondition aCFConditionSet, TInt aExpected=KErrNone)
/**
 * Util function which performs a SetCallForwardingStatus operation and relevant
 * checks using the arguments passed.
 *
 * @param aMobilePhone Mobile phone
 * @param aNewSetting The new setting to be set
 * @param aCFConditionSet The call forwarding condition.
 * @param aExpected A expect value for the status
 * @return TBool which represents whether cfCondition==aCFConditionSet
 */
	{
	// Post notification for RMobilePhone::NotifyCallForwardingStatusChange

	TExtEtelRequestStatus notifyCallForwardStatus(aMobilePhone, EMobilePhoneNotifyCallForwardingStatusChange);
	RMobilePhone::TMobilePhoneCFCondition cfCondition;
	CleanupStack::PushL(notifyCallForwardStatus);
	
	if(aExpected==KErrNone)
		{
		aMobilePhone.NotifyCallForwardingStatusChange(notifyCallForwardStatus, cfCondition);	
		}
	
	TExtEtelRequestStatus setCallForwardStatus(aMobilePhone, EMobilePhoneSetCallForwardingStatus);
	CleanupStack::PushL(setCallForwardStatus);
	aMobilePhone.SetCallForwardingStatus(setCallForwardStatus, aCFConditionSet, aNewSetting);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	ASSERT_EQUALS(setCallForwardStatus.Int(), aExpected, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));
	
	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with cfcondition==aCFConditionSet
	if(aExpected==KErrNone)
		{
		iCallForwardingSupplementalHelper.WaitForMobilePhoneNotifyCallForwardingStatusChange(aMobilePhone,
																						 notifyCallForwardStatus,
																						 cfCondition,
																						 aCFConditionSet,
																					 	 KErrNone);	
		}
		CleanupStack::PopAndDestroy(&setCallForwardStatus);
		CleanupStack::PopAndDestroy(&notifyCallForwardStatus);
	
	if(aExpected==KErrNone)
		{	
		return cfCondition==aCFConditionSet;
		}
	else
		{
		return ETrue;
		}	
	}

CCTSYIntegrationTestCallForwarding0001::CCTSYIntegrationTestCallForwarding0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0001::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0001::~CCTSYIntegrationTestCallForwarding0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0001
 * @SYMFssID BA/CTSY/CFOR-0001
 * @SYMTestCaseDesc Register call forwarding for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is registered for all services with each condition.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding is registered and call forwarding notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		
	TUint32 callServiceCaps = 0;
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);
	
	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	// ===  Register call forwarding and check notification completes ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister,  aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
	
	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingUnconditional
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionRegister;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;

	// $CTSYProblem The status returned from SetCallForwardingStatus does not return KErrNone.
	// Instead the status varies depending on what iAction is requested. It is presumed that this is CTSY problem.
	// Have had to change the test to expect the current retrieved value. 
	// The error codes are being returned by the LTSY and the CTSY is propagating them back to the client correctly
	// therefore these error codes do not indicate an error in the CTSY but rather that the LTSY / network is not supporting the requested operation. 
	if(operatorName==EOperatorVodafone)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
	
		// Repeat whole test with aCondition in RMobilePhone::SetCallForwardingStatus and RMobilePhone::NotifyCallForwardingStatusChange =ECallForwardingBusy, ECallForwardingNoReply, ECallForwardingNotReachable, ECallForwardingAllConditionalCases 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNoReply for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNoReply for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNotReachable for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNotReachable for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingAllConditionalCases for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllConditionalCases, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingAllConditionalCases for EAllServices failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
	
		// Repeat whole test with aCondition in RMobilePhone::SetCallForwardingStatus and RMobilePhone::NotifyCallForwardingStatusChange =ECallForwardingBusy, ECallForwardingNoReply, ECallForwardingNotReachable, ECallForwardingAllConditionalCases 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNoReply for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNoReply for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNotReachable for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNotReachable for EAllServices failed"));
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingAllConditionalCases for EAllServices"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllConditionalCases, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingAllConditionalCases for EAllServices failed"));	
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0001");
	}



CCTSYIntegrationTestCallForwarding0002::CCTSYIntegrationTestCallForwarding0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0002::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0002::~CCTSYIntegrationTestCallForwarding0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0002
 * @SYMFssID BA/CTSY/CFOR-0002
 * @SYMTestCaseDesc Register call forwarding for each provisioned basic service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is registered for each provisioned basic service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding is registered and call forwarding notification completes.
 *
 * @return - TVerdict code
 */
	{
	
	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 callServiceCaps = 0;	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsGetCFStatusNetwork;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 
	
	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
	  
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	// ===  Register call forwarding and check notification completes ===
	
	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionRegister;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	// $CTSYProblem The status returned from SetCallForwardingStatus does not return KErrNone.
	// Instead the status varies depending on what iAction is requested. It is presumed that this is CTSY problem.
	// Have had to change the test to expect the current retrieved value. 
	// The error codes are being returned by the LTSY and the CTSY is propagating them back to the client correctly
	// therefore these error codes do not indicate an error in the CTSY but rather that the LTSY / network is not supporting the requested operation. 
	if(operatorName==EOperatorVodafone)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EVoiceService failed"));
	
		// Repeat test with aInfo.iServiceGroup= EAuxVoiceService...	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		// Repeat test with aInfo.iServiceGroup= ECircuitDataService...	
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
		
		// Repeat test with aInfo.iServiceGroup= EPacketDataService...
		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSBearerServiceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EPacketDataService failed"));
		
		// Repeat test with aInfo.iServiceGroup= EFaxService...
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EFaxService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EFaxService failed"));
			
		// Repeat test with aInfo.iServiceGroup= EShortMessageService...
		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EShortMessageService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EShortMessageService failed"));
		
		// Repeat test with aInfo.iServiceGroup= ETelephony...
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ETelephony failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EVoiceService failed"));
	
		// Repeat test with aInfo.iServiceGroup= EAuxVoiceService...	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		// Repeat test with aInfo.iServiceGroup= ECircuitDataService...	
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
		
		// Repeat test with aInfo.iServiceGroup= EPacketDataService...
		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EPacketDataService failed"));
		
		// Repeat test with aInfo.iServiceGroup= EFaxService...
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EFaxService"));
		
		if(operatorName==EOperatorTMobile)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EFaxService failed"));
			}
		else if(operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EFaxService failed"));			
			}
		
		// Repeat test with aInfo.iServiceGroup= EShortMessageService...
		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EShortMessageService"));
		
		if(operatorName==EOperatorTMobile || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EShortMessageService failed"));
			}
		else if(operatorName==EOperatorOrange)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSCallBarred), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EShortMessageService failed"));
			}
		
		// Repeat test with aInfo.iServiceGroup= ETelephony...
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ETelephony failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
		
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0002");
	}



CCTSYIntegrationTestCallForwarding0003::CCTSYIntegrationTestCallForwarding0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0003::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0003::~CCTSYIntegrationTestCallForwarding0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0003
 * @SYMFssID BA/CTSY/CFOR-0003
 * @SYMTestCaseDesc Erase call forwarding for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is erased for all services.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding is erased and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		
	TUint32 callServiceCaps = 0;
	TPtrC number;
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAllServices failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
		}
	else
		{	
		CHECK_EQUALS_L(1,0,_L("Failed to identify current network"));
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Erase call forwarding ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeout=-1 
	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingUnconditional
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));

	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0003");
	}



CCTSYIntegrationTestCallForwarding0004::CCTSYIntegrationTestCallForwarding0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0004::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0004::~CCTSYIntegrationTestCallForwarding0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0004
 * @SYMFssID BA/CTSY/CFOR-0004
 * @SYMTestCaseDesc Erase call forwarding for each provisioned basic service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is erased for each provisioned service group.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding is erased and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		
	TUint32 callServiceCaps = 0;
	TPtrC number;
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAllServices failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
		}
	else
		{	
		CHECK_EQUALS_L(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Erase call forwarding ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EVoiceService, aInfo.iTimeOut=-1 
	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingUnconditional
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EVoiceService"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EVoiceService failed"));

	newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;

	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAuxVoiceService"));
	
	if(operatorName==EOperatorVodafone)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAuxVoiceService failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAuxVoiceService failed"));
		}
	
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for ECircuitDataService"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for ECircuitDataService failed"));
	
	newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EPacketDataService"));
	
	if(operatorName==EOperatorVodafone)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EPacketDataService failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EPacketDataService failed"));
		}
	
	newSetting.iServiceGroup=RMobilePhone::EFaxService;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EFaxService"));
	
	if(operatorName==EOperatorVodafone || operatorName==EOperatorTMobile)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EFaxService failed"));
		}
	else if(operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EFaxService failed"));
		}
	
	newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EShortMessageService"));
	
	if(operatorName==EOperatorVodafone)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EShortMessageService failed"));
		}
	else if(operatorName==EOperatorTMobile)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EShortMessageService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EShortMessageService failed")); 
		}
	else if(operatorName==EOperatorOrange)
		{
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSCallBarred), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EShortMessageService failed"));
		}
	
	newSetting.iServiceGroup=RMobilePhone::ETelephony;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for ETelephony"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for ETelephony failed"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0004");
	}



CCTSYIntegrationTestCallForwarding0005::CCTSYIntegrationTestCallForwarding0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0005::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0005::~CCTSYIntegrationTestCallForwarding0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0005
 * @SYMFssID BA/CTSY/CFOR-0005
 * @SYMTestCaseDesc Activate call forwarding for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is activated for all services.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding activated and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 callServiceCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeout=-1 
	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iTimeout=-1;
		
	TExtEtelRequestStatus setCallForwardStatus(mobilePhone, EMobilePhoneSetCallForwardingStatus);
	CleanupStack::PushL(setCallForwardStatus);
	mobilePhone.SetCallForwardingStatus(setCallForwardStatus, RMobilePhone::ECallForwardingAllCases, newSetting);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	CHECK_EQUALS_L(setCallForwardStatus.Int(), KErrNone, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));
   
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	// ===  Activate call forwarding and check notification completes ===
	
	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionActivate;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iTimeout=-1;

	DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAllServices"));	
	
	if(operatorName==EOperatorVodafone)
		{		
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAllServices failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSErrorStatus), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
		}
	else
		{	
		CHECK_EQUALS_L(1,0,_L("Failed to identify current network"));
		}
 
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	mobilePhone.SetCallForwardingStatus(setCallForwardStatus, RMobilePhone::ECallForwardingAllCases, newSetting);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	CHECK_EQUALS_L(setCallForwardStatus.Int(), KErrNone, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));

	CleanupStack::PopAndDestroy(&setCallForwardStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0005");
	}



CCTSYIntegrationTestCallForwarding0006::CCTSYIntegrationTestCallForwarding0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0006::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0006::~CCTSYIntegrationTestCallForwarding0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0006
 * @SYMFssID BA/CTSY/CFOR-0006
 * @SYMTestCaseDesc Activate call forwarding for each provisioned basic service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is activated for each provisioned basic service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding activated and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps = 0;
	TPtrC number; 
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
		
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	// ===  Activate call forwarding and check notification completes ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService, aInfo.iTimeOut=-1 

	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingUnconditional
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionActivate;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;

	// $CTSYProblem The status returned from SetCallForwardingStatus does not return KErrNone.
	// Instead the status varies depending on what iAction is requested. It is presumed that this is LTSY / network problem.

	if(operatorName==EOperatorVodafone)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EVoiceService failed"));
	
		// Repeat test with aInfo.iServiceGroup=EAuxVoiceService, ECircuitDataService, EPacketDataService, EFaxService, EShortMessageService, ETelephony 	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EPacketDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EFaxService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EFaxService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EShortMessageService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EShortMessageService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ETelephony failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSErrorStatus), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EVoiceService failed"));
	
		// Repeat test with aInfo.iServiceGroup=EAuxVoiceService, ECircuitDataService, EPacketDataService, EFaxService, EShortMessageService, ETelephony 	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned ), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSErrorStatus), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));

		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EPacketDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EFaxService"));
		
		if(operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned ), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EFaxService failed"));
			}
		else if(operatorName==EOperatorTMobile)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSErrorStatus), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EFaxService failed"));
			}
		
		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EShortMessageService"));
		
		if(operatorName==EOperatorTMobile || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIncompatibility), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EShortMessageService failed"));
			}
		else if(operatorName==EOperatorOrange)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSCallBarred), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EShortMessageService failed"));
			}
		
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSErrorStatus), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ETelephony failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
		
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0006");
	}



CCTSYIntegrationTestCallForwarding0007::CCTSYIntegrationTestCallForwarding0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0007::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0007::~CCTSYIntegrationTestCallForwarding0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0007
 * @SYMFssID BA/CTSY/CFOR-0007
 * @SYMTestCaseDesc Deactivate call forwarding for all services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is deactivated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding deactivated and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 callServiceCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAllServices failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
		}
	else
		{	
		CHECK_EQUALS_L(1,0,_L("Failed to identify current network"));
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Deactivate call forwarding and check notification completes ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionDeactivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut = -1 
	newSetting.iAction=RMobilePhone::EServiceActionDeactivate;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EAllServices failed"));

	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0007");
	}



CCTSYIntegrationTestCallForwarding0008::CCTSYIntegrationTestCallForwarding0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0008::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0008::~CCTSYIntegrationTestCallForwarding0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0008
 * @SYMFssID BA/CTSY/CFOR-0008
 * @SYMTestCaseDesc Deactivate call forwarding for each provisioned basic service.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding is deactivated for each provisioned basic service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding deactivated and notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus 
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps = 0;
	TPtrC number;
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));
	
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	TInt err = iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName );	
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EAllServices failed"));
		}
	else if(operatorName==EOperatorTMobile || operatorName==EOperatorOrange || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EAllServices"));
		CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EAllServices failed"));
		}
	else
		{	
		CHECK_EQUALS_L(1,0,_L("Failed to identify current network"));
		}
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// $CTSYProblem The status returned from SetCallForwardingStatus does not return KErrNone.
	// Instead the status varies depending on what iAction is requested. It is presumed that this is LTSY / network problem.
	
	// ===  Deactivate call forwarding ===

	// RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionDeactivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService, aInfo.iTimeOut=-1 
	// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingUnconditional
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionDeactivate;
	
	if(operatorName==EOperatorVodafone)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService failed"));
		
		// Repeat test with aInfo.iServiceGroup=EAuxVoiceService, ECircuitDataService, EPacketDataService, EFaxService, EShortMessageService, ETelephony 	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for ECircuitDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EPacketDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EFaxService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EFaxService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EShortMessageService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSIllegalOperation), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EShortMessageService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for ETelephony failed"));
		}	
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService failed"));
		
		// Repeat test with aInfo.iServiceGroup=EAuxVoiceService, ECircuitDataService, EPacketDataService, EFaxService, EShortMessageService, ETelephony 	
		newSetting.iServiceGroup=RMobilePhone::EAuxVoiceService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EAuxVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EAuxVoiceService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for ECircuitDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EPacketDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EPacketDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EPacketDataService failed"));
		
		newSetting.iServiceGroup=RMobilePhone::EFaxService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EFaxService"));

		if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorNTN)
			{	
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSTeleserviceNotProvisioned), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EFaxService failed"));
			}
		else if(operatorName==EOperatorTMobile)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EFaxService failed"));
			}

		newSetting.iServiceGroup=RMobilePhone::EShortMessageService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EShortMessageService"));

		if(operatorName==EOperatorO2 || operatorName==EOperatorTMobile || operatorName==EOperatorNTN) 
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EShortMessageService failed"));
			}
		else if(operatorName==EOperatorOrange)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrGsmSSCallBarred), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EShortMessageService failed"));
			}
		
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for ETelephony failed"));
		}
	
	//
	// TEST END
	//
	
	StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
		
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0008");
	}



CCTSYIntegrationTestCallForwarding0009::CCTSYIntegrationTestCallForwarding0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0009::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0009::~CCTSYIntegrationTestCallForwarding0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0009
 * @SYMFssID BA/CTSY/CFOR-0009
 * @SYMTestCaseDesc Interrogate call forwarding status when all services have the same status.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, CRetrieveMobilePhoneCFList::NewL
 * @SYMTestExpectedResults Pass - Call forwarding status is retrieved, contains one entry and is correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding supplementary service list contains one entry and is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsGetCFStatusNetwork | KCapsGetCFStatusCache capabilities.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	TUint32 callServiceCaps = 0;	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsGetCFStatusNetwork;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
		
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL( mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	if(operatorName==EOperatorVodafone)
		{
		// ===  Activate call forwarding unconditional for all services ===
	
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EVoiceService failed"));
	
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
	
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ETelephony failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// ===  Register call forwarding unconditional for all services ===
		
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EAllServices and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EVoiceService failed"));
	
		newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
	
		newSetting.iServiceGroup=RMobilePhone::ETelephony;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ETelephony failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	// ===  Retrieve the call forwarding status list ===

	// Use helper class for CRetrieveMobilePhoneCFList to get the call forwarding status list with aCondition=RMobilePhone::ECallForwardingUnconditional, aServiceGroup=RMobilePhone::EAllServices, aLocation=RMobilePhone::EInfoLocationCachePreferred 
	TInt err;

	CRetrieveMobilePhoneCFListExec* mainCfList = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingUnconditional, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	CleanupStack::PushL(mainCfList);
	CMobilePhoneCFList* callForwardingList = mainCfList->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));
	
	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	TEST_CHECK_POINT_L(_L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	
	// ===  Get the entries from the call forwarding status list and check the entries are correct ===

	// Check CMobilePhoneCFList::Enumerate returns > 0
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
		
	TInt entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
	
	// Check CMobilePhoneCFList::GetEntryL with aIndex=0 returns a RMobilePhone::TMobilePhoneCFInfoEntryV1 with iServiceGroup=RMobilePhone::EAllServices, iStatus=RMobilePhone::ECallForwardingStatusActive, iNumber=same number used to register		
	TBuf<20> numberToCompare;
	
	if(number.Locate('0')==0)
		{
		// For the case when 0 is added
		numberToCompare=number.Mid(1);
		}
	else if(number.Locate('4')==0 || number.Locate('4')==1)
		{
		// For the case when +440 is added (or something like this)
		numberToCompare=number.Mid(5);
		}

	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
			
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::EVoiceService:
			case RMobilePhone::ECircuitDataService:
			case RMobilePhone::ETelephony:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusActive, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusActive"));
				DEBUG_PRINTF2(_L("Value for entry.iNumber.iTelNumber: %S"), &entry.iNumber.iTelNumber);
				ASSERT_TRUE(entry.iNumber.iTelNumber.Find(numberToCompare)>=0, _L("CMobilePhoneCFList::GetEntryL iTelNumber did not find number"));
				
				break;
				}
			case RMobilePhone::EFaxService:
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::EPacketDataService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));			
				break;
				}
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}
				
	// ===  Erase all call forwarding unconditional ===

	// Erase all call forwarding by calling RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));	
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));

	// ===  Retrieve the call forwarding status list ===
	
	// Use the helper class for CRetrieveMobilePhoneCFList to get the call forwarding status list with aCondition=RMobilePhone::ECallForwardingUnconditional, aServiceGroup=RMobilePhone::EAllServices, aLocation=RMobilePhone::EInfoLocationCachePreferred 
	CRetrieveMobilePhoneCFListExec* mainCfList2 = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingUnconditional, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	CleanupStack::PushL(mainCfList2);
	callForwardingList = mainCfList2->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));
	
	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	
	// ===  Get the entries from the call forwarding status list and check the entries are correct ===

	// Check CMobilePhoneCFList::Enumerate returns > 0
	
	entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
			
	// Check CMobilePhoneCFList::GetEntryL with aIndex=0 returns a RMobilePhone::TMobilePhoneCFInfoEntryV1 with iServiceGroup=RMobilePhone::EAllServices and iStatus=RMobilePhone::ECallForwardingStatusNotRegistered		
	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
			
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::EVoiceService:
			case RMobilePhone::ECircuitDataService:
			case RMobilePhone::EFaxService:	
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::EPacketDataService:
			case RMobilePhone::ETelephony:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));			
				break;
				}
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}
			
	//
	// TEST END
	//
	
    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
	
	CleanupStack::PopAndDestroy(2,mainCfList);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0009");
	}



CCTSYIntegrationTestCallForwarding0010::CCTSYIntegrationTestCallForwarding0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0010::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0010::~CCTSYIntegrationTestCallForwarding0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0010
 * @SYMFssID BA/CTSY/CFOR-0010
 * @SYMTestCaseDesc Interrogate call forwarding status for different conditions and provisioned basic services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, CRetrieveMobilePhoneCFList::NewL
 * @SYMTestExpectedResults Pass - Call forwarding status is retrieved and status is that set.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding status list is retrieved with correct entries.
 *
 * @return - TVerdict code
 */
	{
	
	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsGetCFStatusNetwork | KCapsGetCFStatusCache capabilities.
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);		

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));

	TUint32 callServiceCaps = 0;	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsGetCFStatusNetwork;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 
	
	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
		
	// ===   1. Register/Activate call forwarding busy for voice ===

	// Register/Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate (or RMobilePhone::EServiceActionRegister) and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 

	// ===  Retrieve the call forwarding busy status list and check that voice is active and other provisioned basic services are not registered ===
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		// aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingBusy for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}

	// Use the helper class for CRetrieveMobilePhoneCFList::Start to get the call forwarding status list with aCondition=RMobilePhone::ECallForwardingBusy, aServiceGroup=RMobilePhone::EAllServices, aLocation=RMobilePhone::EInfoLocationCachePreferred 
	TInt err;

	CRetrieveMobilePhoneCFListExec* mainCfList = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingBusy, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	CMobilePhoneCFList* callForwardingList = mainCfList->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));

	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));

	// Check CMobilePhoneCFList::Enumerate returns > 0
	RMobilePhone::TMobilePhoneCFInfoEntryV1 entry;
	TBuf<20> numberToCompare;
	
	if(number.Locate('0')==0)
		{
		// For the case when 0 is added
		numberToCompare=number.Mid(1);
		}
	else if(number.Locate('4')==0 || number.Locate('4')==1)
		{
		// For the case when +440 is added (or something like this)
		numberToCompare=number.Mid(5);
		}
	
	TInt entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
		
	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
				
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::EVoiceService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusActive, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusActive"));
				DEBUG_PRINTF2(_L("Value for entry.iNumber.iTelNumber: %S"), &entry.iNumber.iTelNumber);
				ASSERT_TRUE(entry.iNumber.iTelNumber.Find(numberToCompare)>=0, _L("CMobilePhoneCFList::GetEntryL iTelNumber did not find number"));
			
				break;
				}
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::ECircuitDataService:
			case RMobilePhone::EPacketDataService:
			case RMobilePhone::EFaxService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));
				break;
				}				
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}

	// ===  Erase all call forwarding and repeat test ===

	// Erase all call forwarding by calling RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));

	// Repeat from step 1. registering call forwarding with aInfo.iServiceGroup=RMobilePhone::ECircuitDataService, 
	// checking that call forwarding is registered for that basic service and not registered for the others.
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	
	mainCfList = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingUnconditional, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	callForwardingList = mainCfList->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));
	
	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	
	// Check CMobilePhoneCFList::Enumerate returns > 0
	
	entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
		
	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
			
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::ECircuitDataService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusActive, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusActive"));
				DEBUG_PRINTF2(_L("Value for entry.iNumber.iTelNumber: %S"), &entry.iNumber.iTelNumber);
				ASSERT_TRUE(entry.iNumber.iTelNumber.Find(numberToCompare)>=0, _L("CMobilePhoneCFList::GetEntryL iTelNumber did not find number"));
	
				break;
				}
			case RMobilePhone::EVoiceService:
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::EPacketDataService:
			case RMobilePhone::EFaxService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));			
				break;
				}
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}
	
	// Erase ECallForwardingAllCases...
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));	
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	// Repeat the whole test with aCondition=RMobilePhone::ECallForwardingNoReply...
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;

	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingNoReply for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingNoReply for ECircuitDataService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNoReply for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNoReply for ECircuitDataService failed"));		
		}
	
	// Retrieve List	
	mainCfList = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingNoReply, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	callForwardingList = mainCfList->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));
	
	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	
	// Check CMobilePhoneCFList::Enumerate returns > 0
	entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
		
	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
			
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::ECircuitDataService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusActive, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusActive"));
				DEBUG_PRINTF2(_L("Value for entry.iNumber.iTelNumber: %S"), &entry.iNumber.iTelNumber);
				ASSERT_TRUE(entry.iNumber.iTelNumber.Find(numberToCompare)>=0, _L("CMobilePhoneCFList::GetEntryL iTelNumber did not find number"));
	
				break;
				}
			case RMobilePhone::EVoiceService:
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::EPacketDataService:
			case RMobilePhone::EFaxService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));			
				break;
				}
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}
	
	// Erase ECallForwardingAllCases...
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	// Repeat the whole test with aCondition=RMobilePhone::ECallForwardingNotReachable...
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	
	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingNotReachable for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingNotReachable for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNotReachable for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNotReachable for EVoiceService failed"));
		}
	
	// Retrieve List
	mainCfList = CRetrieveMobilePhoneCFListHelper::NewL(mobilePhone, RMobilePhone::ECallForwardingNotReachable, RMobilePhone::EAllServices, RMobilePhone::EInfoLocationCachePreferred);
	callForwardingList = mainCfList->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCFListExec::DoGetList is not able to get the list"));
	
	ASSERT_TRUE(callForwardingList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));
	
	// Check CMobilePhoneCFList::Enumerate returns > 0
	entries = callForwardingList->Enumerate();
		
	ASSERT_TRUE(entries>0, _L("CMobilePhoneCFList::Enumerate return value not greater than 0"));
		
	for(TInt i = 0; i<entries; i++)
		{
		DEBUG_PRINTF2(_L("Performing CMobilePhoneCFList::GetEntryL on entry at position %d"), i);
			
		entry=callForwardingList->GetEntryL(i);
			
		switch(entry.iServiceGroup)
			{
			case RMobilePhone::EVoiceService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusActive, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusActive"));
				DEBUG_PRINTF2(_L("Value for entry.iNumber.iTelNumber: %S"), &entry.iNumber.iTelNumber);
				ASSERT_TRUE(entry.iNumber.iTelNumber.Find(numberToCompare)>=0, _L("CMobilePhoneCFList::GetEntryL iTelNumber did not find number"));
	
				break;
				}
			case RMobilePhone::EFaxService:
			case RMobilePhone::EAuxVoiceService:
			case RMobilePhone::ECircuitDataService:
			case RMobilePhone::EPacketDataService:
				{
				ASSERT_EQUALS(entry.iStatus, RMobilePhone::ECallForwardingStatusNotRegistered, _L("CMobilePhoneCFList::GetEntryL iStatus did not get RMobilePhone::ECallForwardingStatusNotRegistered"));			
				break;
				}
			default:
				{
				DEBUG_PRINTF2(_L("Service %d not accounted for"), entry.iServiceGroup);
				break;
				}
			}
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0010");
	}



CCTSYIntegrationTestCallForwarding0011::CCTSYIntegrationTestCallForwarding0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0011::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0011::~CCTSYIntegrationTestCallForwarding0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0011
 * @SYMFssID BA/CTSY/CFOR-0011
 * @SYMTestCaseDesc Dial a call on a line with call forwarding unconditional active on it.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, RCall::Dial, RCall::HangUp, RMobileCall::NotifyMobileCallStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding active notification completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding active notification compeltes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus  
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	TUint32 callServiceCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));
	
	TPtrC number;
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	
	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	TExtEtelRequestStatus setCallForwardStatus(mobilePhone, EMobilePhoneSetCallForwardingStatus);
	CleanupStack::PushL(setCallForwardStatus);
	mobilePhone.SetCallForwardingStatus(setCallForwardStatus, RMobilePhone::ECallForwardingAllCases, newSetting);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	CHECK_EQUALS_L(setCallForwardStatus.Int(), KErrNone, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));

	// Get Voice Line 1.
	RMobileLine& mobileLine= iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get Call 1.
	RMobileCall& mobileCall= iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// ===  Activate/Register call forwarding unconditional for voice ===
	
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
		
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EVoiceService failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}

	// Post notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatus(mobileCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// Post notifier for RMobilePhone::NotifyCallForwardingActive
	TExtEtelRequestStatus notifyCallForwardStatus(mobilePhone, EMobilePhoneNotifyCallForwardingActive);
	CleanupStack::PushL(notifyCallForwardStatus);
	RMobilePhone::TMobileService serviceGroup;
	RMobilePhone::TMobilePhoneCFActive activeType;
	mobilePhone.NotifyCallForwardingActive(notifyCallForwardStatus, serviceGroup, activeType);
	
	// Dial a number that answers on call 1 with RCall::Dial.
	TPtrC outgoingnumber; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, outgoingnumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &outgoingnumber);
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, outgoingnumber);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial error whilst dialing"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus=RMobileCall::EStatusDialling;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusConnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
		
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// If this API becomes supported, then the following code can be uncommented.
    
	// Check RMobilePhone::NotifyCallForwardingActive completes with aServiceGroup=EVoiceService , aActiveType=ECFUnconditionalActive
	/*iCallForwardingSupplementalHelper.WaitForMobilePhoneNotifyCallForwardingActive(mobilePhone,
																				   notifyCallForwardStatus,
																				   serviceGroup,
																				   RMobilePhone::EVoiceService,
																				   activeType,
																				   KErrNone);*/

	//ASSERT_EQUALS(activeType, RMobilePhone::ECFUnconditionalActive, _L("activeType is not RMobilePhone::ECFUnconditionalActive"));
	
	
	// ===  Check call forwarding active indication completes ===

	// Hang up call with RCall::HangUp
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RCall::NotifyMobileCallStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus=RMobileCall::EStatusDisconnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	// ===  Erase all call forwarding for next part of test ===

	// Erase call forwarding by calling RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	//
	// TEST END
	//
	
    StartCleanup();
    
	CleanupStack::PopAndDestroy(&dialStatus);
	CleanupStack::PopAndDestroy(&notifyCallForwardStatus);
	CleanupStack::PopAndDestroy(&notifyMobileCallStatus);
	CleanupStack::PopAndDestroy(&setCallForwardStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0011");
	}



CCTSYIntegrationTestCallForwarding0012::CCTSYIntegrationTestCallForwarding0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0012::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0012::~CCTSYIntegrationTestCallForwarding0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0012
 * @SYMFssID BA/CTSY/CFOR-0012
 * @SYMTestCaseDesc Dial a call on a line with call forwarding active.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus, RCall::Dial, RCall::HangUp, RMobileCall::NotifyMobileCallStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding active notification completes.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding active notification compeltes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus  
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	TUint32 callServiceCaps = 0;
	TPtrC number;
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));
	
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));

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
	
	// ===  Activate/Register call forwarding ===

	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));

	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingBusy and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingBusy for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingBusy and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	// Post notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallStatus(mobileCall, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);

	// Post notifier for RMobilePhone::NotifyCallForwardingActive
	TExtEtelRequestStatus notifyCallForwardStatus(mobilePhone, EMobilePhoneNotifyCallForwardingActive);
	CleanupStack::PushL(notifyCallForwardStatus);
	RMobilePhone::TMobileService serviceGroup;
	RMobilePhone::TMobilePhoneCFActive activeType;
	mobilePhone.NotifyCallForwardingActive(notifyCallForwardStatus, serviceGroup, activeType);
	
	// Dial a number that answers on call 1 with RCall::Dial. 
	TPtrC outGoingNumber; 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, outGoingNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Board Now Dialling: %S"), &outGoingNumber);
	TCoreEtelRequestStatus<RCall> dialStatus(mobileCall, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	mobileCall.Dial(dialStatus, outGoingNumber);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial error whilst dialing"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus=RMobileCall::EStatusDialling;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusConnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
		
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// ===  Check call forwarding active indication completes ===

	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// The following section can be uncommented if RMobilePhone::NotifyCallForwardingActive is correctly implemented.
	// The request is sent from CTSY to the LTSY, but the CTSY never gets a response back from the LTSY.
	/*
	// Check RMobilePhone::NotifyCallForwardingActive completes with aServiceGroup=EVoiceService , aActiveType=ECFConditionalActive
	iCallForwardingSupplementalHelper.WaitForMobilePhoneNotifyCallForwardingActive(mobilePhone,
																				   notifyCallForwardStatus,
																				   serviceGroup,
																				   RMobilePhone::EVoiceService,
																				   activeType,
																				   KErrNone);
	
	ASSERT_EQUALS(activeType, RMobilePhone::ECFUnconditionalActive, _L("activeType is not RMobilePhone::ECFUnconditionalActive"));
	*/
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// Hang up call with RCall::HangUp 
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RCall::NotifyStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus=RMobileCall::EStatusDisconnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// ===  Erase all call forwarding for next part of test ===

	// Erase call forwarding by calling RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	// Repeat test with ECallForwardingNotReachable
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingNotReachable for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingNotReachable for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNotReachable for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNotReachable), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNotReachable for EVoiceService failed"));
		}

	// Repost notifiers
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// The following section can be uncommented if RMobilePhone::NotifyCallForwardingActive is correctly implemented.
	// The request is sent from CTSY to the LTSY, but the CTSY never gets a response back from the LTSY.
	/*
	mobilePhone.NotifyCallForwardingActive(notifyCallForwardStatus, serviceGroup, activeType);
	*/
	mobileCall.Dial(dialStatus, outGoingNumber);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial error whilst dialing"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCallStatus=RMobileCall::EStatusDialling;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusConnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
		
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// ===  Check call forwarding active indication completes ===

	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// The following section can be uncommented if RMobilePhone::NotifyCallForwardingActive is correctly implemented.
	// The request is sent from CTSY to the LTSY, but the CTSY never gets a response back from the LTSY.
	
	/*
	// Check RMobilePhone::NotifyCallForwardingActive completes with aServiceGroup=EVoiceService , aActiveType=ECFConditionalActive
	iCallForwardingSupplementalHelper.WaitForMobilePhoneNotifyCallForwardingActive(mobilePhone,
			   																	   notifyCallForwardStatus,
			   																	   serviceGroup,
			   																	   RMobilePhone::EVoiceService,
			   																	   activeType,
			   																	   KErrNone);

	ASSERT_EQUALS(activeType, RMobilePhone::ECFUnconditionalActive, _L("activeType is not RMobilePhone::ECFUnconditionalActive"));
	*/
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// Hang up call with RCall::HangUp 
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RCall::NotifyStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus=RMobileCall::EStatusDisconnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	// Erase call forwarding by calling RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));

	// Repeat test with ECallForwardingNoReply and aInfo.iTimeOut=20 
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=20;
	
	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingNoReply for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingNoReply for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingNoReply for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingNoReply), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingNoReply for EVoiceService failed"));
		}
	
	// Repost notifiers
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// The following section can be uncommented if RMobilePhone::NotifyCallForwardingActive is correctly implemented.
	// The request is sent from CTSY to the LTSY, but the CTSY never gets a response back from the LTSY.
	
	/*
	mobilePhone.NotifyCallForwardingActive(notifyCallForwardStatus, serviceGroup, activeType);
	*/
	mobileCall.Dial(dialStatus, outGoingNumber);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial error whilst dialing"));
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusDialling -> EStatusConnecting -> EStatusConnected.
	expectedMobileCallStatus=RMobileCall::EStatusDialling;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusConnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
		
	expectedMobileCallStatus=RMobileCall::EStatusConnected;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	// ===  Check call forwarding active indication completes ===
	
	// $CTSYProblem Currently NotifyCallForwardingActive is not being completed by LTSY. Hence
	// this will not return the correct values which are expected for this test.
	// The following section can be uncommented if RMobilePhone::NotifyCallForwardingActive is correctly implemented.
	// The request is sent from CTSY to the LTSY, but the CTSY never gets a response back from the LTSY.
	
	/*
	// Check RMobilePhone::NotifyCallForwardingActive completes with aServiceGroup=EVoiceService , aActiveType=ECFConditionalActive
	iCallForwardingSupplementalHelper.WaitForMobilePhoneNotifyCallForwardingActive(mobilePhone,
			   																	   notifyCallForwardStatus,
			   																	   serviceGroup,
			   																	   RMobilePhone::EVoiceService,
			   																	   activeType,
			   																	   KErrNone);

	ASSERT_EQUALS(activeType, RMobilePhone::ECFUnconditionalActive, _L("activeType is not RMobilePhone::ECFUnconditionalActive"));
	*/
	
	// Hang up call with RCall::HangUp 
	DEBUG_PRINTF1(_L("Hangup call"));
	ASSERT_EQUALS(mobileCall.HangUp(), KErrNone, _L("RCall::HangUp return value not KErrNone"));
	
	// Check RCall::NotifyStatusChange completes with EStatusDisconnecting -> EStatusIdle.
	expectedMobileCallStatus=RMobileCall::EStatusDisconnecting;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);

	mobileCall.NotifyMobileCallStatusChange(notifyMobileCallStatus, mobileCallStatus);
	
	expectedMobileCallStatus=RMobileCall::EStatusIdle;
	iCallForwardingControlHelper.WaitForMobileCallNotifyMobileCallStatusChange(mobileCall,
																			   notifyMobileCallStatus,
																			   mobileCallStatus,
																			   expectedMobileCallStatus,
																			   KErrNone);
	
	//
	// TEST END
	//

    StartCleanup(); 
	
    CleanupStack::PopAndDestroy(&dialStatus);
	CleanupStack::PopAndDestroy(&notifyCallForwardStatus);
    CleanupStack::PopAndDestroy(&notifyMobileCallStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0012");
	}



CCTSYIntegrationTestCallForwarding0013::CCTSYIntegrationTestCallForwarding0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0013::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0013::~CCTSYIntegrationTestCallForwarding0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0013
 * @SYMFssID BA/CTSY/CFOR-0013
 * @SYMTestCaseDesc Change the 'forward to' number.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding status change notification completes when forward to number is changed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding status change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps = 0;

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// ===  Activate/Register call forwarding busy for voice ===
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with ECallForwardingUnconditional for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with ECallForwardingUnconditional for EVoiceService failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}

	// aInfo.iAction=RMobilePhone::EServiceActionDeactivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService, aInfo.iTimeOut=-1 
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionDeactivate;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionDeactivate ECallForwardingUnconditional for EVoiceService failed"));
	
	// ===  Activate/Register call forwarding busy for voice with a different number ===
	TPtrC anotherNumber; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, anotherNumber) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("New number obtained: %S"), &anotherNumber);
	
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=anotherNumber;

	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=different valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));	
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with ECallForwardingUnconditional for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=different valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));	
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with ECallForwardingUnconditional for EVoiceService failed"));
		}
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingUnconditional for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingUnconditional for EAllServices failed"));
    
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0013");
	}



CCTSYIntegrationTestCallForwarding0014::CCTSYIntegrationTestCallForwarding0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0014::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0014::~CCTSYIntegrationTestCallForwarding0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0014
 * @SYMFssID BA/CTSY/CFOR-0014
 * @SYMTestCaseDesc Change the 'no reply' time out.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallForwardingStatusChange
 * @SYMTestExpectedResults Pass - Call forwarding status change notification completes when time out is changed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding status change notification completes.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	TUint32 callServiceCaps = 0;

	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	CHECK_TRUE_L(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// === Activate/Register call forwarding no reply for voice ===
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=10;

	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingNoReply and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=10 
		// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingBusy
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingBusy for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{	
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingNoReply and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=10 
		// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingBusy
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EVoiceService"));
		if(operatorName==EOperatorTMobile || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));			
			}
		else
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));			
			}
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
	
	// ===  Activate/Register call forwarding no reply for voice with a different time out ===

	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionActivate;
	newSetting.iTimeout=20;
	
	if(operatorName==EOperatorVodafone)
		{	
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingNoReply and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=different valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=20 
		// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingBusy
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingBusy for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingBusy for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingNoReply and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=different valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=20 
		// Check RMobilePhone::NotifyCallForwardingStatusChange completes with aCondition=ECallForwardingBusy
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingBusy for EVoiceService"));
		if(operatorName==EOperatorTMobile || operatorName==EOperatorO2 || operatorName==EOperatorNTN)
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy, KErrGsmSSUnexpectedDataValue), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));
			}
		else
			{
			ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingBusy), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingBusy for EVoiceService failed"));
			}
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionErase ECallForwardingAllCases for EAllServices"));
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingAllCases), _L("SetCallForwardingStatusUtil with EServiceActionErase ECallForwardingAllCases for EAllServices failed"));
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0014");
	}



CCTSYIntegrationTestCallForwarding0015::CCTSYIntegrationTestCallForwarding0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0015::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0015::~CCTSYIntegrationTestCallForwarding0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0015
 * @SYMFssID BA/CTSY/CFOR-0015
 * @SYMTestCaseDesc Get call forwarding indication status.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallForwardingStatus
 * @SYMTestExpectedResults Pass - Call forwarding indicator is retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call forwarding indication retrieved.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus | KCapsNotifyCFStatus
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	TUint32 callServiceCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus | RMobilePhone::KCapsNotifyCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	// Ensure all call forwarding is erased by calling 

	// RMobilePhone::SetCallForwardingStatus with aCondition=RMobilePhone::ECallForwardingAllCases and 

	// aInfo.iAction=RMobilePhone::EServiceActionErase and aInfo.iServiceGroup=RMobilePhone::EAllServices, aInfo.iTimeOut=-1 
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	newSetting.iTimeout=-1;
	
	TExtEtelRequestStatus setCallForwardStatus(mobilePhone, EMobilePhoneSetCallForwardingStatus);
	CleanupStack::PushL(setCallForwardStatus);
	mobilePhone.SetCallForwardingStatus(setCallForwardStatus, RMobilePhone::ECallForwardingAllCases, newSetting);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	CHECK_EQUALS_L(setCallForwardStatus.Int(), KErrNone, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Activate/Register call forwarding unconditional for voice ===
	RMobilePhone::TMobilePhoneNetworkInfoV1 info;
	RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infopckg(info);
	RMobilePhone::TMobilePhoneLocationAreaV1 area;
	TOperatorName operatorName = EOperatorUnknown;
	ASSERT_EQUALS(iNetworkTestHelper.GetCurrentNetworkL(mobilePhone, infopckg, area, operatorName), KErrNone, _L("RMobilePhone::GetCurrentNetwork failed to get current network id (1)"));
	
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;

	if(operatorName==EOperatorVodafone)
		{
		newSetting.iAction=RMobilePhone::EServiceActionActivate;
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for EVoiceService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		newSetting.iAction=RMobilePhone::EServiceActionRegister;
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for EVoiceService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for EVoiceService failed"));
		}
	else
		{	
		ASSERT_EQUALS(1,0,_L("Failed to identify current network"));
		TEST_CHECK_POINT_L(_L("Failed to identify current network."));
		}
		
	// ===  Get call forwarding indicator ===

	// Check RMobilePhone::GetCallForwardingIndicator returns KErrNotSupported
	RMobilePhone::TMobilePhoneCFUIndicatorV6 cfuIndicator;
    RMobilePhone::TMobilePhoneCFUIndicatorV6Pckg cfuIndicatorPackg(cfuIndicator);
    
	TExtEtelRequestStatus getCallForwardingStatus(mobilePhone, EMobilePhoneGetCallForwardingIndicator);
	CleanupStack::PushL(getCallForwardingStatus);
	mobilePhone.GetCallForwardingIndicator(getCallForwardingStatus, cfuIndicatorPackg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCallForwardingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCallForwardingIndicator timed-out"));
	ASSERT_EQUALS(getCallForwardingStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetCallForwardingStatus getCallForwardStatus completed with incorrect error"));
	
	expectedServiceCaps = 0;
	
	ASSERT_BITS_SET(cfuIndicator.iIndicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallForwardingIndicator returned wrong caps"));

	// === Activate/Register call forwarding unconditional for circuit switch data ===	
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::ECircuitDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));	
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::ECircuitDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));	
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
		}

	// ===  Get call forwarding indicator ===

	// Check RMobilePhone::GetCallForwardingIndicator returns KErrNotSupported
	mobilePhone.GetCallForwardingIndicator(getCallForwardingStatus, cfuIndicatorPackg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCallForwardingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCallForwardingIndicator timed-out."));
	ASSERT_EQUALS(getCallForwardingStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetCallForwardingStatus getCallForwardStatus completed with incorrect error"));	
	
	expectedServiceCaps = 0;
	
	ASSERT_BITS_SET(cfuIndicator.iIndicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallForwardingIndicator returned wrong caps"));

	// ===  Activate/Register call forwarding unconditional for circuit switch data ===
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::ECircuitDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::ECircuitDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));		
		}
	
	// ===  Get call forwarding indicator ===

	// Check RMobilePhone::GetCallForwardingIndicator returns KErrNotSupported
	mobilePhone.GetCallForwardingIndicator(getCallForwardingStatus, cfuIndicatorPackg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCallForwardingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCallForwardingIndicator timed-out"));
	ASSERT_EQUALS(getCallForwardingStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetCallForwardingStatus getCallForwardStatus completed with incorrect error"));

	expectedServiceCaps = 0;
	
	ASSERT_BITS_SET(cfuIndicator.iIndicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallForwardingIndicator returned wrong caps"));

	// ===  Activate/Register call forwarding unconditional for circuit switch data ===
	newSetting.iServiceGroup=RMobilePhone::ECircuitDataService;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::EPacketDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::EPacketDataService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 	
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ECircuitDataService failed"));
		}
	
	// ===  Get call forwarding indicator ===

	// Check RMobilePhone::GetCallForwardingIndicator returns KErrNotSupported
	mobilePhone.GetCallForwardingIndicator(getCallForwardingStatus, cfuIndicatorPackg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCallForwardingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCallForwardingIndicator timed-out."));
	ASSERT_EQUALS(getCallForwardingStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetCallForwardingStatus getCallForwardStatus completed with incorrect error"));
	
	expectedServiceCaps = 0;
	
	ASSERT_BITS_SET(cfuIndicator.iIndicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallForwardingIndicator returned wrong caps"));
	
	// ===  Activate/Register call forwarding unconditional for Telephony ===
	
	newSetting.iServiceGroup=RMobilePhone::ETelephony;
	
	if(operatorName==EOperatorVodafone)
		{
		// Activate call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionActivate and aInfo.iServiceGroup=RMobilePhone::ETelephony and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionActivate ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionActivate ECallForwardingUnconditional for ETelephony failed"));
		}
	else if(operatorName==EOperatorO2 || operatorName==EOperatorOrange || operatorName==EOperatorTMobile || operatorName==EOperatorNTN)
		{
		// Register call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionRegister and aInfo.iServiceGroup=RMobilePhone::ETelephony and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1
		DEBUG_PRINTF1(_L("Setting call forward EServiceActionRegister ECallForwardingUnconditional for ETelephony"));
		ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional), _L("SetCallForwardingStatusUtil with EServiceActionRegister ECallForwardingUnconditional for ETelephony failed"));
		}
	
	// ===  Get call forwarding indicator ===

	// Check RMobilePhone::GetCallForwardingIndicator returns KErrNotSupported
	mobilePhone.GetCallForwardingIndicator(getCallForwardingStatus, cfuIndicatorPackg);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(getCallForwardingStatus, ETimeMedium), KErrNone, _L("RMobilePhone::GetCallForwardingIndicator timed-out."));
	ASSERT_EQUALS(getCallForwardingStatus.Int(), KErrNotSupported, _L("RMobilePhone::GetCallForwardingStatus getCallForwardStatus completed with incorrect error"));
    	
	expectedServiceCaps = 0;
	
	ASSERT_BITS_SET(cfuIndicator.iIndicator, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallForwardingIndicator returned wrong caps"));

	//
	// TEST END
	//
	
    StartCleanup();
	
	// Ensure all call forwarding setting are erased
	newSetting.iServiceGroup=RMobilePhone::EAllServices;
	newSetting.iAction=RMobilePhone::EServiceActionErase;
	
	mobilePhone.SetCallForwardingStatus(setCallForwardStatus, RMobilePhone::ECallForwardingAllCases, newSetting);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setCallForwardStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetCallForwardingStatus timed-out"));
	ASSERT_EQUALS(setCallForwardStatus.Int(), KErrNone, _L("RMobilePhone::SetCallForwardingStatus setCallForwardStatus completed with incorrect error"));
		
	CleanupStack::PopAndDestroy(&getCallForwardingStatus);
	CleanupStack::PopAndDestroy(&setCallForwardStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0015");
	}



CCTSYIntegrationTestCallForwarding0016::CCTSYIntegrationTestCallForwarding0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallForwardingBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallForwarding0016::GetTestStepName());
	}

CCTSYIntegrationTestCallForwarding0016::~CCTSYIntegrationTestCallForwarding0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallForwarding0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CFOR-0016
 * @SYMFssID BA/CTSY/CFOR-0016
 * @SYMTestCaseDesc Invoke call forwarding
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCFStatus
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	
	// Call TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL		
	CHECK_EQUALS_L(iNetworkTestHelper.WaitForRegisteredOnHomeNetworkL(mobilePhone), KErrNone, _L("TNetworkTsyTestHelper::WaitForRegisteredOnHomeNetworkL returned with an error"));
	
	TUint32 callServiceCaps = 0;
	CHECK_EQUALS_L(mobilePhone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps return value is not KErrNone"));
	TUint32 expectedServiceCaps = RMobilePhone::KCapsSetCFStatus;
	CHECK_BITS_SET_L(callServiceCaps, expectedServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	TPtrC number; 
	CHECK_TRUE_L(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse, _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF2(_L("Number obtained: %S"), &number);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Invoke call forwarding using RMobilePhone::SetCallForwardingStatus with aCondition=ECallForwardingUnconditional and aInfo.iAction=RMobilePhone::EServiceActionInvoke and aInfo.iServiceGroup=RMobilePhone::EVoiceService and aInfo.iNumber.iNumberPlan=EIsdnNumberPlan, aInfo.iNumber.iTelNumber=valid number, aInfo.iNumber.iTypeOfNumber=ENationalNumber, aInfo.iTimeout=-1 
	// Check KErrArgument is returned.
	RMobilePhone::TMobilePhoneCFChangeV1 newSetting;
	newSetting.iServiceGroup=RMobilePhone::EVoiceService;
	newSetting.iAction=RMobilePhone::EServiceActionInvoke;
	newSetting.iNumber.iTelNumber=number;
	newSetting.iNumber.iNumberPlan=RMobilePhone::EIsdnNumberPlan;
	newSetting.iNumber.iTypeOfNumber=RMobilePhone::ENationalNumber;
	newSetting.iTimeout=-1;
	
	DEBUG_PRINTF1(_L("Setting call forward EServiceActionInvoke ECallForwardingUnconditional for EVoiceService"));	
	ASSERT_TRUE(SetCallForwardingStatusUtilL(mobilePhone, newSetting, RMobilePhone::ECallForwardingUnconditional, KErrArgument), _L("SetCallForwardingStatusUtil with EServiceActionInvoke ECallForwardingUnconditional for EVoiceService failed"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallForwarding0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallForwarding0016");
	}
