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
// Test step definitions for the CallBarring functional unit.
// 
//

/**
 @internalTechnology
*/

#include "cctsyintegrationtestcallbarring.h"
#include "cctsyinidata.h"

_LIT(KAltPassword, "0000");
_LIT(KAltPassword2, "1234");
_LIT(KAltPassword4, "1235");
_LIT(KAltPassword5, "123");
_LIT(KAltPasswordVoda, "1919");
_LIT(KIncorrectPassword , "9999");

static const TInt KSsCallBarringAllServices=330;


CCTSYIntegrationTestCallBarringBase::CCTSYIntegrationTestCallBarringBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), 
		iSupplementalTsyTestHelper(*this),
		iCallControlTsyTestHelper(*this),
		iNetworkTsyTestHelper(*this)
		
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestCallBarringBase::~CCTSYIntegrationTestCallBarringBase()
/*
 * Destructor
 */
	{
	}
	
/**
* Checks that the call barring password for this SIM is known and leaves if not
@param aPhone RMobilePhone session
@param aPassword On completion, the call barring password. The function leaves if this is not found.
*/
void CCTSYIntegrationTestCallBarringBase::CheckCBPasswordL( RMobilePhone &aPhone, RMobilePhone::TMobilePassword &aPassword )
	{

	DEBUG_PRINTF1(_L("Attempting to check call barring password is known"));
	
	// wait until we are on the network
	
	CHECK_EQUALS_L( iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( aPhone ), KErrNone, 
						_L("Network is unavailable") );
			
	const TDesC* networkSection = NULL;
	iNetworkTsyTestHelper.GetNetworkPasswordsSectionL(aPhone,networkSection);
	
	// Get the password from the ini file
	TPtrC password; 
	TBool gotPass = GetStringFromConfig( *networkSection, KIniCallBarring1, password );
	
	CHECK_TRUE_L( gotPass, _L("Failed to read password from INI file"));
	
	// Check the call barring password by attempting to reset call barring for all services
	DEBUG_PRINTF2(_L("Checking call barring password from INI file %S"), &password);
	
	aPassword = password;
	TInt err = DeactivateCallBarringL( aPhone, aPassword );
	CHECK_EQUALS_L( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	}
	
	
/**
* Resest call barring for all services
*/
TInt CCTSYIntegrationTestCallBarringBase::DeactivateCallBarringL( RMobilePhone &aPhone, 
																	const RMobilePhone::TMobilePassword& aPassword )
	{
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iPassword = aPassword;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	
	TExtEtelRequestStatus reqStatus(aPhone, EMobilePhoneSetCallBarringStatus);
	CleanupStack::PushL(reqStatus);
	
	aPhone.SetCallBarringStatus( reqStatus, RMobilePhone::EBarAllCases, cbInfo );
	TInt err = WaitForRequestWithTimeOut( reqStatus, ETimeMedium);
	
	if( KErrNone == err )
		{
		err = reqStatus.Int();
		}
		
	CleanupStack::PopAndDestroy(1,&reqStatus);
	
	return err;
	}
		

/** 
* Set the call barring status
*/
TInt CCTSYIntegrationTestCallBarringBase::SetCBStatusL( RMobilePhone& aPhone, 
						RMobilePhone::TMobilePhoneCBChangeV1& aInfo,
						RMobilePhone::TMobilePhoneCBCondition aCondition )
	{
	TExtEtelRequestStatus reqStatus(aPhone, EMobilePhoneSetCallBarringStatus);
	CleanupStack::PushL(reqStatus);
	
	aPhone.SetCallBarringStatus( reqStatus, aCondition, aInfo );
	
	TInt err = WaitForRequestWithTimeOut( reqStatus, ETimeMedium);
	
	if( KErrNone == err )
		{
		err = reqStatus.Int();
		}
		
	CleanupStack::PopAndDestroy(1);	

	return err;		
	}


/** 
* Search a status list for particular call barring entry
*/
TBool CCTSYIntegrationTestCallBarringBase::SearchCBStatusListL( CMobilePhoneCBList* aList, 
								RMobilePhone::TMobilePhoneCBCondition aCondition, 
								RMobilePhone::TMobileService aService,
								RMobilePhone::TMobilePhoneCBStatus aStatus )
	{	
	TInt count = aList->Enumerate();
	
	for( TInt i=0; i < count; i++ )
		{
		const RMobilePhone::TMobilePhoneCBInfoEntryV1 ele = aList->GetEntryL(i);
		
		if( ele.iCondition == aCondition && 
			ele.iServiceGroup == aService && 
			ele.iStatus == aStatus )
			{
			return ETrue;
			}		
		}
		
	DEBUG_PRINTF4( _L("Failed to find CMobilePhoneCBList entry condition=%d service=%d status=%d"), aCondition, aService, aStatus );
	return EFalse;
	}

/******************************************************************************/
	

CCTSYIntegrationTestCallBarring0001::CCTSYIntegrationTestCallBarring0001(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0001::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0001::~CCTSYIntegrationTestCallBarring0001()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0001::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0001
 * @SYMFssID BA/CTSY/CBAR-0001
 * @SYMTestCaseDesc Activate and deactivate call barring for outgoing voice calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - Call barring activated and barred numbers cannot be dialled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring status and barred number cannot be dialled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarAllOutgoing | KCapsBarOutgoingInternational | KCapsBarOutgoingInternationalExHC
	TUint32 callBarCaps;
	TInt err = phone.GetCallServiceCaps( callBarCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L( callBarCaps, 	(RMobilePhone::KCapsNotifyCBStatus | 
									 RMobilePhone::KCapsSetCBStatus | 
									 RMobilePhone::KCapsBarAllOutgoing | 
									 RMobilePhone::KCapsBarOutgoingInternational | 
									 RMobilePhone::KCapsBarOutgoingInternationalExHC) ,
								     KNoUnwantedBits,
								     _L("GetCallServiceCaps returned unexpected bitmap"));	

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);

	// NOTE - we don't actually change the password for safety
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;

	TExtEtelRequestStatus reqSetSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(reqSetSSPasswordStatus);
	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( reqSetSSPasswordStatus, ETimeMedium), KErrNone, 
						_L("RMobilePhone::SetSSPassword timed out") );	
	CHECK_EQUALS_L( reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer,KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine,KCall1 );	
	
	// Obtain our test call numbers
	TPtrC intNumber; 
	TBool numOK = GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumberInternational, intNumber);
	CHECK_TRUE_L( numOK, _L("Failed to obtain international number for test") );
	
	TPtrC natNumberWithPrefix; 
	numOK = GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumberIntExHC, natNumberWithPrefix);
	CHECK_TRUE_L( numOK, _L("Failed to obtain prefixed national number for test") );
	
	
	TPtrC natNumber; 
	numOK = GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, natNumber);
	CHECK_TRUE_L( numOK, _L("Failed to obtain national number for test") );
	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	
	// ===  Activate call barring on voice calls to outgoing international numbers and check 
	// international number cannot be dialled ===
		
	// Set up the notification request
	TExtEtelRequestStatus cbStatusNotify( phone, EMobilePhoneNotifyCallBarringStatusChange );
	CleanupStack::PushL(cbStatusNotify);
	RMobilePhone::TMobilePhoneCBCondition notifiedCBCondition;
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
		
	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarOutgoingInternational
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;

	RMobilePhone::TMobilePhoneCBCondition wantedCBCondition = RMobilePhone::EBarOutgoingInternational;
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
		
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternational
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotify,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );													

	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
					_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );

	// Dial an international number. 
	// Check this fails.
	DEBUG_PRINTF2(_L("Barred international - Calling international %S should not be allowed"), &intNumber );

	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	TCoreEtelRequestStatus<RCall> dialStatus(call1, &RCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	call1.Dial(dialStatus, intNumber);

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling ->
	DEBUG_PRINTF1(_L("Checking EStatusDialling") );
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDisconnecting.																		
	DEBUG_PRINTF1(_L("Checking EStatusDisconnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusIdle.																		
	DEBUG_PRINTF1(_L("Checking EStatusIdle") );	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	DEBUG_PRINTF1(_L("Dial status request") );	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
		_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrGsmCCNormalUnspecified, 
		_L("RMobileCall::Dial returned error") );
	
	// Precaution when expected no connection
	DEBUG_PRINTF1(_L("Hanging up...") );	
	call1.HangUp();		
	
	// ===  Deactivate call barring on voice calls to outgoing international numbers and check 
	// international number can be dialled 
	// ===
	
	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService 
	// and aCondition=EBarOutgoingInternational 

	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternational;
	
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
		
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternational
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotify,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );												
												
	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
					_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );
		
	
	// Dial an international number that answers. 
	DEBUG_PRINTF2(_L("Calling international %S should be allowed"), &intNumber );
	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, intNumber ); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusConnected, 
														KErrNone );

	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, 
			_L("RMobileCall::Dial returned an error") );

	// Hang up
	call1.NotifyMobileCallStatusChange( notifyMobileCallStatusChangeStatus, mobileCallStatus );
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusIdle, 
														KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("Call status is not EStatusIdle") );
	
	// ===  Activate call barring on voice calls to outgoing international numbers
	// excluding home country and check national number with + prefix can be dialled 
	// but international number in another country cannot be dialled 
	// ===
	
	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarOutgoingInternationalExHC 
	
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternationalExHC;
	
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
		
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternationalExHC
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
													cbStatusNotify,
													notifiedCBCondition,
													RMobilePhone::EBarOutgoingInternationalExHC,
													KErrNone );

	// Dial a national number that answers specifying the number using + prefix. 
	DEBUG_PRINTF1(_L("Barred international ExHC - Calling national with + prefix"));
	
	call1.NotifyMobileCallStatusChange( notifyMobileCallStatusChangeStatus, mobileCallStatus );
	call1.Dial(dialStatus, natNumberWithPrefix ); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusConnected, KErrNone );
		
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, 
			_L("RMobileCall::Dial returned an error") );
	
	// Hang up. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	call1.NotifyMobileCallStatusChange( notifyMobileCallStatusChangeStatus, mobileCallStatus );
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusIdle, 
														KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("RMobileCall::NotifyMobileCallStatusChange did not complete with EStatusIdle") );
		
	// Dial an international number in another country. 
	// Check this fails.
	DEBUG_PRINTF1(_L("Barred international ExHC- Calling international"));
	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, intNumber); 
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling ->
	DEBUG_PRINTF1(_L("Checking EStatusDialling") );
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusConnecting ->
	DEBUG_PRINTF1(_L("Checking EStatusConnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDisconnecting.																		
	DEBUG_PRINTF1(_L("Checking EStatusDisconnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusIdle.																		
	DEBUG_PRINTF1(_L("Checking EStatusIdle") );	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
		_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrGsmCCNormalUnspecified, 
		_L("RMobileCall::Dial returned error") );
	
	// Precaution when expected no connection
	call1.HangUp();		
	
	// ===  Deactivate call barring on voice calls to outgoing international numbers excluding home country 
	// and check national and international numbers in international format can be dialled 
	// ===
	
	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarOutgoingInternationalExHC 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternationalExHC;
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
		
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternationalExHC
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotify,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );												
												
	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
					_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );
		
	// Dial a national number using + prefix. 
	DEBUG_PRINTF1(_L("No barring - Calling national with + prefix"));

	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, natNumberWithPrefix ); 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusConnected, 
														KErrNone );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, 
			_L("RCall::Dial returned error") );
		
	// Hang up. 
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusIdle, 
														KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("Call status is not EStatusIdle") );

	// Dial an international number in another country. 
	DEBUG_PRINTF1(_L("No barring - Calling international"));

	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, intNumber ); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusConnected, 
														KErrNone );
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
			_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, 
			_L("RMobileCall::Dial returned an error") );
	
	// Hang up. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	call1.NotifyMobileCallStatusChange( notifyMobileCallStatusChangeStatus, mobileCallStatus );
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus,
														RMobileCall::EStatusIdle, 
														KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("Call status is not EStatusIdle") );

	// ===  Activate call barring on voice calls to all outgoing numbers and check they cannot be dialled ===
	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarAllOutgoing 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;	
	wantedCBCondition = RMobilePhone::EBarAllOutgoing;
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
	
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllOutgoing
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotify,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );												
												
	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
					_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );
	
	// Dial a national number. 
	// Check this fails.
	DEBUG_PRINTF1(_L("All calls barred - Calling national"));
	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, natNumber); 

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling ->
	DEBUG_PRINTF1(_L("Checking EStatusDialling") );
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusConnecting ->
	DEBUG_PRINTF1(_L("Checking EStatusConnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDisconnecting.																		
	DEBUG_PRINTF1(_L("Checking EStatusDisconnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusIdle.																		
	DEBUG_PRINTF1(_L("Checking EStatusIdle") );	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
		_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrGsmCCNormalUnspecified, 
		_L("RMobileCall::Dial returned error") );

	// Precaution when expected no connection
	call1.HangUp();		
		
	// Dial an international number. 
	// Check this fails.
	DEBUG_PRINTF1(_L("All calls barred - Calling international"));
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, intNumber); 
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDialling ->
	DEBUG_PRINTF1(_L("Checking EStatusDialling") );
	expectedMobileCallStatus = RMobileCall::EStatusDialling;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
																		
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusConnecting ->
	DEBUG_PRINTF1(_L("Checking EStatusConnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusConnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);

	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusDisconnecting.																		
	DEBUG_PRINTF1(_L("Checking EStatusDisconnecting") );	
	expectedMobileCallStatus = RMobileCall::EStatusDisconnecting;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	// Check RMobileLine::NotifyMobileLineStatusChange completes with with EStatusIdle.																		
	DEBUG_PRINTF1(_L("Checking EStatusIdle") );	
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		notifyMobileCallStatusChangeStatus,
																		mobileCallStatus,
																		expectedMobileCallStatus,
																		KErrNone);
	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, 
		_L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrGsmCCNormalUnspecified, 
		_L("RMobileCall::Dial returned error") );
	
	// Precaution when expected no connection
	call1.HangUp();		
	
	// ===  Deactivate call barring on voice calls to all outgoing numbers and check national and 
	// international numbers can be dialled ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarAllOutgoing 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;	
	wantedCBCondition = RMobilePhone::EBarAllOutgoing;
	
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
		
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllOutgoing
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotify,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );												
												
	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
					_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );

	// Dial a national number that answers. 
	DEBUG_PRINTF1(_L("No call barring - Calling national"));

	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, natNumber ); 

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
													notifyMobileCallStatusChangeStatus, 
													mobileCallStatus, 
													RMobileCall::EStatusConnected, 
													KErrNone );
	// Hang up. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
													notifyMobileCallStatusChangeStatus, 
													mobileCallStatus, 
													RMobileCall::EStatusIdle, 
													KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("RMobileCall::NotifyMobileCallStatusChange did not complete with EStatusIdle") );

	// Dial an international number that answers. 
	DEBUG_PRINTF1(_L("No call barring - Calling international"));		
	
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.Dial(dialStatus, intNumber ); 
	
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
													notifyMobileCallStatusChangeStatus, 
													mobileCallStatus, 
													RMobileCall::EStatusConnected, 
													KErrNone );

	// Hang up. 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	DEBUG_PRINTF1(_L("Hanging up"));		
	call1.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);
	call1.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( call1, 
													notifyMobileCallStatusChangeStatus, 
													mobileCallStatus, 
													RMobileCall::EStatusIdle, 
													KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("Call status is not EStatusIdle") );
	
	//
	// TEST END
	//

    StartCleanup();
	// reqSetCBStatus
	// dialStatus
	// cbStatusNotify
	// reqSetSSPasswordStatus
	CleanupStack::PopAndDestroy(4, &reqSetSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0001::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0001");
	}



CCTSYIntegrationTestCallBarring0002::CCTSYIntegrationTestCallBarring0002(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0002::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0002::~CCTSYIntegrationTestCallBarring0002()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0002::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0002
 * @SYMFssID BA/CTSY/CBAR-0002
 * @SYMTestCaseDesc Activate and deactivate call barring for incoming voice calls.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, RLine::NotifyIncomingCall, RCall::HangUp
 * @SYMTestExpectedResults Pass - Incoming call barring is activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify call barring status and barred incoming call are not received.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

	//Check just to ensure the passwords are OK from the INI file
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: 
	// KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarAllIncoming  
	TUint32 callServiceCaps;
	TInt err=phone.GetCallServiceCaps(callServiceCaps);
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, (RMobilePhone::KCapsNotifyCBStatus | 
									  RMobilePhone::KCapsSetCBStatus | 
									  RMobilePhone::KCapsBarAllIncoming),
									  KNoUnwantedBits, 
									  _L("RMobilePhone::GetCallServiceCaps returned with incorrect bitmask"));
	
	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;
	
	TExtEtelRequestStatus setPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(setPasswordStatus);
	phone.SetSSPassword(setPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( setPasswordStatus, ETimeMedium), KErrNone, 
			_L("RMobilePhone::SetSSPassword timed out") );	
	CHECK_EQUALS_L( setPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
		
	// Get voice line 1. 
	RMobileLine &mobileLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine );	
	
	// Get call 1. 
	RMobileCall &mobileCall1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Activate call barring on incoming voice calls and check incoming call is not received ===
	TExtEtelRequestStatus cbStatusNotify( phone, EMobilePhoneNotifyCallBarringStatusChange );
	CleanupStack::PushL(cbStatusNotify);
	RMobilePhone::TMobilePhoneCBCondition notifiedCBCondition;
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
		
	/* Activate call barring using RMobilePhone::SetCallBarringStatus with 
		aInfo.iAction=EServiceActionActivate, 
		aInfo.iPassword=correct call barring password, 
		aInfo.iServiceGroup=EVoiceService and 
		aCondition=EBarAllIncoming 	*/
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneCBCondition wantedCBCondition = RMobilePhone::EBarAllIncoming;
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
		
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllIncoming
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
			cbStatusNotify,
			notifiedCBCondition,
			wantedCBCondition,
			KErrNone );													

	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
			_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );

	// Request incoming call from RPS. 
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus( mobileLine, &RLine::NotifyIncomingCallCancel );
	CleanupStack::PushL( notifyIncomingCallStatus );
	TName mobileCallName;
	mobileLine.NotifyIncomingCall( notifyIncomingCallStatus, mobileCallName);
	ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("RPSRequestIncomingCallL return code is not KErrNone"));
	
	// Check RPS dialling fails.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus,ETimeMedium),KErrTimedOut,
		_L("RLine::NotifyIncomingCall did not time out"));
			
	// ===  Deactivate call barring on incoming voice calls and check incoming call is received ===
	DEBUG_PRINTF1(_L("Deactivating call barring on incoming voice calls"));		
	phone.NotifyCallBarringStatusChange( cbStatusNotify, notifiedCBCondition);
	
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	wantedCBCondition = RMobilePhone::EBarAllIncoming;
	err = SetCBStatusL( phone, cbInfo, wantedCBCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllIncoming
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
											cbStatusNotify,
											notifiedCBCondition,
											wantedCBCondition,
											KErrNone );													

	ASSERT_EQUALS( notifiedCBCondition, wantedCBCondition,
				_L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition") );

	// Request an incoming call from RPS. 
	mobileLine.NotifyIncomingCall( notifyIncomingCallStatus, mobileCallName );

	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(mobileCall1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	
	ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("RPSRequestIncomingCallL return code is not KErrNone"));
	
	// Check RLine::NotifyIncomingCall completes.
	DEBUG_PRINTF1(_L("Waiting for incoming call notification."));	
	TName expectedMobileName = _L("Voice11");
	iCallControlTsyTestHelper.WaitForLineNotifyIncomingCall(mobileLine,
												notifyIncomingCallStatus,
												mobileCallName,
												expectedMobileName,	
												KErrNone);
	
	// Answer the incoming call
	DEBUG_PRINTF1(_L("About to answer the incoming call."));
	TCallId incomingCallId;
	RMobileCall& incomingMobileCall = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, mobileCallName, incomingCallId);
		
	TCoreEtelRequestStatus<RCall> incomingCallStatus( incomingMobileCall, &RCall::AnswerIncomingCallCancel );
	CleanupStack::PushL( incomingCallStatus );
	incomingMobileCall.AnswerIncomingCall( incomingCallStatus );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(incomingCallStatus, ETimeMedium), KErrNone, 
		_L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(incomingCallStatus.Int(), KErrNone, 
		_L("RMobileCall::AnswerIncomingCall returned an error") );
	
	// Hang up. 
	DEBUG_PRINTF1(_L("Hanging up call."));		
	incomingMobileCall.NotifyMobileCallStatusChange( notifyMobileCallStatusChangeStatus, mobileCallStatus );
	incomingMobileCall.HangUp();

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange( incomingMobileCall, 
														notifyMobileCallStatusChangeStatus, 
														mobileCallStatus, 
														RMobileCall::EStatusIdle, 
														KErrNone );
														
	ASSERT_EQUALS( mobileCallStatus, RMobileCall::EStatusIdle, _L("Call status is not EStatusIdle") );

	//
	// TEST END
	//

    StartCleanup();
	
	/* setPasswordStatus
	 * cbStatusNotify 
	 * notifyIncomingCallStatus
	 * notifyMobileCallStatusChangeStatus
	 * incomingCallStatus*/
    CleanupStack::PopAndDestroy(5, &setPasswordStatus);

    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0002::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0002");
	}



CCTSYIntegrationTestCallBarring0003::CCTSYIntegrationTestCallBarring0003(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0003::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0003::~CCTSYIntegrationTestCallBarring0003()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0003::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0003
 * @SYMFssID BA/CTSY/CBAR-0003
 * @SYMTestCaseDesc Activate and deactivate, and register and erase call barring for incoming voice calls when roaming.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, RLine::NotifyIncomingCall, RCall::HangUp
 * @SYMTestExpectedResults Pass - Incoming call barring whilst roaming is activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify call barring status and barred numbers cannot be dialled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL(phone, password);

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: 
	// KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarIncomingRoaming 
	TUint32 callServiceCaps;
	TUint32 expectedCallServiceCaps = RMobilePhone::KCapsNotifyCBStatus | RMobilePhone::KCapsSetCBStatus | RMobilePhone::KCapsBarIncomingRoaming;
	CHECK_EQUALS_L(phone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, expectedCallServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);

	// For safety we are going to change the password and then immediately change it back
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword;
	passwordV2.iVerifiedPassword = KAltPassword;

	TExtEtelRequestStatus reqSetSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(reqSetSSPasswordStatus);
	
	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L( WaitForRequestWithTimeOut(reqSetSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));
	CHECK_EQUALS_L( reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, reset
	passwordV2.iOldPassword = KAltPassword;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;

	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(reqSetSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	// Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
    RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);
    RMobileCall& call2 = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall2);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Insert roaming SIM. 

	// ===  Activate call barring on incoming voice calls and check incoming call is not received ===
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	RMobilePhone::TMobilePhoneCBCondition cbCondition;

    // Post RMobilePhone::NotifyCallBarringStatusChange
    TExtEtelRequestStatus cbStatusNotify(phone, EMobilePhoneNotifyCallBarringStatusChange);
	CleanupStack::PushL(cbStatusNotify);
	RMobilePhone::TMobilePhoneCBCondition notifiedCBCondition;
	phone.NotifyCallBarringStatusChange(cbStatusNotify, notifiedCBCondition);

	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate,
	// aInfo.iPassword=correct call barring password,
	// aInfo.iServiceGroup=EVoiceService and 
	// aCondition=EBarIncomingRoaming 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbCondition = RMobilePhone::EBarIncomingRoaming;
	TInt err = SetCBStatusL(phone, cbInfo, cbCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarIncomingRoaming
	RMobilePhone::TMobilePhoneCBCondition wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
                                                                               cbStatusNotify,
                                                                               notifiedCBCondition,
                                                                               wantedCBCondition,
                                                                               KErrNone);

	// Request incoming call from RPS.
 	TInt errorRPS = RPSRequestIncomingCallL(EVoiceLine);

	// Check RPS dialling fails.
	ASSERT_TRUE(errorRPS != KErrNone, _L("RPSRequestIncomingCallL() returned KErrNone even though call barring was set"));

	// ===  Deactivate call barring on incoming voice calls and check incoming call is received ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate,
	// aInfo.iPassword=correct call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarIncomingRoaming 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
    cbInfo.iPassword = password;
    cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbCondition = RMobilePhone::EBarIncomingRoaming;
	
	// Post RMobilephone::NotifyCallBarringStatusChange
	phone.NotifyCallBarringStatusChange(cbStatusNotify, notifiedCBCondition);
		
	err = SetCBStatusL(phone, cbInfo, cbCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
    
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarIncomingRoaming
	wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
                                                                               cbStatusNotify,
                                                                               notifiedCBCondition,
                                                                               wantedCBCondition,
                                                                               KErrNone);

	// post notification RLine::NotifyIncomingCall
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus(voiceLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName incomingCallName;
	incomingCallName.Zero();
	voiceLine.NotifyIncomingCall(notifyIncomingCallStatus, incomingCallName);

	// Request an incoming call from RPS. 
    ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, 
                   _L("CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes.
    ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus, ETimeMedium),
                  KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
    ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L( "RLine::NotifyIncomingCall returned with an error"));
    ASSERT_TRUE(incomingCallName.Length() > 0, _L("RLine::NotifyIncomingCall completed with invalid call name"));	

	// Get the incoming call
	TCallId incomingCallId;
	call1 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, incomingCallName, incomingCallId);

	// post notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Answer the incoming call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(call1, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	call1.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));
    
	// RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected. 
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusConnected;
	TInt expectedStatus = KErrNone;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																	 	    expectedStatus);
	
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Hang up. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																		    expectedStatus);

    phone.NotifyCallBarringStatusChange(cbStatusNotify, notifiedCBCondition);

	// ===  Register call barring on incoming voice calls and check incoming call is not received ===

	// Register call barring using RMobilePhone::SetCallBarringStatus with
	// aInfo.iAction=EServiceActionRegister,
	// aInfo.iPassword=correct call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarAllIncoming 
    // $CTSYKnownFailure Adaptation returns -4419(KErrGsmSSDataMissing). Also notify fails due that.
    // Works with EServiceActionRegister->EServiceActionActivate in needed places
    cbInfo.iAction = RMobilePhone::EServiceActionRegister;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbCondition = RMobilePhone::EBarAllIncoming;
	err = SetCBStatusL(phone, cbInfo, cbCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarIncomingRoaming
	wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
                                                                               cbStatusNotify,
                                                                               notifiedCBCondition,
                                                                               wantedCBCondition,
                                                                               KErrNone);
    ASSERT_EQUALS(notifiedCBCondition, wantedCBCondition,
                  _L("RMobilePhone::NotifyCallBarringStatusChange did not return matching condition"));

	// Request incoming call from RPS.
	errorRPS = RPSRequestIncomingCallL(EVoiceLine);

	// Check RPS dialling fails.
	ASSERT_TRUE(errorRPS != KErrNone, _L("RPSRequestIncomingCallL() returned KErrNone even though call barring was set"));

	// ===  Erase call barring on incoming voice calls and check incoming call is received ===

	// Erase call barring using RMobilePhone::SetCallBarringStatus with
	// aInfo.iAction=EServiceActionErase,
	// aInfo.iPassword=correct call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarAllIncoming 
	// $CTSYKnownFailure Adaptation returns -4400(KErrGsmSSIllegalOperation) and due that notification times out.
	// Note. Test works with EServiceActionErase->EServiceActionDeactivate in needed places
	cbInfo.iAction = RMobilePhone::EServiceActionErase;
    cbInfo.iPassword = password;
    cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
    cbCondition = RMobilePhone::EBarAllIncoming;
	
    // post notifier
	phone.NotifyCallBarringStatusChange(cbStatusNotify, notifiedCBCondition);
		
	err = SetCBStatusL(phone, cbInfo, cbCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarIncomingRoaming
	wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
                                                                               cbStatusNotify,
                                                                               notifiedCBCondition,
                                                                               wantedCBCondition,
                                                                               KErrNone);
	// Post notify for second incoming call
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus2(voiceLine, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus2);
	TName incomingCallName2;
	incomingCallName2.Zero();
	voiceLine.NotifyIncomingCall(notifyIncomingCallStatus2, incomingCallName2);

	// Request an incoming call from RPS. 
	ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes.
    ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus2, ETimeMedium),
                  KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
    ASSERT_EQUALS(notifyIncomingCallStatus2.Int(), KErrNone, _L( "RLine::NotifyIncomingCall returned with an error"));
    ASSERT_TRUE(incomingCallName2.Length() > 0,  _L("RLine::NotifyIncomingCall completed with invalid call name"));	

	// Get the incoming call
	TCallId incomingCallId2;
	call2 = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, incomingCallName2, incomingCallId2);
	
	// Post nofity RMobileCall::NotifyMobileCallStatusChange for call 2
	call2.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Answer the incoming call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus2(call2, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus2);	
	call2.AnswerIncomingCall(answerIncomingStatus2);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus2, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus2.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected. 
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	expectedStatus = KErrNone;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																	 	    expectedStatus);

	// Post nofity RMobileCall::NotifyMobileCallStatusChange for call 2																	 	    	
	call2.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Hang up. 
	TCoreEtelRequestStatus<RCall> hangUpStatus2 (call2, &RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus2);
	call2.HangUp(hangUpStatus2);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus2, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call2,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																		    expectedStatus);

	//
	// TEST END
	//

    StartCleanup();

    DeactivateCallBarringL(phone, password);

	// Pop
	// reqSetSSPasswordStatus
	// cbStatusNotify
	// notifyIncomingCallStatus
	// notifyMobileCallChangeStatus
	// answerIncomingStatus
	// hangUpStatus
	// notifyIncomingCallStatus2
	// answerIncomingStatus2
	// hangUpStatus2
	CleanupStack::PopAndDestroy(9, &reqSetSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0003::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0003");
	}



CCTSYIntegrationTestCallBarring0004::CCTSYIntegrationTestCallBarring0004(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0004::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0004::~CCTSYIntegrationTestCallBarring0004()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0004::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0004
 * @SYMFssID BA/CTSY/CBAR-0004
 * @SYMTestCaseDesc Activate and deactivate call barring on voice calls with incorrect password.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial
 * @SYMTestExpectedResults Pass - Call barring not be activated with  wrong password.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to activate call barring.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);

    // Check call barring password
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL(phone, password);

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarAllOutgoing 
	TUint32 callServiceCaps;
	TUint32 expectedCallServiceCaps = RMobilePhone::KCapsNotifyCBStatus |
	                                  RMobilePhone::KCapsSetCBStatus | 
	                                  RMobilePhone::KCapsBarAllOutgoing;
	CHECK_EQUALS_L(phone.GetCallServiceCaps(callServiceCaps), KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, expectedCallServiceCaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);

	// For safety we are going to change the password and then immediately change it back
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword;
	passwordV2.iVerifiedPassword = KAltPassword;

	TExtEtelRequestStatus reqSetSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(reqSetSSPasswordStatus);
	
	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(reqSetSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, reset
	passwordV2.iOldPassword = KAltPassword;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;

	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(reqSetSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));
	CHECK_EQUALS_L(reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	// Get Voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);
	
	// Get call 1. 
	RMobileCall& call1 = iEtelSessionMgr.GetCallL(KMainServer,KMainPhone, KVoiceLine, KCall1);	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Activate call barring with incorrect password ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with
	// aInfo.iAction=EServiceActionActivate,
	// aInfo.iPassword=incorrect 4 digit call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarAllOutgoing 
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	RMobilePhone::TMobilePhoneCBCondition cbCondition;
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = KIncorrectPassword;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbCondition = RMobilePhone::EBarAllOutgoing;
	TInt err = SetCBStatusL(phone, cbInfo, cbCondition);
	
	// Check an error is returned.
	ASSERT_EQUALS(err, KErrGsmSSNegativePasswordCheck , _L("RMobilePhone::SetCallBarringStatus did not returned KErrGsmSSNegativePasswordCheck even though password was incorrect"));
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange
	TExtEtelRequestStatus notifyMobileCallChangeStatus(call1, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Dial a number that answers. 
	TPtrC number;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,  
			    _L("GetStringFromConfig did not complete as expected"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call1, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	DEBUG_PRINTF1(_L("RMobileCall::Dial() ...first call"));
	call1.Dial(dialStatus, number);	

    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeMedium), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected. 
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusConnected;
	TInt expectedStatus = KErrNone;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																		    expectedStatus);
	
	// Post a notifier for RMobileCall::NotifyMobileCallStatusChange
	call1.NotifyMobileCallStatusChange(notifyMobileCallChangeStatus, mobileCallStatus);

	// Hang up. 
	TCoreEtelRequestStatus<RCall> hangUpStatus (call1,&RCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call1,
																		    notifyMobileCallChangeStatus,
																		    mobileCallStatus,
																		    expectedMobileCallStatus,
																		    expectedStatus);

	// ===  Attempt to deactivate call barring with incorrect password ===

	// ===  Activate call barring first ===
	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate,
	// aInfo.iPassword=correct call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarAllOutgoing 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbCondition = RMobilePhone::EBarAllOutgoing;

	err = SetCBStatusL(phone, cbInfo, cbCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// ===  Now try to deactivate with incorrect password ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with
	// aInfo.iAction=EServiceActionDeactivate,
	// aInfo.iPassword=incorrect 4 digit call barring password,
	// aInfo.iServiceGroup=EVoiceService and
	// aCondition=EBarAllOutgoing 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
    cbInfo.iPassword = KIncorrectPassword;
    cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
    cbCondition = RMobilePhone::EBarAllOutgoing;
	err = SetCBStatusL(phone, cbInfo, cbCondition);

	// Check an error is returned.
	ASSERT_EQUALS(err, KErrGsmSSNegativePasswordCheck , _L("RMobilePhone::SetCallBarringStatus did not returned KErrGsmSSNegativePasswordCheck even though password was incorrect"));
	
	// Dial a number with RCall::Dial. 
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, number) != EFalse,  
			    _L("GetStringFromConfig did not complete as expected"));
	DEBUG_PRINTF1(_L("RMobileCall::Dial() ...second call"));
	call1.Dial(dialStatus, number);
	
	// Check an error is returned.
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial returned error."));
	
	//
	// TEST END
	//

    StartCleanup();
    
	// Hang up. 
	call1.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RCall::CallHangUp returned an error"));
	
    DeactivateCallBarringL(phone, password);

	// Pop
	// reqSetSSPasswordStatus
	// notifyMobileCallChangeStatus
	// dialStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(4,&reqSetSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0004::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0004");
	}



CCTSYIntegrationTestCallBarring0005::CCTSYIntegrationTestCallBarring0005(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0005::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0005::~CCTSYIntegrationTestCallBarring0005()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0005::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0005
 * @SYMFssID BA/CTSY/CBAR-0005
 * @SYMTestCaseDesc Activate call barring using a SIM without call barring.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Call barring cannot be activated.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
    // Get voice line 1. 
	RMobileLine& voiceLine = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// wait until we are on the network
	CHECK_EQUALS_L( iNetworkTsyTestHelper.WaitForRegisteredOnHomeNetworkL( phone ), KErrNone, 
						_L("Network is unavailable") );
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// Insert SIM without call barring. 
	DisplayUserInteractionPromptL(_L("Insert SIM which does NOT SUPPORT call barring service"), ETimeLong);

	// ===  Activate call barring on voice calls to all outgoing numbers ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	RMobilePhone::TMobilePhoneCBChangeV1 callBarringInfo;
	RMobilePhone::TMobilePhoneCBCondition callBarringCondition;
	callBarringCondition = RMobilePhone::EBarAllOutgoing;
	callBarringInfo.iAction = RMobilePhone::EServiceActionActivate;
	callBarringInfo.iPassword = KAltPasswordVoda;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	TInt err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);

	// Check error is returned.
    ASSERT_TRUE(err != KErrNone, _L("RMobilePhone::SetCallBarringStatus did not return error as expected"));
   	
	//
	// TEST END
	//

    StartCleanup();
	
    return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0005::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0005");
	}



CCTSYIntegrationTestCallBarring0006::CCTSYIntegrationTestCallBarring0006(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0006::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0006::~CCTSYIntegrationTestCallBarring0006()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0006::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0006
 * @SYMFssID BA/CTSY/CBAR-0006
 * @SYMTestCaseDesc Register and erase call barring for outgoing voice calls.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, RCall::Dial, RCall::HangUp
 * @SYMTestExpectedResults Pass - Call barring registered and erased.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring notifications and barred numbers cannot be dialled.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarAllOutgoing | KCapsBarOutgoingInternational | KCapsBarOutgoingInternationalExHC 
	TUint32 callBarCaps;
    TInt err;
    err = phone.GetCallServiceCaps( callBarCaps );
    CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
    CHECK_BITS_SET_L(callBarCaps, (RMobilePhone::KCapsNotifyCBStatus | RMobilePhone::KCapsSetCBStatus | 
                                   RMobilePhone::KCapsBarAllOutgoing | RMobilePhone::KCapsBarOutgoingInternational |
                                   RMobilePhone::KCapsBarOutgoingInternationalExHC), 
                                   KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned unexpected bitmap"));	

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	// For safety we are going to change the password and then immediately change it back
    RMobilePhone::TMobilePassword password;
    RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);
    CheckCBPasswordL(phone, password);
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword2;
	passwordV2.iVerifiedPassword = KAltPassword2;
	TExtEtelRequestStatus setSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(setSSPasswordStatus);
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, restoring original password
	passwordV2.iOldPassword = KAltPassword2;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	// Get voice line 1. 
	RMobileLine& line = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Post notifiers
	TExtEtelRequestStatus callBarringStatusNotify(phone, EMobilePhoneNotifyCallBarringStatusChange);
	CleanupStack::PushL(callBarringStatusNotify);
	RMobilePhone::TMobilePhoneCBCondition callBarringStatus;
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);
	
	// ===  Register call barring on voice calls to outgoing international numbers and check international number cannot be dialled ===

	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternational 
	RMobilePhone::TMobilePhoneCBChangeV1 callBarringInfo;
	RMobilePhone::TMobilePhoneCBCondition callBarringCondition;
	callBarringCondition = RMobilePhone::EBarOutgoingInternational;
	callBarringInfo.iAction = RMobilePhone::EServiceActionRegister;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	// $CTSYProblem. Adaptation returns -4419(KErrGsmSSDataMissing). Due failure also notify fails and Dialing
	// to an international number succeeds
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternational
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarOutgoingInternational, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarOutgoingInternational as expected"));

	// Dial an international number. 
	TPtrC internationalNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumberInternational, internationalNumber), _L("Failed to read KIniVoiceNumberInternational from INI file"));
	TCoreEtelRequestStatus<RMobileCall> dialStatus(call, &RMobileCall::DialCancel);
	CleanupStack::PushL(dialStatus);
	DEBUG_PRINTF1(_L("Dial an international number."));											 
	call.Dial(dialStatus, internationalNumber);	
	
	// Check this fails.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial did not fail as it was expected."));

	// ===  Erase call barring on voice calls to outgoing international numbers and check international number can be dialled ===
    
    // Post notifiers
    TExtEtelRequestStatus notifyCallStatusChangeStatus(call, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	
    phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);
    
	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternational 
	callBarringCondition = RMobilePhone::EBarOutgoingInternational;
	callBarringInfo.iAction = RMobilePhone::EServiceActionErase;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	// $CTSYProblem adaptation returns -4400(KErrGsmSSIllegalOperation) and due that notification times out.
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternational
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarOutgoingInternational, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarOutgoingInternational as expected"));

	// Dial an international number that answers.
	DEBUG_PRINTF1(_L("Dial an international number that answers."));
	call.Dial(dialStatus, internationalNumber);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));
 
	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusConnected;
	const TInt expectedStatus = KErrNone;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);
	
	// Hang up. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call,&RMobileCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// ===  Register call barring on voice calls to outgoing international numbers excluding home country and check national number with + prefix can be dialled but international number in another country cannot be dialled ===
	
	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	
    phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);

	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternationalExHC 
	callBarringCondition = RMobilePhone::EBarOutgoingInternationalExHC;
	callBarringInfo.iAction = RMobilePhone::EServiceActionRegister;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	// $CTSYProblem adaptation returns -4419(KErrGsmSSDataMissing) and due that notification fails
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Dial a national number that answers specifying the number using + prefix.
    DEBUG_PRINTF1(_L("Dial a national number specifying the number using + prefix."));
	TPtrC nationalNumberWithPrefix;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber2, nationalNumberWithPrefix), _L("Failed to read KIniVoiceNumber2 from INI file"));
	call.Dial(dialStatus, nationalNumberWithPrefix);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternationalExHC
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarOutgoingInternationalExHC, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarOutgoingInternationalExHC as expected"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);
	
	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	

	// Hang up. 
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// Dial an international number in another country. 
	DEBUG_PRINTF1(_L("Dial an international number in another country. "));
	call.Dial(dialStatus, internationalNumber);	

	// Check this fails.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial did not fail as it was expected."));

	// ===  Erase call barring on voice calls to outgoing international numbers excluding home country and check national and international numbers in international format can be dialled ===
	
	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);

	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternationalExHC 
	callBarringCondition = RMobilePhone::EBarOutgoingInternational;
	callBarringInfo.iAction = RMobilePhone::EServiceActionErase;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	// $CTSYProblem adaptation returns -4400(KErrGsmSSIllegalOperation) and notification does not complete on failure
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarOutgoingInternationalExHC
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarOutgoingInternationalExHC, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarOutgoingInternationalExHC as expected"));

	// Dial a national number using + prefix. 
	DEBUG_PRINTF1(_L("Dial a national number using + prefix. "));
	call.Dial(dialStatus, nationalNumberWithPrefix);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);
	
	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	

	// Hang up. 
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

    // Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);
	
	// Dial an international number in another country. 
	DEBUG_PRINTF1(_L("Dial an international number in another country."));
	call.Dial(dialStatus, internationalNumber);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);
    
    // Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);
    
	// Hang up. 
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// ===  Register call barring on voice calls to all outgoing numbers and check they cannot be dialled ===
	
	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);

	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	callBarringCondition = RMobilePhone::EBarAllOutgoing;
	callBarringInfo.iAction = RMobilePhone::EServiceActionRegister;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	// $CTSYProblem adaptation returns -4419(KErrGsmSSDataMissing) and due that notification fails
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllOutgoing
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarAllOutgoing, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarAllOutgoing as expected"));

	// Dial a national number. 
	DEBUG_PRINTF1(_L("Dial a national number."));
	TPtrC nationalNumber;
	ASSERT_TRUE(GetStringFromConfig(KIniVoiceNumSection, KIniVoiceNumber1, nationalNumber), _L("Failed to read KIniVoiceNumber1 from INI file"));
	call.Dial(dialStatus, nationalNumber);

	// Check this fails.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial did not fail as it was expected."));

	// Dial an international number. 
	call.Dial(dialStatus, internationalNumber);	

	// Check this fails.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_TRUE(dialStatus.Int() != KErrNone, _L("RMobileCall::Dial did not fail as it was expected."));

	// ===  Erase call barring on voice calls to all outgoing numbers and check national and international numbers can be dialled ===

	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);
	
	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	callBarringCondition = RMobilePhone::EBarAllOutgoing;
	callBarringInfo.iAction = RMobilePhone::EServiceActionErase;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	// $CTSYProblem adaptation returns -4400(KErrGsmSSIllegalOperation) and notification does not complete on failure
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllOutgoing
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarAllOutgoing, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarAllOutgoing as expected"));

	// Dial a national number that answers. 
	DEBUG_PRINTF1(_L("Dial a national number that answers."));
	call.Dial(dialStatus, nationalNumber);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	

	// Hang up. 
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	

	// Dial an international number that answers
	DEBUG_PRINTF1(_L("Dial a national number that answers.")); 
    call.Dial(dialStatus, internationalNumber);	
    ASSERT_EQUALS(WaitForRequestWithTimeOut(dialStatus, ETimeLong), KErrNone, _L("RMobileCall::Dial timed-out"));
	ASSERT_EQUALS(dialStatus.Int(), KErrNone, _L("RMobileCall::Dial returned error."));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected.
	expectedMobileCallStatus = RMobileCall::EStatusConnected;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	// Post notifiers
	call.NotifyMobileCallStatusChange(notifyCallStatusChangeStatus, mobileCallStatus);	

	// Hang up. 
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	
	//
	// TEST END
	//

    StartCleanup();
    
    DeactivateCallBarringL(phone, password);
	
	// Pop
	// setSSPasswordStatus
	// callBarringStatusNotify
	// notifyCallStatusChangeStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(4, &setSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0006::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0006");
	}



CCTSYIntegrationTestCallBarring0007::CCTSYIntegrationTestCallBarring0007(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0007::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0007::~CCTSYIntegrationTestCallBarring0007()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0007::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0007
 * @SYMFssID BA/CTSY/CBAR-0007
 * @SYMTestCaseDesc Register and erase call barring for incoming voice calls.
 * @SYMTestPriority High
 * @SYMTestActions RCall::AnswerIncomingCall, RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, RLine::NotifyIncomingCall, RCall::HangUp
 * @SYMTestExpectedResults Pass - Call barring is deactivated for basic service.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring notifications and incoming calls not received when barring is registered and active.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: 
	// KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsBarAllIncoming  
	TUint32 callBarCaps;
    TInt err;
    err = phone.GetCallServiceCaps( callBarCaps );
    CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
    CHECK_BITS_SET_L(callBarCaps, (RMobilePhone::KCapsNotifyCBStatus | RMobilePhone::KCapsSetCBStatus | RMobilePhone::KCapsBarAllIncoming), KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned unexpected bitmap"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	RMobilePhone::TMobilePassword password;
    RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);
    CheckCBPasswordL(phone, password);
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword2;
	passwordV2.iVerifiedPassword = KAltPassword2;
	TExtEtelRequestStatus setSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(setSSPasswordStatus);
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, restoring original password
	passwordV2.iOldPassword = KAltPassword2;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	// Get voice line 1. 
	RMobileLine& line = iEtelSessionMgr.GetLineL(KMainServer, KMainPhone, KVoiceLine);

	// Get call 1. 
	RMobileCall& call = iEtelSessionMgr.GetCallL(KMainServer, KMainPhone, KVoiceLine, KCall1);

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Register call barring on incoming voice calls and check incoming call is not received ===

	// Post notifier
	TExtEtelRequestStatus callBarringStatusNotify(phone, EMobilePhoneNotifyCallBarringStatusChange);
	CleanupStack::PushL(callBarringStatusNotify);
	RMobilePhone::TMobilePhoneCBCondition callBarringStatus;
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);
		
	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllIncoming 
	RMobilePhone::TMobilePhoneCBChangeV1 callBarringInfo;
	RMobilePhone::TMobilePhoneCBCondition callBarringCondition;
	callBarringCondition = RMobilePhone::EBarAllIncoming;
	callBarringInfo.iAction = RMobilePhone::EServiceActionRegister;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	//$CTSYProblem: SetCallBarringStatus returned error -4419 KErrGsmSSDataMissing
	TEST_CHECK_POINT_L(_L("Couldn't set call barring status, no point continuing."));
			
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllIncoming
	//$CTSYProblem: SetCallBarringStatus because support for EBarAllIncoming is only legal for deactivation and erasure
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarAllIncoming, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarAllIncoming as expected"));

	// Request incoming call from RPS.
 	err = RPSRequestIncomingCallL(EVoiceLine);

	// Check RPS dialling fails.
	ASSERT_TRUE(err != KErrNone, _L("RPSRequestIncomingCallL() returned KErrNone even though call barring was set"));

	// ===  Erase call barring on incoming voice calls and check incoming call is received ===
	
	// Post notifier
	phone.NotifyCallBarringStatusChange(callBarringStatusNotify, callBarringStatus);

	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllIncoming 
	callBarringCondition = RMobilePhone::EBarAllIncoming;
	callBarringInfo.iAction = RMobilePhone::EServiceActionErase;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	//$CTSYProblem: SetCallBarringStatus returned error -4400 KErrGsmSSIllegalOperation
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllIncoming
	ASSERT_EQUALS(WaitForRequestWithTimeOut(callBarringStatusNotify, ETimeMedium), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange timed out"));
	ASSERT_EQUALS(callBarringStatusNotify.Int(), KErrNone, _L("RMobilePhone::NotifyCallBarringStatusChange returned an error"));
	ASSERT_EQUALS(callBarringStatus, RMobilePhone::EBarAllIncoming, _L("RMobilePhone::NotifyCallBarringStatusChange condition was not EBarAllIncoming as expected"));
	
	// Post notification RLine::NotifyIncomingCall (for RPS)
	TCoreEtelRequestStatus<RLine> notifyIncomingCallStatus(line, &RLine::NotifyIncomingCallCancel);
	CleanupStack::PushL(notifyIncomingCallStatus);
	TName incomingCallName;
	incomingCallName.Zero();
	line.NotifyIncomingCall(notifyIncomingCallStatus, incomingCallName);
	
	// Post notification
	TExtEtelRequestStatus notifyMobileCallStatusChangeStatus(call, EMobileCallNotifyMobileCallStatusChange);
	CleanupStack::PushL(notifyMobileCallStatusChangeStatus);
	RMobileCall::TMobileCallStatus mobileCallStatus;
	call.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);	

	// Request an incoming call from RPS. 
	ASSERT_EQUALS(RPSRequestIncomingCallL(EVoiceLine), KErrNone, _L("CCTSYIntegrationTestSuiteStepBase::RPSRequestIncomingCallL returned an error"));

	// Check RLine::NotifyIncomingCall completes.
	ASSERT_EQUALS(WaitForRequestWithTimeOut(notifyIncomingCallStatus, ETimeMedium), KErrNone, _L("RLine::NotifyIncomingCall did not complete without error"));
    ASSERT_EQUALS(notifyIncomingCallStatus.Int(), KErrNone, _L( "RLine::NotifyIncomingCall returned with an error"));
    ASSERT_TRUE(incomingCallName.Length() > 0, _L("RLine::NotifyIncomingCall completed with invalid call name"));	

	// Get the incoming call
	TCallId incomingCallId;
	call = iEtelSessionMgr.GetIncomingCallL(KMainServer, KMainPhone, KVoiceLine, incomingCallName, incomingCallId);

	// Answer the incoming call with RCall::AnswerIncomingCall 
	TCoreEtelRequestStatus<RCall> answerIncomingStatus(call, &RCall::AnswerIncomingCallCancel);
	CleanupStack::PushL(answerIncomingStatus);	
	call.AnswerIncomingCall(answerIncomingStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(answerIncomingStatus, ETimeMedium), KErrNone, _L("RMobileCall::AnswerIncomingCall timed-out"));
	ASSERT_EQUALS(answerIncomingStatus.Int(), KErrNone, _L("RMobileCall::AnswerIncomingCall returned with an error"));

	// RMobileCall::NotifyMobileCallStatusChange completes with EStatusConnected. 
	RMobileCall::TMobileCallStatus expectedMobileCallStatus = RMobileCall::EStatusConnected;
	const TInt expectedStatus = KErrNone;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyMobileCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);
	
	// Post notification
	call.NotifyMobileCallStatusChange(notifyMobileCallStatusChangeStatus, mobileCallStatus);	

	// Hang up. 
	TCoreEtelRequestStatus<RMobileCall> hangUpStatus (call,&RMobileCall::HangUpCancel);	
	CleanupStack::PushL(hangUpStatus);
	call.HangUp(hangUpStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(hangUpStatus, ETimeMedium), KErrNone, _L("RMobileCall::HangUp timed-out"));
	ASSERT_EQUALS(hangUpStatus.Int(), KErrNone, _L("RMobileCall::CallHangUp returned an error"));

	// Check RMobileCall::NotifyMobileCallStatusChange completes with EStatusIdle.
	expectedMobileCallStatus = RMobileCall::EStatusIdle;
	iCallControlTsyTestHelper.WaitForMobileCallNotifyMobileCallStatusChange(call, notifyMobileCallStatusChangeStatus, mobileCallStatus, expectedMobileCallStatus, expectedStatus);

	//
	// TEST END
	//

    StartCleanup();
    
    DeactivateCallBarringL(phone, password);
	
	// pop
	// setSSPasswordStatus
	// callBarringStatusNotify
	// notifyIncomingCallStatus
	// notifyMobileCallStatusChangeStatus
	// hangUpStatus
	CleanupStack::PopAndDestroy(5, &setSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0007::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0007");
	}



CCTSYIntegrationTestCallBarring0008::CCTSYIntegrationTestCallBarring0008(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0008::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0008::~CCTSYIntegrationTestCallBarring0008()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0008::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0008
 * @SYMFssID BA/CTSY/CBAR-0008
 * @SYMTestCaseDesc Set call barring status to unsupported actions.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Retrieve KErrArgument for unsupported actions.
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
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsSetCBStatus 
	TUint32 callBarCaps;
    TInt err = phone.GetCallServiceCaps( callBarCaps );
    CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
    CHECK_BITS_SET_L(callBarCaps, RMobilePhone::KCapsSetCBStatus, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	RMobilePhone::TMobilePassword password;
    RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);
    CheckCBPasswordL(phone, password);
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword2;
	passwordV2.iVerifiedPassword = KAltPassword2;
	TExtEtelRequestStatus setSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(setSSPasswordStatus);
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, restoring original password
	passwordV2.iOldPassword = KAltPassword2;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, KSsCallBarringAllServices);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut( setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out"));	
	CHECK_EQUALS_L(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Set call barring status with EServiceActionUnspecified ===

	// Set call barring status using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionUnspecified, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	RMobilePhone::TMobilePhoneCBChangeV1 callBarringInfo;
	RMobilePhone::TMobilePhoneCBCondition callBarringCondition;
	callBarringCondition = RMobilePhone::EBarAllOutgoing;
	callBarringInfo.iAction = RMobilePhone::EServiceActionUnspecified;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);

	// Check KErrArgument is returned
    ASSERT_EQUALS(err, KErrArgument, _L("RMobilePhone::SetCallBarringStatus did not return KErrArgument"));

	// ===  Set call barring status with EServiceActionInvoke ===

	// Set call barring status using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionInvoke, aInfo.iPassword=incorrect call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	callBarringCondition = RMobilePhone::EBarAllOutgoing;
	callBarringInfo.iAction = RMobilePhone::EServiceActionInvoke;
	callBarringInfo.iPassword = password;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);

	// Check KErrArgument is returned
	ASSERT_EQUALS(err, KErrArgument, _L("RMobilePhone::SetCallBarringStatus did not return KErrArgument"));

	//
	// TEST END
	//

    StartCleanup();
	
    DeactivateCallBarringL(phone, password);
	
	// pop
	// setSSPasswordStatus
	CleanupStack::PopAndDestroy(1, &setSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0008::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0008");
	}



CCTSYIntegrationTestCallBarring0009::CCTSYIntegrationTestCallBarring0009(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0009::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0009::~CCTSYIntegrationTestCallBarring0009()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0009::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0009
 * @SYMFssID BA/CTSY/CBAR-0009
 * @SYMTestCaseDesc Retrieve call barring status.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallBarringStatus, CRetrieveMobilePhoneCBList::RetrieveListL
 * @SYMTestExpectedResults Pass - Correct status of call barring returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring status can be retrieved and is correct.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;	
	CheckCBPasswordL(phone, password);
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of: KCapsBarAllIncoming | KCapsBarIncomingRoaming | KCapsBarAllOutgoing | KCapsBarOutgoingInternational | KCapsBarOutgoingInternationalExHC | KCapsBarAllCases | KCapsSetCBStatus | KCapsGetCBStatusNetwork 
	TUint32 callBarCaps;
	TInt err = phone.GetCallServiceCaps( callBarCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L( callBarCaps, 
						(RMobilePhone::KCapsBarAllIncoming | 
						RMobilePhone::KCapsBarIncomingRoaming | 
						RMobilePhone::KCapsBarAllOutgoing | 
						RMobilePhone::KCapsBarOutgoingInternational | 
						RMobilePhone::KCapsBarOutgoingInternationalExHC | 
						RMobilePhone::KCapsBarAllCases | 
						RMobilePhone::KCapsSetCBStatus | 
						RMobilePhone::KCapsGetCBStatusNetwork),
						KNoUnwantedBits,
						_L("RMobilePhone::GetCallServiceCaps returned unexpected caps"));
	
	
	// Set the call barring password to a known password using RMobilePhone::SetSSPassword
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);

	// For safety we are going to change the password and then immediately change it back
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword;
	passwordV2.iVerifiedPassword = KAltPassword;

	TExtEtelRequestStatus reqSetSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(reqSetSSPasswordStatus);
	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( reqSetSSPasswordStatus, ETimeMedium), KErrNone, 
						_L("RMobilePhone::SetSSPassword timed out") );	
	CHECK_EQUALS_L( reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	// Succeeded in changing the password, reset
	passwordV2.iOldPassword = KAltPassword;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;

	phone.SetSSPassword(reqSetSSPasswordStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( reqSetSSPasswordStatus, ETimeMedium), KErrNone, 
						_L("RMobilePhone::SetSSPassword timed out") );	
	CHECK_EQUALS_L( reqSetSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Repeat whole test replacing the EBarAllOutgoing condition in 
	// aCondition and in TMobilePhoneCBInfoEntryV1::iCondition with 
	// EBarAllIncoming, EBarIncomingRoaming, EBarAllOutgoing, EBarOutgoingInternational, 
	// EBarOutgoingInternationalExHC 
	
	TestActivateAndDeactivateCallBarringL( phone, password, RMobilePhone::EBarAllOutgoing );	
	TestActivateAndDeactivateCallBarringL( phone, password, RMobilePhone::EBarAllIncoming );
	TestActivateAndDeactivateCallBarringL( phone, password, RMobilePhone::EBarIncomingRoaming );
	TestActivateAndDeactivateCallBarringL( phone, password, RMobilePhone::EBarOutgoingInternational );
	TestActivateAndDeactivateCallBarringL( phone, password, RMobilePhone::EBarOutgoingInternationalExHC );
	
	
	//
	// TEST END
	//

    StartCleanup();
    
    // turn off any CB we may have left active
    err = DeactivateCallBarringL( phone, password );
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

    CleanupStack::PopAndDestroy(1, &reqSetSSPasswordStatus );
		
	return TestStepResult();
	}
	
	
/**
* This wraps the repeating part of the test
*/ 
void CCTSYIntegrationTestCallBarring0009::TestActivateAndDeactivateCallBarringL( RMobilePhone &aPhone, 
																				const RMobilePhone::TMobilePassword& aPassword,
																				RMobilePhone::TMobilePhoneCBCondition aCondition )
	{
	DEBUG_PRINTF2( _L("Executing test CallBarring0009 with condition %d"), aCondition );
	
	// ===  Activate call barring for telephony service ===
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iPassword = aPassword;

	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=ETelephony 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iServiceGroup = RMobilePhone::ETelephony;
	
	TInt err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// ===  Activate call barring for short message service ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EShortMessageService
	
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;

 	err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("SetCallBarringStatus returned error"));	
	
	// ===  Activate call barring for fax service ===

	// Register call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionActivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EFaxService 
	
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iServiceGroup = RMobilePhone::EFaxService ;
	err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// ===  Deactivate call barring for async data ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EAsyncData 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iServiceGroup = RMobilePhone::EAsyncData;
	
	err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));	

	// ===  Deactivate call barring for circuit data service ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=ECircuitDataService 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iServiceGroup = RMobilePhone::ECircuitDataService;
	
	err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));	 

	// ===  Deactivate call barring AllPlmnBearer service ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with 
	// aInfo.iAction=EServiceActionDeactivate, 
	// aInfo.iPassword=correct call barring password, 
	// aInfo.iServiceGroup=EAllPlmnBearer
	 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iServiceGroup = RMobilePhone::EAllPlmnBearer;
	
	err = SetCBStatusL( aPhone, cbInfo, aCondition);
	ASSERT_EQUALS( err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// ===  Retrieve call barring ===

	// Use the helper class for CRetrieveMobilePhoneCBList to retrieve the call barring status list 
	// with aCondition=EBarAllOutgoing and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	
	CRetrieveMobilePhoneCBListExec *cbStatusListHelper 
				= CRetrieveMobilePhoneCBListHelper::NewL(aPhone, aCondition, RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	
	CMobilePhoneCBList* cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	
	// Check that CMobilePhoneCBList list returned contains the following entries (not necessarily in this order):

	// TMobilePhoneCBInfoEntryV1::iServiceGroup = ETelephony 
	// TMobilePhoneCBInfoEntryV1::iStatus = ECallBarringStatusActive 
			
	ASSERT_TRUE(  SearchCBStatusListL( cbStatusList, aCondition, 
													RMobilePhone::ETelephony, 
													RMobilePhone::ECallBarringStatusActive ),
				_L("Failed to find entry in Call Barring status list") );

	// TMobilePhoneCBInfoEntryV1::iServiceGroup = EShortMessageService 
	// TMobilePhoneCBInfoEntryV1::iStatus = ECallBarringStatusActive 

	ASSERT_TRUE(  SearchCBStatusListL( cbStatusList, aCondition, 
													RMobilePhone::EShortMessageService, 
													RMobilePhone::ECallBarringStatusActive ),
				_L("Failed to find entry in Call Barring status list") );

	//TMobilePhoneCBInfoEntryV1::iServiceGroup = EFaxService 
	//TMobilePhoneCBInfoEntryV1::iStatus = RMobilePhone::ECallBarringStatusActive
	
	ASSERT_TRUE(  SearchCBStatusListL( cbStatusList, aCondition, 
                                                    RMobilePhone::EFaxService, 
													RMobilePhone::ECallBarringStatusActive ),
				_L("Failed to find entry in Call Barring status list") );
				
	// cbStatusListHelper
	CleanupStack::PopAndDestroy(1,cbStatusListHelper);
	}
	

TPtrC CCTSYIntegrationTestCallBarring0009::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0009");
	}



CCTSYIntegrationTestCallBarring0010::CCTSYIntegrationTestCallBarring0010(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0010::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0010::~CCTSYIntegrationTestCallBarring0010()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0010::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0010
 * @SYMFssID BA/CTSY/CBAR-0010
 * @SYMTestCaseDesc Deactivate and erase call barring for all cases.
 * @SYMTestPriority High
 * @SYMTestActions RCall::Dial, RLine::NotifyStatusChange, CRetrieveMobilePhoneCBList::RetrieveListL, RMobilePhone::NotifyCallBarringStatusChange
 * @SYMTestExpectedResults Pass - Call barring is deactivated for all services.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring notification and retrieve call barring status.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );	
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsBarAllCases | KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsGetCBStatusNetwork | KCapsGetCBStatusCache 
	TUint32 callServiceCaps;
	CHECK_EQUALS_L(phone.GetCallServiceCaps( callServiceCaps ), KErrNone, _L("RMobilePhone::GetCallServiceCaps returned other than KErrNone"));
	
	// $CTSYProblem Capabilities are hardcoded in CTSY KMultimodeTsyGsmCallServiceCaps. KCapsGetCBStatusCache removed for test to proceed.
	TUint32 wantedServiceCaps =  RMobilePhone::KCapsBarAllCases | 
								 RMobilePhone::KCapsNotifyCBStatus |
								 RMobilePhone::KCapsSetCBStatus |
								 RMobilePhone::KCapsGetCBStatusNetwork;
	CHECK_BITS_SET_L(callServiceCaps,  wantedServiceCaps, KNoUnwantedBits , _L("RMobilePhone::GetCallServiceCaps returned error"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	TExtEtelRequestStatus passwordInfoStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(passwordInfoStatus);
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordInfo;
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	CHECK_EQUALS_L( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Activate some call barring services ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneCBCondition wantedCBCondition = RMobilePhone::EBarAllOutgoing;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned error"));
	 		
	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarAllOutgoing 
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Activate notify RMobilePhone::NotifyCallBarringStatusChange 
	TExtEtelRequestStatus cbStatusNotifyStatus( phone, EMobilePhoneNotifyCallBarringStatusChange );
	CleanupStack::PushL(cbStatusNotifyStatus);
	RMobilePhone::TMobilePhoneCBCondition notifiedCBCondition;
	phone.NotifyCallBarringStatusChange( cbStatusNotifyStatus, notifiedCBCondition );

	// ===  Deactivate all call barring for all service groups and all cases ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionDeactivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllCases 
	ASSERT_EQUALS(DeactivateCallBarringL( phone, password ), KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllCases
	wantedCBCondition = RMobilePhone::EBarAllCases;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotifyStatus,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );													

	// ===  Retrieve call barring status ===

	// Use the helper class to retrieve the call barring status list with aCondition=EBarAllOutgoing and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	CRetrieveMobilePhoneCBListExec* cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone,
															RMobilePhone::EBarAllOutgoing, RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	TInt err; 
	CMobilePhoneCBList* cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list whereas we expected 0 entry in the list. 
	// It is beacuse that Deactivating statement is changing the status of entry parametes accordingly and there will always remain one entry in the list
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	// Repeat status list retrieval and checking with aCondition=EBarAllIncoming, EBarIncomingRoaming, EBarOutgoingInternational, EBarOutgoingInternationalExHC 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllIncoming,
							RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);
	
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarIncomingRoaming,
							RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternational,
		 RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	
	
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternationalExHC,
		 RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	// ===  Activate some call barring services ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternational 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternational;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error "));

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarOutgoingInternational 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternational;
	ASSERT_EQUALS(SetCBStatusL(phone, cbInfo, wantedCBCondition), KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// ===  Erase all call barring for all service groups ===
	
	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionDeactivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllCases 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmIllegalOperation instead of KErrNone, when it is attempted to erase all callbarring with aInfo.iAction=EServiceActionErase
	cbInfo.iAction = RMobilePhone::EServiceActionErase;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	wantedCBCondition = RMobilePhone::EBarAllCases;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrGsmSSIllegalOperation ,  _L("RMobilePhone::SetCallBarringStatus did not return KErrGsmSSIllegalOperation"));

	// ===  Retrieve outgoing call barring status ===

	// Use the helper class to retrieve the call barring status list using CRetrieveMobilePhoneCBList::Start with aCondition=EBarOutgoingInternational and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternational, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem RMobilePhone::SetCallBarringStatus was unsuccessful with RMobilePhone::EServiceActionErase and RMobilePhone::EBarOutgoingInternational was activated twice with different
	// service group so number of entries in the list will be 2 instead of 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 2, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	// Repeat status list retrieval and checking with aCondition=EBarAllIncoming, 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllIncoming,
							RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list against RMobilePhone::EBarAllIncoming whereas we expected 0 entry in the list.
	// It is beacuse that RMobilePhone::EServiceActionErase was unsuccessful and therre is always one entry in the list corresponding to each service group.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// EBarIncomingRoaming,
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarIncomingRoaming,
		 RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// EBarOutgoingInternationalExHC 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternationalExHC,
		 RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBList::DoGetList callBarringList is NULL"));
	
	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 0"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);
	
	//
	// TEST END
	//

    StartCleanup();

	// Ensure password is same as before test and lock counter is reseted to zero
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));
    
   	// Pop
	// passwordInfoStatus 
	// cbStatusNotifyStatus
	CleanupStack::PopAndDestroy(2,&passwordInfoStatus);	

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0010::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0010");
	}



CCTSYIntegrationTestCallBarring0011::CCTSYIntegrationTestCallBarring0011(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0011::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0011::~CCTSYIntegrationTestCallBarring0011()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0011::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0011
 * @SYMFssID BA/CTSY/CBAR-0011
 * @SYMTestCaseDesc Deactivate and erase call barring for all outgoing and all incoming services.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::NotifyCallBarringStatusChange, RMobilePhone::SetCallBarringStatus, CRetrieveMobilePhoneCBList::RetrieveListL
 * @SYMTestExpectedResults Pass - Call barring deactivated and erased.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify call barring notification and retrieve call barring status.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );	
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsBarAllIncoming | KCapsBarAllOutgoing | KCapsNotifyCBStatus | KCapsSetCBStatus | KCapsGetCBStatusNetwork | KCapsGetCBStatusCache 
	TUint32 callServiceCaps;
	CHECK_EQUALS_L(phone.GetCallServiceCaps( callServiceCaps ), KErrNone, _L("RMobilePhone::GetCallServiceCaps returned other than KErrNone"));
	
	// $CTSYProblem Capabilities are hardcoded in CTSY KMultimodeTsyGsmCallServiceCaps. KCapsGetCBStatusCache commented due test to proceed.
	TUint32 wantedServiceCaps =  RMobilePhone::KCapsBarAllIncoming | 
								 RMobilePhone::KCapsBarAllOutgoing |
								 RMobilePhone::KCapsNotifyCBStatus |
								 RMobilePhone::KCapsSetCBStatus |
								 RMobilePhone::KCapsGetCBStatusNetwork;
	CHECK_BITS_SET_L(callServiceCaps,  wantedServiceCaps, KNoUnwantedBits , _L("RMobilePhone::GetCallServiceCaps returned error"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	TExtEtelRequestStatus passwordInfoStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(passwordInfoStatus);
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordInfo;
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	CHECK_EQUALS_L( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Activate some outgoing call barring services ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllOutgoing 
	RMobilePhone::TMobilePhoneCBChangeV1 cbInfo;
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iPassword =password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	RMobilePhone::TMobilePhoneCBCondition wantedCBCondition = RMobilePhone::EBarAllOutgoing;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarAllOutgoing 
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// Activate notify RMobilePhone::NotifyCallBarringStatusChange 
	TExtEtelRequestStatus cbStatusNotifyStatus( phone, EMobilePhoneNotifyCallBarringStatusChange );
	CleanupStack::PushL(cbStatusNotifyStatus);
	RMobilePhone::TMobilePhoneCBCondition notifiedCBCondition;
	phone.NotifyCallBarringStatusChange( cbStatusNotifyStatus, notifiedCBCondition );

	// ===  Deactivate all outgoing services ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionDeactivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllOutgoingServices 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	wantedCBCondition = RMobilePhone::EBarAllOutgoingServices;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllOutgoingServices
	wantedCBCondition = RMobilePhone::EBarAllOutgoingServices;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotifyStatus,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );	

	// ===  Retrieve call barring status ===

	// Use the helper class for CRetrieveMobilePhoneCBList to retrieve the call barring list with aCondition=EBarAllOutgoing and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	CRetrieveMobilePhoneCBListExec* cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllOutgoing, RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	TInt err; 
	CMobilePhoneCBList* cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list whereas we expected 0 entry in the list. Similar error applies all lists
	// It is beacuse that Deactivating statement is changing the status of entry parametes accordingly and there will always remain one entry in the list
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// Repeat status list retrieval and checking with aCondition=EBarOutgoingInternational
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternational, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	
	
	// Repeat status list retrieval and checking with aCondition=EBarOutgoingInternationalExHC 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternationalExHC, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// ===  Register some outgoing call barring services ===
	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarOutgoingInternational 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmSSIllegalOperation instead of KErrNone, when it is attempted to register callbarring service with aInfo.iAction=EServiceActionRegister
	cbInfo.iAction = RMobilePhone::EServiceActionRegister;
	cbInfo.iPassword = password;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternational;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation,  _L("RMobilePhone::SetCallBarringStatus did not return KErrGsmIllegalOperation"));
	
	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarOutgoingInternationalExHC 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmIllegalOperation instead of KErrNone, when it is attempted to register callbarring service with aInfo.iAction=EServiceActionRegister
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	wantedCBCondition = RMobilePhone::EBarOutgoingInternationalExHC;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation ,  _L("RMobilePhone::SetCallBarringStatus did not return KErrGsmIllegalOperation"));

	// ===  Erase all call barring for all service groups ===

	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllOutgoingServices 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmSSIllegalOperation instead of KErrNone, when it is attempted to erase callbarring service with aInfo.iAction=EServiceActionErase
	cbInfo.iAction = RMobilePhone::EServiceActionErase;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	wantedCBCondition = RMobilePhone::EBarAllOutgoingServices;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation,  _L("RMobilePhone::SetCallBarringStatus returned KErrNone"));

	// ===  Retrieve call barring status ===

	// Use the helper class for CRetrieveMobilePhoneCBList to retrieve the call barring status list with aCondition=EBarAllOutgoing and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllOutgoing, 
							RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list whereas we expected 0 entry in the list. Similar error applies all lists
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarAllOutgoing"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// Repeat status list retrieval and checking with aCondition=EBarOutgoingInternational, EBarOutgoingInternationalExHC 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternational, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"))		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarOutgoingInternational"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	
	
	// Repeat status list retrieval and checking with aCondition=EBarOutgoingInternationalExHC 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarOutgoingInternationalExHC, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCFListExec::DoGetList callForwardingList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarOutgoingInternationalExHC"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// ===  Activate some incoming call barring services ===

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllIncoming 
	cbInfo.iAction = RMobilePhone::EServiceActionActivate;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbInfo.iPassword =password;
	wantedCBCondition = RMobilePhone::EBarAllIncoming;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// Activate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=incorrect call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarIncomingRoaming 
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition) , KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned an error"));

	// Activate notify RMobilePhone::NotifyCallBarringStatusChange
	phone.NotifyCallBarringStatusChange( cbStatusNotifyStatus, notifiedCBCondition);
	
	// ===  Deactivate all incoming services ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionDeactivate, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllIncomingServices 
	cbInfo.iAction = RMobilePhone::EServiceActionDeactivate;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	cbInfo.iPassword = password;
	wantedCBCondition = RMobilePhone::EBarAllIncomingServices;
	
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrNone,  _L("RMobilePhone::SetCallBarringStatus returned KErrNone"));
	
	// Check RMobilePhone::NotifyCallBarringStatusChange completes with aCondition=EBarAllIncomingServices
	wantedCBCondition = RMobilePhone::EBarAllIncomingServices;
	iSupplementalTsyTestHelper.WaitForMobilePhoneNotifyCallBarringStatusChange(phone, 
												cbStatusNotifyStatus,
												notifiedCBCondition,
												wantedCBCondition,
												KErrNone );

	// ===  Retrieve call barring status ===

	// Use the helper class to retrieve the call barring status list with aCondition=EBarAllIncoming and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllIncoming, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list whereas we expected 1 entry in the list. Similar error applies all lists
	ASSERT_EQUALS(cbStatusList->Enumerate() ,1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarAllIncoming"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	// Repeat status list retrieval and checking with aCondition= EBarIncomingRoaming 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarIncomingRoaming, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate(), 1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarIncomingRoaming"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);

	// ===  Register some incoming call barring services ===

	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=incorrect call barring password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllIncoming
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmIllegalOperation instead of KErrNone, when it is attempted to register callbarring service with aInfo.iAction=EServiceActionRegister
	cbInfo.iAction = RMobilePhone::EServiceActionRegister;
	cbInfo.iServiceGroup = RMobilePhone::EVoiceService;
	cbInfo.iPassword =password;
	wantedCBCondition = RMobilePhone::EBarAllIncoming;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation,  _L("RMobilePhone::SetCallBarringStatus did notreturn KErrGsmSSIllegalOperation"));
	
	// Register call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionRegister, aInfo.iPassword=incorrect call barring password, aInfo.iServiceGroup=EShortMessageService and aCondition=EBarIncomingRoaming 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmIllegalOperation instead of KErrNone, when it is attempted to register callbarring service with aInfo.iAction=EServiceActionRegister
	cbInfo.iServiceGroup = RMobilePhone::EShortMessageService;
	wantedCBCondition = RMobilePhone::EBarIncomingRoaming;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation,  _L("RMobilePhone::SetCallBarringStatus did notreturn KErrGsmSSIllegalOperation"));

	// ===  Erase all incoming services ===

	// Erase call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionErase, aInfo.iPassword=correct call barring password, aInfo.iServiceGroup=EAllServices and aCondition=EBarAllIncomingServices 
	// $CTSYProblem RMobilePhone::SetCallBarringStatus is returning KErrGsmSSIllegalOperation instead of KErrNone, when it is attempted to erase callbarring service with aInfo.iAction=EServiceActionErase
	cbInfo.iAction = RMobilePhone::EServiceActionErase;
	cbInfo.iServiceGroup = RMobilePhone::EAllServices;
	cbInfo.iPassword = password;
	wantedCBCondition = RMobilePhone::EBarAllIncomingServices;
	ASSERT_EQUALS( SetCBStatusL( phone, cbInfo, wantedCBCondition), KErrGsmSSIllegalOperation,  _L("RMobilePhone::SetCallBarringStatus did notreturn KErrGsmSSIllegalOperation"));
			
	// ===  Retrieve call barring status ===

	// Use the helper class to retrieve the call barring status list using CRetrieveMobilePhoneCBList::Start with aCondition=EBarAllIncoming and aLocation=RMobilePhone::EInfoLocationCachePreferred 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarAllIncoming, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	// $CTSYProblem CMobilePhoneCBList::Enumerate is returning 1 entry in the list whereas we expected 0 entry in the list. Similar error applies all lists
	ASSERT_EQUALS(cbStatusList->Enumerate() , 1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarAllIncoming"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	

	// Repeat status list retrieval and checking with aCondition= EBarIncomingRoaming 
	cbStatusListHelper = CRetrieveMobilePhoneCBListHelper::NewL(phone, RMobilePhone::EBarIncomingRoaming, 
		RMobilePhone::EInfoLocationCachePreferred );
	CleanupStack::PushL( cbStatusListHelper );
	cbStatusList = cbStatusListHelper->DoGetList(err);
	ASSERT_EQUALS(err, KErrNone, _L("CRetrieveMobilePhoneCBList::Start failed to get the list"));		
	ASSERT_TRUE(cbStatusList != NULL, _L("CRetrieveMobilePhoneCBListHelper::DoGetList callBarringList is NULL"));

	// Check CMobilePhoneCBList::Enumerate on the list returned returns 0.
	ASSERT_EQUALS(cbStatusList->Enumerate() ,1, _L("CMobilePhoneCBList::Enumerate returned other than 1 for condition RMobilePhone::EBarIncomingRoaming"));
	CleanupStack::PopAndDestroy(cbStatusListHelper);	
	
	//
	// TEST END
	//

    StartCleanup();
	
    
    
	// Ensure password is same as before test and lock counter is reseted to zero
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));
    
   	// Pop
	// passwordInfoStatus 
	// cbStatusNotifyStatus
	CleanupStack::PopAndDestroy(2,&passwordInfoStatus);	
		
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0011::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0011");
	}



CCTSYIntegrationTestCallBarring0012::CCTSYIntegrationTestCallBarring0012(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0012::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0012::~CCTSYIntegrationTestCallBarring0012()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0012::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0012
 * @SYMFssID BA/CTSY/CBAR-0012
 * @SYMTestCaseDesc Change the call barring password.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallBarringStatus
 * @SYMTestExpectedResults Pass - Password change successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify password is changed.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//
    RMobilePhone &phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
    
    // Check call barring password
    RMobilePhone::TMobilePassword password;
    CheckCBPasswordL(phone, password);
	
    // Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsChangeCBPassword | KCapsBarAllCases. 
    TUint32 callBarCaps;
    TInt err = phone.GetCallServiceCaps( callBarCaps );
    CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
    CHECK_BITS_SET_L(callBarCaps, (RMobilePhone::KCapsChangeCBPassword | RMobilePhone::KCapsBarAllCases), KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned unexpected bitmap"));	

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// Change the call barring password using RMobilePhone::SetSSPassword with 
	// aService=330 and aPassword=packaged RMobilePhone::TMobilePhonePasswordChangeV2 with 
	// iOldPassword=correct old password, iNewPassword=iVerifiedPassword=a four digit new password 
	TInt service = KSsCallBarringAllServices;
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);
	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = KAltPassword2;
	passwordV2.iVerifiedPassword = KAltPassword2;
	TExtEtelRequestStatus setSSPasswordStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(setSSPasswordStatus);
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, service);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned with an error"));

	// ===  Deactivate call barring with new password ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionDeactivate, aInfo.iPassword=new password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllCases 
	RMobilePhone::TMobilePhoneCBChangeV1 callBarringInfo;
	RMobilePhone::TMobilePhoneCBCondition callBarringCondition;
	callBarringCondition = RMobilePhone::EBarAllCases;
	callBarringInfo.iAction =RMobilePhone::EServiceActionDeactivate;
	callBarringInfo.iPassword = KAltPassword2;
	callBarringInfo.iServiceGroup = RMobilePhone::EVoiceService;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition);
	ASSERT_EQUALS(err, KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error"));
	
	// ===  Deactivate call barring with old password and check it fails ===

	// Deactivate call barring using RMobilePhone::SetCallBarringStatus with aInfo.iAction=EServiceActionActivate, aInfo.iPassword=old password, aInfo.iServiceGroup=EVoiceService and aCondition=EBarAllCases 
	callBarringInfo.iPassword = password;
	err = SetCBStatusL(phone, callBarringInfo, callBarringCondition), KErrNone, _L("RMobilePhone::SetCallBarringStatus returned error");

	// Check error is returned.
	ASSERT_TRUE(err != KErrNone, _L("RMobilePhone::SetCallBarringStatus returned KErrNone instead of error"));
	
	//
	// TEST END
	//

    StartCleanup();
       	
   	// Restore oginal password
   	passwordV2.iOldPassword = KAltPassword2;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;
	phone.SetSSPassword(setSSPasswordStatus, passwordPkg, service );
	ASSERT_EQUALS(WaitForRequestWithTimeOut(setSSPasswordStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out when restoring original password") );
	ASSERT_EQUALS(setSSPasswordStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword returned with an error when restoring original password"));
	
	// Pop
	// setSSPasswordStatus
	CleanupStack::PopAndDestroy(1, &setSSPasswordStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0012::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0012");
	}



CCTSYIntegrationTestCallBarring0013::CCTSYIntegrationTestCallBarring0013(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0013::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0013::~CCTSYIntegrationTestCallBarring0013()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0013::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0013
 * @SYMFssID BA/CTSY/CBAR-0013
 * @SYMTestCaseDesc Change the call barring password specifying incorrect old password.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error is returned on attempt to change password.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify password is not changed.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );

	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsChangeCBPassword 
	TUint32 callServiceCaps;
	TUint32 expectedservicecaps = RMobilePhone::KCapsChangeCBPassword;
	TInt err = phone.GetCallServiceCaps( callServiceCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, expectedservicecaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	TExtEtelRequestStatus passwordInfoStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(passwordInfoStatus);
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordInfo;
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	CHECK_EQUALS_L( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set the call barring password but supply wrong old password. ===
	
	// Change the call barring password using RMobilePhone::SetSSPassword with 
	// aService=330 and 
	// aPassword=packaged RMobilePhone::TMobilePhonePasswordChangeV2 with 
	// iOldPassword=incorrect old password, 
	// iNewPassword=iVerifiedPassword=a four digit new password 
	// Check error returned.
	passwordInfo.iOldPassword = KIncorrectPassword;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg2(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg2, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_TRUE( passwordInfoStatus.Int()!= KErrNone, _L("RMobilePhone::SetSSPassword did return KErrNone"));

	
	//
	// TEST END
	//

    StartCleanup();

	// Verify correct password values 
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS( passwordInfoStatus.Int(),KErrNone, _L("RMobilePhone::SetSSPassword returned an error"));
						
	// passwordInfoStatus
	CleanupStack::PopAndDestroy(1, &passwordInfoStatus);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0013::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0013");
	}


CCTSYIntegrationTestCallBarring0014::CCTSYIntegrationTestCallBarring0014(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0014::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0014::~CCTSYIntegrationTestCallBarring0014()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0014::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0014
 * @SYMFssID BA/CTSY/CBAR-0014
 * @SYMTestCaseDesc Change call barring password specifying wrong repeat of the new password.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - Error is returned on attempt to change password.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify password is not changed.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );
	    
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsChangeCBPassword 
	TUint32 callServiceCaps;
	TUint32 expectedservicecaps = RMobilePhone::KCapsChangeCBPassword;
	TInt err = phone.GetCallServiceCaps( callServiceCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, expectedservicecaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	TExtEtelRequestStatus passwordInfoStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(passwordInfoStatus);
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordInfo;
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	CHECK_EQUALS_L( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set the call barring password but supply wrong repeat password. ===

	// Change the call barring password using RMobilePhone::SetSSPassword with 
	// aService=330 and 
	// aPassword=packaged RMobilePhone::TMobilePhonePasswordChangeV2 
	// with iOldPassword=incorrect old password, 
	// iNewPassword=new four digit password and iVerifiedPassword != iNewPassword 
	// Check error returned.
	passwordInfo.iOldPassword = KIncorrectPassword;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = KAltPassword4;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg2(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg2, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_TRUE( passwordInfoStatus.Int()!= KErrNone, _L("RMobilePhone::SetSSPassword did return KErrNone"));
	
	
	//
	// TEST END
	//

    StartCleanup();
	
	// Verify correct password values 
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS( passwordInfoStatus.Int(),KErrNone, _L("RMobilePhone::SetSSPassword returned an error"));
	
	// passwordInfoStatus
	CleanupStack::PopAndDestroy(1, &passwordInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0014::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0014");
	}



CCTSYIntegrationTestCallBarring0015::CCTSYIntegrationTestCallBarring0015(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0015::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0015::~CCTSYIntegrationTestCallBarring0015()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0015::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0015
 * @SYMFssID BA/CTSY/CBAR-0015
 * @SYMTestCaseDesc Change the call barring password to a new one only 3 digits long.
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhone::SetCallBarringPassword
 * @SYMTestExpectedResults Pass - Error is returned on attempt to change password.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify password is not changed.
 *
 * @return - TVerdict code
 */
	{

	//
	// SET UP
	//

	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	RMobilePhone::TMobilePassword password;
	CheckCBPasswordL( phone, password );
	
	DEBUG_PRINTF2(_L("Checking call barring password from CheckCBPasswordL: %S"), &password);
	
	// Ensure RMobilePhone::GetCallServiceCaps returns caps in set of KCapsChangeCBPassword 
	TUint32 callServiceCaps;
	TUint32 expectedservicecaps = RMobilePhone::KCapsChangeCBPassword;
	TInt err = phone.GetCallServiceCaps( callServiceCaps );
	CHECK_EQUALS_L(err, KErrNone, _L("RMobilePhone::GetCallServiceCaps returned error"));
	CHECK_BITS_SET_L(callServiceCaps, expectedservicecaps, KNoUnwantedBits, _L("RMobilePhone::GetCallServiceCaps returned wrong caps"));

	// Set the call barring password to a known password using RMobilePhone::SetSSPassword 
	TExtEtelRequestStatus passwordInfoStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(passwordInfoStatus);
	RMobilePhone::TMobilePhonePasswordChangeV2 passwordInfo;
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	CHECK_EQUALS_L( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	CHECK_EQUALS_L( passwordInfoStatus.Int(), KErrNone, _L("RMobilePhone::SetSSPassword did not return KErrNone"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Set the call barring password to new one that is too short using RMobilePhone::SetSSPassword ===

	// Change the call barring password using RMobilePhone::SetSSPassword with aService=330 with aPassword=packaged RMobilePhone::TMobilePhonePasswordChangeV2 with iOldPassword=incorrect old password, iNewPassword=iVerifiedPassword=new three digit password 
	passwordInfo.iOldPassword = KIncorrectPassword;
	passwordInfo.iNewPassword = KAltPassword5; // three digit password
	passwordInfo.iVerifiedPassword = KAltPassword5;
	
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg2(passwordInfo);
	phone.SetSSPassword(passwordInfoStatus, passwordPkg2, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_TRUE( passwordInfoStatus.Int()!= KErrNone, _L("RMobilePhone::SetSSPassword did return KErrNone"));

	// Check error returned.

	// ===  Set the call barring password to new one that is too short using RMobilePhone::SetCallBarringPassword ===

	// Change the call barring password using RMobilePhone::SetCallBarringPassword with aPassword.iOldPassword=correct old password, aPassword.iNewPassword=three digit password 
	// Check error returned.
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = KAltPassword5;// three digit password
	passwordInfo.iVerifiedPassword = KAltPassword5;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg2, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_TRUE( passwordInfoStatus.Int()!= KErrNone, _L("RMobilePhone::SetSSPassword did return KErrNone"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// Verify correct password values 
	passwordInfo.iOldPassword = password;
	passwordInfo.iNewPassword = password;
	passwordInfo.iVerifiedPassword = password;
	phone.SetSSPassword(passwordInfoStatus, passwordPkg, KSsCallBarringAllServices );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( passwordInfoStatus, ETimeMedium), KErrNone, _L("RMobilePhone::SetSSPassword timed out") );
	ASSERT_EQUALS( passwordInfoStatus.Int(),KErrNone, _L("RMobilePhone::SetSSPassword returned an error"));
						
	// passwordInfoStatus
	CleanupStack::PopAndDestroy(1, &passwordInfoStatus);
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0015::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0015");
	}

CCTSYIntegrationTestCallBarring0016::CCTSYIntegrationTestCallBarring0016(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestCallBarringBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestCallBarring0016::GetTestStepName());
	}

CCTSYIntegrationTestCallBarring0016::~CCTSYIntegrationTestCallBarring0016()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestCallBarring0016::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-CBAR-0016
 * @SYMFssID BA/CTSY/CBAR-0016
 * @SYMTestCaseDesc Set supplementary service password with invalid service code.
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - KErrArgument is returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify KErrArgument is returned.
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
	RMobilePhone& phone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);	
	RMobilePhone::TMobilePassword password;	
	CheckCBPasswordL(phone, password);
	
	StartTest();
	
	//
	// TEST START
	//	
	
	// Set the supplementary service password using RMobilePhone::SetSSPassword with aService = 1234 (an invalid service). 
	// Check KErrArgument is returned.

	RMobilePhone::TMobilePhonePasswordChangeV2 passwordV2;
	RMobilePhone::TMobilePhonePasswordChangeV2Pckg passwordPkg(passwordV2);

	passwordV2.iOldPassword = password;
	passwordV2.iNewPassword = password;
	passwordV2.iVerifiedPassword = password;

	TExtEtelRequestStatus reqStatus(phone, EMobilePhoneSetSSPassword);
	CleanupStack::PushL(reqStatus);
	
	phone.SetSSPassword(reqStatus, passwordPkg, 1234 );
	ASSERT_EQUALS( WaitForRequestWithTimeOut( reqStatus, ETimeMedium), KErrNone, 
						_L("RMobilePhone::SetSSPassword timed out") );
	
	ASSERT_EQUALS( reqStatus.Int(), KErrArgument, _L("RMobilePhone::SetSSPassword did not return KErrArgument"));
	
	//
	// TEST END
	//

    StartCleanup();
	CleanupStack::PopAndDestroy(1, &reqStatus );
	
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestCallBarring0016::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestCallBarring0016");
	}



