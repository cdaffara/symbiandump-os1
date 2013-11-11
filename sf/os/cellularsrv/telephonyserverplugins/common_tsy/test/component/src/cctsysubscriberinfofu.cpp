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
// The TEFUnit test suite for SubscriberInfo in the Common TSY.
// 
//

/**
 @file 
*/
#include "cctsysubscriberinfofu.h"
#include <etel.h>
#include <etelmm.h>
#include <etelmmerr.h>
#include <et_clsvr.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "tmockltsydata.h"
#include <ctsy/serviceapi/gsmerror.h>

CTestSuite* CCTsySubscriberInfoFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0001aL);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0001bL);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0001cL);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetServiceTable0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetSubscriberId0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetSubscriberId0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetSubscriberId0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetIccAccessCaps0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0002bL);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0003L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetCustomerServiceProfile0005L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestNotifyIccAccessCapsChange0001L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestNotifyIccAccessCapsChange0002L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestNotifyIccAccessCapsChange0004L);
	ADD_TEST_STEP_ISO_CPP(CCTsySubscriberInfoFU, TestGetIccIdentity0001L);

	END_SUITE;
	}


/**
Fill in a service table
@param aTable the table to fill in
@param aValue the value of the table members
*/
LOCAL_C void FillInServiceTable(RMobilePhone::TMobilePhoneServiceTableV1 &aTable, TUint8 aValue)
	{
	aTable.iServices1To8   = aValue;
	aTable.iServices9To16  = aValue;
	aTable.iServices17To24 = aValue;
	aTable.iServices25To32 = aValue;
	aTable.iServices33To40 = aValue;
	aTable.iServices41To48 = aValue;
	aTable.iServices49To56 = aValue;
	}


/**
Fill in a csp file
@aCsp aCsp the file to fill in
*/
LOCAL_C void FillInCspFile(RMobilePhone::TMobilePhoneCspFileV1 &aCsp)
	{
	aCsp.iCallOfferingServices = 1;
	aCsp.iCallRestrictionServices = 2;
	aCsp.iOtherSuppServices = 3;
	aCsp.iCallCompletionServices = 4;
	aCsp.iTeleservices = 5;
	aCsp.iCphsTeleservices = 6;
	aCsp.iCphsFeatures = 7;
	aCsp.iNumberIdentServices = 8;
	aCsp.iPhase2PlusServices = 9;
	aCsp.iValueAddedServices = 10;
	}
/**
Opens phone object and connects to mockLTSY.
*/
void CCTsySubscriberInfoFU::OpenPhone()
    {
	TInt err = iPhone.Open(iTelServer,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, err);
	
	err = iMockLTSY.Connect();
	ASSERT_EQUALS(KErrNone, err);
    }

/**
Initializes CTSY and caches SIM and USIM service tables.
@param aSimServiceTable If not NULL contains SIM service table to be cached in CTSY
@param aUSimServiceTable If not NULL contains USIM service table to be cached in CTSY
*/
void CCTsySubscriberInfoFU::InitAndCacheSimServiceTableL(
    const RMobilePhone::TMobilePhoneServiceTableV1* aSimServiceTable,
    const RMobilePhone::TMobilePhoneServiceTableV1* aUSimServiceTable,
    TInt aSimServiceTableError)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	
	// EMmTsyBootNotifyModemStatusReadyIPC
	iMockLTSY.CompleteL(EMmTsyBootNotifyModemStatusReadyIPC, KErrNone);

	// EMobilePhoneGetNetworkRegistrationStatus
	iMockLTSY.ExpectL(EMobilePhoneGetNetworkRegistrationStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetNetworkRegistrationStatus, KErrNone, 0);
	
	// EMmTsyBootNotifySimStatusReadyIPC
	iMockLTSY.ExpectL(EMmTsyBootNotifySimStatusReadyIPC);
	iMockLTSY.CompleteL(EMmTsyBootNotifySimStatusReadyIPC, KErrNone, 0);
	
	// EMobilePhoneGetHomeNetwork
	iMockLTSY.ExpectL(EMobilePhoneGetHomeNetwork);
	RMobilePhone::TMobilePhoneNetworkInfoV5 homeNetwork;
	homeNetwork.iMode = RMobilePhone::ENetworkModeWcdma;
	homeNetwork.iStatus = RMobilePhone::ENetworkStatusCurrent;
	homeNetwork.iBandInfo = RMobilePhone::EBandUnknown;
	homeNetwork.iCountryCode = _L("234");
	homeNetwork.iCdmaSID = _L("");
	homeNetwork.iAnalogSID = _L("");
	homeNetwork.iNetworkId = _L("23499");
	homeNetwork.iDisplayTag = _L("symbian");
	homeNetwork.iShortName = _L("symbian");
	homeNetwork.iLongName = _L("symbian mobile");
	homeNetwork.iAccess = RMobilePhone::ENetworkAccessUtran;
	homeNetwork.iEgprsAvailableIndicator = ETrue;
	homeNetwork.iHsdpaAvailableIndicator = ETrue;
	TMockLtsyData1<RMobilePhone::TMobilePhoneNetworkInfoV5> homeNetworkData(homeNetwork);
	homeNetworkData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetHomeNetwork,KErrNone, data, 0);

	// EMmTsyPhoneGetPin1DisableSupportedIPC
 	iMockLTSY.ExpectL(EMmTsyPhoneGetPin1DisableSupportedIPC);
	TBool pin1DisableSupport = ETrue;
	TMockLtsyData1<TBool> pin1DisableSupportData(pin1DisableSupport);
	data.Close();
	pin1DisableSupportData.SerialiseL(data);
	iMockLTSY.CompleteL(EMmTsyPhoneGetPin1DisableSupportedIPC, KErrNone, data, 0);

	// EMmTsySimRefreshRegisterIPC
	iMockLTSY.ExpectL(EMmTsySimRefreshRegisterIPC);
	iMockLTSY.CompleteL(EMmTsySimRefreshRegisterIPC, KErrNone, 0);

	// EMobilePhoneGetServiceTable
	if ( aSimServiceTable != NULL )
	    {
        RMobilePhone::TMobilePhoneServiceTable serviceTable = RMobilePhone::ESIMServiceTable;
        TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> serviceTableData(serviceTable);
        data.Close();
        serviceTableData.SerialiseL(data);
        iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);

        RMobilePhone::TMobilePhoneServiceTableV1Pckg simServiceTableData(
            *const_cast<RMobilePhone::TMobilePhoneServiceTableV1*>(aSimServiceTable));
        iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, aSimServiceTableError, simServiceTableData, 0);
        }

	// EMobilePhoneGetALSLine
	iMockLTSY.ExpectL(EMobilePhoneGetALSLine);

	RMobilePhone::TMobilePhoneALSLine alsLine = RMobilePhone::EAlternateLinePrimary;
	TMockLtsyData1<RMobilePhone::TMobilePhoneALSLine> alsLineData(alsLine);
	data.Close();
	alsLineData.SerialiseL(data);
	iMockLTSY.CompleteL(EMobilePhoneGetALSLine, KErrNone, data, 0);
	
	iMockLTSY.ExpectL(ECustomGetIccCallForwardingStatusIPC);

	// EMobilePhoneGetIccMessageWaitingIndicators
	iMockLTSY.ExpectL(EMobilePhoneGetIccMessageWaitingIndicators);

	RMobilePhone::TMobilePhoneMessageWaitingV1 expectedMessageIndicators;
	TMockLtsyData1<RMobilePhone::TMobilePhoneMessageWaitingV1>
	                                indicatorsData(expectedMessageIndicators);
    data.Close();
    indicatorsData.SerialiseL(data); 
	iMockLTSY.CompleteL(EMobilePhoneGetIccMessageWaitingIndicators, KErrNone, data);

	iMockLTSY.ExpectL(ECustomCheckAlsPpSupportIPC);
	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	
	if ( aUSimServiceTable != NULL )
	    {
	    RMobilePhone::TMobilePhoneServiceTable USIMserviceTable = RMobilePhone::EUSIMServiceTable;
    	TMockLtsyData1<RMobilePhone::TMobilePhoneServiceTable> USIMserviceTableData(USIMserviceTable);
    	data.Close();
    	USIMserviceTableData.SerialiseL(data);
    	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, data);
    	
    	RMobilePhone::TMobilePhoneServiceTableV1Pckg usimServiceTableData(
    			*const_cast<RMobilePhone::TMobilePhoneServiceTableV1*>(aUSimServiceTable));
    	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, KErrNone, usimServiceTableData, 0);
	    }
	    
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	CleanupStack::PopAndDestroy(&data);
	AssertMockLtsyStatusL();
	
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable for cached service tables
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0001L()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RMobilePhone::TMobilePhoneServiceTableV1 completeTable;
	FillInServiceTable(completeTable, 0xFF);

    OpenPhone();
	InitAndCacheSimServiceTableL(&completeTable, NULL, KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);

	//-------------------------------------------------------------------------
	// TEST D1: Successful completion of RMobilePhone::GetServiceTable request
	// when CTSY gets SIM service table from the cache
 	//-------------------------------------------------------------------------
 	
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE( TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::IsEqual(completeTable, serviceTable) );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C: Successful completion of RMobilePhone::GetServiceTable request,
	// this time request USIM table for USIM ICC 
 	//-------------------------------------------------------------------------
	
	// change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);
	
	// prepare mockLtsy
	RMobilePhone::TMobilePhoneServiceTable expTable = RMobilePhone::EUSIMServiceTable;
	TPckg<RMobilePhone::TMobilePhoneServiceTable> expLtsyData(expTable);
	
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable2;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg completeLtsyData2(completeTable2);
	FillInServiceTable(completeTable2, 0x08);

	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, expLtsyData);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, KErrNone, completeLtsyData2);

	// test
	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE( TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::IsEqual(completeTable2, serviceTable) );
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST D2: RMobilePhone::GetServiceTable again,
	// this time request SIM table for USIM ICC,
	// CTSY gets SIM service table from the cache
 	//-------------------------------------------------------------------------
	
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE( TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::IsEqual(completeTable, serviceTable) );
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: request USIM table for SIM ICC 
 	//-------------------------------------------------------------------------

	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrMMEtelWrongMode, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: request unsupported tables
 	//-------------------------------------------------------------------------

	iPhone.GetServiceTable(reqStatus, RMobilePhone::ECDMAServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMEnabledServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EISIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0001a
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable for non-cached service tables (SIM table first)
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0001aL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

    OpenPhone();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);
            
    
        // Change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);

	//In case when SIM is not ready EMobilePhoneGetServiceTable request is writen 
	//to CMmPhoneTsy::iServiceTableRequests array of cached requests to be completed 
	//later when SIM is ready.
	
	//-------------------------------------------------------------------------
	// TEST C1: Successful completion of RMobilePhone::GetServiceTable request,
	// request SIM table for SIM ICC 
 	//-------------------------------------------------------------------------
	
	// Call GetServiceTable
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);

	// complete data for SimServiceTable
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable;
	FillInServiceTable(completeTable, 0xFF);
	RMobilePhone::TMobilePhoneServiceTableV1Pckg completeLtsyData(completeTable);
	
	// complete data for USimServiceTable
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable2;
	FillInServiceTable(completeTable2, 0x08);
	RMobilePhone::TMobilePhoneServiceTableV1Pckg completeLtsyData2(completeTable2);

	InitAndCacheSimServiceTableL(&completeTable, &completeTable2, KErrNone);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST C2: Successful completion of RMobilePhone::GetServiceTable request,
	// this time request USIM table for USIM ICC 
 	//-------------------------------------------------------------------------

	// Call GetServiceTable
	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMServiceTable, tableData);	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: request unsupported tables
 	//-------------------------------------------------------------------------

	iPhone.GetServiceTable(reqStatus, RMobilePhone::ECDMAServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMEnabledServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EISIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotFound, reqStatus.Int());

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST: request USIM table for SIM ICC 
 	//-------------------------------------------------------------------------

	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrMMEtelWrongMode, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0001b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable for non-cached service tables (USIM table first)
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhone();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);

	//-------------------------------------------------------------------------
	// TEST C: Successful completion of RMobilePhone::GetServiceTable request,
	// request USIM table for USIM ICC when cache for SIM table is empty
 	//-------------------------------------------------------------------------

	// change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);

	// both SIM and USIM service tables are cached sequentially
	// prepare mockLtsy
	
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable;
	FillInServiceTable(completeTable, 0xFF);
	RMobilePhone::TMobilePhoneServiceTableV1Pckg completeLtsyData(completeTable);
	
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, KErrTimedOut, completeLtsyData);
		
	// test
	iPhone.GetServiceTable(reqStatus, RMobilePhone::EUSIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetServiceTable again,
	// this time request SIM table for USIM ICC,
	// CTSY gets SIM service table from the cache
 	//-------------------------------------------------------------------------

	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, KErrTimedOut, completeLtsyData);
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0001c
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable to test negative flows through the CTSY
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0001cL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	RMobilePhone::TMobilePhoneServiceTableV1 completeTable;
	FillInServiceTable(completeTable, 0xFF);

    OpenPhone();
	InitAndCacheSimServiceTableL(&completeTable, NULL, KErrUnknown);

	RMobilePhone::TMobilePhoneServiceTable expTable(RMobilePhone::ESIMServiceTable);
	TPckg<RMobilePhone::TMobilePhoneServiceTable> expLtsyData(expTable);

	RMobilePhone::TMobilePhoneServiceTableV1Pckg completeLtsyData(completeTable);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);

	//-------------------------------------------------------------------------
	// TEST B: failure on pending LTSY->CTSY request completion
 	//-------------------------------------------------------------------------

    iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, expLtsyData, KErrNone);
	iMockLTSY.CompleteL(EMobilePhoneGetServiceTable, KErrUnknown, completeLtsyData);

	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	
	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrUnknown, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, expLtsyData, KErrUnknown);

	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	
	User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrUnknown, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetServiceTable
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhone();

	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::GetServiceTable
 	//-------------------------------------------------------------------------
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);
	
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);
	iPhone.CancelAsyncRequest(EMobilePhoneGetServiceTable);

	User::WaitForRequest(reqStatus);	
	// There is no cancel for this ipc, so status is KErrNone
#ifdef __WINS__
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
#else
	// No support for cancel in hardware. See EMobilePhoneGetServiceTable in CMmPhoneTsy::CancelService.
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
#endif
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetServiceTable
 	//-------------------------------------------------------------------------

	TBuf8<1> buf(KNullDesC8);
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, buf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetServiceTable
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetServiceTable
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));

	OpenPhone();

	// change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);
	
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable;
	FillInServiceTable(completeTable, 0xFF);
	
	RMobilePhone::TMobilePhoneServiceTableV1 completeTable2;
	FillInServiceTable(completeTable2, 0x08);

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

    TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable2;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData2(serviceTable2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetServiceTable
 	//-------------------------------------------------------------------------

	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);

	phone2.GetServiceTable(reqStatus2, RMobilePhone::EUSIMServiceTable, tableData2);

    InitAndCacheSimServiceTableL(&completeTable, &completeTable2, KErrNone);
     
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE( TComparator<RMobilePhone::TMobilePhoneServiceTableV1>::IsEqual(completeTable, serviceTable) );

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrServerBusy, reqStatus2.Int());
	
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGST-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetServiceTable with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetServiceTable and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetServiceTable0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));

	OpenPhone();
	
	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetServiceTable
 	//-------------------------------------------------------------------------

	RMobilePhone::TMobilePhoneServiceTable expTable(RMobilePhone::EUSIMServiceTable);
	TPckg<RMobilePhone::TMobilePhoneServiceTable> expLtsyData(expTable);
	iMockLTSY.ExpectL(EMobilePhoneGetServiceTable, expLtsyData);
	
	RMobilePhone::TMobilePhoneServiceTableV1 serviceTable;
	RMobilePhone::TMobilePhoneServiceTableV1Pckg tableData(serviceTable);
	TRequestStatus reqStatus;
	iPhone.GetServiceTable(reqStatus, RMobilePhone::ESIMServiceTable, tableData);

	User::WaitForRequest(reqStatus);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 310108);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MSI-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetSubscriberId
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetSubscriberId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetSubscriberId0001L()
	{

	OpenEtelServerL();
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	_LIT8(KImsi,"01234567890123");
	_LIT8(KImsi2,"98765432100123");

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	// Queue response
	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId, KErrNotSupported);
	// Post request
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	iPhone.GetSubscriberId(requestStatus, subscriberId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNotSupported, requestStatus.Int());

	//-------------------------------------------------------------------------
	// TEST B: failure on pending LTSY->CTSY request completion
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrGeneral, KNullDesC8);
	iPhone.GetSubscriberId(requestStatus, subscriberId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion of RMobilePhone::GetSubscriberId request
	// when result is not cached.
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi);

	iPhone.GetSubscriberId(requestStatus, subscriberId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// Check result
	TBuf8<RMobilePhone::KIMSISize> convertedSubscriberId;
	convertedSubscriberId.Copy(subscriberId);
	ASSERT_TRUE(convertedSubscriberId == KImsi);

 	//-------------------------------------------------------------------------
	// TEST D: RMobilePhone::GetSubscriberId again, this time CTSY
	// will get result from the cache.
 	//-------------------------------------------------------------------------

	iPhone.GetSubscriberId(requestStatus, subscriberId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	// Check result
	convertedSubscriberId.Copy(subscriberId);
	ASSERT_TRUE(convertedSubscriberId == KImsi);

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetSubscriberId
	// from LTSY.
 	//-------------------------------------------------------------------------
	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi2, 0);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	// Check cache has been updated
	iPhone.GetSubscriberId(requestStatus, subscriberId);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	convertedSubscriberId.Copy(subscriberId);
	ASSERT_TRUE(convertedSubscriberId == KImsi2); 	// Check result

 	//-------------------------------------------------------------------------
	// Invoke GetSubscriberId from ltsy 
	// by sim refresh procedure
 	//-------------------------------------------------------------------------
 	
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	TUint16 files = 0;
	TPckg<TUint16> filesData(files);
	iMockLTSY.CompleteL(EMmTsySimRefreshNowIPC, KErrNone, filesData);
	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi2);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MSI-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetSubscriberId
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetSubscriberId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetSubscriberId0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

	//-------------------------------------------------------------------------
	// Test cancellation of RMobilePhone::GetSubscriberId
 	//-------------------------------------------------------------------------

	// Needs to be done in separate test as it requires the CTSY
	// to not have cached the subscriberID otherwise it will just
	// get it from the cache and return immediately.

	// Expect call down to LTSY for this IPC
	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);

	// Queue response to be delayed to give time for cancel to come through
	// Return data is needed because CTSY does not pass the cancel to
	// LTSY
	_LIT8(KImsi,"01234567890123");
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi, 10);

	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	iPhone.GetSubscriberId(requestStatus, subscriberId);
	iPhone.CancelAsyncRequest(EMobilePhoneGetSubscriberId);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	}



/**
@SYMTestCaseID BA-CTSY-SUBI-MSI-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetSubscriberId
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetSubscriberId
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetSubscriberId0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

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
	// Test A: Test multiple clients requesting RMobilePhone::GetSubscriberId
 	//-------------------------------------------------------------------------

	// Expect call down to LTSY for this IPC from first client
	iMockLTSY.ExpectL(EMobilePhoneGetSubscriberId);

	_LIT8(KImsi,"01234567890123");
	// warning: CTSY expect the IMSI as a 8 bit descriptor!
	// First request completion should complete the request for both clients.
	iMockLTSY.CompleteL(EMobilePhoneGetSubscriberId, KErrNone, KImsi, 10);

	// First client makes request
	TRequestStatus requestStatus;
	RMobilePhone::TMobilePhoneSubscriberId subscriberId;
	iPhone.GetSubscriberId(requestStatus, subscriberId);

	// Second client makes request
	TRequestStatus requestStatus2;
	RMobilePhone::TMobilePhoneSubscriberId subscriberId2;
	phone2.GetSubscriberId(requestStatus2, subscriberId2);

	// Check result for first client
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	TBuf8<RMobilePhone::KIMSISize> convertedSubscriberId;
	convertedSubscriberId.Copy(subscriberId);
	ASSERT_TRUE(convertedSubscriberId == KImsi);

	// Check result for second client
	User::WaitForRequest(requestStatus2);
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());
	convertedSubscriberId.Copy(subscriberId2);
	ASSERT_TRUE(convertedSubscriberId == KImsi);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // telServer2, phone2, this
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGIAC-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIccAccessCaps
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIccAccessCaps
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetIccAccessCaps0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// change ICC to unknown type
	ChangeUsedSimCardTypeL(EICCTypeSimUnknown);

	TUint32 caps(0);
	TInt res = iPhone.GetIccAccessCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint32>(0), caps);

	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

	res = iPhone.GetIccAccessCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsSimAccessSupported), caps);

	// change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);

	res = iPhone.GetIccAccessCaps(caps);
	ASSERT_EQUALS(KErrNone, res);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsUSimAccessSupported +
            RMobilePhone::KCapsSimAccessSupported), caps);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCustomerServiceProfile
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCustomerServiceProfile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST A: failure to dispatch request to LTSY
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile, KErrNotSupported);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCspFileV1 csp;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg(csp);
	iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on pending LTSY->CTSY request completion
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	
	RMobilePhone::TMobilePhoneCspFileV1 completeCsp;
	FillInCspFile(completeCsp);
	TPckg<RMobilePhone::TMobilePhoneCspFileV1> completeLtsyData(completeCsp);
	
	iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrGeneral, completeLtsyData);
	
	iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrGeneral, reqStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion of RMobilePhone::GetCustomerServiceProfile request
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrNone, completeLtsyData);
	iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	AssertMockLtsyStatusL();
	ASSERT_TRUE(TComparator<RMobilePhone::TMobilePhoneCspFileV1>::IsEqual(completeCsp, csp));

 	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RMobilePhone::GetCustomerServiceProfile
	// from LTSY.
 	//-------------------------------------------------------------------------

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrNone, completeLtsyData, 0);
	User::WaitForRequest(mockLtsyStatus);
	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}

/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::GetCustomerServiceProfile
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::GetCustomerServiceProfile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0002L()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //-------------------------------------------------------------------------
    // Test cancelling of RMobilePhone::GetCustomerServiceProfile
    //-------------------------------------------------------------------------
    
    iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
    
    TRequestStatus reqStatus;
    RMobilePhone::TMobilePhoneCspFileV1 csp;
    RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg(csp);
    
    iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
    
    iPhone.CancelAsyncRequest(EMobilePhoneGetCustomerServiceProfile);
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(this); // this
    }

/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0002b
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCustomerServiceProfile combined with a request
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCustomerServiceProfile, cancels the first request
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0002bL()
    {
    OpenEtelServerL(EUseExtendedError);
    CleanupStack::PushL(TCleanupItem(Cleanup,this));
    OpenPhoneL();

    // Open second client
    RTelServer telServer2;
    TInt ret = telServer2.Connect();
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(telServer2);
    
    RMobilePhone phone2;
    ret = phone2.Open(telServer2,KMmTsyPhoneName);
    ASSERT_EQUALS(KErrNone, ret);
    CleanupClosePushL(phone2);

    TRequestStatus mockLtsyStatus;
    iMockLTSY.NotifyTerminated(mockLtsyStatus);

    //---------------------------------------------------------------------------------
    // Test cancelling of RMobilePhone::GetCustomerServiceProfile with multiple clients
    //---------------------------------------------------------------------------------
    
    // expect and complete the second request
    iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
    
    RMobilePhone::TMobilePhoneCspFileV1 completeCsp;
    FillInCspFile(completeCsp);
    TPckg<RMobilePhone::TMobilePhoneCspFileV1> completeLtsyData(completeCsp);
    iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrNone, completeLtsyData, 10);

    TRequestStatus reqStatus;
    RMobilePhone::TMobilePhoneCspFileV1 csp;
    RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg(csp);
    
    iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
 
    TRequestStatus reqStatus2;
    RMobilePhone::TMobilePhoneCspFileV1 csp2;
    RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg2(csp2);
    
    phone2.GetCustomerServiceProfile(reqStatus2, cspPckg2);
    
    // Cancel the first request
    iPhone.CancelAsyncRequest(EMobilePhoneGetCustomerServiceProfile);
    
    User::WaitForRequest(reqStatus2);
    ASSERT_EQUALS(KErrNone, reqStatus2.Int());
    ASSERT_TRUE(TComparator<RMobilePhone::TMobilePhoneCspFileV1>::IsEqual(completeCsp, csp2));
    
    User::WaitForRequest(reqStatus);
    ASSERT_EQUALS(KErrCancel, reqStatus.Int());

    // Wait for completion of iMockLTSY.NotifyTerminated
    User::WaitForRequest(mockLtsyStatus);
    ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
    
    AssertMockLtsyStatusL();
    CleanupStack::PopAndDestroy(3, this); // telserver2 phone2, this
    }

/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0003
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCustomerServiceProfile with bad parameter data
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCustomerServiceProfile with bad parameter data
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0003L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test B: Test passing wrong descriptor size to parameter in
	// RMobilePhone::GetCustomerServiceProfile
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TBuf8<1> smallBuf(KNullDesC8);
	iPhone.GetCustomerServiceProfile(reqStatus, smallBuf);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrArgument, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::GetCustomerServiceProfile
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::GetCustomerServiceProfile
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	// Open second client
	RTelServer telServer2;
	TInt ret = telServer2.Connect();
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(telServer2);

	RMobilePhone phone2;
	ret = phone2.Open(telServer2 ,KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RMobilePhone::GetCustomerServiceProfile
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	
	RMobilePhone::TMobilePhoneCspFileV1 completeCsp;
	FillInCspFile(completeCsp);
	TPckg<RMobilePhone::TMobilePhoneCspFileV1> completeLtsyData(completeCsp);
	iMockLTSY.CompleteL(EMobilePhoneGetCustomerServiceProfile, KErrNone, completeLtsyData);

	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCspFileV1 csp;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg(csp);
	iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
	
	TRequestStatus reqStatus2;
	RMobilePhone::TMobilePhoneCspFileV1 csp2;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg2(csp2);
	phone2.GetCustomerServiceProfile(reqStatus2, cspPckg2);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_TRUE(TComparator<RMobilePhone::TMobilePhoneCspFileV1>::IsEqual(completeCsp, csp));

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_TRUE(TComparator<RMobilePhone::TMobilePhoneCspFileV1>::IsEqual(completeCsp, csp2));

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGCSP-0005
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetCustomerServiceProfile with timeout
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetCustomerServiceProfile and tests for timeout
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetCustomerServiceProfile0005L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	//-------------------------------------------------------------------------
	// Test A: Test timeout of RMobilePhone::GetCustomerServiceProfile
 	//-------------------------------------------------------------------------

	iMockLTSY.ExpectL(EMobilePhoneGetCustomerServiceProfile);
	
	TRequestStatus reqStatus;
	RMobilePhone::TMobilePhoneCspFileV1 csp;
	RMobilePhone::TMobilePhoneCspFileV1Pckg cspPckg(csp);
	iPhone.GetCustomerServiceProfile(reqStatus, cspPckg);
	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrTimedOut, reqStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MNISS-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::NotifyIccAccessCapsChange
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::NotifyIccAccessCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestNotifyIccAccessCapsChange0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion of RMobilePhone::NotifyIccAccessCapsChange request
 	//-------------------------------------------------------------------------
	
	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyIccAccessCapsChange(reqStatus, caps);
	
	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

    iMockLTSY.CompleteL(EMobilePhoneNotifyIccAccessCapsChange, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsSimAccessSupported), caps);
	AssertMockLtsyStatusL();

	// test for UICC icc type
	iPhone.NotifyIccAccessCapsChange(reqStatus, caps);
	
	// change ICC to USIM (3G)
	ChangeUsedSimCardTypeL(EICCTypeSim3G);

	iMockLTSY.CompleteL(EMobilePhoneNotifyIccAccessCapsChange, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsUSimAccessSupported), caps);
	AssertMockLtsyStatusL();

	// test for unknown icc type
	iPhone.NotifyIccAccessCapsChange(reqStatus, caps);
	
	// change ICC to Unknown
	ChangeUsedSimCardTypeL(EICCTypeSimUnknown);

    iMockLTSY.CompleteL(EMobilePhoneNotifyIccAccessCapsChange, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(0), caps);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MNISS-0002
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RMobilePhone::NotifyIccAccessCapsChange
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RMobilePhone::NotifyIccAccessCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestNotifyIccAccessCapsChange0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RMobilePhone::NotifyIccAccessCapsChange
 	//-------------------------------------------------------------------------
 	
	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyIccAccessCapsChange(reqStatus, caps);
	
	iPhone.CancelAsyncRequest(EMobilePhoneNotifyIccAccessCapsChange);

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCancel, reqStatus.Int());
	AssertMockLtsyStatusL();

	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MNISS-0004
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RMobilePhone::NotifyIccAccessCapsChange
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RMobilePhone::NotifyIccAccessCapsChange
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestNotifyIccAccessCapsChange0004L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup, this));
	OpenPhoneL();

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
	// Test A: Test multiple clients requesting RMobilePhone::NotifyIccAccessCapsChange
 	//-------------------------------------------------------------------------

	TRequestStatus reqStatus;
	TUint32 caps(0);
	iPhone.NotifyIccAccessCapsChange(reqStatus, caps);
	
	TRequestStatus reqStatus2;
	TUint32 caps2(0);
	phone2.NotifyIccAccessCapsChange(reqStatus2, caps2);
	
	// change ICC to SIM (2G)
	ChangeUsedSimCardTypeL(EICCTypeSim2GGsm);

    iMockLTSY.CompleteL(EMobilePhoneNotifyIccAccessCapsChange, KErrNone);	

	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsSimAccessSupported), caps);

	User::WaitForRequest(reqStatus2);
	ASSERT_EQUALS(KErrNone, reqStatus2.Int());
	ASSERT_EQUALS(static_cast<TUint32>(RMobilePhone::KCapsSimAccessSupported), caps2);
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(3, this); // phone2, telServer2, this

	}


/**
@SYMTestCaseID BA-CTSY-SUBI-MGII-0001
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RMobilePhone::GetIccIdentity
@SYMTestPriority High
@SYMTestActions Invokes RMobilePhone::GetIccIdentity
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCTsySubscriberInfoFU::TestGetIccIdentity0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();

	TRequestStatus reqStatus;
	RMobilePhone::TIccIdentity identity;
	iPhone.GetIccIdentity(reqStatus, identity);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNotSupported, reqStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this);
	
	}

