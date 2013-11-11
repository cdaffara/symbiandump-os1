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
 @file The TEFUnit test suite for SimControl in the Common TSY.
*/

#include "cctsysimfu.h"
#include <etel.h>
#include <etelmm.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <test/tmockltsydata.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/ltsy/mltsydispatchsiminterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <etelmmerr.h>
#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <ctsy/ltsy/mltsydispatchsecurityinterface.h>
#include "mockltsyindicatorids.h"

#include <etelmmerr.h>
#include <e32math.h> 

CTestSuite* CCTsySimFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	//add use-case tests
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUseCase0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUseCase0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUseCase0003L);

	//add other unit tests
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0006L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0007L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0008L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0009L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0010L);

	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0011L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0012L);
	
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0013L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0014L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0015L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0016L);
	ADD_TEST_STEP_ISO_CPP(CCTsySimFU, TestUnit0017L);
	
	END_SUITE;
	}


//
// Use-case tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UC0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for get, set and notify for MWIS
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIccMessageWaitingIndicatorsChange, RMobilePhone::GetIccMessageWaitingIndicators, RMobilePhone::SetIccMessageWaitingIndicators
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySimFU::TestUseCase0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	/*
	 * NotifyIccMessageWaitingIndicatorsChange
	 */

	TRequestStatus requestStatus;
	TRequestStatus mockLtsyStatus;

	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiRet;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mwiRetPckg(mwiRet);
	iPhone.NotifyIccMessageWaitingIndicatorsChange(requestStatus, mwiRetPckg);

	// Prepare the NotifyIccMessageWaitingIndicatorsChange data for CompleteL
	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiNotifComplete;
	mwiNotifComplete.iAuxVoiceMsgs = 1;
	mwiNotifComplete.iDataMsgs = 2;
	mwiNotifComplete.iDisplayStatus = 3;
	mwiNotifComplete.iEmailMsgs = 4;
	mwiNotifComplete.iFaxMsgs = 5;
	mwiNotifComplete.iOtherMsgs = 6;
	mwiNotifComplete.iVoiceMsgs = 7;

	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1> mockMwisData1(mwiNotifComplete);

	// Send the CompleteL to MockLtsy
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	data.Close();
	mockMwisData1.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimNotifyIccMessageWaitingIndicatorsChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // Check completion of api
    User::WaitForRequest(requestStatus);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatus.Int());

    // Check returned data
    ASSERT_EQUALS(mwiRet.iAuxVoiceMsgs, mwiNotifComplete.iAuxVoiceMsgs);
    ASSERT_EQUALS(mwiRet.iDataMsgs, mwiNotifComplete.iDataMsgs);
    ASSERT_EQUALS(mwiRet.iDisplayStatus, mwiNotifComplete.iDisplayStatus);
    ASSERT_EQUALS(mwiRet.iEmailMsgs, mwiNotifComplete.iEmailMsgs);
    ASSERT_EQUALS(mwiRet.iFaxMsgs, mwiNotifComplete.iFaxMsgs);
    ASSERT_EQUALS(mwiRet.iOtherMsgs, mwiNotifComplete.iOtherMsgs);
    ASSERT_EQUALS(mwiRet.iVoiceMsgs, mwiNotifComplete.iVoiceMsgs);

	/*
	 * SetIccMessageWaitingIndicators.
	 * Also test NotifyIccMessageWaitingIndicatorsChange triggered
	 * from a client set when the SetIccMessageWaitingIndicators data values are different to previous values.
	 */

    // Post notification
    TRequestStatus requestStatusNotif;
    iPhone.NotifyIccMessageWaitingIndicatorsChange(requestStatusNotif, mwiRetPckg);

    // Prepare ExpectL data for the Set
	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiSet;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mwiSetRetPckg(mwiSet);
	// Use values for setting the data that is different to what was used before, so that
	// Ctsy detects a change and hence issues a notification.
	// (Not each of the fields have necessarily to be different).
	mwiSet.iAuxVoiceMsgs = ++mwiNotifComplete.iAuxVoiceMsgs;
	mwiSet.iDataMsgs = ++mwiNotifComplete.iDataMsgs;
	mwiSet.iDisplayStatus = ++mwiNotifComplete.iDisplayStatus;
	mwiSet.iEmailMsgs = ++mwiNotifComplete.iEmailMsgs;
	mwiSet.iFaxMsgs = ++mwiNotifComplete.iFaxMsgs;
	mwiSet.iOtherMsgs = ++mwiNotifComplete.iOtherMsgs;
	mwiSet.iVoiceMsgs = ++mwiNotifComplete.iVoiceMsgs;

	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>setData(mwiSet);
	data.Close();
	setData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSimSetIccMessageWaitingIndicators::KLtsyDispatchSimSetIccMessageWaitingIndicatorsApiId, KErrNone);

	// Call api
	TRequestStatus requestStatusSet;
	iPhone.SetIccMessageWaitingIndicators(requestStatusSet, mwiSetRetPckg);
	User::WaitForRequest(requestStatusSet);
	ASSERT_EQUALS(KErrNone, requestStatusSet.Int());
	AssertMockLtsyStatusL();

	// Wait for notificaion
    User::WaitForRequest(requestStatusNotif);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatusNotif.Int());

	/*
	 * GetIccMessageWaitingIndicators
	 */

	TRequestStatus requestStatusGet;

	// Send the ExpectL
	iMockLTSY.ExpectL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId);

	// Prepare data for the mockLtsy's CompleteL
	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiComp;
	mwiComp.iDisplayStatus = 0xff;
	mwiComp.iVoiceMsgs = 0x01;
	mwiComp.iAuxVoiceMsgs = 0x02;
	mwiComp.iDataMsgs = 0x03;
	mwiComp.iFaxMsgs = 0x04;
	mwiComp.iEmailMsgs = 0x05;
	mwiComp.iOtherMsgs = 0x06;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1> mwisdCompData(mwiComp);
	data.Close();
	mwisdCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetIccMessageWaitingIndicators::KLtsyDispatchSimGetIccMessageWaitingIndicatorsApiId, KErrNone, data);

	// Now call the ipc
	RMobilePhone::TMobilePhoneMessageWaitingV1 mwiGet;
	RMobilePhone::TMobilePhoneMessageWaitingV1Pckg mwiGetPckg(mwiGet);
	iPhone.GetIccMessageWaitingIndicators(requestStatusGet, mwiGetPckg);

	User::WaitForRequest(requestStatusGet);
	ASSERT_EQUALS(KErrNone, requestStatusGet.Int());
	AssertMockLtsyStatusL();

	// Check returned data
    ASSERT_EQUALS(mwiGet.iAuxVoiceMsgs, mwiComp.iAuxVoiceMsgs);
    ASSERT_EQUALS(mwiGet.iDataMsgs, mwiComp.iDataMsgs);
    ASSERT_EQUALS(mwiGet.iDisplayStatus, mwiComp.iDisplayStatus);
    ASSERT_EQUALS(mwiGet.iEmailMsgs, mwiComp.iEmailMsgs);
    ASSERT_EQUALS(mwiGet.iFaxMsgs, mwiComp.iFaxMsgs);
    ASSERT_EQUALS(mwiGet.iOtherMsgs, mwiComp.iOtherMsgs);
    ASSERT_EQUALS(mwiGet.iVoiceMsgs, mwiComp.iVoiceMsgs);

    data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UC0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for APN Control List operations
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetAPNControlListServiceStatus , RMobilePhone::NotifyAPNControlListServiceStatusChange, RMobilePhone::SetAPNControlListServiceStatus
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySimFU::TestUseCase0002L()
	{
  OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;

	/*
	 * GetAPNControlListServiceStatus
	 */

	TRequestStatus requestStatusGet;

	// Send the ExpectL
	iMockLTSY.ExpectL(MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId);

	// Prepare data for the mockLtsy's CompleteL
	RMobilePhone::TAPNControlListServiceStatus statusComp = RMobilePhone::EAPNControlListServiceDisabled;

	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> statusCompData(statusComp);
	data.Close();
	statusCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetApnControlListServiceStatus::KLtsyDispatchSimGetApnControlListServiceStatusApiId, KErrNone, data);

	// Now call the ipc
	RMobilePhone::TAPNControlListServiceStatus statusGet;
	iPhone.GetAPNControlListServiceStatus(requestStatusGet, statusGet);

	User::WaitForRequest(requestStatusGet);
	ASSERT_EQUALS(KErrNone, requestStatusGet.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(statusGet, statusComp);

	/*
	 * Set the APN Control List status to Enabled using SetAPNControlListServiceStatus and check for update using
	 * NotifyAPNControlListServiceStatusChange.
	 */

	// Post notification
	TRequestStatus requestStatusNotif;
	RMobilePhone::TAPNControlListServiceStatus statusNotif;
	iPhone.NotifyAPNControlListServiceStatusChange(requestStatusNotif, statusNotif);

	// Prepare ExpectL data for the Set
	RMobilePhone::TAPNControlListServiceStatus statusSet = RMobilePhone::EAPNControlListServiceEnabled;

	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus>setData(statusSet);
	data.Close();
	setData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSimSetApnControlListServiceStatus::KLtsyDispatchSimSetApnControlListServiceStatusApiId, KErrNone);

	// Call api
	TRequestStatus requestStatusSet;
	iPhone.SetAPNControlListServiceStatus(requestStatusSet, statusSet);
	User::WaitForRequest(requestStatusSet);
	ASSERT_EQUALS(KErrNone, requestStatusSet.Int());
	AssertMockLtsyStatusL();

	// Prepare CompleteL data for the notification (will be same as that used in the Set).
	RMobilePhone::TAPNControlListServiceStatus statusNotifComp = statusSet;
	TMockLtsyData1<RMobilePhone::TAPNControlListServiceStatus> statusNotifCompData1(statusNotifComp);
	data.Close();
	statusNotifCompData1.SerialiseL(data);
	// Send CompleteL for the notification
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimNotifyApnControlListServiceStatusChangeIndId, KErrNone, data);
	User::WaitForRequest(mockLtsyStatus);
	AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    // Check completion of notification
    User::WaitForRequest(requestStatusNotif);
    AssertMockLtsyStatusL();
    ASSERT_EQUALS(KErrNone, requestStatusNotif.Int());

    // Check data returned in the notification same as date used in the Set.
    ASSERT_EQUALS(statusNotif, statusSet);

    data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UC0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for APN list operations
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::AppendAPNName, RMobilePhone::DeleteAPNName, RMobilePhone::GetAPNname, RMobilePhone::EnumerateAPNEntries, RMobilePhone::NotifyAPNListChanged
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCTsySimFU::TestUseCase0003L()
	{
  OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	/*
	 * EnumerateAPNEntries
	 */

	TRequestStatus requestStatusEnmrt;

	// Prepare ExpectL and CompleteL data
	iMockLTSY.ExpectL(MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId);

	const TUint32 KNumAPNListEntries = 5;
	TUint32 enmrtComp = KNumAPNListEntries;
	TMockLtsyData1<TUint32> enmrtCompData(enmrtComp);
	data.Close();
	enmrtCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimEnumerateApnEntries::KLtsyDispatchSimEnumerateApnEntriesApiId, KErrNone, data);

	// Call ipc
	TUint32 enmrtRet;
	iPhone.EnumerateAPNEntries(requestStatusEnmrt, enmrtRet);
	User::WaitForRequest(requestStatusEnmrt);
	ASSERT_EQUALS(KErrNone, requestStatusEnmrt.Int());
	AssertMockLtsyStatusL();

	// Check returned data
	ASSERT_EQUALS(enmrtComp, enmrtRet);

	data.Close();
	AssertMockLtsyStatusL();

	/*
	 * GetAPNname
	 */

	TRequestStatus requestStatusGet;

	// Prepare ExpectL and CompleteL data
	TUint32 index = 69;
	TMockLtsyData1<TUint32> indexData(index);
	indexData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId, data, KErrNone);

	RMobilePhone::TAPNEntryV3 apnEntryComp;
	const TBuf8<RMobilePhone::KMaxApnName> KApnName(_L8("Mobile Network access point 1"));
	apnEntryComp.iApn = KApnName;
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> apnEntryData(apnEntryComp);
	data.Close();
	apnEntryData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetApnName::KLtsyDispatchSimGetApnNameApiId, KErrNone, data);

	// Call ipc
	RMobilePhone::TAPNEntryV3 apnEntryGet;
	RMobilePhone::TAPNEntryV3Pckg apnEntryGetPckg(apnEntryGet);
	iPhone.GetAPNname(requestStatusGet, index, apnEntryGetPckg);
	User::WaitForRequest(requestStatusGet);
	ASSERT_EQUALS(KErrNone, requestStatusGet.Int());
	AssertMockLtsyStatusL();

	// Check returned data
	ASSERT_EQUALS(apnEntryGet.iApn, apnEntryComp.iApn);

    data.Close();
	AssertMockLtsyStatusL();

	/*
	 * DeleteAPNName and notify of change to APN list using NotifyAPNListChanged
	 */

	// Post notification
	TRequestStatus requestStatusNotif;
	iPhone.NotifyAPNListChanged(requestStatusNotif);

	// Prepare ExpectL and CompleteL data for the delete
	TUint32 indexDel = 49;
	TMockLtsyData1<TUint32> indexDelData(indexDel);
	indexDelData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId, data, KErrNone);

	iMockLTSY.CompleteL(MLtsyDispatchSimDeleteApnName::KLtsyDispatchSimDeleteApnNameApiId, KErrNone);

	// Now call the ipc
	TRequestStatus requestStatusDel;
	iPhone.DeleteAPNName(requestStatusDel, indexDel);
	User::WaitForRequest(requestStatusDel);
	ASSERT_EQUALS(KErrNone, requestStatusDel.Int());
	AssertMockLtsyStatusL();

	// Send CompleteL for noticn
	iMockLTSY.CompleteL(KMockLtsyDispatchSimNotifyApnListChangeIndId , KErrNone);
	User::WaitForRequest(requestStatusNotif);
	ASSERT_EQUALS(KErrNone, requestStatusNotif.Int());

    data.Close();
	AssertMockLtsyStatusL();

	/*
	 * AppendAPNName
	 */

	// Prepare ExpectL and CompleteL data for the append
	const TBuf8<RMobilePhone::KMaxApnName> KApnNameAppend(_L8("New Mobile Network access point 2"));
	RMobilePhone::TAPNEntryV3 entryExp;
	entryExp.iApn = KApnNameAppend;
	TMockLtsyData1<RMobilePhone::TAPNEntryV3> entryExpData(entryExp);
	data.Close();
	entryExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSimAppendApnName::KLtsyDispatchSimAppendApnNameApiId, KErrNone);

	// Now call the ipc
	TRequestStatus requestStatusApp;
	RMobilePhone::TAPNEntryV3Pckg entryAppPckg(entryExp);
	iPhone.AppendAPNName(requestStatusApp, entryAppPckg);
	User::WaitForRequest(requestStatusApp);
	ASSERT_EQUALS(KErrNone, requestStatusApp.Int());
	AssertMockLtsyStatusL();

    data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}



//
// Other unit tests
//

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0001
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	RMobilePhone::TMobilePhoneServiceTableV1 simServiceTable;
	simServiceTable.iServices1To8  = 0xFF;
	simServiceTable.iServices9To16 = 0xFF;
	simServiceTable.iServices17To24= 0xFF;
	simServiceTable.iServices25To32= 0xFF;
	simServiceTable.iServices33To40= 0xFF;
	simServiceTable.iServices41To48= 0xFF;
	simServiceTable.iServices49To56= 0xFF;
	
	OpenPhoneL(simServiceTable);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	RMobilePhone::TMobilePhoneServiceTableV1 tbData;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg pkg(tbData);

	//test getting service table when TSY not ready (ICC type is unknown)
	
	TRequestStatus reqStat;
	iPhone.GetServiceTable(reqStat, RMobilePhone::EUSIMServiceTable, pkg);
	User::WaitForRequest(reqStat);
	ASSERT_EQUALS(reqStat.Int(), KErrMMEtelWrongMode);

	//now get the ICC type.  We do this by invoking NotifySecurityEvent - why I don't know!
	iMockLTSY.ExpectL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId);
	MLtsyDispatchSimGetActiveIccApplicationType::TIccType type =  MLtsyDispatchSimGetActiveIccApplicationType::EIccTypeSim3G;
	TMockLtsyData1<MLtsyDispatchSimGetActiveIccApplicationType::TIccType> retIccType(type);
	retIccType.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetActiveIccApplicationType::KLtsyDispatchSimGetActiveIccApplicationTypeApiId,KErrNone,data, 0);

	iMockLTSY.ExpectL(MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId);

	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin1;
	TMockLtsyData1<RMobilePhone::TMobilePhoneSecurityCode> secCodeData(secCode);
	data.Close();
	secCodeData.SerialiseL(data);	
	iMockLTSY.CompleteL(MLtsyDispatchSecurityGetCurrentActivePin::KLtsyDispatchSecurityGetCurrentActivePinApiId, KErrNone, data);
							
	TRequestStatus reqStat2;
	RMobilePhone::TMobilePhoneSecurityEvent event(RMobilePhone::EUniversalPukVerified);
	iPhone.NotifySecurityEvent(reqStat2, event);

	User::WaitForRequest(mockLtsyStatus);
	iPhone.CancelAsyncRequest(EMobilePhoneNotifySecurityEvent);

	User::After(5000000);  // A delay is required by MockSY when being tested on hardware.

	
	//test getting cached service table
	
	iPhone.GetServiceTable(reqStat, RMobilePhone::ESIMServiceTable, pkg);
	User::WaitForRequest(reqStat);
	ASSERT_EQUALS(reqStat.Int(), KErrNone);
	ASSERT_EQUALS(tbData.iServices1To8,simServiceTable.iServices1To8);
	ASSERT_EQUALS(tbData.iServices9To16,simServiceTable.iServices9To16);
	ASSERT_EQUALS(tbData.iServices17To24,simServiceTable.iServices17To24);
	ASSERT_EQUALS(tbData.iServices25To32,simServiceTable.iServices25To32);
	ASSERT_EQUALS(tbData.iServices33To40,simServiceTable.iServices33To40);
	ASSERT_EQUALS(tbData.iServices41To48,simServiceTable.iServices41To48);
	ASSERT_EQUALS(tbData.iServices49To56,simServiceTable.iServices49To56);

	//finally test successful retrieval of service table

	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::EUSIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	data.Close();
	serviceTableData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);

	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;

	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	data.Close();
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,KErrNone,data,0);

	iPhone.GetServiceTable(reqStat, RMobilePhone::EUSIMServiceTable, pkg);
	User::WaitForRequest(reqStat);
	ASSERT_EQUALS(reqStat.Int(), KErrNone);
	ASSERT_EQUALS(tbData.iServices1To8, serviceTableResult.iServices1To8);
	ASSERT_EQUALS(tbData.iServices9To16, serviceTableResult.iServices9To16);
	ASSERT_EQUALS(tbData.iServices17To24, serviceTableResult.iServices17To24);
	ASSERT_EQUALS(tbData.iServices25To32, serviceTableResult.iServices25To32);
	ASSERT_EQUALS(tbData.iServices33To40, serviceTableResult.iServices33To40);
	ASSERT_EQUALS(tbData.iServices41To48, serviceTableResult.iServices41To48);
	ASSERT_EQUALS(tbData.iServices49To56, serviceTableResult.iServices49To56);


#ifdef EMULATOR_ONLY
	//test cancel
	
	data.Close();
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);
	//no complete as testing cancel

	iPhone.GetServiceTable(reqStat, RMobilePhone::EUSIMServiceTable, pkg);
	iPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);
	User::WaitForRequest(reqStat);
	ASSERT_EQUALS(KErrCancel, reqStat.Int());
#endif // EMULATOR_ONLY

	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0002
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for RMobilePhone::ChangeSecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::ChangeSecurityCode
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;

	_LIT(KOldPwd,"oldPswd");
	_LIT(KNewPwd,"newPswd");
	RMobilePhone::TMobilePhonePasswordChangeV1 pwdChange;
	pwdChange.iNewPassword = KNewPwd;
	pwdChange.iOldPassword = KOldPwd;

	// Choose Phone Device Lock
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePhonePassword;

	// Prepare Mock Ltsy data for the ExpectL
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode,
				   RMobilePhone::TMobilePhonePasswordChangeV1> pwdData(secCode, pwdChange);
	pwdData.SerialiseL(data);

	iMockLTSY.ExpectL(MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId, data);

	// The Mock Ltsy CompleteL doesn't require data
	iMockLTSY.CompleteL(MLtsyDispatchSimChangeSecurityCode::KLtsyDispatchSimChangeSecurityCodeApiId, KErrNone);

	// Call the API
	iPhone.ChangeSecurityCode(reqStatus, secCode, pwdChange);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	data.Close();

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0003
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for RMobilePhone::GetSubscriberId
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSubscriberId
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0003L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;
	_LIT8  (KIMSI8, "012012012012999");
	_LIT16(KIMSI16, "012012012012999");

	// Send the ExpectL
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);

	// Prepare data for the mockLtsy's CompleteL
	// Note the CTSY expects the data in 8-bit format.
	TBuf8<RMobilePhone::KIMSISize> idComp(KIMSI8);
	TMockLtsyData1<TBuf8<RMobilePhone::KIMSISize> > idCompData(idComp);

	data.Close();
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);

	// Now call the ipc
	RMobilePhone::TMobilePhoneSubscriberId idGet;
	iPhone.GetSubscriberId(reqStatus, idGet);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_TRUE(idGet == KIMSI16); // ASSERT_EQUALS will not work here

	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0004
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for RMobilePhone::GetCustomerServiceProfile
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCustomerServiceProfile
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus;

	// Send the ExpectL
	iMockLTSY.ExpectL(MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId);

	// Prepare data for the mockLtsy's CompleteL
	RMobilePhone::TMobilePhoneCspFileV1 cspComp;
	cspComp.iCallOfferingServices = 0x01;
	cspComp.iCallRestrictionServices = 0x02;
	cspComp.iOtherSuppServices = 0x03;
	cspComp.iCallCompletionServices = 0x04;
	cspComp.iTeleservices = 0x05;
	cspComp.iCphsTeleservices = 0x06;
	cspComp.iCphsFeatures = 0x07;
	cspComp.iNumberIdentServices = 0x08;
	cspComp.iPhase2PlusServices = 0x09;
	cspComp.iValueAddedServices = 0x0A;
	TMockLtsyData1<RMobilePhone::TMobilePhoneCspFileV1> cspCompData(cspComp);
	data.Close();
	cspCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetCustomerServiceProfile::KLtsyDispatchSimGetCustomerServiceProfileApiId, KErrNone, data);

	// Now call the ipc
	RMobilePhone::TMobilePhoneCspFileV1 cspGet;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspGetPckg(cspGet);
	iPhone.GetCustomerServiceProfile(reqStatus, cspGetPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(cspComp.iCallOfferingServices, cspGet.iCallOfferingServices);
	ASSERT_EQUALS(cspComp.iCallRestrictionServices, cspGet.iCallRestrictionServices);
	ASSERT_EQUALS(cspComp.iOtherSuppServices, cspGet.iOtherSuppServices);
	ASSERT_EQUALS(cspComp.iCallCompletionServices, cspGet.iCallCompletionServices);
	ASSERT_EQUALS(cspComp.iTeleservices, cspGet.iTeleservices);
	ASSERT_EQUALS(cspComp.iCphsTeleservices, cspGet.iCphsTeleservices);
	ASSERT_EQUALS(cspComp.iCphsFeatures, cspGet.iCphsFeatures);
	ASSERT_EQUALS(cspComp.iNumberIdentServices, cspGet.iNumberIdentServices);
	ASSERT_EQUALS(cspComp.iPhase2PlusServices, cspGet.iPhase2PlusServices);
	ASSERT_EQUALS(cspComp.iValueAddedServices, cspGet.iValueAddedServices);

	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0005
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for RMobilePhone::SetFdnSetting, RMobilePhone::VerifySecurityCode
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::SetFdnSetting
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0005L()
	{
	/*
	 * This test uses VerifySecurityCode with SetFdnSetting to get the SetFdnSetting sent to the LTSY
	 */

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus reqStatus, reqStatusNotif, reqStatusVerify;

	// Prepare data for mockLtsy for the VerifySecurityCode.
	// This mimics the scenario that Pin2 code is required to make SetFdnSetting complete successfully.
	// Therefore we use VerifySecurityCode to supply the Pin2 code.
	RMobilePhone::TMobilePhoneSecurityCode secCode = RMobilePhone::ESecurityCodePin2;

	_LIT(KPwd,"pwd");
	_LIT(KUnblockPwd, "unblockPwd");
	RMobilePhone::TMobilePassword pwd;
	RMobilePhone::TMobilePassword unblockPwd;
	pwd.Copy(KPwd);
	unblockPwd.Copy(KUnblockPwd);

	RMobilePhone::TCodeAndUnblockCode twoCodes;
	twoCodes.iCode.Copy(KPwd);
	twoCodes.iUnblockCode.Copy(KUnblockPwd);
	TMockLtsyData2<RMobilePhone::TMobilePhoneSecurityCode, RMobilePhone::TCodeAndUnblockCode> verifyData(secCode, twoCodes);
	verifyData.SerialiseL(data);

	// Call the ipc for SetFdnSetting.
	// Also call NotifyFdnStatusChange as this is triggered by CTSY when SetFdnSetting completes
    RMobilePhone::TMobilePhoneFdnStatus  fdnStatus(RMobilePhone::EFdnUnknown);
    iPhone.NotifyFdnStatusChange(reqStatusNotif, fdnStatus);
	RMobilePhone::TMobilePhoneFdnSetting  fdnSetting(RMobilePhone::EFdnSetOff);
	iPhone.SetFdnSetting(reqStatus, fdnSetting);

	// Send the ExpectL and CompleteL for VerifySecurityCode
	iMockLTSY.ExpectL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, data, KErrNone);
	iMockLTSY.CompleteL(MLtsyDispatchSecurityVerifySecurityCode::KLtsyDispatchSecurityVerifySecurityCodeApiId, KErrNone);

	// Send ExpectL for SetFdnSetting
	data.Close();
	TMockLtsyData1<RMobilePhone::TMobilePhoneFdnSetting> setData(fdnSetting);
	data.Close();
	setData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId, data, KErrGeneral);

	// Call ipc for VerifySecurityCode
	iPhone.VerifySecurityCode(reqStatusVerify, secCode, pwd, unblockPwd);

	// Wait for VerifySecurityCode...
	User::WaitForRequest(reqStatusVerify);
	ASSERT_EQUALS(KErrNone, reqStatusVerify.Int());
	AssertMockLtsyStatusL();

	// Now send the CompleteL for SetFdnSetting since CTSY sends this ipc down to LTSY after completing VerifySecurityCode.
	iMockLTSY.CompleteL(MLtsyDispatchSimSetFdnSetting::KLtsyDispatchSimSetFdnSettingApiId, KErrNone);

	// Wait for SetFdnSetting...
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	User::WaitForRequest(reqStatusNotif);
	ASSERT_EQUALS(KErrNone, reqStatusNotif.Int());
	ASSERT_EQUALS(fdnStatus, RMobilePhone::EFdnNotActive);

	data.Close();
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0006
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for a Sim Refresh indicator from teh LTSY
@SYMTestPriority High
@SYMTestActions Invokes SIM Refresh IPC 
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0006L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	/****************************Nothing to Refresh***********************************************/

	//KCacheAdn KCacheFdn KCacheServiceTable KCacheALSline
	TUint16 refreshFileList = 0;
	
	TMockLtsyData1<TUint16> refreshFileListData(refreshFileList);
	refreshFileListData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimRefreshSimFilesIndId, KErrNone, data);
	data.Close();
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);
	
	// Prepare data for the mockLtsy's CompleteL Note the CTSY expects the data in 8-bit format.
	_LIT8  (KIMSI8, "012012012012999");
	TBuf8<RMobilePhone::KIMSISize> idComp(KIMSI8);
	TMockLtsyData1<TBuf8<RMobilePhone::KIMSISize> > idCompData(idComp);
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);
	data.Close();

	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();
	
	/****************************Service Table Refresh***********************************************/
	
	//KCacheAdn KCacheFdn KCacheServiceTable KCacheALSline
	refreshFileList = KCacheServiceTable;

	refreshFileListData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimRefreshSimFilesIndId, KErrNone, data);
	data.Close();
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);
	
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);
	data.Close();

	
	RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);
	data.Close();
	
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTableResult;
	serviceTableResult.iServices1To8  = 0xFF;
	serviceTableResult.iServices9To16 = 0xFF;
	serviceTableResult.iServices17To24= 0xFF;
	serviceTableResult.iServices25To32= 0xFF;
	serviceTableResult.iServices33To40= 0xFF;
	serviceTableResult.iServices41To48= 0xFF;
	serviceTableResult.iServices49To56= 0xFF;
	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTableV1> serviceTableResultData(serviceTableResult);
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,KErrNone,data,0);
	data.Close();

	
	TInt error = KErrNone;
	TMockLtsyData1<TInt> simRefreshDoneExpLtsyData(error);
	simRefreshDoneExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,data);
	data.Close();
		
	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();

	/****************************ALS Line Refresh***********************************************/

	//KCacheAdn KCacheFdn KCacheServiceTable KCacheALSline
	refreshFileList = KCacheALSline;

	refreshFileListData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimRefreshSimFilesIndId, KErrNone, data);
	data.Close();
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);
	
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);
	data.Close();
	
	error = 0; //CTSY sends down 0 - EFalse, (No Read Error) see CMmPhoneTsy::ResetMEAlsInfo
	simRefreshDoneExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,data);
	data.Close();
		
	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();

	/****************************Service Table Refresh && ALS Line Refresh*************************************/

	//KCacheAdn KCacheFdn KCacheServiceTable KCacheALSline
	refreshFileList = KCacheServiceTable | KCacheALSline;

	refreshFileListData.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSimRefreshSimFilesIndId, KErrNone, data);
	data.Close();

	iMockLTSY.ExpectL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId);
	
	idCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSubscriberId::KLtsyDispatchSimGetSubscriberIdApiId, KErrNone, data);
	data.Close();

	serviceTableData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId, data);
	data.Close();
	
	serviceTableResultData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetServiceTable::KLtsyDispatchSimGetServiceTableApiId,KErrNone,data,0);
	data.Close();

	//CTSY defect would cause teh below expect, now if def'ed out see CMmPhoneTsy::CompleteCacheSimL
	//error = 1; //CTSY sends down 1
	//simRefreshDoneExpLtsyData.SerialiseL(data);
	//iMockLTSY.ExpectL(MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,data);
	//data.Close();

	error = 0; 
	simRefreshDoneExpLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSimRefreshDone::KLtsyDispatchSimSimRefreshDoneApiId,data);
	data.Close();

	
	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();
	
	//see phonebookfu for full refresh of Fdn and Adn phonebooks.
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0007
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Power SIM On and Off
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::PowerSimOn() and RMmCustomAPI::PowerSimOff()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0007L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;

	/*******************************************************************/
	
	//power sim on with complete returning KErrNone
	TInt error = KErrNone;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId,error);
	
	customApi.PowerSimOn(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();

	/*******************************************************************/

	//power sim on with complete returning KErrGeneral
	error = KErrGeneral;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimPowerSimOn::KLtsyDispatchSimPowerSimOnApiId,error);
	
	customApi.PowerSimOn(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();

	/*******************************************************************/

	//power sim off with complete returning KErrNone
	error = KErrNone;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId,error);
	
	customApi.PowerSimOff(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();

	/*******************************************************************/

	//power sim off with complete returning KErrGeneral
	error = KErrGeneral;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimPowerSimOff::KLtsyDispatchSimPowerSimOffApiId,error);
	
	customApi.PowerSimOff(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();


	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0008
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for a send APDU request (async and sync)
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SendAPDUReq()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0008L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	RBuf8 data;
	CleanupClosePushL(data);
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;
	
	_LIT8(KApduDataExp,"APDU DATA EXP ");
	_LIT8(KApduDataComp,"APDU DATA COMP");
	TDesC8* commandData = const_cast<TDesC8*>(&KApduDataExp);
	TDesC8* responseData = const_cast<TDesC8*>(&KApduDataComp);
	
	
	/*****************************************************************************/
	//test SendAPDUReq async method in KErrNone case
	
	TInt error = KErrNone;
	
	TUint8 serviceTypeExp  = 101;
	TUint8 cardReaderNumberExp = 102;
	TUint8 applicationTypeExp = 103;
	TMockLtsyData4<TUint8,TUint8,TUint8,TDesC8*> sendApduReqExpData(serviceTypeExp,cardReaderNumberExp,applicationTypeExp,commandData);
	sendApduReqExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,data);
	data.Close();
	
	TUint8 serviceTypeComp  = 201;
	TUint8 cardReaderNumberComp = 202;
	TUint8 applicationTypeComp = 203;
	TMockLtsyData4<TUint8,TUint8,TUint8,TDesC8*> sendApduReqCompData(serviceTypeComp,cardReaderNumberComp,applicationTypeComp,responseData);
	sendApduReqCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,error,data);
	data.Close();
	
	TBuf8<3> info;
	info.Append(serviceTypeExp);
	info.Append(cardReaderNumberExp);
	info.Append(applicationTypeExp);
	
	RBuf8 dataBuf;
	CleanupClosePushL(dataBuf);
	dataBuf.CreateL(KApduDataExp);
	
	RMmCustomAPI::TApdu apdu(info,dataBuf);
	customApi.SendAPDUReq(reqStatus,apdu);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(info[0],serviceTypeComp);
	ASSERT_EQUALS(info[1],cardReaderNumberComp);
	ASSERT_EQUALS(info[2],applicationTypeComp);
	ASSERT_TRUE(dataBuf == KApduDataComp);
	
	AssertMockLtsyStatusL();
	
	
	/*****************************************************************************/
	//test SendAPDUReq async method in KErrGeneral case
	
	error = KErrGeneral;
	
	sendApduReqExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,data);
	data.Close();
	
	sendApduReqCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,error,data);
	data.Close();
	
	info.Zero();
	info.Append(serviceTypeExp);
	info.Append(cardReaderNumberExp);
	info.Append(applicationTypeExp);
	
	dataBuf = KApduDataExp;
	
	customApi.SendAPDUReq(reqStatus,apdu);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(info[0],serviceTypeExp); //no change
	ASSERT_EQUALS(info[1],cardReaderNumberExp); //no change
	ASSERT_EQUALS(info[2],applicationTypeExp); //no change
	ASSERT_TRUE(dataBuf == KApduDataExp); //no change
	
	AssertMockLtsyStatusL();
		
	/*****************************************************************************/
	//test SendAPDUReq sync method in KErrNone case
	
	error = KErrNone;
	
	sendApduReqExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,data);
	data.Close();
	
	sendApduReqCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSendApduRequest::KLtsyDispatchSimSendApduRequestApiId,error,data);
	data.Close();
	
	info.Zero();
	info.Append(serviceTypeExp);
	info.Append(cardReaderNumberExp);
	info.Append(applicationTypeExp);
	
	dataBuf.Copy(KApduDataExp);
		
	TInt ret = customApi.SendAPDUReq(apdu);
	ASSERT_EQUALS(ret,error);
	ASSERT_EQUALS(info[0],serviceTypeComp);
	ASSERT_EQUALS(info[1],cardReaderNumberComp);
	ASSERT_EQUALS(info[2],applicationTypeComp);
	ASSERT_TRUE(dataBuf == KApduDataComp);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(&dataBuf);
	
	/*****************************************************************************/
	//test SendAPDUReq V2 async method in KErrNone case
	
	error = KErrNone;
	
	const TUint8 KCardReaderId = 1;
	RMmCustomAPI::TApduParameters apduParameters;
	apduParameters.iCardReaderId = KCardReaderId;
	apduParameters.iCmdData = KApduDataExp;
	apduParameters.iRspData.Zero();
	
	TMockLtsyData2<TUint8,TDesC8*> sendApduV2ReqExpData(apduParameters.iCardReaderId,commandData);
	sendApduV2ReqExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId,data);
	data.Close();

	
	TMockLtsyData1<TDesC8*> sendApduV2ReqCompData(responseData);
	sendApduV2ReqCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId,error,data);
	data.Close();
	
	customApi.SendAPDUReq(reqStatus, apduParameters);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(apduParameters.iCardReaderId,KCardReaderId); 	//not changed
	ASSERT_TRUE(apduParameters.iCmdData == KApduDataExp); 		//not changed
	ASSERT_TRUE(apduParameters.iRspData == KApduDataComp);		

	AssertMockLtsyStatusL();
	
	/*****************************************************************************/
	//test SendAPDUReq V2 async method in KErrGeneral case
	error = KErrGeneral;
		
	apduParameters.iCardReaderId = KCardReaderId;
	apduParameters.iCmdData = KApduDataExp;
	apduParameters.iRspData.Zero();
	
	sendApduV2ReqExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId,data);
	data.Close();

	sendApduV2ReqCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSendApduRequestV2::KLtsyDispatchSimSendApduRequestV2ApiId,error,data);
	data.Close();
	
	customApi.SendAPDUReq(reqStatus, apduParameters);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(apduParameters.iCardReaderId,KCardReaderId); 	//not changed
	ASSERT_TRUE(apduParameters.iCmdData == KApduDataExp); 		//not changed
	ASSERT_TRUE(apduParameters.iRspData == KNullDesC8);			//not changed

	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0009
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for SIM warm reset
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SimWarmReset()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0009L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;
	
	TInt error = KErrNone;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId,error);
	
	customApi.SimWarmReset(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();
	
	
	error = KErrGeneral;
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimWarmReset::KLtsyDispatchSimSimWarmResetApiId,error);
	
	customApi.SimWarmReset(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0010
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Notify Sim Card Status indicator
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::NotifySimCardStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0010L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;
	
	TInt error = KErrNone;
	
	RMmCustomAPI::TSIMCardStatus cardStatus;
	customApi.NotifySimCardStatus(reqStatus, cardStatus);
	
	RMmCustomAPI::TSIMCardStatus expectedStatus = RMmCustomAPI::SimCardInserted;
	RBuf8 data;
	TMockLtsyData1<RMmCustomAPI::TSIMCardStatus> completeData(expectedStatus);
	completeData.SerialiseL(data);
	iMockLTSY.CompleteL(KLtsyDispatchSimNotifySimCardStatusApiId,error,data);
	data.Close();
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_EQUALS(cardStatus, expectedStatus);
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0011
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Set SIM Message Status Read
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::SetSimMessageStatusRead()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0011L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);


	TInt error = KErrNone;
	
	TTime expectedTime;
	expectedTime.HomeTime();
	
	TInt timeZoneDiff = 8; //2 hours ahead of GMT
	
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus); 
	
	RBuf8 data;
	TMockLtsyData2<TTime, TInt> completeData(expectedTime, timeZoneDiff);
	completeData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId,data,error);
	data.Close();
	iMockLTSY.CompleteL(MLtsyDispatchSimSetSimMessageStatusRead::KLtsyDispatchSimSetSimMessageStatusReadApiId,error);
	
	customApi.SetSimMessageStatusRead(expectedTime, timeZoneDiff);
	

	User::WaitForRequest(reqStatus);
	ASSERT_TRUE(reqStatus.Int() == KErrNone);

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(this);
	}
/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0012
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Read Sim File
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ReadSimFile()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0012L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	TRequestStatus reqStatus;
	
	TInt error = KErrNone;

	//example Ef(Imsi) path
	RMmCustomAPI::TSimFilePath path;
	path.Append(0x3F);
	path.Append(0x00);
	path.Append(0x7F);
	path.Append(0x00);
	path.Append(0x6F);
	path.Append(0x07);
	path.Append(0x00);
	path.Append(0x00);
	
	const TUint16 offset = 0;
	const TUint16 size = 15;
	
	RMmCustomAPI::TSimFileInfo simFileInfo;
	simFileInfo.iPath = path;
	simFileInfo.iOffSet = offset;
	simFileInfo.iSize = size;
	RMmCustomAPI::TSimFileInfoPckg simFileInfoPckg(simFileInfo);
	
	TDesC8* pathPtr =  &simFileInfo.iPath;
	TMockLtsyData3<TDesC8*,TUint16,TUint16> readSimExpData(pathPtr,simFileInfo.iOffSet,simFileInfo.iSize);
	readSimExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId,data);
	data.Close();
	
	_LIT8(KResponseBytes,"RESPONSE BYTES");
	TDesC8* responseBytesPtr = const_cast<TDesC8*>(&KResponseBytes);
	TMockLtsyData1<TDesC8*> readSimCompData(responseBytesPtr);
	readSimCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId,error,data);
	data.Close();
		
	TBuf8<20> responseBytes;
	customApi.ReadSimFile(reqStatus,simFileInfoPckg,responseBytes);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_TRUE(responseBytes == KResponseBytes);
	AssertMockLtsyStatusL();
	
	/*****************************************************************************/
	
	error = KErrGeneral;
	readSimExpData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId,data);
	data.Close();

	readSimCompData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSimReadSimFile::KLtsyDispatchSimReadSimFileApiId,error,data);
	data.Close();
	
	responseBytes.Zero();
	
	customApi.ReadSimFile(reqStatus,simFileInfoPckg,responseBytes);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(reqStatus.Int(),error);
	ASSERT_TRUE(responseBytes == KNullDesC8);
	AssertMockLtsyStatusL();
	
	
	CleanupStack::PopAndDestroy(&customApi);
	CleanupStack::PopAndDestroy(&data);
	CleanupStack::PopAndDestroy(this);
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0013
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Activate SimLock
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::ActivateSimLock()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0013L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus reqStatus;
	
	_LIT(KPassword, "12345");
	
	RMmCustomAPI::TSimLockPassword simLockPassword;
	RMmCustomAPI::TLockNumber lockNumber;
	
	lockNumber = RMmCustomAPI::EOperator;
	simLockPassword.Copy(KPassword);
	
	TDesC* expectedSimLockPassword = const_cast<TDesC*> (&KPassword);
	RMmCustomAPI::TLockNumber expectedlockNumber = lockNumber;
	TMockLtsyData2<TDesC*, RMmCustomAPI::TLockNumber> activateSimLockData(expectedSimLockPassword, expectedlockNumber);
	data.Close();
	activateSimLockData.SerialiseL(data);
	
	// asynchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, KErrNone);
	
	customApi.ActivateSimLock(reqStatus, simLockPassword, lockNumber);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	// synchronous request
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, KErrNone);
	
	TInt ret = customApi.ActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(KErrNone, ret);
	
	// failure case for asynchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, KErrGeneral);
	
	customApi.ActivateSimLock(reqStatus, simLockPassword, lockNumber);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	// failure case for synchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockActivate::KLtsyDispatchSimSimLockActivateApiId, KErrGeneral);
	
	ret = customApi.ActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(KErrGeneral, ret);
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0014
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for DeActivate SimLock
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::DeActivateSimLock()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0014L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
	
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TRequestStatus reqStatus;
	
	_LIT(KPassword, "12345");
	
	RMmCustomAPI::TSimLockPassword simLockPassword;
	RMmCustomAPI::TLockNumber lockNumber;
	
	lockNumber = RMmCustomAPI::EOperator;
	simLockPassword.Copy(KPassword);
	
	TDesC* expectedSimLockPassword = const_cast<TDesC*> (&KPassword);
	RMmCustomAPI::TLockNumber expectedlockNumber = lockNumber;
	TMockLtsyData2<TDesC*, RMmCustomAPI::TLockNumber> activateSimLockData(expectedSimLockPassword, expectedlockNumber);
	data.Close();
	activateSimLockData.SerialiseL(data);
	
	// asynchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, KErrNone);
	
	customApi.DeActivateSimLock(reqStatus, simLockPassword, lockNumber);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	// synchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, KErrNone);
	
	TInt ret = customApi.DeActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(KErrNone, ret);
	
	// failure case for asynchronous request
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, KErrGeneral);
	
	customApi.DeActivateSimLock(reqStatus, simLockPassword, lockNumber);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	// failure case for synchronous request
	
	iMockLTSY.ExpectL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, data);
	iMockLTSY.CompleteL(MLtsyDispatchSimSimLockDeActivate::KLtsyDispatchSimSimLockDeActivateApiId, KErrGeneral);
	
	ret = customApi.DeActivateSimLock(simLockPassword, lockNumber);
	ASSERT_EQUALS(KErrGeneral, ret);
	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); //data, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0015
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Get Answer to Reset
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetATR()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0015L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();

	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);	
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);
	
	TRequestStatus reqStatus;
	
	_LIT8(KAnswerToResetSend, "ExampleAnswerToResetSend");
	_LIT8(KAnswerToResetReceive, "ExampleAnswerToResetReturn");
	TBuf8<100> answerToReset(KAnswerToResetSend);
	
	TDesC8* expectedAnswerToReset = const_cast<TDesC8*> (&KAnswerToResetSend);
	TMockLtsyData1<TDesC8*> getATRData(expectedAnswerToReset);
	dataExpect.Close();
	getATRData.SerialiseL(dataExpect);
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId, dataExpect);
	
	TDesC8* returnAnswerToReset = const_cast<TDesC8*> (&KAnswerToResetReceive);
	TMockLtsyData1<TDesC8*> getATRDataReturn(returnAnswerToReset);
	dataComplete.Close();
	getATRDataReturn.SerialiseL(dataComplete);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId, KErrNone, dataComplete);
	
	customApi.GetATR(reqStatus, answerToReset);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(0, answerToReset.Compare(KAnswerToResetReceive));
	
	// failure case
		
	TBuf8<100> answerToReset2(KAnswerToResetSend);
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId, dataExpect);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetAnswerToReset::KLtsyDispatchSimGetAnswerToResetApiId, KErrGeneral, dataComplete);
		
	customApi.GetATR(reqStatus, answerToReset2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // dataComplete, dataExpect, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0016
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Get Sim Card Reader Status
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetSimCardReaderStatus()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0016L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);
	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
	
	TRequestStatus reqStatus;
	
	RMmCustomAPI::TSimCardReaderStatus simCardReaderStatus;

	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId);
	
	TUint8 responseStatus = 10;
	TMockLtsyData1<TUint8> getSimReaderStatusDataReturn(responseStatus);
	dataComplete.Close();
	getSimReaderStatusDataReturn.SerialiseL(dataComplete);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId, KErrNone, dataComplete);
	
	customApi.GetSimCardReaderStatus(reqStatus, simCardReaderStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(responseStatus, simCardReaderStatus.GetStatus());
	
	// failure case
	
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimCardReaderStatus::KLtsyDispatchSimGetSimCardReaderStatusApiId, KErrGeneral, dataComplete);
	
	RMmCustomAPI::TSimCardReaderStatus simCardReaderStatus2;
	simCardReaderStatus2.SetStatus(0);
	
	customApi.GetSimCardReaderStatus(reqStatus, simCardReaderStatus2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(3, this); // dataComplete, customApi, this
	}

/**
@SYMTestCaseID BA-CTSYD-DIS-SIM-UN0017
@SYMComponent telephony_ctsy
@SYMTestCaseDesc Test support in CTSYDispatch for Get Wlan Sim Authentication Data
@SYMTestPriority High
@SYMTestActions Invokes RMmCustomAPI::GetWlanSimAuthenticationData()
@SYMTestExpectedResults Pass
@SYMTestType UT
 */
void CCTsySimFU::TestUnit0017L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
		
	OpenPhoneL();
		
	RMmCustomAPI customApi;
	OpenCustomApiLC(customApi);

	RBuf8 dataExpect;
	CleanupClosePushL(dataExpect);	
	RBuf8 dataComplete;
	CleanupClosePushL(dataComplete);	
		
	TRequestStatus reqStatus;
	TUint32 randomValue; 

	_LIT8(KCK,"CipheringKey");
	_LIT8(KIK,"IntegrityKey");
	_LIT8(KRES,"Response");
	_LIT8(KAUTN,"AUTN Value");
	_LIT8(KAUTS,"AUTS Value");
	
	TDesC8* responseCipheringKey = const_cast<TDesC8*>(&KCK);
	TDesC8* responseIntegrityKey = const_cast<TDesC8*>(&KIK);
	TDesC8* responseResponse = const_cast<TDesC8*>(&KRES);
	TDesC8* responseAUTN = const_cast<TDesC8*>(&KAUTN);
	TDesC8* responseAUTS = const_cast<TDesC8*>(&KAUTS);

	TInt rfStateInfo ( 0 ); // assume that rfStateInfo has not been set
	
	// generate a random parameter 
	TBuf8<16> randomParameter;
	for (TInt i = 0; i< 16; i++)
		{
		randomValue = Math::Random();
		randomParameter.Append(randomValue);
		}
	TDesC8* responseRandomParam = &randomParameter;
	
	// EapSim method
	TMockLtsyData2<TDesC8*, TInt> getSimAuthenticationEapSimData(responseRandomParam, rfStateInfo);
	dataExpect.Close();
	getSimAuthenticationEapSimData.SerialiseL(dataExpect);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId, dataExpect);
	
	TMockLtsyData2<TDesC8*,TDesC8*> getSimAuthenticationEapSimDataReturn(responseResponse, responseCipheringKey);
	dataComplete.Close();
	getSimAuthenticationEapSimDataReturn.SerialiseL(dataComplete);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId, KErrNone, dataComplete);
	
	
	RMmCustomAPI::TSimAuthenticationEapSim eapSim;
	eapSim.iRandomParameters.Copy(randomParameter);
	RMmCustomAPI::TSimDataPckg simDataPckg(eapSim);
	customApi.GetWlanSimAuthenticationData(reqStatus, simDataPckg);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(0, eapSim.iSRES.Compare(KRES));
	ASSERT_EQUALS(0, eapSim.iKC.Compare(KCK));
	
	// failure case of EapSim method 
	
	randomParameter.Zero();
	for (TInt j = 0; j< 16; j++)
		{
		randomValue = Math::Random();
		randomParameter.Append(randomValue);
		}
	responseRandomParam = &randomParameter;
	
	TMockLtsyData2<TDesC8*, TInt> getSimAuthenticationEapSimData2(responseRandomParam, rfStateInfo);
	dataExpect.Close();
	getSimAuthenticationEapSimData2.SerialiseL(dataExpect);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId, dataExpect);
	
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimAuthenticationEapSimData::KLtsyDispatchSimGetSimAuthenticationEapSimDataApiId, KErrGeneral, dataComplete);
		
	
	RMmCustomAPI::TSimAuthenticationEapSim eapSim2;
	eapSim2.iRandomParameters.Copy(randomParameter);
	RMmCustomAPI::TSimDataPckg simDataPckg2(eapSim2);
	customApi.GetWlanSimAuthenticationData(reqStatus, simDataPckg2);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
		
	// EapAka method
	
	// generate a random parameter 
	TBuf8<16> randomParameter2;
	for (TInt k = 0; k< 16; k++)
		{
		randomValue = Math::Random();
		randomParameter2.Append(randomValue);
		}
	TDesC8* responseRandomParam2 = &randomParameter2;
	
	TMockLtsyData3<TDesC8*, TDesC8*, TInt> getSimAuthenticationEapAkaData(responseRandomParam2, responseAUTN, rfStateInfo);
	dataExpect.Close();
	getSimAuthenticationEapAkaData.SerialiseL(dataExpect);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId, dataExpect);
	
	TMockLtsyData4<TDesC8*, TDesC8*, TDesC8*, TDesC8*> getSimAuthenticationEapAkaDataReturn(responseResponse, responseCipheringKey, responseIntegrityKey, responseAUTS);
	dataComplete.Close();
	getSimAuthenticationEapAkaDataReturn.SerialiseL(dataComplete);
	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId, KErrNone, dataComplete);
	
	RMmCustomAPI::TSimAuthenticationEapAka eapAka;
	eapAka.iRandomParameters.Copy(randomParameter2);
	eapAka.iAUTN.Copy(KAUTN);
	RMmCustomAPI::TAkaDataPckg akaDataPckg(eapAka);
	
	customApi.GetWlanSimAuthenticationData(reqStatus, akaDataPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(0, eapAka.iRES.Compare(KRES));
	ASSERT_EQUALS(0, eapAka.iCK.Compare(KCK));
	ASSERT_EQUALS(0, eapAka.iIK.Compare(KIK));

	// failure case of EapAka method to check AUTS value
	
	// generate a random parameter 
	TBuf8<16> randomParameter3;
	for (TInt l = 0; l< 16; l++)
		{
		randomValue = Math::Random();
		randomParameter3.Append(randomValue);
		}
	TDesC8* responseRandomParam3 = &randomParameter3;
	
	TMockLtsyData3<TDesC8*, TDesC8*, TInt> getSimAuthenticationEapAkaData2(responseRandomParam3, responseAUTN, rfStateInfo);
	dataExpect.Close();
	getSimAuthenticationEapAkaData2.SerialiseL(dataExpect);
	iMockLTSY.ExpectL(MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId, dataExpect);

	iMockLTSY.CompleteL(MLtsyDispatchSimGetSimAuthenticationEapAkaData::KLtsyDispatchSimGetSimAuthenticationEapAkaDataApiId, KErrGeneral, dataComplete);
		
	RMmCustomAPI::TSimAuthenticationEapAka eapAka2;
	eapAka2.iRandomParameters.Copy(randomParameter3);
	eapAka2.iAUTN.Copy(KAUTN);
	RMmCustomAPI::TAkaDataPckg akaDataPckg2(eapAka2);
	
	customApi.GetWlanSimAuthenticationData(reqStatus, akaDataPckg2);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	ASSERT_EQUALS(0, eapAka2.iAUTS.Compare(KAUTS));
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); //dataComplete, dataExpect, customApi, this
	}
