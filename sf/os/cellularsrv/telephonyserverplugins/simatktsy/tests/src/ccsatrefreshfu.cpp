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
 @file The TEFUnit test suite for Refresh in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>
#include <cstktsy/ttlv.h>


#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h> 
#include <test/mockltsyindicatorids.h>

#include <test/tmockltsydata.h>
#include "ccsatrefreshfu.h"

// ICC file identifier
_LIT8(KICCID_file, "\x2F\xE2");	
_LIT16(KICCID16, "\x2FE2");
_LIT8(KAID, "123");
const TInt KDelay = 10;

CTestSuite* CCSatRefreshFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatRefreshFU, TestNotifyRefreshPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatRefreshFU, TestNotifyRefreshPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatRefreshFU, TestNotifyRefreshPCmd0004L);
	ADD_TEST_STEP_ISO_CPP(CCSatRefreshFU, TestNotifyRefreshPCmd0001bL);

	END_SUITE;
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-RF-SNRFPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyRefreshPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyRefreshPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatRefreshFU::TestNotifyRefreshPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 KPCmdNumber = 1;

	TTlv tlv;
	PrepareTlv( tlv, KPCmdNumber, KFileChangeNotification);
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	TRequestStatus refreshStatus;
	TRequestStatus refreshRequiredStatus;
	RSat::TRefreshV2 refresh;
	RSat::TRefreshV2Pckg refreshPck(refresh);
	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPck(refreshRequired);
	
	// test C
	// calling both NotifyRefreshRequired() and NotifyRefreshPCmd(), 
	// then completing NotifyRefreshRequired
	// and then completing NotifyRefreshPCmd through using RefreshAllowed()
	// making terminalResponse in the end
	// Other test C variants implemented in TestNotifyRefreshPCmd0001bL()

	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KICCID16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, 0x00, KICCID_file);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();

	//terminal response	
	RSat::TRefreshRspV1 refreshTerminalRsp;
	RSat::TRefreshRspV1Pckg refreshTerminalRspPckg(refreshTerminalRsp);
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber);
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;
	
	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	KFileChangeNotification,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();

	// test B, for RefreshAllowed

	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KICCID16));

	refreshTerminalRsp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	refreshTerminalRsp.iInfoType = RSat::KMeProblem;	
	refreshTerminalRsp.iAdditionalInfo.Zero();
	refreshTerminalRsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);;

	PrepareTerminalResponseMockDataL( KPCmdNumber, KRefresh, KFileChangeNotification,
			refreshTerminalRsp.iAdditionalInfo, refreshTerminalRsp.iGeneralResult, KNullDesC8);
	
	PrepareAndCallRefreshAllowedL(RSat::KMeUnableToProcessCmd);
	
	PrepareAndCallRefreshAllowedL(RSat::KSuccess, 0x00, KICCID_file);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();
	
	// test B, for NotifyRefreshRequired

	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End(), KErrGeneral );

	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrGeneral, refreshRequiredStatus.Int());

	AssertMockLtsyStatusL();
	
	// test E1	- Unsolicited completion, calling CSatNotifyRefresh::CompleteNotifyL() 		
		
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);	    
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, KErrNone, data, KDelay);
	data.Close();
	
	refreshTerminalRsp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo.Zero();
	refreshTerminalRsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);;

	PrepareTerminalResponseMockDataL( KPCmdNumber, KRefresh, KFileChangeNotification,
			refreshTerminalRsp.iAdditionalInfo, refreshTerminalRsp.iGeneralResult, KNullDesC8);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	// test E2, calling CSatNotifyRefresh::CompleteRequest() 	
	
	iMockLTSY.NotifyTerminated(requestStatus);	    
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId, KErrNone, KNullDesC8, KDelay);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	
	AssertMockLtsyStatusL();	
	
	//File Change Notification with no files in a filelist
	
	tlv.Begin(KBerTlvProactiveSimCommandTag);
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(KPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KRefresh); //ETLV_TypeOfCommand
	tlv.AddByte(KFileChangeNotification); //ETLV_CommandQualifier
	tlv.AddTag(KTlvDeviceIdentityTag); 
	tlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	tlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);
	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber);
	refreshTerminalRsp.iGeneralResult = RSat::KErrorRequiredValuesMissing;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo.Zero();

	PrepareTerminalResponseMockDataL( KPCmdNumber, KRefresh, KFileChangeNotification,
			refreshTerminalRsp.iAdditionalInfo, refreshTerminalRsp.iGeneralResult, KNullDesC8);
		
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrCorrupt, refreshRequiredStatus.Int());	
	
	AssertMockLtsyStatusL();		
	
	CleanupStack::PopAndDestroy(2, this); 	// data, this
	}


/**
@SYMTestCaseID BA-CSAT-RF-SNRFPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyRefreshPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyRefreshPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatRefreshFU::TestNotifyRefreshPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	PrepareRefreshDispatchL();
	
	RSat::TRefreshV2 refresh;
	RSat::TRefreshV2Pckg refreshPck(refresh);
	TRequestStatus refreshStatus;
	TRequestStatus refreshRequiredStatus;

	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPck(refreshRequired);

	// cancelling requests
	
	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);
	
	iSat.CancelAsyncRequest(ESatNotifyRefreshRequiredParam);

	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS( KErrCancel, refreshRequiredStatus.Int());

	iSat.CancelAsyncRequest(ESatNotifyRefreshPCmd);

	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS( KErrCancel, refreshStatus.Int());

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1, this); 
	}


/**
@SYMTestCaseID BA-CSAT-RF-SNRFPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyRefreshPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyRefreshPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatRefreshFU::TestNotifyRefreshPCmd0004L()
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
	
	TUint8 KPCmdNumber = 2;

	TTlv tlv;
	PrepareTlv( tlv, KPCmdNumber, KFileChangeNotification);

	TRequestStatus refreshStatus;
	TRequestStatus refreshRequiredStatus;
	RSat::TRefreshV2 refresh;
	RSat::TRefreshV2Pckg refreshPck(refresh);
	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPck(refreshRequired);

	PrepareRefreshDispatchL();
	
	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	TRequestStatus refreshStatus2;
	RSat::TRefreshV2 refresh2;
	RSat::TRefreshV2Pckg refreshPck2(refresh2);
	
	// only one client should call NotifyRefreshRequired(), others just call NotifyRefreshPCmd()
	sat2.NotifyRefreshPCmd(refreshStatus2, refreshPck2);
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KICCID16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, 0x00, KICCID_file);
		
	// should be enough for all clients
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	// first client
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();
	
	// second client	
	User::WaitForRequest(refreshStatus2);
	ASSERT_EQUALS(KErrNone, refreshStatus2.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refresh2.iType);
	ASSERT_EQUALS(0, refresh2.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh2.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); 	//sat2, phone2, telServer2, this
	}


/**
@SYMTestCaseID BA-CSAT-RF-SNRFPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc additional test support in CSAT for RSat::NotifyRefreshPCmd 
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyRefreshPCmd 
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatRefreshFU::TestNotifyRefreshPCmd0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	//(REFRESH, SIM Initialization)

	TUint8 KPCmdNumber = 1;

	TTlv tlv;
	
	PrepareTlv( tlv, KPCmdNumber, KSimInit);
	
	TRequestStatus refreshStatus;
	TRequestStatus refreshRequiredStatus;
	RSat::TRefreshV2 refresh;
	RSat::TRefreshV2Pckg refreshPck(refresh);
	RSat::TRefreshV2 refreshRequired;
	RSat::TRefreshV2Pckg refreshRequiredPck(refreshRequired);
	
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::ESimInit, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, ECacheEFSST | ECacheEFCBMID);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::ESimInit, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	AssertMockLtsyStatusL();	
	
	RSat::TRefreshRspV1 refreshTerminalRsp;
	RSat::TRefreshRspV1Pckg refreshTerminalRspPckg(refreshTerminalRsp);
	refreshTerminalRsp.SetPCmdNumber(KPCmdNumber);
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;

	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	KSimInit,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();	

	// completion of ESatTsyImsiChanged after terminal responce for full test coverage 	
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);	    
	iMockLTSY.CompleteL(KMockLtsyDispatchSatImsiChangedIndId, KErrNone, KDelay);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	//(REFRESH, File Change Notification)

	PrepareTlv( tlv, KPCmdNumber, KFileChangeNotification);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KICCID16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, 0x00, KICCID_file);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::EFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();

	refreshTerminalRsp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	refreshTerminalRsp.iInfoType = RSat::KMeProblem;	
	refreshTerminalRsp.iAdditionalInfo.Zero();
	refreshTerminalRsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);;
	
	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	KFileChangeNotification,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();	
	
	//(REFRESH, USIM Initialization and File Change Notification)

	PrepareTlv( tlv, KPCmdNumber, KSimInitFileChangeNotification);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::ESimInitFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KICCID16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, 0x00, KICCID_file);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::ESimInitFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KICCID16));
	AssertMockLtsyStatusL();

	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo.Zero();
	
	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	KSimInitFileChangeNotification,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();	
	
	//(REFRESH, USIM Initialization and Full File Change Notification)

	PrepareTlv( tlv, KPCmdNumber, KSimInitFullFileChangeNotification);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::ESimInitFullFileChangeNotification, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, ECacheEFSST | ECacheEFCBMID);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);

	// calling terminal responce through CSatNotifyRefresh::CompleteTerminalRespDataL
	RSat::TAdditionalInfo additionalInfo(KNullDesC);
	TDesC* addInfoPtr = &additionalInfo;
    RSat::TPCmdResult generalResult = RSat::KSuccess;
    TMockLtsyData2<RSat::TPCmdResult,TDesC*> refreshDataPck(generalResult,addInfoPtr);
    refreshDataPck.SerialiseL(data);	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatCreateRefreshTerminalRspIndId,KErrNone,data, KDelay);
	data.Close();		
		
	PrepareTerminalResponseMockDataL( KPCmdNumber, KRefresh, KSimInitFullFileChangeNotification,
			refreshTerminalRsp.iAdditionalInfo, refreshTerminalRsp.iGeneralResult, KNullDesC8);

	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::ESimInitFullFileChangeNotification, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	
	AssertMockLtsyStatusL();	
	
	//(REFRESH, UICC Reset)

	PrepareTlv( tlv, KPCmdNumber, KSimReset);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::ESimReset, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, ECacheEFSST | ECacheEFCBMID);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::ESimReset, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	AssertMockLtsyStatusL();

	// for refresh terminal response processed without calling ltsy 
	TerminalResponseL( RSat::ERefresh, refreshTerminalRspPckg);
	
	AssertMockLtsyStatusL();	
	
	// IMSI refresh through SIM init
	
	PrepareTlv( tlv, KPCmdNumber, KSimInit);
	
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::ESimInit, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, ECacheEFSST | ECacheEFCBMID);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::ESimInit, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	AssertMockLtsyStatusL();

	// completion of ESatTsyImsiChanged		
	iMockLTSY.NotifyTerminated(requestStatus);	    
	iMockLTSY.CompleteL(KMockLtsyDispatchSatImsiChangedIndId, KErrNone, KDelay);
	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
		
	refreshTerminalRsp.iGeneralResult = RSat::KSuccess;
	refreshTerminalRsp.iInfoType = RSat::KNoAdditionalInfo;
	refreshTerminalRsp.iAdditionalInfo = KNullDesC;

	//SIM Init shall not be used if EFIMSI is changed, and TerminalResponse will not reach ltsy
	TerminalResponseL( RSat::ERefresh, refreshTerminalRspPckg, KErrNone);	
	
	AssertMockLtsyStatusL();	
	
	//(REFRESH, USIM Application Reset)

	PrepareTlv( tlv, KPCmdNumber, KUSIMApplicationReset);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::EUsimApplicationReset, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KAID));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL(RSat::KSuccess, ECacheEFSST | ECacheEFCBMID, KNullDesC8, KAID);
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::EUsimApplicationReset, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KAID));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	AssertMockLtsyStatusL();

	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	KUSIMApplicationReset,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();	
	
	// 3G session reset

	PrepareTlv( tlv, KPCmdNumber, K3GSessionReset);
		
	PrepareRefreshDispatchL();

	iSat.NotifyRefreshRequired(refreshRequiredStatus, refreshRequiredPck);
	iSat.NotifyRefreshPCmd(refreshStatus, refreshPck);

	CompleteRefreshDispatchL( tlv.End() );
	
	User::WaitForRequest(refreshRequiredStatus);
	ASSERT_EQUALS(KErrNone, refreshRequiredStatus.Int());
	ASSERT_EQUALS(RSat::E3GSessionReset, refreshRequired.iType);
	ASSERT_EQUALS(0, refreshRequired.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refreshRequired.iFileList.Compare(KNullDesC16));

	PrepareAndCallRefreshAllowedL();
	
	iMockLTSY.CompleteL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,KErrNone, KDelay);
	
	User::WaitForRequest(refreshStatus);
	ASSERT_EQUALS(KErrNone, refreshStatus.Int());
	ASSERT_EQUALS(RSat::E3GSessionReset, refresh.iType);
	ASSERT_EQUALS(0, refresh.iAid.Compare(KNullDesC8));
	ASSERT_EQUALS(0, refresh.iFileList.Compare(KNullDesC16));
	AssertMockLtsyStatusL();

	GenerateTerminalResponseL(KPCmdNumber,	KRefresh,	K3GSessionReset,
			RSat::ERefresh, refreshTerminalRspPckg,	refreshTerminalRsp.iAdditionalInfo, 
			refreshTerminalRsp.iGeneralResult);
	
	AssertMockLtsyStatusL();	
	
	CleanupStack::PopAndDestroy(2, this);
	}



void CCSatRefreshFU::PrepareTlv(TTlv& aTlv, TUint8 aCmdNum, TUint8 aCmdQualifier) 
	{
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(aCmdNum);//ETLV_CommandNumber
	aTlv.AddByte(KRefresh); //ETLV_TypeOfCommand
	aTlv.AddByte(aCmdQualifier); //ETLV_CommandQualifier

	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe); //ETLV_DestinationDeviceIdentity

	switch(aCmdQualifier)
		{
		case KFileChangeNotification:
		case KSimInitFileChangeNotification:
			{
		   	aTlv.AddTag(KTlvFileListTag);
		   	aTlv.AddByte(1);	// num files
		   	aTlv.AddData(KICCID_file);	
		   	break;
			}
		case KUSIMApplicationReset:
			{
			aTlv.AddTag(KTlvAIDTag);	//ETLV_AID 
			aTlv.AddData(KAID);
			break;
			}
		default:
			break;
		}
	}

void CCSatRefreshFU::CompleteRefreshDispatchL(const TDesC8& aTlvDsc, TInt aRes)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	TDesC8* tlvDscPtr = const_cast<TDesC8*>(&aTlvDsc);
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);

	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aRes, data, KDelay);
	data.Close();		

	CleanupStack::PopAndDestroy(1); 
	}

void CCSatRefreshFU::PrepareRefreshDispatchL()
	{
	RBuf8 data;
	CleanupClosePushL(data);

	TUint8 pcmdCode = KRefresh;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	expLtsyData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId,data);
	data.Close();
 	
	CleanupStack::PopAndDestroy(1); 
	}

void CCSatRefreshFU::PrepareAndCallRefreshAllowedL(RSat::TPCmdResult aRes, TUint16 aCache, 
		const TDesC8& aFileList, const TDesC8& aAid)
	{
	RBuf8 data;
	CleanupClosePushL(data);

	if(RSat::KSuccess == aRes)
		{
	    //RSat::TAid aid(aAid); 
		TDesC8* aidPtr = const_cast<TDesC8*>(&aAid);
		TDesC8* refreshFileListPtr = const_cast<TDesC8*>(&aFileList);
		TUint16 cache = aCache;
		TMockLtsyData3<TDesC8*, TDesC8*, TUint16> refreshAllowedPack(refreshFileListPtr,aidPtr,cache);
		refreshAllowedPack.SerialiseL(data);
		iMockLTSY.ExpectL(MLtsyDispatchSatRefreshAllowed::KLtsyDispatchSatRefreshAllowedApiId,data,KErrNone);
		data.Close();
		}

	RSat::TRefreshRspV1 refreshAllowedRsp;
	refreshAllowedRsp.iGeneralResult = aRes;
	if(RSat::KSuccess == aRes)
		{
		refreshAllowedRsp.iInfoType = RSat::KNoAdditionalInfo;
		refreshAllowedRsp.iAdditionalInfo = KNullDesC;		
		}
	else
		{		
		refreshAllowedRsp.iInfoType = RSat::KMeProblem;	
		refreshAllowedRsp.iAdditionalInfo.Zero();
		refreshAllowedRsp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);;
		}		
	RSat::TRefreshRspV1Pckg refreshAllowedRspPckg(refreshAllowedRsp);	
	TRequestStatus refreshAllowedReqStatus;
	
	iSat.RefreshAllowed(refreshAllowedReqStatus,refreshAllowedRspPckg);
	
	User::WaitForRequest(refreshAllowedReqStatus);
	// it is completed with KErrNone anyway
	ASSERT_EQUALS(KErrNone, refreshAllowedReqStatus.Int());	
	
	CleanupStack::PopAndDestroy(1); 
	}



