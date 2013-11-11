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
 @file  The TEFUnit test suite for GetInkey in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <ctsy/ltsy/sat_defs.h>


#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/mockltsyindicatorids.h>
#include <test/tmockltsydata.h>

#include "ccsatgetinkeyfu.h"

static const TUint8 KCommandNumber = 1;
static const TUint8 KCommandType   = KGetInkey;
static const TUint8 KSourceDevice = KSim;
static const TUint8 KDestinationDevice = KMe;

static const TInt KDelay = 10;

_LIT8(KEnterPlus, "Enter \"+\"");
_LIT8(KPlus, "+");
_LIT8(KNOCAUSE, "\x00");

CCSatGetInkeyFU::TData::TIcon::TIcon() 
	{ 
	Clear();
	}

void CCSatGetInkeyFU::TData::TIcon::Clear()
	{
	iIfUse = EFalse; 
	iQualifier = 0;
	iIdentifier = 0;
	}

CTestSuite* CCSatGetInkeyFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatGetInkeyFU, TestNotifyGetInkeyPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetInkeyFU, TestNotifyGetInkeyPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetInkeyFU, TestNotifyGetInkeyPCmd0004L);
	
	ADD_TEST_STEP_ISO_CPP(CCSatGetInkeyFU, TestNotifyGetInkeyPCmd0001bL);

	END_SUITE;
	}

void CCSatGetInkeyFU::PrepareTlvData ( TTlv& aTlvData )
	{	
	aTlvData.Begin(KBerTlvProactiveSimCommandTag);
	
	aTlvData.AddTag(KTlvCommandDetailsTag);         // Command details
	aTlvData.AddByte(KCommandNumber);                // Command number     
	aTlvData.AddByte(KCommandType);                  // Command type     
	aTlvData.AddByte(iData.iCommandQualifier);      // Command qualifier  
	
	aTlvData.AddTag(KTlvDeviceIdentityTag);         // Device identities          
	aTlvData.AddByte(KSourceDevice);          // Source device     
	aTlvData.AddByte(KDestinationDevice);     // Destination device     

	if (iData.iText.Length() > 0 )
		{
		aTlvData.AddTag(KTlvTextStringTag);
		aTlvData.AddByte(iData.iCodingScheme); 
		aTlvData.AddData(iData.iText); 
		}

	if ( iData.iIcon.iIfUse )
		{
		aTlvData.AddTag(KTlvIconIdentifierTag);
		aTlvData.AddByte(iData.iIcon.iQualifier);
		aTlvData.AddByte(iData.iIcon.iIdentifier);
		}
	}

void CCSatGetInkeyFU::PrepareExpectDataL ()
	{
	RBuf8 data;
	CleanupClosePushL(data);	
	
	TUint8 commandType = KGetInkey;
	TMockLtsyData1<TUint8> expLtsyData(commandType);
	expLtsyData.SerialiseL(data);
	iMockLTSY.ExpectL(MLtsyDispatchSatPCmdNotification::KLtsyDispatchSatPCmdNotificationApiId, data);

	CleanupStack::PopAndDestroy(1); // data
	}

void CCSatGetInkeyFU::PrepareCompleteDataL ( TInt aResultCode )
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

void CCSatGetInkeyFU::ProactiveCommandCallL( RSat::TGetInkeyV2& aGetInkeyData,
		                                     TInt aCompleteErrorCode,
		                                     TInt aResultCode )
	{
	TRequestStatus requestStatus;
	
	PrepareExpectDataL ();
	
	TPckg< RSat::TGetInkeyV2 > setpPck(aGetInkeyData);
	iSat.NotifyGetInkeyPCmd(requestStatus, setpPck);
		
	PrepareCompleteDataL ( aCompleteErrorCode );
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aResultCode, requestStatus.Int());
	}

void CCSatGetInkeyFU::TerminalResponseCallL(
		const RSat& aSat,
		RSat::TPCmdResult aGeneralResult,
		TUint aInfoType,
		const TDesC8& aText,
		TUint8 aCodingScheme,
		TInt aCodeError,
		RSat::TGetInkeyRspFormat aRspFormat)
	{
	RSat::TGetInkeyRspV2 getInkeyRspV2;
	RSat::TGetInkeyRspV2Pckg getInkeyRspV2Pckg(getInkeyRspV2);
	TTlv getInpRspTlv;
	TRequestStatus requestStatus;

	getInkeyRspV2.iGeneralResult = aGeneralResult;
	getInkeyRspV2.iRspFormat = aRspFormat;
	getInkeyRspV2.SetPCmdNumber(KCommandNumber);
	getInkeyRspV2.iInfoType = aInfoType;

	TInt length = aText.Length();
	if (length > 0)
		{
		for (TInt i = 0; i < length; i+=2)
			{
			TUint16 element = aText[i];
			if ( (i+1) < length )
				{
				element = ( element<<8 ) | aText[i+1];
				}
			getInkeyRspV2.iAdditionalInfo.Append( element );
			}

		getInpRspTlv.AddTag(KTlvTextStringTag);
		getInpRspTlv.AddByte(aCodingScheme);
		getInpRspTlv.AddData(aText);
		}

	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			iData.iCommandQualifier,
			KNullDesC16,
			getInkeyRspV2.iGeneralResult,
			getInpRspTlv.GetDataWithoutTopLevelTag());

	aSat.TerminalRsp(requestStatus, RSat::EGetInkey, getInkeyRspV2Pckg);
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(aCodeError, requestStatus.Int());
	}

void CCSatGetInkeyFU::DataInitialization(
		const TData::TIcon& aIcon,
		const TDesC8& aText,
		TUint8 aCommandQualifier,
		TUint8 aCodingScheme)
	{
	iData.iCommandQualifier = aCommandQualifier;
	iData.iCodingScheme  = aCodingScheme;
	iData.iText.Copy(aText);
	iData.iIcon = aIcon;
	}

void CCSatGetInkeyFU::DataCompareL( const RSat::TGetInkeyV2& aGetInkeyData )
{
	RSat::TGetInkeyRspFormat rspFormat;
	switch( iData.iCommandQualifier )
		{
		case 0x0:
			rspFormat = RSat::EDigitOnly;
			break;
		case 0x01:
			rspFormat = RSat::ECharSmsDefaultAlphabet;
			break;
		case 0x03:
			rspFormat = RSat::ECharUcs2Alphabet;
			break;
		case 0x04:
			rspFormat = RSat::EYesNo;
			break;
		default:
			rspFormat = RSat::EGetInkeyRspFormatNotSet;
		}

	RSat::TIconQualifier iconQualifier;
	if ( !iData.iIcon.iIfUse )
		{
        iconQualifier = RSat::ENoIconId;
		}
	else
		{
	    if ( iData.iIcon.iQualifier )
	        {
	        iconQualifier = RSat::ENotSelfExplanatory;
	        }
	    else
	        {
	        iconQualifier = RSat::ESelfExplanatory;
	        }
		}

	ASSERT_EQUALS( rspFormat, aGetInkeyData.iRspFormat );
	
	ASSERT_EQUALS( iconQualifier,  aGetInkeyData.iIconId.iQualifier  );
	ASSERT_EQUALS( iData.iIcon.iIdentifier, aGetInkeyData.iIconId.iIdentifier );

	RSat::TGetInkeyText text;
	if ( iData.iCodingScheme == K8BitDCS )
		{
		text.Copy( iData.iText );
		}
	else if ( iData.iCodingScheme == KUCS2DCS )
		{
		TSatUtility::Copy8to16LE( iData.iText, text );
		}
	else if ( iData.iCodingScheme == K7BitDefaultSmsDCS )
		{
		TBuf8<RSat::KGetInkeyTextMaxSize> string;
		TSatUtility::Packed7to8Unpacked( iData.iText, string );

		TInt maxLength = text.MaxLength();
		if ( maxLength < string.Length() )
			{
			string.SetLength( maxLength );
			}
		// Convert to unicode format
		TSatUtility::Convert7BitToUnicode16( string, text );
		}

	ASSERT_EQUALS( 0, text.Compare( aGetInkeyData.iText ) );

}

//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-GUI-NGIPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyGetInkeyPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetInkeyPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInkeyFU::TestNotifyGetInkeyPCmd0001L()
	{

	static const RSat::TPCmdResult KArrayGR[] = { 
			RSat::KErrorRequiredValuesMissing,
			RSat::KHelpRequestedByUser
			};

	static const TUint KArrayInfoType[] = { 
			RSat::KMeProblem,
			RSat::KTextString 
			};

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus requestStatus;

	RSat::TGetInkeyV2 getInkeyData;
	TData::TIcon icon;

	DataInitialization(icon);

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyGetInkeyPCmd 
 	//-------------------------------------------------------------------------

	// test C - implemented in 0001b function

 	//-------------------------------------------------------------------------
	// TEST E: Coverage test(s) for RSat::NotifyGetInkeyPCmd
 	//-------------------------------------------------------------------------

	iMockLTSY.NotifyTerminated(requestStatus);
	
	PrepareCompleteDataL ();

	RSat::TAdditionalInfo additionalInfo;
	additionalInfo.Append(KNoCause);

	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			iData.iCommandQualifier,
			additionalInfo,
			RSat::KMeUnableToProcessCmd,
			KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::CompleteNotifyL coverage 
	// in case of "if ( KHelpAvailabilityMask & cmdQualifier )"
	// in CSatNotifyGetInkey::CompleteNotifyL()
 	//-------------------------------------------------------------------------

	DataInitialization(
			icon,
			KEnterPlus,
			KHelpAvailabilityMask);

	ProactiveCommandCallL(getInkeyData);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::CompleteNotifyL coverage 
	// in case of "if ( KImmediateDigitResponse & cmdQualifier )"
	// in CSatNotifyGetInkey::CompleteNotifyL()
 	//-------------------------------------------------------------------------

	DataInitialization(
			icon,
			KEnterPlus,
			KImmediateDigitResponse);

	ProactiveCommandCallL(getInkeyData);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::CompleteNotifyL coverage 
	// in case of "if ( KImmediateDigitResponse & cmdQualifier )"
	// in CSatNotifyGetInkey::CreateTerminalRespL()
 	//-------------------------------------------------------------------------

	TInt size = sizeof(KArrayGR)/sizeof(KArrayGR[0]);

	for ( TInt i = 0; i < size; i++ )
		{
		DataInitialization(
				icon,
				KEnterPlus);

		ProactiveCommandCallL(getInkeyData);
		TerminalResponseCallL(iSat, KArrayGR[i]);
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::CompleteNotifyL coverage 
	// in case of "if ( KErrNone == returnValue )"
 	//-------------------------------------------------------------------------

	RSat::TGetInkeyRspV2 getInkeyRspV2;
	RSat::TGetInkeyRspV2Pckg getInkeyRspV2Pckg(getInkeyRspV2);

	getInkeyRspV2.iGeneralResult = RSat::KErrorRequiredValuesMissing;
	getInkeyRspV2.iRspFormat = RSat::EGetInkeyRspFormatNotSet;
	getInkeyRspV2.SetPCmdNumber(KCommandNumber);
	getInkeyRspV2.iInfoType = RSat::KNoAdditionalInfo;
	getInkeyRspV2.iAdditionalInfo.Copy( KNOCAUSE );

	DataInitialization(
			icon,
			KNullDesC8);

	PrepareExpectDataL ();
	TPckg< RSat::TGetInkeyV2 > setpPck(getInkeyData);
	iSat.NotifyGetInkeyPCmd(requestStatus, setpPck);
		
	PrepareCompleteDataL ( KErrNone );

	PrepareTerminalResponseMockDataL(
			KCommandNumber,
			KCommandType,
			iData.iCommandQualifier,
			KNullDesC16,
			getInkeyRspV2.iGeneralResult,
			KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::TerminalResponseL coverage for 
	// RSat::TGetInkeyRspV2::iInfotype
 	//-------------------------------------------------------------------------

	RSat::TPCmdResult generalResult = RSat::KSuccess;
	TUint infoType = RSat::KSatNetworkErrorInfo; // bad infoType

	TerminalResponseCallL(
			iSat,
			generalResult, 
			infoType, 
			KNOCAUSE(),
			K8BitDCS,
			KErrCorrupt);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::TerminalResponseL coverage for 
	// RSat::TGetInkeyRspV2::IRspFormat
 	//-------------------------------------------------------------------------

	RSat::TGetInkeyRspFormat rspFormat = RSat::EGetInkeyRspFormatNotSet;
	infoType = RSat::KNoAdditionalInfo;

	TerminalResponseCallL(
			iSat,
			generalResult, 
			infoType, 
			KNOCAUSE(),
			K8BitDCS,
			KErrCorrupt,
			rspFormat);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// to increase CSatNotifyGetInkey::TerminalResponseL coverage for 
	// Rsat::TGetInkeyRspV2::iAdditionalInfo
 	//-------------------------------------------------------------------------

	getInkeyRspV2.iAdditionalInfo.Zero();
	size = sizeof(KArrayInfoType)/sizeof(KArrayInfoType[0]);
	for ( TInt i = 0; i < size; i++ )
		{
		TTlv tlv;
		tlv.AddTag(KTlvTextStringTag);
		tlv.AddByte(K8BitDCS);
		tlv.AddData(KNOCAUSE());

		getInkeyRspV2.iGeneralResult = RSat::KPartialComprehension;
		getInkeyRspV2.iRspFormat = RSat::EDigitOnly;
		getInkeyRspV2.SetPCmdNumber(KCommandNumber);
		getInkeyRspV2.iInfoType = KArrayInfoType[i];

		PrepareTerminalResponseMockDataL(
				KCommandNumber,
				KCommandType,
				iData.iCommandQualifier,
				KNullDesC16,
				getInkeyRspV2.iGeneralResult,
				tlv.GetDataWithoutTopLevelTag());

		iSat.TerminalRsp(requestStatus, RSat::EGetInkey, getInkeyRspV2Pckg);
		User::WaitForRequest(requestStatus);
		ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
		AssertMockLtsyStatusL();
		}

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->CTSY
 	//-------------------------------------------------------------------------

	ProactiveCommandCallL( getInkeyData, KErrUnknown, KErrUnknown);

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(1); // this

	}


/**
@SYMTestCaseID BA-CSAT-GUI-NGIPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyGetInkeyPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyGetInkeyPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInkeyFU::TestNotifyGetInkeyPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus requestStatus;

	RSat::TGetInkeyV2 getInkey;
	RSat::TGetInkeyV2Pckg getInkeyPck(getInkey);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyGetInkeyPCmd
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();

	iSat.NotifyGetInkeyPCmd(requestStatus, getInkeyPck);

	iSat.CancelAsyncRequest(ESatNotifyGetInkeyPCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(1); // this

	}

/**
@SYMTestCaseID BA-CSAT-GUI-NGIPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyGetInkeyPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyGetInkeyPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInkeyFU::TestNotifyGetInkeyPCmd0004L()
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
	ret = phone2.Open(telServer2, KMmTsyPhoneName);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(phone2);
	
	RSat sat2;
	ret = sat2.Open(iPhone);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	TRequestStatus requestStatus1;
	TRequestStatus requestStatus2;

	TData::TIcon icon;
	DataInitialization(
			icon,
			KEnterPlus);
	
	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifyGetInkeyPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	PrepareExpectDataL();
	
	RSat::TGetInkeyV2 getInkey1;
	RSat::TGetInkeyV2Pckg getInkey1Pck1(getInkey1);
	iSat.NotifyGetInkeyPCmd(requestStatus1, getInkey1Pck1);

	RSat::TGetInkeyV2 getInkey2;
	RSat::TGetInkeyV2Pckg getInkey1Pck2(getInkey2);
	sat2.NotifyGetInkeyPCmd(requestStatus2, getInkey1Pck2);

	PrepareCompleteDataL();

	User::WaitForRequest(requestStatus1);
	User::WaitForRequest(requestStatus2);

	ASSERT_EQUALS(KErrNone, requestStatus1.Int());
	//probably «KErrServerBusy?is more appropriate result here
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	RSat::TPCmdResult generalResult = RSat::KSuccess;
	TUint infoType = RSat::KTextString;

	TerminalResponseCallL(
			iSat,
			generalResult, 
			infoType, 
			KNOCAUSE() );

	TerminalResponseCallL(
			sat2,
			generalResult, 
			infoType, 
			KNOCAUSE() );

	DataCompareL( getInkey1 );
	DataCompareL( getInkey2 );

	AssertMockLtsyStatusL();
	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}

/*
@SYMTestCaseID BA-CSAT-GUI-NGIPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyGetInkeyPCmd ( Expected Sequence 1 See ETSI TS 102 384 ) 
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetInkeyPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInkeyFU::TestNotifyGetInkeyPCmd0001bL()
	{
	// "Enter "0"" string in K7BitDefaultSmsDCS coding scheme
	_LIT8( KStringPC1, "\x45\x37\xBD\x2C\x07\x89\x60\x22" );
	// string in KUCS2DCS coding scheme 
	_LIT8( KStringPC2, "\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\
x04\x22\x04\x15" );
	// string in KUCS2DCS coding scheme 
	_LIT8( KStringTR1, "\x04\x14" ); // char in KUCS2DCS coding scheme
	_LIT8( KGOBACKWARDS, "<GO-BACKWARDS>" );
	_LIT8( KABORT, "<ABORT>" );
	_LIT8( KEnterq, "Enter \"q\"" );
	_LIT8( KLongString1, "Enter \"x\". This command instructs the ME to display text, and to \
expect the user to enter a single character. Any response entered by the user shall be passed t" );
	_LIT8( KTIMEOUT, "<TIME-OUT>" );
	_LIT8( KEnter, "Enter" );
	_LIT8( KEnterYES, "Enter YES" );
	_LIT8( KEnterNO, "Enter NO" );
	_LIT8( KNOICON, "<NO-ICON>" );
	_LIT8( KBASICICON, "<BASIC-ICON>" );
	_LIT8( KCOLOURICON, "<COLOUR-ICON>" );
	_LIT8( KNULL, "0" );
	_LIT8( Kq, "q" );
	_LIT8( Kx, "x" );
	_LIT8( KHexNULL, "\x00" );
	_LIT8( KHexOne, "\x01" );
	_LIT8( KLargeString, "\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\
x04\x22\x04\x15\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\
x04\x22\x04\x15\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\
x04\x22\x04\x15\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19\
x04\x22\x04\x15\x04\x17\x04\x14\x04\x20\x04\x10\x04\x12\x04\x21\x04\x22\x04\x12\x04\x23\x04\x19" );

	const TPtrC8 KTextPCArray[] =
	{
		KEnterPlus(),
		KStringPC1(),
		KGOBACKWARDS(),
		KABORT(),
		KEnterq(),
		KLongString1(),
		KTIMEOUT(),
		KStringPC2(),
		KLargeString(),
		KEnter(),
		KEnterYES(),
		KEnterNO(),
		KNOICON(),
		KBASICICON(),
		KNOICON(),
		KCOLOURICON(),
		KEnterPlus()
	};
	
	const TPtrC8 KTextTRArray[] =
	{
		KPlus(),
		KNULL(),
		KNullDesC8(),
		KNullDesC8(),
		Kq(),
		Kx(),
		KNullDesC8(),
		KPlus(),
		KPlus(),
		KStringTR1(),
		KHexOne(),
		KHexNULL(),
		KPlus(),
		KPlus(),
		KPlus(),
		KPlus(),
		KNullDesC8()
	};

	const TUint8 KSizeArray = sizeof(KTextPCArray)/sizeof(KTextPCArray[0]);

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	//
	// See ETSI TS 102 384 V6.5.0 (2007-02) in subclause 27.22.4.2.1 
	// Expected Sequence 1.1 - 7.1
	//

	for (TUint8 i = 0; i < KSizeArray; i++)
		{
		TUint8 commandQualifier = 0; // digits (0-9, *, # and +) only, no help information available
		TUint8 codingSchemePC = K8BitDCS;
		TUint8 codingSchemeTR = K8BitDCS;
		RSat::TPCmdResult generalResult = RSat::KSuccess;
		TUint infoType = RSat::KTextString;
		TData::TIcon icon;
		TBool iconNotDisplayed = EFalse;

		switch( i )
			{
			case 1:
				{
				// in case of Expected Sequence 1.2
				codingSchemePC = K7BitDefaultSmsDCS;
				break;
				}
			case 2:
				{
				// in case of Expected Sequence 1.3
				generalResult = RSat::KBackwardModeRequestedByUser;
				infoType = RSat::KNoAdditionalInfo;
				break;
				}
			case 3:
				{
				// in case of Expected Sequence 1.4
				generalResult = RSat::KPSessionTerminatedByUser;
				infoType = RSat::KNoAdditionalInfo;
				break;
				}
			case 4:
				{
				// in case of Expected Sequence 1.5
				commandQualifier = 0x01; // SMS default alphabet, no help information available
				break;
				}
			case 5:
				{
				// in case of Expected Sequence 1.6
				commandQualifier = 0x01; // SMS default alphabet, no help information available
				break;
				}
			case 6:
				{
				// in case of Expected Sequence 2.1
				infoType = RSat::KNoAdditionalInfo;
				generalResult = RSat::KNoResponseFromUser;
				break;
				}
			case 7:
				{
				// in case of Expected Sequence 3.1
				codingSchemePC = KUCS2DCS;
				break;
				}
			case 8:
				{
				// in case of Expected Sequence 3.2
				codingSchemePC = KUCS2DCS;
				break;
				}
			case 9:
				{
				// in case of Expected Sequence 4.1
				codingSchemeTR = KUCS2DCS;
				commandQualifier = 0x03; // characters from UCS2 alphabet, no help information available
				break;
				}
			case 10:
				{
				// in case of Expected Sequence 5.1
				commandQualifier = 0x04; // "Yes/No" Response, no help information available
				break;
				}
			case 11:
				{
				// in case of Expected Sequence 5.2
				commandQualifier = 0x04; // "Yes/No" Response, no help information available
				break;
				}
			case 12:
				{
				// in case of Expected Sequence 6.1A, 6.1B
				icon.iIfUse = ETrue;
				icon.iIdentifier = 0;
				icon.iQualifier  = 0x01;
				iconNotDisplayed = ETrue;
				break;
				}
			case 13:
				{
				// in case of Expected Sequence 6.2A, 6.2B
				icon.iIfUse = ETrue;
				icon.iIdentifier = 0x01;
				icon.iQualifier  = 0x01;
				iconNotDisplayed = ETrue;
				break;
				}
			case 14:
				{
				// in case of Expected Sequence 6.3A, 6.3B
				icon.iIfUse = ETrue;
				icon.iIdentifier = 0x00;
				icon.iQualifier  = 0x02;
				iconNotDisplayed = ETrue;
				break;
				}
			case 15:
				{
				// in case of Expected Sequence 6.4A, 6.4B
				icon.iIfUse = ETrue;
				icon.iIdentifier = 0x01;
				icon.iQualifier  = 0x02;
				iconNotDisplayed = ETrue;
				break;
				}
			case 16:
				{
				// in case of Expected Sequence 7.1
				generalResult = RSat::KHelpRequestedByUser;
				infoType = RSat::KNoAdditionalInfo;
				break;
				}
			default:
				// in case of Expected Sequence 1.1
				break;
			}

		TInt iteration_number = 1;
		if ( iconNotDisplayed )
			{
			iteration_number = 2;
			}

		for (TInt j = 0; j < iteration_number; j++)
			{
			RSat::TGetInkeyV2 getInkeyData;

			DataInitialization(
					icon,
					KTextPCArray[i],
					commandQualifier,
					codingSchemePC);
			ProactiveCommandCallL(getInkeyData);

			TerminalResponseCallL(
					iSat,
					generalResult, 
					infoType, 
					KTextTRArray[i], 
					codingSchemeTR);
			AssertMockLtsyStatusL();
		
			DataCompareL( getInkeyData );

			if ( iconNotDisplayed )
				{
				generalResult = RSat::KSuccessRequestedIconNotDisplayed;
				}
			}
		}

	CleanupStack::PopAndDestroy(this); // this
	}

