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
 @file The TEFUnit test suite for LaunchBrowser in the SAT.
*/

#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>

#include "ccsatlaunchbrowserfu.h"

static const TUint8 KCommandNumber = 1;
static const TUint8 KCommandType   = KLaunchBrowser;
static const TUint8 KSourceDevice = KSim;
static const TUint8 KDestinationDevice = KMe;
static const TUint8 KCodingScheme = K8BitDCS;

static const TInt KDelay = 10;

_LIT8(KURL, "www.symbian.ru");
_LIT8(KBearerList, "\x01\x03\x01\x03");
_LIT8(KText, "Gateway/Proxy");
_LIT8(KAlphaId, "phase");
_LIT8(KNULL, "\x00");
_LIT8(KFileRef, "1234567890");

CTestSuite* CCSatLaunchBrowserFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatLaunchBrowserFU, TestNotifyLaunchBrowserPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatLaunchBrowserFU, TestNotifyLaunchBrowserPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatLaunchBrowserFU, TestNotifyLaunchBrowserPCmd0004L);

	END_SUITE;
	}

void CCSatLaunchBrowserFU::PrepareTlvData ( TTlv& aTlvData )
	{	
	aTlvData.Begin(KBerTlvProactiveSimCommandTag);

	aTlvData.AddTag(KTlvCommandDetailsTag);         // Command details
	aTlvData.AddByte(KCommandNumber);               // Command number
	aTlvData.AddByte(KCommandType);                 // Command type
	aTlvData.AddByte(iData.iCommandQualifier);      // Command qualifier

	aTlvData.AddTag(KTlvDeviceIdentityTag);         // Device identities
	aTlvData.AddByte(KSourceDevice);                // Source device
	aTlvData.AddByte(KDestinationDevice);           // Destination device

	if ( iData.iUrl.Length() > 0 )
		{
		aTlvData.AddTag(KTlvUrlTag);
		aTlvData.AddData(iData.iUrl);
		}

	if ( iData.iBearerList.Length() > 0 )
		{
		aTlvData.AddTag(KTlvBearerTag);
		aTlvData.AddData(iData.iBearerList);
		}

	if ( iData.iText.Length() > 0 )
		{
		aTlvData.AddTag(KTlvTextStringTag);
		aTlvData.AddByte(KCodingScheme);
		aTlvData.AddData(iData.iText);
		}

	if ( iData.iAlphaId.Length() > 0 || iData.iIfAlphaId )
		{
		aTlvData.AddTag(KTlvAlphaIdentifierTag);
		aTlvData.AddData(iData.iAlphaId);
		}
	
	if ( iData.iIfUsedBrowserId )
		{
		aTlvData.AddTag(KTlvBrowserIdentityTag);
		aTlvData.AddByte(iData.iBrowserId);
		}

	if ( iData.iFileRef.Length() > 0 )
		{
		aTlvData.AddTag(KTlvProvisioningReferenceFileTag);
		aTlvData.AddData(iData.iFileRef);
		aTlvData.AddTag(KTwoByteLengthCoding);
		}
	}

void CCSatLaunchBrowserFU::PrepareExpectDataL ()
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TUint8 commandType = KLaunchBrowser;
	TMockLtsyData1<TUint8> expLtsyData(commandType);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatLaunchBrowserFU::PrepareCompleteDataL ( TInt aResultCode )
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TTlv tlv;
	PrepareTlvData( tlv );
	
	TPtrC8 tlvDsc = tlv.End();
	TDesC8* tlvDscPtr = &tlvDsc;
	TMockLtsyData1<TDesC8*> tlvDscPack(tlvDscPtr);
	tlvDscPack.SerialiseL(data);
	iMockLTSY.CompleteL(KMockLtsyDispatchSatPcmdIndId, aResultCode, data, KDelay);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatLaunchBrowserFU::ProactiveCommandCallL( 
		RSat::TLaunchBrowserV2& aLaunchBrowserData,
		TInt aCompleteErrorCode,
		TInt aResultCode,
		TBool aIfCreateTerminal,
		RSat::TPCmdResult aGeneralResult,
		TUint8 aCommandQualifier,
		const TDesC& aAdditionalInfo
		)
	{
	TRequestStatus requestStatus;
	
	PrepareExpectDataL ();

	RSat::TLaunchBrowserV2Pckg setpPck(aLaunchBrowserData);
	iSat.NotifyLaunchBrowserPCmd(requestStatus, setpPck);
		
	PrepareCompleteDataL ( aCompleteErrorCode );

	if ( aIfCreateTerminal )
		{
		PrepareTerminalResponseMockDataL(
				KCommandNumber,
				KCommandType,
				aCommandQualifier,
				aAdditionalInfo,
				aGeneralResult,
				KNullDesC8);
		}

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aResultCode, requestStatus.Int());
	AssertMockLtsyStatusL();
	}

void CCSatLaunchBrowserFU::DataInitialization(
		TUint8 aCommandQualifier,
		const TDesC8& aUrl,
		const TDesC8& aBearerList,
		const TDesC8& aText,
		const TDesC8& aAlphaId,
		const TDesC8& aFileRef,
		TBool aIfUsedBrowserId,
		TUint aBrowserId,
		TBool aIfAlphaId
		)
	{
	iData.iUrl.Copy(aUrl);
	iData.iBearerList.Copy(aBearerList);
	iData.iText.Copy(aText);
	iData.iAlphaId.Copy(aAlphaId);
	iData.iFileRef.Copy(aFileRef);
	iData.iCommandQualifier = aCommandQualifier;
	iData.iBrowserId = aBrowserId;
	iData.iIfUsedBrowserId = aIfUsedBrowserId;
	iData.iIfAlphaId = aIfAlphaId;
	}

void CCSatLaunchBrowserFU::TerminalResponseCallL(
		const RSat& aSat,
		RSat::TPCmdResult aGeneralResult,
		const TDesC& aAdditionalInfo,
		TInt aResultCode,
		TUint aInfoType
		)
	{
	RSat::TLaunchBrowserRspV2 launchBrowser;
	RSat::TLaunchBrowserRspV2Pckg launchBrowserPckg(launchBrowser);
	TRequestStatus requestStatus;
	
	launchBrowser.iGeneralResult = aGeneralResult;
	launchBrowser.SetPCmdNumber(KCommandNumber);
	launchBrowser.iInfoType = aInfoType;
	
	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			iData.iCommandQualifier,
			aAdditionalInfo,
			launchBrowser.iGeneralResult,
			KNullDesC8);

	aSat.TerminalRsp(requestStatus, RSat::ELaunchBrowser, launchBrowserPckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aResultCode, requestStatus.Int());
	}

void CCSatLaunchBrowserFU::DataCompareL( const RSat::TLaunchBrowserV2& aLaunchBrowser )
	{
	
	RSat::TBrowserSelection browserSel;
	switch( iData.iCommandQualifier )
		{
		case KLaunchBrowserIfNotYetLaunched:
			browserSel = RSat::ELaunchBrowserIfNotAlreadyLaunched;
			break;
		case KUseExistingBrowser:
			browserSel = RSat::EUseExistingBrowser;
			break;
		case KCloseAndLaunchNewBrowser:
			browserSel = RSat::ECloseExistingLaunchNewBrowserSession;
			break;
		case KLaunchBrowserCmdQualifierNotUsed:
		case KLaunchBrowserCmdQualifierReserved:
		default:
			browserSel = RSat::EBrowserSelectionNotSet;
		}

	ASSERT_EQUALS( browserSel, aLaunchBrowser.iBrowserSel );

	RSat::TAlphaIdBuf alphaId;
	alphaId.Copy(iData.iAlphaId);
	ASSERT_EQUALS( 0, alphaId.Compare( aLaunchBrowser.iAlphaId.iAlphaId ) );

	RSat::TTextString text;
	text.Copy( iData.iText );
	ASSERT_EQUALS( 0, text.Compare( aLaunchBrowser.iText ) );

	ASSERT_EQUALS( 0, iData.iBearerList.Compare( aLaunchBrowser.iBearerList ) );
	ASSERT_EQUALS( 0, iData.iUrl.Compare( aLaunchBrowser.iUrl ) );
	
	}

void CCSatLaunchBrowserFU::ClearData( RSat::TLaunchBrowserV2& aLaunchBrowser )
	{
	aLaunchBrowser.ResetFileRef();
	aLaunchBrowser.iBrowserSel = RSat::EBrowserSelectionNotSet;
	aLaunchBrowser.iBrowserId  = RSat::EBrowserIdNotSet;
	aLaunchBrowser.iUrl.Zero();
	aLaunchBrowser.iBearerList.Zero();
	aLaunchBrowser.iText.Zero();
	aLaunchBrowser.iAlphaId.iAlphaId.Zero();
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-LB-SNLBPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyLaunchBrowserPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyLaunchBrowserPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLaunchBrowserFU::TestNotifyLaunchBrowserPCmd0001L()
	{

	static const TUint8 KCmdQualifierArray[] = { 
			KLaunchBrowserCmdQualifierNotUsed, 
			KUseExistingBrowser, 
			KCloseAndLaunchNewBrowser,
			KLaunchBrowserCmdQualifierReserved, 
			KLaunchBrowserCmdQualifierReserved + 1 };

	static const RSat::TPCmdResult KGenResultArray[] = {
			RSat::KPSessionTerminatedByUser,
			RSat::KBackwardModeRequestedByUser,
			RSat::KNoResponseFromUser,
			RSat::KMeUnableToProcessCmd,
			RSat::KCmdBeyondMeCapabilities,
			RSat::KCmdDataNotUnderstood,
			RSat::KLaunchBrowserError,
			RSat::KPCmdNotAcceptedByUser,
			RSat::KCmdTypeNotUnderstood,
			RSat::KMissingInformation,
			RSat::KPartialComprehension,
			RSat::KNetworkUnableToProcessCmd,
			RSat::KCmdNumberNotKnown };

	static const TUint KArrayInfoType[] = {
			RSat::KMeProblem,
			RSat::KBearerUnvailable,
			RSat::KBrowserUnavailable,
			RSat::KMeUnableToReadProvisioningData,
			RSat::KNoSpecificBrowserError,
			RSat::KServiceError };

	const TInt KSize = sizeof(KCmdQualifierArray)/sizeof(KCmdQualifierArray[0]);
	const TInt KArraySize = sizeof(KGenResultArray)/sizeof(KGenResultArray[0]);
	const TInt KArrayITSize = sizeof(KArrayInfoType)/sizeof(KArrayInfoType[0]);
	const TUint8 KCommandQualifier = KLaunchBrowserIfNotYetLaunched;

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	RSat::TLaunchBrowserV2 launchBrowser;
	TRequestStatus requestStatus;
	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KAlphaId, KFileRef);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyLaunchBrowserPCmd 
 	//-------------------------------------------------------------------------

	ProactiveCommandCallL(launchBrowser);
	TerminalResponseCallL(iSat);
	DataCompareL( launchBrowser );
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST E: Coverage test(s) for RSat::NotifyLaunchBrowserPCmd
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);
	
	PrepareCompleteDataL ();

	RSat::TAdditionalInfo additionalInfo;
	additionalInfo.Append(RSat::KNoSpecificMeProblem);

	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			KCommandQualifier,
			additionalInfo,
			RSat::KMeUnableToProcessCmd,
			KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::CompleteNotifyL coverage 
	// in case of "switch ( cmdQualifier )"
 	//-------------------------------------------------------------------------

	TBool prepareMockData = EFalse;
	TUint8 element = 0;

	for ( TInt i = 0; i < KSize; i++ )
		{
		element = KCmdQualifierArray[i];
		prepareMockData = 
			element == KLaunchBrowserCmdQualifierNotUsed || 
			element == KLaunchBrowserCmdQualifierReserved;

		DataInitialization(element, KURL, KBearerList, KText, KAlphaId, KFileRef);

		ClearData(launchBrowser);
		if ( prepareMockData )
			{
			ProactiveCommandCallL(
					launchBrowser, 
					KErrNone, 
					KErrCorrupt, 
					ETrue, 
					RSat::KCmdTypeNotUnderstood,
					element);
			}
		else
			{
			ProactiveCommandCallL(launchBrowser); 
			}

		if ( !prepareMockData )
			{
			TerminalResponseCallL(iSat);
			DataCompareL( launchBrowser );
			}
		
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::TerminalResponseL coverage 
	// in case of general result value isn't valid
 	//-------------------------------------------------------------------------

	RSat::TPCmdResult elementResult;
	TInt resultCode = KErrNone;
	for (TInt i = 0; i < KArraySize; i++)
		{
		elementResult = KGenResultArray[i];
		additionalInfo.Zero();
		
		switch( elementResult )
			{
			case RSat::KCmdDataNotUnderstood:
			case RSat::KCmdTypeNotUnderstood:
			case RSat::KMissingInformation:
			case RSat::KPartialComprehension:
				{
				break;
				}			
			default:
				{
				additionalInfo.Append(RSat::KNoSpecificMeProblem);
				}
			}
		
		TerminalResponseCallL(iSat, elementResult, additionalInfo, resultCode);
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::TerminalResponseL coverage 
	// in case of infotype isn't valid
 	//-------------------------------------------------------------------------

	TUint elementIT;
	for (TInt i = 0; i < KArrayITSize; i++)
		{
		elementIT = KArrayInfoType[i];

		if ( RSat::KMeProblem    == elementIT ||
			 RSat::KServiceError == elementIT )
			{
			resultCode = KErrCorrupt;
			}
		else
			{
			resultCode = KErrNone;
			}

		TerminalResponseCallL(iSat, RSat::KSuccess, KNullDesC, resultCode, elementIT);
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseBrowserIdL coverage 
	// in case of "if ( 0x00 == aLaunchBrowserV2.iBearerList.Length() )"
 	//-------------------------------------------------------------------------

	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KAlphaId, KFileRef, ETrue);
	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseBrowserIdL coverage 
	//  in case of "if ( 0x00 != browserId.GetShortInfo( ETLV_BrowserIdentity ) )"
 	//-------------------------------------------------------------------------

	TUint browserId = RSat::EBrowserIdNotPresent;
	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KAlphaId, KFileRef, ETrue, browserId);
	ClearData(launchBrowser);
	ProactiveCommandCallL(
			launchBrowser, 
			KErrNone, 
			KErrCorrupt, 
			ETrue, 
			RSat::KCmdDataNotUnderstood);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseUrlL coverage 
	// in case of KTlvUrlTag isn't found.
 	//-------------------------------------------------------------------------

	DataInitialization(KCommandQualifier, KNullDesC8, KBearerList, KText, KAlphaId, KFileRef);
	ClearData(launchBrowser);
	ProactiveCommandCallL(
			launchBrowser, 
			KErrNone, 
			KErrCorrupt, 
			ETrue, 
			RSat::KErrorRequiredValuesMissing);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseBearerListL coverage 
	// in case of KTlvBearerTag isn't found.
 	//-------------------------------------------------------------------------

	DataInitialization(KCommandQualifier, KURL, KNullDesC8, KText, KAlphaId, KFileRef);
	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseBearerListL coverage 
	// in case of "if ( 0x00 == aLaunchBrowserV2.iBearerList.Length() )"
 	//-------------------------------------------------------------------------

	additionalInfo.Zero();
	additionalInfo.Append(RSat::KBearerUnvailable);

	DataInitialization(KCommandQualifier, KURL, KNULL, KText, KAlphaId, KFileRef);
	ClearData(launchBrowser);
	ProactiveCommandCallL(
			launchBrowser, 
			KErrNone, 
			KErrCorrupt, 
			ETrue, 
			RSat::KLaunchBrowserError,
			KCommandQualifier,
			additionalInfo);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::CompleteNotifyL coverage 
	// in case of KTlvTextStringTag isn't found.
 	//-------------------------------------------------------------------------

	DataInitialization(KCommandQualifier, KURL, KBearerList, KNullDesC8, KAlphaId, KFileRef);
	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::CompleteNotifyL coverage 
	// in case of KTlvAlphaIdentifierTag isn't found.
 	//-------------------------------------------------------------------------

	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KNullDesC8, KFileRef);
	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseProvisioningFileListL coverage 
	// in  case of "if ( ( RSat::KFileRefMaxSize < stringLength ) || 
    // ( 4 > stringLength ) )"
 	//-------------------------------------------------------------------------

	additionalInfo.Zero();
	additionalInfo.Append(RSat::KMeUnableToReadProvisioningData);

	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KAlphaId, KNULL);
	ClearData(launchBrowser);
	ProactiveCommandCallL(
			launchBrowser, 
			KErrNone, 
			KErrCorrupt, 
			ETrue, 
			RSat::KLaunchBrowserError,
			KCommandQualifier,
			additionalInfo);

	//-------------------------------------------------------------------------
	// to increase CSatNotifyLaunchBrowser::ParseBrowserIdL coverage 
	// in case of "if ( KErrNotFound == returnValue )"
 	//-------------------------------------------------------------------------

	DataInitialization(
			KCommandQualifier, 
			KURL, KBearerList,
			KText,
			KNullDesC8,
			KFileRef,
			EFalse,
			RSat::EBrowserIdNotSet,
			ETrue);
	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser);

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	ClearData(launchBrowser);
	ProactiveCommandCallL(launchBrowser, KErrUnknown, KErrUnknown);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(this); // this

	}


/**
@SYMTestCaseID BA-CSAT-LB-SNLBPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyLaunchBrowserPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyLaunchBrowserPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLaunchBrowserFU::TestNotifyLaunchBrowserPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus requestStatus;

	RSat::TLaunchBrowserV2 launchBrowser;
	RSat::TLaunchBrowserV2Pckg launchBrowserPck(launchBrowser);

	DataInitialization(KCommandQualifier, KURL, KBearerList, KText, KAlphaId, KFileRef);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyLaunchBrowserPCmd
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();

	iSat.NotifyLaunchBrowserPCmd(requestStatus, launchBrowserPck);

	iSat.CancelAsyncRequest(ESatNotifyLaunchBrowserPCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1); // this

	}

/**
@SYMTestCaseID BA-CSAT-LB-SNLBPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyLaunchBrowserPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyLaunchBrowserPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatLaunchBrowserFU::TestNotifyLaunchBrowserPCmd0004L()
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

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;
	TUint8 commandQualifier = KLaunchBrowserIfNotYetLaunched;

	DataInitialization(commandQualifier, KURL, KBearerList, KText, KAlphaId, KFileRef);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifyLaunchBrowserPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();
	
	RSat::TLaunchBrowserV2 launchBrowser1;
	RSat::TLaunchBrowserV2Pckg launchBrowserPck1(launchBrowser1);
	iSat.NotifyLaunchBrowserPCmd(requestStatus1, launchBrowserPck1);

	RSat::TLaunchBrowserV2 launchBrowser2;
	RSat::TLaunchBrowserV2Pckg launchBrowserPck2(launchBrowser2);
	sat2.NotifyLaunchBrowserPCmd(requestStatus2, launchBrowserPck2);

	PrepareCompleteDataL();

	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	//probably «KErrServerBusy?is more appropriate result here
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	TerminalResponseCallL(iSat);
	TerminalResponseCallL(sat2);

	DataCompareL( launchBrowser1 );
	DataCompareL( launchBrowser2 );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}


