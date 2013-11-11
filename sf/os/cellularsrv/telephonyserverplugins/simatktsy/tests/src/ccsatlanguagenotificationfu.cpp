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
//



/**
 @file The TEFUnit test suite for LanguageNotification in the SAT.
*/

#include "ccsatlanguagenotificationfu.h"
#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <satcs.h>                 
#include <ctsy/ltsy/sat_defs.h>
#include <cstktsy/ttlv.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h> 
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>


const TUint8 KCmdId = 1;

// command specifier for language notification
const TUint8 KSpecific = 0x01;
const TUint8 KNonSpecific = 0x00;

_LIT8(KSpanishLang, "se");


CTestSuite* CCSatLanguageNotificationFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatLanguageNotificationFU, TestNotifyLanguageNotificationPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatLanguageNotificationFU, TestNotifyLanguageNotificationPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatLanguageNotificationFU, TestNotifyLanguageNotificationPCmd0004L);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-LN-SNLNPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyLanguageNotificationPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLanguageNotificationPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLanguageNotificationFU::TestNotifyLanguageNotificationPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	

	TTlv tlv;	
	
	TRequestStatus notifyStatus;
	RSat::TLanguageNotificationV2 lang;
	RSat::TLanguageNotificationV2Pckg langPck(lang);
	
	RBuf8 data;
	CleanupClosePushL(data);
	
 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyLanguageNotificationPCmd in case of NonSpecific notification 
 	//-------------------------------------------------------------------------
	
	PrepareTlv( tlv, KNonSpecific);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	PrepareDispatchL();

	iSat.NotifyLanguageNotificationPCmd(notifyStatus, langPck);

	CompleteDispatchL( *tlvDscPtr);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS(RSat::ENonSpecificLangNotification, lang.iNotificationType);
	ASSERT_EQUALS(KCmdId, lang.PCmdNumber());
	
	//terminal response	
	RSat::TLanguageNotificationRspV2 langRsp;
	RSat::TLanguageNotificationRspV2Pckg langRspPckg(langRsp);
	langRsp.SetPCmdNumber(KCmdId);
	langRsp.iGeneralResult = RSat::KSuccess;
	langRsp.iInfoType = RSat::KNoAdditionalInfo;
	langRsp.iAdditionalInfo = KNullDesC;
	
	GenerateTerminalResponseL(KCmdId,	KLanguageNotification,	KNonSpecific,
			RSat::ELanguageNotification, langRspPckg,	langRsp.iAdditionalInfo, 
			langRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyLanguageNotificationPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	PrepareTlv( tlv, KSpecific, KSpanishLang);	
	tlv.End();
	
	TRequestStatus mocklStatus;
	iMockLTSY.NotifyTerminated(mocklStatus);	    
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data);
	
	langRsp.iAdditionalInfo.Zero();
	langRsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);;

	PrepareTerminalResponseMockDataL( KCmdId, KLanguageNotification, KSpecific,
			langRsp.iAdditionalInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);

	User::WaitForRequest(mocklStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, mocklStatus.Int());
		
 	//-------------------------------------------------------------------------
	// TEST C2: Successful completion request of
	// RSat::NotifyLanguageNotificationPCmd in case of Specific notification 
 	//-------------------------------------------------------------------------
		
	PrepareDispatchL();

	iSat.NotifyLanguageNotificationPCmd(notifyStatus, langPck);

	CompleteDispatchL( *tlvDscPtr);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS(RSat::ESpecificLangNotification, lang.iNotificationType);
	ASSERT_EQUALS(KCmdId, lang.PCmdNumber());
	TUint16 tmpLang;
	TSatUtility::CopyTwo8toOne16LE( KSpanishLang, tmpLang, 0 );
	ASSERT_EQUALS(tmpLang, lang.iLanguage);
	
	//terminal response	
	langRsp.iGeneralResult = RSat::KSuccess;
	langRsp.iInfoType = RSat::KNoAdditionalInfo;
	langRsp.iAdditionalInfo = KNullDesC;
	
	GenerateTerminalResponseL(KCmdId,	KLanguageNotification,	KSpecific,
			RSat::ELanguageNotification, langRspPckg,	langRsp.iAdditionalInfo, 
			langRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();
		
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	PrepareDispatchL();

	iSat.NotifyLanguageNotificationPCmd(notifyStatus, langPck);

	CompleteDispatchL( *tlvDscPtr, KErrUnknown );
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrUnknown, notifyStatus.Int());	

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CSAT-LN-SNLNPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyLanguageNotificationPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyLanguageNotificationPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLanguageNotificationFU::TestNotifyLanguageNotificationPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyLanguageNotificationPCmd
 	//-------------------------------------------------------------------------
	TRequestStatus notifyStatus;
	RSat::TLanguageNotificationV2 lang;
	RSat::TLanguageNotificationV2Pckg langPck(lang);

	PrepareDispatchL();

	iSat.NotifyLanguageNotificationPCmd(notifyStatus, langPck);
	
	iSat.CancelAsyncRequest(ESatNotifyLanguageNotificationPCmd);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS( KErrCancel, notifyStatus.Int());
	
	CleanupStack::PopAndDestroy(1, this); 
	}


/**
@SYMTestCaseID BA-CSAT-LN-SNLNPC-0004
@SYMComponent  telephony_csat
@SYMPREQ 1780
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyLanguageNotificationPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyLanguageNotificationPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLanguageNotificationFU::TestNotifyLanguageNotificationPCmd0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RSat sat2;	
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifyLanguageNotificationPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	TTlv tlv;	
	
	PrepareTlv( tlv, KNonSpecific);
	TDesC8* tlvDscPtr = &const_cast<TDesC8&>(tlv.End());
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	PrepareDispatchL();

	TRequestStatus notifyStatus;
	TRequestStatus notifyStatus2;
	RSat::TLanguageNotificationV2 lang;
	RSat::TLanguageNotificationV2 lang2;
	RSat::TLanguageNotificationV2Pckg langPck(lang);
	RSat::TLanguageNotificationV2Pckg langPck2(lang2);
	
	iSat.NotifyLanguageNotificationPCmd(notifyStatus, langPck);
	sat2.NotifyLanguageNotificationPCmd(notifyStatus2, langPck2);

	CompleteDispatchL( *tlvDscPtr);
	
	User::WaitForRequest(notifyStatus);
	ASSERT_EQUALS(KErrNone, notifyStatus.Int());	
	ASSERT_EQUALS(RSat::ENonSpecificLangNotification, lang.iNotificationType);
	ASSERT_EQUALS(KCmdId, lang.PCmdNumber());

	User::WaitForRequest(notifyStatus2);
	ASSERT_EQUALS(KErrNone, notifyStatus2.Int());	
	ASSERT_EQUALS(RSat::ENonSpecificLangNotification, lang2.iNotificationType);
	ASSERT_EQUALS(KCmdId, lang2.PCmdNumber());	

	CleanupStack::PopAndDestroy(4, this); //sat2 phone2 telServer2 this
	}


/**
* prepare tlv buffer for refresh proactive command 
* @param aTlv - tlv buffer to prepare
* @param aCmdQualifier - type of refresh command
* @param aLang - two-byte language code
*/   	
void CCSatLanguageNotificationFU::PrepareTlv(TTlv& aTlv, TUint8 aCmdQualifier, const TDesC8& aLang) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(KCmdId);//ETLV_CommandNumber
	aTlv.AddByte(KLanguageNotification); //ETLV_TypeOfCommand
	aTlv.AddByte(aCmdQualifier); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
	
	if(0 < aLang.Length())
		{
		aTlv.AddTag(KTlvLanguageTag); 
		aTlv.AddData(aLang);
		}
	}

/**
* prepare mock complete for processing of proactive command
* @param aTlvDsc - tlv buffer to send
* @param aRes - result for request completion
*/   	
void CCSatLanguageNotificationFU::CompleteDispatchL(const TDesC8& aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC8* tlvDscPtr = const_cast<TDesC8*>(&aTlvDsc);
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data);

	CleanupStack::PopAndDestroy(1); 
	}

/**
* prepare mock for dispatch of sat notification  
*/   	
void CCSatLanguageNotificationFU::PrepareDispatchL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KLanguageNotification;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
 	
	CleanupStack::PopAndDestroy(1); 
	}

