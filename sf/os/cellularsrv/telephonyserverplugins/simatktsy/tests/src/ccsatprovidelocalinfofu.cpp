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
 @file The TEFUnit test suite for ProvideLocalInfo in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <ctsy/ltsy/sat_defs.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <ctsy/ltsy/mltsydispatchphoneinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>
#include <cstktsy/ttlv.h>
#include <satcs.h>                  // Etel SAT IPC definitions
#include "ccsatprovidelocalinfofu.h"


typedef TBuf8<KImeiLength> TImeiBuf;

CTestSuite* CCSatProvideLocalInfoFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;	
	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0002L);	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0004L);	
	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001bL);		
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001cL);		
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001dL);
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001eL);	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001fL);	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001gL);	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001hL);
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001iL);
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001jL);	
	ADD_TEST_STEP_ISO_CPP(CCSatProvideLocalInfoFU, TestNotifyLocalInfoPCmd0001kL);
	
	END_SUITE;
	}


//
// Actual test cases
//

/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for RSat::NotifyLocalInfoPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->SATTSY
	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifyLocalInfoPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KProvideLocalInformation);

	// post request 
	RSat::TLocalInfoV6 localInfo;
	RSat::TLocalInfoV6Pckg localInfoPckg(localInfo);	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPckg);

	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoLanguage);
	TPtrC8 tlv(localInfoTlv.End());
	

	// post Complete with KErrGeneral
	CompleteMockDispatchSatPcmdL(tlv, KErrUnknown);

	User::WaitForRequest(stat);	
	ASSERT_EQUALS(KErrUnknown, stat.Int());
	AssertMockLtsyStatusL();
	
	//-------------------------------------------------------------------------
	// TEST C: see tests TestProvideLocalInfo0001bL, TestProvideLocalInfo0001cL, ...
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyLocalInfoPCmd
	// from LTSY.
	//-------------------------------------------------------------------------	
	
	// post request 
	TRequestStatus requestStatus;
	iMockLTSY.NotifyTerminated(requestStatus);
	
	// post CompleteL
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);
	
	// prepare data and call 
	// iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);
	RSat::TAdditionalInfo additionalInfo;	
	additionalInfo.Append(RSat::KNoSpecificMeProblem);	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoLanguage,
			additionalInfo,
			RSat::KMeUnableToProcessCmd,
			KNullDesC8);

	User::WaitForRequest(requestStatus);
	AssertMockLtsyStatusL();
	ASSERT_EQUALS(KErrNone, requestStatus.Int());	
	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for cancelling of RSat::NotifyLocalInfoPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0002L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();	
	
	RBuf8 data;
	CleanupClosePushL(data);
	
	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyLocalInfoPCmd
	//-------------------------------------------------------------------------
	
	//Prepare data for ExpectL needed by NotifyLocalInfoPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KProvideLocalInformation);
	
	RSat::TLocalInfoV6 localInfo;
	RSat::TLocalInfoV6Pckg localInfoPckg(localInfo);	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPckg);
	
	iSat.CancelAsyncRequest(ESatNotifyLocalInfoPCmd);
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrCancel, stat.Int());	
	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test support in CTSY for multiple client requests to RSat::NotifyLocalInfoPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0004L()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
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
	User::LeaveIfError(sat2.Open(phone2));
	CleanupClosePushL(sat2);
	
	//-------------------------------------------------------------------------
	// Test multiple clients requesting RSat::NotifyLocalInfoPCmd
	//-------------------------------------------------------------------------	
	//Prepare data for ExpectL needed by NotifyLocalInfoPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KProvideLocalInformation);
		
	// Request from first client
	RSat::TLocalInfoV6 localInfo;
	RSat::TLocalInfoV6Pckg localInfoPckg(localInfo);	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPckg);
	
	// Request from second client
	RSat::TLocalInfoV6 localInfo2;
	RSat::TLocalInfoV6Pckg localInfoPckg2(localInfo2);	
	TRequestStatus stat2;
	sat2.NotifyLocalInfoPCmd(stat2, localInfoPckg2);
				
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoLanguage);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
	    
	// Wait for first client complete
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrNone, stat.Int());
	ASSERT_EQUALS(RSat::KUICC, localInfo.iDevideId);
	ASSERT_EQUALS(RSat::KProvideLocalInfoLanguage, localInfo.iInfoType);	
	
	// Wait for second client complete
	User::WaitForRequest(stat2);
	ASSERT_EQUALS(KErrNone, stat2.Int());
	ASSERT_EQUALS(RSat::KUICC, localInfo2.iDevideId);
	ASSERT_EQUALS(RSat::KProvideLocalInfoLanguage, localInfo2.iInfoType);	
	
	
	//Now prepare the FIRST terminal response	 
	_LIT(KTermRespLangCode,"EN"); //language code 	
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvLanguageTag );
	TBuf8<2> character;
	TSatUtility::Copy16to8LE( KTermRespLangCode, character);
	termRespAddInfo.AddData( character );	
	const TDesC8& termRespAddInfoData = termRespAddInfo.GetDataWithoutTopLevelTag();
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoLanguage,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfoData);	
	
 	// call the FIRST TerminalRsp
	RSat::TLocalInfoRspV3 resp;
	RSat::TLocalInfoRspV3Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KProvideLocalInfoLanguage;
	resp.iAdditionalInfo = KTermRespLangCode; //language code 
	resp.SetPCmdNumber(pcmdNumber);	
	TerminalResponseL(respPckg);
	
	// There is an ambiguity: should be there the Terminal Response from the SECOND client
	// and should such Response has a successful flow?	
	
	// So, consider that the SECOND cliend doesn't know about the FIRST one, 
	// prepare and...	
	PrepareTerminalResponseMockDataWithExpL(
				pcmdNumber,			
				RSat::KProvideLocalInfoLanguage,
				KNullDesC,
				RSat::KSuccess,
				termRespAddInfoData);
	// ...post the SECOND Terminal Response
	sat2.TerminalRsp(stat, RSat::ELocalInfo, respPckg);
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrNone, stat.Int());	
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(5, this); 	
	}

/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Expected Sequence 1.1 (PROVIDE LOCAL INFORMATION, Local Info)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001bL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	//
	// Expected Sequence 1.1 See ETSI TS 100 607-4  27.22.4.15.4.2, Expected Sequence 1.1.
	// (PROVIDE LOCAL INFORMATION, Local Info)
	//	
	
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfo);
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);	
	
	// Prepare ExpectL needed by previous complete;	
	iMockLTSY.ExpectL(MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId);
	
	TRequestStatus reqStatus;
	iMockLTSY.NotifyTerminated(reqStatus);
	User::WaitForRequest(reqStatus);
	ASSERT_TRUE(reqStatus.Int() == KErrNone);

	// Complete 
	_LIT8(operatorCode,"926"); //code of some operator in Moscow
	TLocalInformation localInfoPCmdData;
	localInfoPCmdData.iCellId = KCellId;
	localInfoPCmdData.iLocationAreaCode = KLocationAreaCode;
	localInfoPCmdData.iOperatorCode = operatorCode;
	TDesC8* operatorCodePtr = &localInfoPCmdData.iOperatorCode;
	TMockLtsyData3<TUint16,TUint16,TDesC8* > locationInfoPCmdDataPack(	localInfoPCmdData.iLocationAreaCode,
	  																					 	localInfoPCmdData.iCellId,
	  																					 	operatorCodePtr);	
	locationInfoPCmdDataPack.SerialiseL(data);
    iMockLTSY.CompleteL(MLtsyDispatchSatProvideLocationInfo::KLtsyDispatchSatProvideLocationInfoApiId, KErrNone, data);
	
	// --- Now prepare the terminal response ---    
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvLocationInformationTag );
	termRespAddInfo.AddData( operatorCode );
	termRespAddInfo.AddByte( static_cast<TUint8>( KLocationAreaCode >> 8 ) );//msb
	termRespAddInfo.AddByte( static_cast<TUint8>( KLocationAreaCode));//lsb
	termRespAddInfo.AddByte( static_cast<TUint8>( KCellId >> 8 ) );//msb
	termRespAddInfo.AddByte( static_cast<TUint8>( KCellId));//lsb
	    
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfo,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag()); 	
 	
 	// Terminal response is called from CTSY 	
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int());
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001c
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc  Expected Sequence 1.2 (PROVIDE LOCAL INFORMATION, IMEI of the Terminal)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001cL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	// Expected Sequence 1.1 See ETSI TS 102 384 in subclause 27.22.4.15.4.2, Expected Sequence 1.2.
	// (PROVIDE LOCAL INFORMATION, IMEI of the Terminal)
	//	
		
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoImei);
	const TDesC8& tlv = localInfoTlv.End();
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);		
	
	// prepare Tlv buffer with additional info
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvImeiTag );
	TBuf8<KImeiLength/2> bcdImei;	
	_LIT8 (KDigits, "*12345670");	
	// Convert IMEI to BCD format
	TSatUtility::AsciiToBCD( KDigits(), bcdImei );
	// Add IMEI code to the terminal response 
	termRespAddInfo.AddData( bcdImei );	
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoImei,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag()); 
 	
 	// Terminal response is called from CTSY 
 	TRequestStatus reqStatus;
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int());
 	
	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001d
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Expected Sequence 1.3 (PROVIDE LOCAL INFORMATION, Network Measurement Results)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001dL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	//
	// Expected Sequence 1.3 See ETSI TS 100 607-4  27.22.4.15.4.2, Expected Sequence 1.3.
	// (PROVIDE LOCAL INFORMATION, Network Measurement Results)
	//	
	
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 105;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInformationNmr);		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		
		
	// Prepare ExpectL and CompleteL needed by 
	// TInt CSatNotifyLocalInfo::CompleteNotifyL()
	_LIT8(KNmrResults,"qwerty");
	TDesC8* nmr = &const_cast<TDesC8&>(KNmrResults());
	_LIT(KBcchList,"ytrewq");
	TDesC* bcch = &const_cast<TDesC&>(KBcchList());
	TMockLtsyData2<TDesC8*,TDesC*> nmrMockData(nmr,bcch);	
	nmrMockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatLocalInformationNmr::KLtsyDispatchSatLocalInformationNmrApiId, KErrNone, data);	
		
	//Now prepare and generate CTSY Terminal response
	TTlv tlvAddInfo;
	tlvAddInfo.AddTag( KTlvNetworkMeasurementResultsTag );
	tlvAddInfo.AddData(KNmrResults);		
	tlvAddInfo.AddTag (KTlvBccdChannelListTag);
	TUint8 charData[]={30, 71, 65, 200, 101, 29, 199, 16};
	
	for (int i = 0; i < 8; i++)
		{
		tlvAddInfo.AddByte( charData[i] );
		}
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,		
			RSat::KProvideLocalInformationNmr,
			KNullDesC,
			RSat::KSuccess,
			tlvAddInfo.GetDataWithoutTopLevelTag());
	
	// Terminal response is called from CTSY 
 	TRequestStatus reqStatus;
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int());
 	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001e
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc  Expected Sequence 1.4 (PROVIDE LOCAL INFORMATION, Date, Time, Time Zone))
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
 */
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001eL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);
	
	TRequestStatus reqStatus;	
	TUint8 pcmdNumber = 1;	
	//
	// See ETSI TS 102 384 in subclause 27.22.4.15.4.2, Expected Sequence 1.4.
	// (PROVIDE LOCAL INFORMATION, Date, Time, Time Zone))
	//	
			
	// set same Time Zone Parameter
	iMockLTSY.NotifyTerminated(reqStatus);	
	TUint8 timeZone(123);	
	TMockLtsyData1<TUint8> mockData(timeZone);	
	mockData.SerialiseL(data);	
	iMockLTSY.CompleteL(KMockLtsyDispatchSatTimeZoneChangeIndId, KErrNone, data);
		
	// wait for Time Zone setting	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());
	
	// Prepare TLV buffer for CompleteL
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoDateTimeTimeZone);		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);	
	
	// prepare Tlv buffer with additional info
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvDateTimeAndTimeZoneTag );
	RSat::TLocalInfoRspV3 localInfoRspV3;	
	SetDateTimeTimeZone ( localInfoRspV3, timeZone );
	
    for ( int i = 0; i < localInfoRspV3.iAdditionalInfo.Length(); i++ )
        {
        termRespAddInfo.AddByte( static_cast<TUint8>( localInfoRspV3.iAdditionalInfo[i] 
                                            & 0x00FF ) );
        }
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoDateTimeTimeZone,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag()); 

	// Terminal response is called from CTSY  	
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	
 	// It's not a defect!!!
	// We can't prepare correct time, because CTSY get's Universal time.
 	// Time befor sendinq request to CTSY  and time in CTSY is different.
 	// That's why we don't check got error
 	
 	CleanupStack::PopAndDestroy(2, this); // data, this		
	}

/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001f
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Expected Sequence 1.5 (PROVIDE LOCAL INFORMATION, Language setting)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001fL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	// Expected Sequence 1.5 See ETSI TS 102 384 in subclause 27.22.4.15.4.2, Expected Sequence 1.5.
	// (PROVIDE LOCAL INFORMATION, Language setting)
	//	
	
	//Prepare data for ExpectL needed by NotifyLocalInfoPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KProvideLocalInformation);
	
	RSat::TLocalInfoV6 localInfo;
	RSat::TLocalInfoV6Pckg localInfoPckg(localInfo);	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPckg);
	
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoLanguage);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
	    
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrNone, stat.Int());
	ASSERT_EQUALS(RSat::KUICC, localInfo.iDevideId);
	ASSERT_EQUALS(RSat::KProvideLocalInfoLanguage, localInfo.iInfoType);		
	
	//Now prepare the terminal response	 
	_LIT(KTermRespLangCode,"EN"); //language code 	
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvLanguageTag );
	TBuf8<2> character;
	TSatUtility::Copy16to8LE( KTermRespLangCode, character);
	termRespAddInfo.AddData( character );
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoLanguage,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag());
	
	
 	// call TerminalRsp
	RSat::TLocalInfoRspV3 resp;
	RSat::TLocalInfoRspV3Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KProvideLocalInfoLanguage;
	resp.iAdditionalInfo = KTermRespLangCode; //language code 
	resp.SetPCmdNumber(pcmdNumber);	
	TerminalResponseL(respPckg);
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001g
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Expected Sequence 1.6 (PROVIDE LOCAL INFORMATION, Timing advance)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001gL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	// Expected Sequence 1.5 See ETSI TS 100 607-4 V8.14.0  in subclause 27.22.4.15.4.2, Expected Sequence 1.6.
	// ((PROVIDE LOCAL INFORMATION, Timing advance)
	//	
	
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoTimingAdv);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);
	TBool localInfoIsOngoing = ETrue;
	TMockLtsyData1<TBool> mockData(localInfoIsOngoing);	
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId, data);
	TUint8 timingAdvance(0x11);
	TUint8 meStatus(0x22);
	TMockLtsyData2<TUint8, TUint8> mockData2(timingAdvance, meStatus);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatTimingAdvance::KLtsyDispatchSatTimingAdvanceApiId, KErrNone, data);		
	
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvTimingAdvanceTag );
	termRespAddInfo.AddByte( meStatus );
	termRespAddInfo.AddByte( timingAdvance );
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoTimingAdv,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag()); 

	// Terminal response is called from CTSY  	
	TRequestStatus reqStatus;
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 	
 	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001h
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Expected Sequence 1.7 PROVIDE LOCAL INFORMATION, Access Technology
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001hL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	 
	TRequestStatus reqStatus;	
	//
	// This Sequence isn't describe in docs. It's used just for coverage
	// Expected Sequence 1.7 (PROVIDE LOCAL INFORMATION, Access Technology)
	// The test method is not defined in the present document as it depends on a present NAA.
	//	see ETSI TS 102 384 V6.5.0	
	
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoAccTech);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);
	TBool localInfoIsOngoing = ETrue;
	TMockLtsyData1<TBool> mockData(localInfoIsOngoing);	
	mockData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId, data);
	
	// wait for CompleteMockDispatchSatPcmdL
	iMockLTSY.NotifyTerminated(reqStatus); 	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
	
	// set up data for CSatNotifyLocalInfo::iCurrentAccTech
	TUint8 currentAccTech(0xAA);
	TMockLtsyData1<TUint8> mockData2(currentAccTech);
	data.Close();
	mockData2.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetAccessTechnology::KLtsyDispatchSatGetAccessTechnologyApiId, KErrNone, data);		
	
	// Prepare Terminal Response
	TTlv termRespAddInfo;
	termRespAddInfo.AddTag( KTlvAccessTechnologyTag );
	termRespAddInfo.AddByte( currentAccTech );	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoAccTech,
			KNullDesC,
			RSat::KSuccess,
			termRespAddInfo.GetDataWithoutTopLevelTag()); 

	// Terminal response is called from CTSY
	// so, just wait for it
	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 	
 	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001i
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc Test error flow of Terminal responce
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001iL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	
	//
	// Test error flow of Terminal responce
	// This test is just for coverage and has NO references in documents
	//	
	
	//Prepare data for ExpectL needed by NotifyLocalInfoPCmd
	//and post that ExpectL
	PrepareMockLDataWithExpL(KProvideLocalInformation);
		
	RSat::TLocalInfoV6 localInfo;
	RSat::TLocalInfoV6Pckg localInfoPckg(localInfo);	
	TRequestStatus stat;
	iSat.NotifyLocalInfoPCmd(stat, localInfoPckg);
		
	// Prepare TLV buffer for CompleteL
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoLanguage);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);		    
	User::WaitForRequest(stat);		
	
	//PrepareTerminalResponse
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoLanguage,
			KNullDesC,
			RSat::KNoResponseFromUser,
			KNullDesC8);
		
	// call TerminalRsp with some ERROR
	RSat::TLocalInfoRspV3 resp;
	RSat::TLocalInfoRspV3Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KNoResponseFromUser;
	resp.iInfoType = RSat::KMeProblem;	
	resp.SetPCmdNumber(pcmdNumber);	
	// Call TerminalResponse with expected error = KErrCorrupt
	TerminalResponseL(respPckg, KErrCorrupt);
	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001j
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc test not supported commands (KProvideLocalInfoESN, KProvideLocalInfoIMEISV and KProvideLocalInfoSearchMode)
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001jL()
	{	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RBuf8 data;
	CleanupClosePushL(data);	
	 
	TRequestStatus reqStatus;	
	
	//
	//Test not supported commands 
	//	
	
	// --- KProvideLocalInfoESN ---		
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoESN);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv.End(), KErrNone);
	
	// Prepare Terminal Response		
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoESN,
			KNullDesC,
			RSat::KCmdBeyondMeCapabilities,
			KNullDesC8); 

	// Terminal response is called from CTSY
	// so, just wait for it
	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 	
 	
 	
	// --- KProvideLocalInfoIMEISV ---	
	TTlv localInfoTlv2;	
	PrepareTlv(localInfoTlv2, pcmdNumber, RSat::KProvideLocalInfoIMEISV);
		
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(localInfoTlv2.End(), KErrNone);
	
	// Prepare Terminal Response		
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoIMEISV,
			KNullDesC,
			RSat::KCmdTypeNotUnderstood,
			KNullDesC8); 

	// Terminal response is called from CTSY
	// so, just wait for it
	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 	
 	
 	
 	// --- KProvideLocalInfoSearchMode ---	
 	TTlv localInfoTlv3;	
 	PrepareTlv(localInfoTlv3, pcmdNumber, RSat::KProvideLocalInfoSearchMode);
 			
 	// post Complete with KErrNone
 	CompleteMockDispatchSatPcmdL(localInfoTlv3.End(), KErrNone);
 		
 	// Prepare Terminal Response		
 	PrepareTerminalResponseMockDataWithExpL(
 			pcmdNumber,			
 			RSat::KProvideLocalInfoSearchMode,
 			KNullDesC,
 			RSat::KCmdTypeNotUnderstood,
 			KNullDesC8); 

 	// Terminal response is called from CTSY
 	// so, just wait for it
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 
 	
	AssertMockLtsyStatusL();	
	CleanupStack::PopAndDestroy(2, this); // data, this
	}


/**
@SYMTestCaseID BA-CSAT-PLI-SNLIPC-0001k
@SYMPREQ 1780
@SYMComponent  telephony_ctsy
@SYMTestCaseDesc  Just for coverage increasing. Test zeroed IMEI
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLocalInfoPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatProvideLocalInfoFU::TestNotifyLocalInfoPCmd0001kL()
	{
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	
	RBuf8 data;
	CleanupClosePushL(data);	
	
	// Open Sat with NULL-Length IMEI	
	OpenSatWithZeroImeiL();		
	
	//
	//Test  (PROVIDE LOCAL INFORMATION, IMEI of the Terminal) with IMEI of NULL-Length 
	//	
	TUint8 pcmdNumber = 1;
	TTlv localInfoTlv;	
	PrepareTlv(localInfoTlv, pcmdNumber, RSat::KProvideLocalInfoImei);
	const TDesC8& tlv = localInfoTlv.End();
	
	// post Complete with KErrNone
	CompleteMockDispatchSatPcmdL(tlv, KErrNone);	
	
	TBuf<RSat::KAdditionalInfoMaxSize> additionalInfo; 
	additionalInfo.Append(KNoSpecificCauseCanBeGiven );
	
	PrepareTerminalResponseMockDataWithExpL(
			pcmdNumber,			
			RSat::KProvideLocalInfoImei,
			additionalInfo,
			RSat::KMeUnableToProcessCmd,
			KNullDesC8);  	
	
 	// Terminal response is called from CTSY 
 	TRequestStatus reqStatus;
 	iMockLTSY.NotifyTerminated(reqStatus); 	
 	User::WaitForRequest(reqStatus);
 	ASSERT_EQUALS(KErrNone, reqStatus.Int());  	
 	
 	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(2, this); // data, this	
	}


//----------------------------------------------------------------------------------------------------------
// Utility functions
//----------------------------------------------------------------------------------------------------------
/**
 *  Prepare data for ExpectL() for NotifyLocalInfoPCmd() and post ExpectL.	
 * @param aPCmdNumber - command number	 
 */ 
void CCSatProvideLocalInfoFU::PrepareMockLDataWithExpL( TUint8 aPCmdNumber )
	{
	TPckg<TUint8> pcmdCodePckg(aPCmdNumber);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, pcmdCodePckg);
	}

/**
 * prepare tlv buffer for PROVIDE LOCAL INFO proactive command 
 * @param aTlv - tlv buffer to prepare
 * @param aPCmdNumber - command number
 * @param aLocalInfoQualifier - type of command
 */ 
void CCSatProvideLocalInfoFU::PrepareTlv(TTlv& aTlv, TUint8 aPCmdNumber, TUint8 aLocalInfoQualifier)
	{	
	aTlv.Begin(KBerTlvProactiveSimCommandTag);
	aTlv.AddTag(KTlvCommandDetailsTag);
	aTlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	aTlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	aTlv.AddByte(aLocalInfoQualifier); //ETLV_CommandQualifier			
	aTlv.AddTag(KTlvDeviceIdentityTag); 
	aTlv.AddByte(KSim); //ETLV_SourceDeviceIdentity
	aTlv.AddByte(KMe);
	}

/**
* prepare mock for processing of proactive command
* @param aCompleteTlv - tlv buffer to send
* @param aError - result for request completion
*/ 
void CCSatProvideLocalInfoFU::CompleteMockDispatchSatPcmdL( TPtrC8 aCompleteTlv, TInt aError )
	{
	RBuf8 data;
	CleanupClosePushL(data);	

	TDesC8* tlvPtr = &aCompleteTlv;	
	TMockLtsyData1<TDesC8*> dspTxtDscPack(tlvPtr);
	dspTxtDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aError, data, 10);
	
	CleanupStack::PopAndDestroy(&data);
	}

/**
* prepare data and call  iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);	
* @param aPCmdNumber - command number
* @param aCommandQualifier - type of PROVIDE LOCAL INFO - command
* @param aGeneralResultAddInfo - additional info for complete  
* @param aGeneralResult - for request completion
* @param aAdditionalInfoTlv -  tlv buffer
*/ 
void CCSatProvideLocalInfoFU::PrepareTerminalResponseMockDataWithExpL(
		TUint8 aPCmdNumber,		
		TUint8 aCommandQualifier,
		const TDesC& aGeneralResultAddInfo,
		RSat::TPCmdResult aGeneralResult,
		const TDesC8& aAdditionalInfoTlv)
	{
	RBuf8 data;
	CleanupClosePushL(data);
	
	RBuf8 terminalRsp;
	CleanupClosePushL(terminalRsp);
	terminalRsp.CreateL(KTlvMaxSize);
	
	TTlv tlv;
	tlv.AddTag(KTlvCommandDetailsTag);
	tlv.AddByte(aPCmdNumber);//ETLV_CommandNumber
	tlv.AddByte(KProvideLocalInformation); //ETLV_TypeOfCommand
	tlv.AddByte(aCommandQualifier); //ETLV_CommandQualifier

	tlv.AddTag(KTlvDeviceIdentityTag);
	tlv.AddByte(KMe);
	tlv.AddByte(KSim);
		
	tlv.AddTag(KTlvResultTag);
	tlv.AddByte(aGeneralResult);
	TUint genResAddInfoLength = aGeneralResultAddInfo.Length();
	for (TUint i = 0; i < genResAddInfoLength; i++)
		{
		tlv.AddByte( static_cast< TUint8 >(aGeneralResultAddInfo[i]) );
		}

	terminalRsp.Zero();
	terminalRsp.Append(tlv.GetDataWithoutTopLevelTag());
	terminalRsp.Append(aAdditionalInfoTlv);	
	
	TDesC8* terminalRspPtr = &terminalRsp;
	
	TMockLtsyData1<TDesC8*> termRespData(terminalRspPtr);
	termRespData.SerialiseL(data);
	
	iMockLTSY.ExpectL(MLtsyDispatchSatTerminalRsp::KLtsyDispatchSatTerminalRspApiId, data);
	
	CleanupStack::PopAndDestroy(&terminalRsp);
	CleanupStack::PopAndDestroy(&data);
	}

/**
* prepare data and call iSat.TerminalRsp(reqStatus, aRSatTypeOfCommand, aRspPckg);
* @param aRspPckg - data for response (RSat::TLocalInfoRspV3Pckg)
* @param aExpectedError - error, which is expected from CTSY 
*/ 
void CCSatProvideLocalInfoFU::TerminalResponseL(const TDesC8& aRspPckg, TInt aExpectedError)
	{
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::ELocalInfo, aRspPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(aExpectedError, reqStatus.Int());
	}


void CCSatProvideLocalInfoFU::SetDateTimeTimeZone ( RSat::TLocalInfoRspV3& aLocalInfoRspV3, TUint8 aTimeZone)
	{          
    TTime time;
    TLocale locale;
    TDateTime dateTime;
    // Get Universal time
    time.UniversalTime();
    // Get Universal time offset ( Time zone in seconds )
    TTimeIntervalSeconds universalTimeOffset( 
        locale.UniversalTimeOffset() );
    // Add locale's universal time offset to universal time
    // to get the local time
    time += universalTimeOffset;
    // Converts a TTime object into a TDateTime object
    dateTime = time.DateTime();
    // Get and conver year
    TUint8 temp( TSatUtility::ConvertToSemiOctet(
        dateTime.Year() ) );
    // Append year to additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );                        
    // Convert month
    // Offset from zero. So adding one.
    temp = TSatUtility::ConvertToSemiOctet( 
        dateTime.Month() + 1 );
    // Append month value to the additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Convert day to the semi-octec presentation
    // The day.Offset from zero, so add one before displaying
    // the day number.
    temp = TSatUtility::ConvertToSemiOctet( 
        dateTime.Day() + 1 );
    // Append the day value to the additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Conver current time to the semi-octec presentation.
    // First hour
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Hour() );
    // Append the hour value to the additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Minute value
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Minute() );
    // Append the minute value to the additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );
    // Seconds   
    temp = TSatUtility::ConvertToSemiOctet(
        dateTime.Second() );
    // Append the seconds value to the additional info
    aLocalInfoRspV3.iAdditionalInfo.Append( temp );
   
    // Append time zone    
    aLocalInfoRspV3.iAdditionalInfo.Append( aTimeZone );             
    // Set generall result as Success
    aLocalInfoRspV3.iGeneralResult = RSat::KSuccess;	
	}

/**
* Opens Sat with INEI of NULL-Length
*/ 
void CCSatProvideLocalInfoFU::OpenSatWithZeroImeiL()
	{
	TRequestStatus reqStatus;
	
	RBuf8 data;
	CleanupClosePushL(data);
		
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId);
		
	TBool smsMoControlActivated = KSmsMoControlEnabled;
	TMockLtsyData1<TBool> comLtsyData(smsMoControlActivated);
	comLtsyData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsControlActivated::KLtsyDispatchSatGetSmsControlActivatedApiId, KErrNone, data, KDefaultDelay);

	TUint8 pcmdCode = KPollInterval;
	TMockLtsyData1<TUint8> expLtsyData(pcmdCode);
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );

	pcmdCode = KPollingOff;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
		
	pcmdCode = KMoreTime;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
		
	pcmdCode = KTimerManagement;
	data.Close();
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data );
		
	iMockLTSY.ExpectL(MLtsyDispatchPhoneGetPhoneId::KLtsyDispatchPhoneGetPhoneIdApiId);		
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId);

	TBuf8<KMaxLengthDefaultBearerData> bearerCapabilities(_L8("TEST BEARER!"));
	TDesC8* bearerCapabilitiesPtr = &bearerCapabilities;
	TMockLtsyData1<TDesC8*> bearerCapabilitiesData(bearerCapabilitiesPtr);
	data.Close();
	bearerCapabilitiesData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetDefaultBearerCapability::KLtsyDispatchSatGetDefaultBearerCapabilityApiId, KErrNone, data, KDefaultDelay);
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId);

	TBool ussdSupported = KUssdEnabled;
	TMockLtsyData1<TBool> ussdSupportedData(ussdSupported);
	data.Close();
	ussdSupportedData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetUssdControlSupported::KLtsyDispatchSatGetUssdControlSupportedApiId, KErrNone, data, KDefaultDelay);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatUssdControlSupportedChangeIndId, KErrNone, data, KDefaultDelay); //do a future indicator completion as well as this could happen at some point
	
	iMockLTSY.ExpectL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId);
	iMockLTSY.CompleteL(MLtsyDispatchSatReady::KLtsyDispatchSatReadyApiId,KErrNone, KDefaultDelay);
		
	iMockLTSY.ExpectL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId);

	TBool smsPpDdlSupported = KSmsPpDownloadEnabled;
	TMockLtsyData1<TBool> smsPpData(smsPpDdlSupported);
	data.Close();
	smsPpData.SerialiseL(data);
	iMockLTSY.CompleteL(MLtsyDispatchSatGetSmsPpDownloadSupported::KLtsyDispatchSatGetSmsPpDownloadSupportedApiId, KErrNone, data, KDefaultDelay);
		
	User::LeaveIfError(iSat.Open(iPhone));

	iMockLTSY.NotifyTerminated(reqStatus); 	
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int()); 	
	
	CleanupStack::PopAndDestroy(1); //data
	}


