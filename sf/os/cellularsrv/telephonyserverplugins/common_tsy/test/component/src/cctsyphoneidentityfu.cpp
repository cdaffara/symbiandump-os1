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
// The TEFUnit test suite for PhoneIdentity in the Common TSY.
// 
//

/**
 @file 
*/

#include "cctsyphoneidentityfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <e32property.h>

CCTsyPhoneIdentityFU::CCTsyPhoneIdentityFU()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	// required to test BAFL on ARMV5
	_LIT_SECURITY_POLICY_PASS(KSecurityPolicyNone); 

	RProperty::Define(KUidSystemCategory, 0x10285B40,
	                  RProperty::EInt, KSecurityPolicyNone,  
	                  KSecurityPolicyNone);
	
	RProperty::Set(KUidSystemCategory, 0x10285B40, ETrue);
#endif
	}

CTestSuite* CCTsyPhoneIdentityFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001dL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001eL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001fL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001gL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001hL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001iL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0001jL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0004bL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0004cL);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetPhoneId0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetIdentityCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetIdentityServiceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetIdentityServiceStatus0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetIdentityServiceStatus0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestGetIdentityServiceStatus0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestSetIdentityServiceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestNotifyIdentityServiceStatus0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsyPhoneIdentityFU, TestNotifyIdentitySuppressionRejected0001L);

	END_SUITE;
	}


//
// Actual test cases
//
/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPhoneId
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001L()
	{
	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	// Test data
	_LIT(KSER,"0123456789");
	_LIT(KSER2,"9876543210");

#ifdef SYMBIAN_BAFL_SYSUTIL 
    _LIT(KManufacturerName, "SymbianTestManufacturerNameBAFL");
    _LIT(KModelName, "SymbianTestModelNameBAFL");
    _LIT(KRevisionID, "124589_Symbian.RevisionBAFL");
#else
    _LIT(KManufacturerName, "SymbianTestManufacturerName");
    _LIT(KModelName, "SymbianTestModelName");
    _LIT(KRevisionID, "124589_Symbian.Revision");
#endif
    
	// Do the failures first because once it has successfully retrievd the serial ID
	// it caches it and makes no calls to LTSY.

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId, KErrNotSupported);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNotSupported);

	iPhone.GetPhoneId(requestStatus, phoneId);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetPhoneId when result is not cached.
	// Send empty SID
 	//-------------------------------------------------------------------------
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId(KNullDesC8);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	// Note: For this API
	// LTSY does not return the whole TMobilePhoneIdentityV1 but just the
	// serial no. as a TBuf8<KSerialNumberLength>

	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();
	
	// Check result
	ASSERT_TRUE( 0 == KNullDesC().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
	
	//-------------------------------------------------------------------------
	// this time send not empty SID
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	retPhoneSerialId.Copy(KSER);
	data.Close();
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	// Check result
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );

	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetPhoneId again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------
	iPhone.GetPhoneId(requestStatus, phoneId);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetPhoneId
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	retPhoneSerialId.Copy(KSER2);
	data.Close();
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	// Check cache has been updated
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER2().Compare(phoneId.iSerialNumber) );

	CleanupStack::PopAndDestroy(2); // data, this
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test missing ManufacturerName in deviceattributes.ini
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini is missing data.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001bL()
    {
#ifdef SYMBIAN_BAFL_SYSUTIL
    //Testdata
    _LIT(KSER,"0123456789");
    _LIT(KManufacturerName, "SymbianTestManufacturerName");
    _LIT(KModelName, "TestModelNameTestModelNameTestModelNameTestModelNa");
    _LIT(KRevisionID, "012345678901234567890123456789.0123456789012345678");

    
    DeletePSProperties(); //Clear cached values read from deviceattributes.ini
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();
    
    RBuf8 data;
    CleanupClosePushL(data);
    iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
    TBuf8<KSerialNumberLength> retPhoneSerialId;
    retPhoneSerialId.Copy(KSER);
    TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
    retPhoneIdData.SerialiseL(data);
    iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

    TRequestStatus requestStatus;
    RMobilePhone::TMobilePhoneIdentityV1 phoneId;
    iPhone.GetPhoneId(requestStatus, phoneId);
    User::WaitForRequest(requestStatus);
        
    ASSERT_EQUALS(KErrNone, requestStatus.Int());
    ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
    ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
    ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
    ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
    
    
    DeletePSProperties(); //Clear cached values read from deviceattributes.ini
    CleanupStack::PopAndDestroy(2,this); //data, this
#else
    INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
    }

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test missing ModelName in deviceattributes.ini
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini is missing data.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001cL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
    _LIT(KManufacturerName, "TestManufacturerTestManufacturerTestManufacturerTe");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "012345678901234567890123456789.0123456789012345678");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
	
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001d
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test missing RevisionID in deviceattributes.ini
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini is missing data.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001dL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "TestManufacturerTestManufacturerTestManufacturerTe");
	_LIT(KModelName, "TestModelNameTestModelNameTestModelNameTestModelNa");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
	
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001e
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test missing all data in deviceattributes.ini
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini is missing data.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001eL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "SymbianTestManufacturerName");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );

	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001f
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc test reading from deviceattributes.ini encoded in an invalid format
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini not using a valid encoding.
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001fL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "SymbianTestManufacturerName");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );

	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001g
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test reading from deviceattributes.ini with corrupt data format
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini has corrupt data format
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001gL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "SymbianTestManufacturerName");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
    
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001h
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test reading from deviceattributes.ini with too long data strings
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini has too long data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001hL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrOverflow, requestStatus.Int());
	
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(this); 
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001i
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test reading from deviceattributes.ini with correct tags that are empty
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when deviceattributes.ini has empty data tags
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001iL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "SymbianTestManufacturerName");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
	
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this 
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0001j
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc test reading from deviceattributes.ini which does not exist.
@SYMTestPriority High
@SYMTestActions Calls GetPhoneId when there is no deviceattributes.ini file
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0001jL()
	{
#ifdef SYMBIAN_BAFL_SYSUTIL
	//Testdata
	_LIT(KSER,"0123456789");
	_LIT(KManufacturerName, "SymbianTestManufacturerName");
	_LIT(KModelName, "SymbianTestModelName");
	_LIT(KRevisionID, "124589_Symbian.Revision");
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KSER);
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data);;

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE( 0 == KSER().Compare(phoneId.iSerialNumber) );
	ASSERT_TRUE( 0 == KManufacturerName().Compare(phoneId.iManufacturer) );
	ASSERT_TRUE( 0 == KModelName().Compare(phoneId.iModel) );
	ASSERT_TRUE( 0 == KRevisionID().Compare(phoneId.iRevision) );
	
	DeletePSProperties(); //Clear cached values read from deviceattributes.ini
	CleanupStack::PopAndDestroy(2,this); //data, this
#else
	INFO_PRINTF1(_L("<font color=Orange>Test not valid when SYMBIAN_BAFL_SYSUTIL is not defined. Pass by default.</font>"));
#endif
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetPhoneId
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetPhoneId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: cancelling of RMobilePhone::GetPhoneId
 	//-------------------------------------------------------------------------

	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);

	// Queue response to be delayed to give time for cancel to come through
	// Return data is needed because CTSY does not pass the cancel to
	// LTSY
	TBuf8<KSerialNumberLength> retPhoneSerialId = _L8("0123456789");
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);
	iPhone.CancelAsyncRequest(EMobilePhoneGetPhoneId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetPhoneId
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetPhoneId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetPhoneId
 	//-------------------------------------------------------------------------

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// First client request
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	_LIT(KRetPhoneSerialId,"0123456789");
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KRetPhoneSerialId());
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);

	// Second client request
	TRequestStatus requestStatus2;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId2;
	phone2.GetPhoneId(requestStatus2, phoneId2);

	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_TRUE(0 == KRetPhoneSerialId().Compare(phoneId2.iSerialNumber));

	User::WaitForRequest(requestStatus);
	
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE(0 == KRetPhoneSerialId().Compare(phoneId.iSerialNumber));

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0004b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetPhoneId combined with cancelling a request
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetPhoneId, cancels the first request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0004bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetPhoneId
 	//-------------------------------------------------------------------------

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// First client request
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	_LIT(KRetPhoneSerialId,"0123456789");
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KRetPhoneSerialId());
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);

	// Second client request
	TRequestStatus requestStatus2;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId2;
	phone2.GetPhoneId(requestStatus2, phoneId2);
	
	//Cancel the first request
	iPhone.CancelAsyncRequest(EMobilePhoneGetPhoneId);

	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	ASSERT_TRUE(0 == KRetPhoneSerialId().Compare(phoneId2.iSerialNumber));

	//Check that first request is cancelled.
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}

/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0004c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetPhoneId combined with cancelling a request
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetPhoneId, cancels the second request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0004cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetPhoneId
 	//-------------------------------------------------------------------------

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	// First client request
	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);
	_LIT(KRetPhoneSerialId,"0123456789");
	TBuf8<KSerialNumberLength> retPhoneSerialId;
	retPhoneSerialId.Copy(KRetPhoneSerialId());
	TMockLtsyData1<TBuf8<KSerialNumberLength> > retPhoneIdData(retPhoneSerialId);
	retPhoneIdData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetPhoneId, KErrNone, data, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);

	// Second client request
	TRequestStatus requestStatus2;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId2;
	phone2.GetPhoneId(requestStatus2, phoneId2);
	
	//Cancel the second request
	phone2.CancelAsyncRequest(EMobilePhoneGetPhoneId);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_TRUE(0 == KRetPhoneSerialId().Compare(phoneId.iSerialNumber));

	//Check that second request is cancelled.
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrCancel, requestStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGPI-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetPhoneId with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetPhoneId and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetPhoneId0005L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetPhoneId
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetPhoneId);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneIdentityV1 phoneId;
	iPhone.GetPhoneId(requestStatus, phoneId);

	// never completes
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 300103);
	ASSERT_TRUE( EFalse );
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrTimedOut, requestStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGIC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIdentityCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIdentityCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetIdentityCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetIdentityCaps when result is not cached.
 	//-------------------------------------------------------------------------

	// CTSY returns always KIdentityCaps actually
	TUint32 caps;
	TInt ret = iPhone.GetIdentityCaps(caps);
	ASSERT_EQUALS(KErrNone, ret);
	TUint32 expCaps = KIdentityCaps;
	ASSERT_EQUALS(expCaps, caps);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-PID-MGISS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetIdentityServiceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceUnspecified);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data, KErrNotSupported);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(expService);
	RMobilePhone::TMobilePhoneIdServiceStatus status(RMobilePhone::EIdServiceUnknown);
	iPhone.GetIdentityServiceStatus(reqStatus, service, status);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data);
	
	RMobilePhone::TMobilePhoneIdServiceStatus completeStatus(RMobilePhone::EIdServiceActivePermanent);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdServiceStatus> completeLtsyData(completeStatus);
    data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetIdentityServiceStatus, KErrGeneral, data);

	iPhone.GetIdentityServiceStatus(reqStatus, service, status);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RMobilePhone::GetIdentityServiceStatus when result is not cached.
 	//-------------------------------------------------------------------------

	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data);
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetIdentityServiceStatus, KErrNone, data);

	iPhone.GetIdentityServiceStatus(reqStatus, service, status);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(completeStatus, status);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetIdentityServiceStatus again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobileInfoLocation location(RMobilePhone::EInfoLocationCache);
	iPhone.GetIdentityServiceStatus(reqStatus, service, status, location);
	User::WaitForRequest(reqStatus);
	// there is no the cache in CTSY, so it should return KErrNotFound
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetIdentityServiceStatus
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetIdentityServiceStatus, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(2, this); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGISS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetIdentityServiceStatus0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetIdentityServiceStatus
 	//-------------------------------------------------------------------------
 	
	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceUnspecified);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data);

	RMobilePhone::TMobilePhoneIdServiceStatus completeStatus(RMobilePhone::EIdServiceActivePermanent);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdServiceStatus> completeLtsyData(completeStatus);
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetIdentityServiceStatus, KErrNone, data, 10);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(expService);
	RMobilePhone::TMobilePhoneIdServiceStatus status(RMobilePhone::EIdServiceUnknown);
	iPhone.GetIdentityServiceStatus(reqStatus, service, status);
	
	iPhone.CancelAsyncRequest(EMobilePhoneGetIdentityServiceStatus);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2); // data, this
	
	}


/**
@SYMTestCaseID BA-CTSY-PID-MGISS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetIdentityServiceStatus0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
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

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetIdentityServiceStatus
 	//-------------------------------------------------------------------------

	// send 1st request
	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceUnspecified);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data);

	RMobilePhone::TMobilePhoneIdServiceStatus completeStatus(RMobilePhone::EIdServiceActivePermanent);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdServiceStatus> completeLtsyData(completeStatus);
	data.Close();
	completeLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetIdentityServiceStatus, KErrNone, data, 10);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(expService);
	RMobilePhone::TMobilePhoneIdServiceStatus status(RMobilePhone::EIdServiceUnknown);
	iPhone.GetIdentityServiceStatus(reqStatus, service, status);
	
	// send 2nd request
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneIdService service2(RMobilePhone::EIdServiceUnspecified);
	RMobilePhone::TMobilePhoneIdServiceStatus status2(RMobilePhone::EIdServiceUnknown);
	phone2.GetIdentityServiceStatus(reqStatus2, service2, status2);
	
	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // phone2, telServer2, data, this

	}


/**
@SYMTestCaseID BA-CTSY-PID-MGISS-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIdentityServiceStatus with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIdentityServiceStatus and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestGetIdentityServiceStatus0005L()
	{


	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetIdentityServiceStatus
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneIdService expService(RMobilePhone::EIdServiceUnspecified);
	TMockLtsyData1<RMobilePhone::TMobilePhoneIdService> expLtsyData(expService);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(EMobilePhoneGetIdentityServiceStatus, data);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(expService);
	RMobilePhone::TMobilePhoneIdServiceStatus status(RMobilePhone::EIdServiceUnknown);
	iPhone.GetIdentityServiceStatus(reqStatus, service, status);

	// check results
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this

	}


/**
@SYMTestCaseID BA-CTSY-PID-MSISS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::SetIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestSetIdentityServiceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(RMobilePhone::EIdServiceUnspecified);
	RMobilePhone::TMobilePhoneIdServiceSetting setting(RMobilePhone::EIdServiceNetworkDefault);
	iPhone.SetIdentityServiceStatus(reqStatus, service, setting);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PID-MNISS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIdentityServiceStatus
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIdentityServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestNotifyIdentityServiceStatus0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneIdService service(RMobilePhone::EIdServiceUnspecified);
	RMobilePhone::TMobilePhoneIdServiceStatus setting(RMobilePhone::EIdServiceActivePermanent);
	iPhone.NotifyIdentityServiceStatus(reqStatus, service, setting);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-PID-MNISR-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIdentitySuppressionRejected
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIdentitySuppressionRejected
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsyPhoneIdentityFU::TestNotifyIdentitySuppressionRejected0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	iPhone.NotifyIdentitySuppressionRejected(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}

#ifdef SYMBIAN_BAFL_SYSUTIL
// Helper function for clearing out cached values, only valid when 
// SYMBIAN_BAFL_SYSUTIL is defined.
TInt CCTsyPhoneIdentityFU::DeletePSProperties()
	{
	_LIT( KExeName, "Z:\\sys\\bin\\sysutilsetup.exe" );
	TUid myId = TUid::Uid(0x10285B3B);
	const TUidType KExeUid( KNullUid, KNullUid, myId );

	RProcess setupProcess;
	TInt err = setupProcess.Create( KExeName, _L("-delete"), KExeUid );
	if( KErrNone != err )
		{
		return err;
		}

	TRequestStatus status;
	setupProcess.Rendezvous( status );
	if( KRequestPending != status.Int() )
		{
		setupProcess.Kill( 0 );		// Abort startup
		}
	else
		{
		setupProcess.Resume();		// Logon OK - start the server
		}
	
	User::WaitForRequest( status );	// Wait for start or death

	err = setupProcess.ExitType() == EExitPanic ? KErrGeneral : status.Int();
	setupProcess.Close();
	return err;
	}
#endif
