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
 @file The TEFUnit test suite for GetInput in the SAT.
*/

#include <cstktsy/bertlv_defs.h>
#include <etel.h>
#include <etelmm.h>
#include <etelsat.h>
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/mmtsy_names.h>
#include <satcs.h>

#include <ctsy/ltsy/mltsydispatchsatinterface.h>
#include <test/tmockltsydata.h>
#include <cstktsy/ttlv.h>

#include "ccsatgetinputfu.h"


CTestSuite* CCSatGetInputFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	
	ADD_TEST_STEP_ISO_CPP(CCSatGetInputFU, TestNotifyGetInputPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetInputFU, TestNotifyGetInputPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatGetInputFU, TestNotifyGetInputPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatGetInputFU, TestNotifyGetInputPCmd0004L);

	END_SUITE;
	}


/**
Fills in a Tlv with command details and device identities for GetInput proactive command
@param aTlv Tlv to fill in
@param aPCmdNumber proactive command number
@param aCommandQualifier GetInput proactive command qualifier
*/
void GetInputProactiveCmdTlvBegin( 
		TTlv& aTlv, 
		TUint8 aPCmdNumber,
		TUint8 aCommandQualifier)
	{
	ProactiveCmdTlvBegin( aTlv, aPCmdNumber, KGetInput, aCommandQualifier, KMe );
	}


/**
Prepares a default GetInput data from the data to compare with lately in binary mode,
so that the data coincides in the fields whos default values is not specified by standards
(e.g. the rest of buffers)
@param aDefGetInpData GetInput structure to fill in the default data
@param aGetInpData GetInput structure containing the default values that is not specified by standards
*/
void PrepareDefaultGetInpData(RSat::TGetInputV1& aDefGetInpData, const RSat::TGetInputV1& aGetInpData)
	{
	Mem::Copy(&aDefGetInpData, &aGetInpData, sizeof(RSat::TGetInputV1));

	aDefGetInpData.iText.Zero();
	aDefGetInpData.iInputDisplayOption = RSat::EDisplayUserInput;
	aDefGetInpData.iRspFormat = RSat::EDigitOnlyUnpacked;
	aDefGetInpData.iRspLength.iMinRspLength = 0;
	aDefGetInpData.iRspLength.iMaxRspLength = 0;
	aDefGetInpData.iDefaultText.Zero();
	aDefGetInpData.iIconId.iQualifier = RSat::ENoIconId;
	aDefGetInpData.iHelp = RSat::ENoHelpAvailable;
	}


/**
Test support in SATTSY for successful RSat::NotifyGetInputPCmd request without terminal response generation
@param aPCmdNumber proactive command number
@param aGetInputQualifier GetInput proactive command qualifier
@param aGetInputText text for the Terminal to display in conjunction with asking the user to respond
@param aMinRspLength minimum length of the user response
@param aMaxRspLength maximum length of the user response
*/
void CCSatGetInputFU::TestGetInputSuccessfulWithoutTerminalResponseL(
		TUint8 aPCmdNumber,
		TUint8 aGetInputQualifier,
		const TDesC8& aGetInputText,
		TUint8 aMinRspLength,
		TUint8 aMaxRspLength)
	{
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);

	RSat::TGetInputV1 getInpData;
	RSat::TGetInputV1Pckg getInpDataPckg(getInpData);
	TRequestStatus requestStatus;
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	TTlv getInpTlv;
	GetInputProactiveCmdTlvBegin( getInpTlv, aPCmdNumber, aGetInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpTlv.AddData(aGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(aMinRspLength); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(aMaxRspLength); //ETLV_MaximumLengthOfResponse
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	User::WaitForRequest(requestStatus);

	// check result
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	RSat::TGetInputV1 expGetInpData;
	RSat::TGetInputV1Pckg expGetInpDataPckg(expGetInpData);

	PrepareDefaultGetInpData(expGetInpData, getInpData);
	expGetInpData.iText.Copy(aGetInputText);
	expGetInpData.iRspLength.iMinRspLength = aMinRspLength;
	expGetInpData.iRspLength.iMaxRspLength = aMaxRspLength;

	if ( KUserInputMask & aGetInputQualifier )
		{
		// User input hidden
		expGetInpData.iInputDisplayOption = RSat::EHideUserInput;
		}
	else
		{
		// User input shown
		expGetInpData.iInputDisplayOption = RSat::EDisplayUserInput;
		}

	// Character set selected
	if ( KUCS2AlphabetMask & aGetInputQualifier )
		{
		// UCS2 alphabet selected
		expGetInpData.iRspFormat = RSat::EUcs2Alphabet;
		}
	else if ( KAlphabetMask & aGetInputQualifier )
		{
		if ( KGetInputSmsPackedMask & aGetInputQualifier )
			{
			// SMS default alphabet
			expGetInpData.iRspFormat = RSat::ESmsDefaultAlphabetPacked;
			}
		else
			{
			expGetInpData.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
			}
		}
	
	else
		{
		if  ( KGetInputSmsPackedMask & aGetInputQualifier )
			{
			// Digit only
			expGetInpData.iRspFormat = RSat::EDigitOnlyPacked;
			}
		else
			{
			expGetInpData.iRspFormat = RSat::EDigitOnlyUnpacked;
			}
		}

	// Help information.
	if ( KHelpAvailabilityMask & aGetInputQualifier )
		{
		// Help information available
		expGetInpData.iHelp = RSat::EHelpAvailable;
		}
	else
		{
		// No help
		expGetInpData.iHelp = RSat::ENoHelpAvailable;
		}

	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg));

	}


/**
Test support in SATTSY for successful RSat::NotifyGetInputPCmd request with terminal response about unsuccessful command processing
@param aPCmdNumber proactive command number
@param aError error code of the proactive command processing
@param aMeProblemAddInfo additional information in case of Me problem
@param aExpResult expected result of TerminalRsp() execution
*/
void CCSatGetInputFU::TestGetInputWithUnsuccessfulTerminalResponseL(
		TUint8 aPCmdNumber,
		RSat::TPCmdResult aError,
		const TDesC& aMeProblemAddInfo,
		const TInt aExpResult)
	{

	const TUint8 KGetInputQualifier = KAlphabetMask;	// SMS default alphabet, input in unpacked format, 
														// Terminal to echo text, no help information available
	_LIT8(KGetInputText, "<GET-INPUT-TEXT>"); 
	TestGetInputSuccessfulWithoutTerminalResponseL(aPCmdNumber, KGetInputQualifier, KGetInputText, 2, 30);
	
	//generate the terminal response
	RSat::TGetInputRspV1 resp;
	RSat::TGetInputRspV1Pckg respPckg(resp);

	resp.iGeneralResult = aError;
	resp.iAdditionalInfo.Zero();
	if ( RSat::KMeUnableToProcessCmd == aError )
		{
		resp.iInfoType = RSat::KMeProblem;
		resp.iAdditionalInfo.Append(aMeProblemAddInfo);
		}
	else
		{
		resp.iInfoType = RSat::KNoAdditionalInfo;
		}
	resp.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
	resp.SetPCmdNumber(aPCmdNumber);

	GenerateTerminalResponseL(KGetInputQualifier, resp, K8BitDCS, aExpResult);
	AssertMockLtsyStatusL();

	}


/**
Handles sending a terminal response
@param aCommandQualifier get input proactive command qualifier
@param aResp terminal response data
@param aCodingScheme coding scheme of input data
@param aExpResult expected result of TerminalRsp() execution
*/
void CCSatGetInputFU::GenerateTerminalResponseL(
		TUint8 aCommandQualifier,
		const RSat::TGetInputRspV1& aResp,
		TUint8 aCodingScheme,
		const TInt aExpResult)
	{

	TPtrC genResultAddInfo(KNullDesC);
	TPtrC8 additionalInfoTlv(KNullDesC8);
	if ( RSat::KTextString == aResp.iInfoType ) 
		{
		TTlv textStringTlv;
		textStringTlv.AddTag(KTlvTextStringTag);
		textStringTlv.AddByte(aCodingScheme); //ETLV_DataCodingScheme

		TBuf8<RSat::KAdditionalInfoMaxSize> addInfo;
		addInfo.Zero();
		switch (aCodingScheme)
			{
			case KUCS2DCS:
				TSatUtility::Copy16to8LE(aResp.iAdditionalInfo, addInfo);
				break;
			case K7BitDefaultSmsDCS:
				TSatUtility::UCSToPacked7(aResp.iAdditionalInfo, addInfo);
				break;
			case K8BitDCS:
			default:
				addInfo.Copy(aResp.iAdditionalInfo);
				break;
			}
		textStringTlv.AddData(addInfo);

		additionalInfoTlv.Set( textStringTlv.GetDataWithoutTopLevelTag() );
		}
	else if ( RSat::KMeProblem == aResp.iInfoType )
		{
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}

	RSat::TGetInputRspV1Pckg respPckg(aResp);
	CCSatComponentTestBase::GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KGetInput,
			aCommandQualifier,
			RSat::EGetInput,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			additionalInfoTlv,
			aExpResult);
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-GUI-TNGIPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyGetInputPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetInputPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInputFU::TestNotifyGetInputPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyGetInputPCmd when result is not cached.
 	//-------------------------------------------------------------------------

	//
	// (GET INPUT, digits only, SMS default alphabet, Terminal to echo text, Terminal supporting 8 bit data Message)
	//

	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);

	RSat::TGetInputV1 getInpData;
	RSat::TGetInputV1Pckg getInpDataPckg(getInpData);
	TRequestStatus requestStatus;
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	TUint8 pcmdNumber = 1;

	TTlv getInpTlv;
	TUint8 getInputQualifier = 0; // digits (0-9, *, # and +) only, SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KGetInputText, "Enter 12345");
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(5); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(5); //ETLV_MaximumLengthOfResponse
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	User::WaitForRequest(requestStatus);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KGetInputText, getInpData.iText));
	ASSERT_EQUALS(RSat::EDisplayUserInput, getInpData.iInputDisplayOption);
	ASSERT_EQUALS(RSat::EDigitOnlyUnpacked, getInpData.iRspFormat);
	ASSERT_EQUALS(static_cast<TUint8>(5), getInpData.iRspLength.iMinRspLength);
	ASSERT_EQUALS(static_cast<TUint8>(5), getInpData.iRspLength.iMaxRspLength);
	ASSERT_EQUALS(0, KNullDesC().Compare(getInpData.iDefaultText));
	ASSERT_EQUALS(RSat::ENoIconId, getInpData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoHelpAvailable, getInpData.iHelp);

	// check result by package
	RSat::TGetInputV1 expGetInpData;
	RSat::TGetInputV1Pckg expGetInpDataPckg(expGetInpData);

	PrepareDefaultGetInpData(expGetInpData, getInpData);
	expGetInpData.iText.Copy(KGetInputText);
	expGetInpData.iRspLength.iMinRspLength = 5;
	expGetInpData.iRspLength.iMaxRspLength = 5;
	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg));

	//generate the terminal response
	RSat::TGetInputRspV1 resp;
	RSat::TGetInputRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	_LIT8(KUserInputText, "12345"); 
	resp.iAdditionalInfo.Copy(KUserInputText);
	resp.iRspFormat = RSat::EDigitOnlyUnpacked;
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(getInputQualifier, resp);
	AssertMockLtsyStatusL();

	//
	// (GET INPUT, digits only, SMS default alphabet, packed, Hide user input)
	//

	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	getInputQualifier = KUserInputMask|KHelpAvailabilityMask|KGetInputSmsPackedMask; // digits (0-9, *, # and +) only, SMS default alphabet, input in packed format, Hide user input, help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KGetInputText2, "Enter a digit"); 
	getInpTlv.AddData(KGetInputText2);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(1); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(1); //ETLV_MaximumLengthOfResponse
	
	getInpTlv.AddTag(KTlvDefaultTextTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KDefaultText, "1"); 
	getInpTlv.AddData(KDefaultText);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	User::WaitForRequest(requestStatus);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	PrepareDefaultGetInpData(expGetInpData, getInpData);
	expGetInpData.iText.Copy(KGetInputText2);
	expGetInpData.iRspLength.iMinRspLength = 1;
	expGetInpData.iRspLength.iMaxRspLength = 1;
	expGetInpData.iDefaultText.Copy(KDefaultText);
	expGetInpData.iHelp = RSat::EHelpAvailable;
	expGetInpData.iInputDisplayOption = RSat::EHideUserInput;
	expGetInpData.iRspFormat = RSat::EDigitOnlyPacked;
	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg));

	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	_LIT(KUserInputText2, "2"); 
	resp.iAdditionalInfo.Copy(KUserInputText2);
	resp.iRspFormat = RSat::EDigitOnlyPacked;
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(getInputQualifier, resp, K7BitDefaultSmsDCS);
	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, Terminal supporting 8 bit data Message)
	//
	
	getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	_LIT8(KGetInputText3, "Enter your account name"); 
	TestGetInputSuccessfulWithoutTerminalResponseL(pcmdNumber, getInputQualifier, KGetInputText3, 2, 30);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	_LIT8(KUserInputText3, "Roman"); 
	resp.iAdditionalInfo.Copy(KUserInputText3);
	resp.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
	resp.SetPCmdNumber(pcmdNumber);

	TTlv getInpRspTlv3;
	getInpRspTlv3.AddTag(KTlvTextStringTag);
	getInpRspTlv3.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpRspTlv3.AddData(KUserInputText3);

	GenerateTerminalResponseL(getInputQualifier, resp);
	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, packed, Terminal to echo text)
	//
	
	getInputQualifier = KAlphabetMask|KGetInputSmsPackedMask; // SMS default alphabet, input in packed format, Terminal to echo text, no help information available
	TestGetInputSuccessfulWithoutTerminalResponseL(pcmdNumber, getInputQualifier, KGetInputText3, 2, 30);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	_LIT(KUserInputText4, "Vladimir"); 
	resp.iAdditionalInfo.Copy(KUserInputText4);
	resp.iRspFormat = RSat::ESmsDefaultAlphabetPacked;
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(getInputQualifier, resp, K7BitDefaultSmsDCS);
	AssertMockLtsyStatusL();

	//
	// (GET INPUT, UCS2 alphabet, Terminal to echo text)
	//
	
	getInputQualifier = KUCS2AlphabetMask; // UCS2 alphabet, Terminal to echo text, no help information available
	TestGetInputSuccessfulWithoutTerminalResponseL(pcmdNumber, getInputQualifier, KGetInputText3, 2, 30);
	
	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	resp.iAdditionalInfo.Copy(KUserInputText4);
	resp.iRspFormat = RSat::EUcs2Alphabet;
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(getInputQualifier, resp, KUCS2DCS);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyGetInputPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	getInputQualifier = 0; // digits (0-9, *, # and +) only, SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(5); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(5); //ETLV_MaximumLengthOfResponse

	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, resp.iAdditionalInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);
	WaitForMockLTSYTerminated();
	
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->SATTSY
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	getInputQualifier = 0; // digits (0-9, *, # and +) only, SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(5); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(5); //ETLV_MaximumLengthOfResponse

	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrGeneral);

	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-GUI-TNGIPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyGetInputPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyGetInputPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInputFU::TestNotifyGetInputPCmd0001bL()
	{
	
	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();
	
	TUint8 pcmdNumber = 1;

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, no text string tag given, required values missing in tlv from sim)
	//
	
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);

	RSat::TGetInputV1 getInpData;
	RSat::TGetInputV1Pckg getInpDataPckg(getInpData);
	TRequestStatus requestStatus;
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	TTlv getInpTlv;
	TUint8 getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(2); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(30); //ETLV_MaximumLengthOfResponse
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, KNullDesC, RSat::KErrorRequiredValuesMissing, KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());

	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, no response length tag given, required values missing in tlv from sim)
	//
	
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KGetInputText, "Enter your account name"); 
	getInpTlv.AddData(KGetInputText);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, KNullDesC, RSat::KErrorRequiredValuesMissing, KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());

	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, minimum length of response exceed, command data not understood)
	//
	
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte( RSat::KGetInputTextMaxSize + 1 ); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte( 30 ); //ETLV_MaximumLengthOfResponse
	
	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, KNullDesC, RSat::KCmdDataNotUnderstood, KNullDesC8);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());

	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, user answer is empty string)
	//
	
	getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	TestGetInputSuccessfulWithoutTerminalResponseL(pcmdNumber, getInputQualifier, KGetInputText, 2, 30);
	
	//generate the terminal response
	RSat::TGetInputRspV1 resp;
	RSat::TGetInputRspV1Pckg respPckg(resp);

	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	resp.iAdditionalInfo.Zero();
	resp.iRspFormat = RSat::EGetInputRspFormatNotSet;
	resp.SetPCmdNumber(pcmdNumber);

	TTlv getInpRspTlv2;
	getInpRspTlv2.AddTag(KTlvTextStringTag);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, KNullDesC, resp.iGeneralResult, getInpRspTlv2.GetDataWithoutTopLevelTag());
	
	TRequestStatus reqStatus;
	iSat.TerminalRsp(reqStatus, RSat::EGetInput, respPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrCorrupt, reqStatus.Int());

	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, user answer is empty string and minimal length is 0)
	// 
	
	getInputQualifier = KAlphabetMask; // SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	TestGetInputSuccessfulWithoutTerminalResponseL(pcmdNumber, getInputQualifier, KGetInputText, 0, 10);
	
	//generate the terminal response

	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	resp.iAdditionalInfo.Zero();
	resp.iRspFormat = RSat::ESmsDefaultAlphabetUnpacked;
	resp.SetPCmdNumber(pcmdNumber);
	
	getInpRspTlv2.Begin(KTlvTextStringTag);

	PrepareTerminalResponseMockDataL(pcmdNumber, KGetInput, getInputQualifier, KNullDesC, resp.iGeneralResult, getInpRspTlv2.GetDataWithoutTopLevelTag());
	
	iSat.TerminalRsp(reqStatus, RSat::EGetInput, respPckg);
	User::WaitForRequest(reqStatus);
	ASSERT_EQUALS(KErrNone, reqStatus.Int());

	AssertMockLtsyStatusL();

	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, proactive session terminated by user)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KPSessionTerminatedByUser);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, backward mode requested by user)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KBackwardModeRequestedByUser);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, no response from user)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KNoResponseFromUser);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, help requested by user)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KHelpRequestedByUser);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, command beyond Me capabilities)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KCmdBeyondMeCapabilities);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, screen busy)
	//

	RSat::TAdditionalInfo additionalInfo;
	additionalInfo.Append(RSat::KScreenBusy);
	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KMeUnableToProcessCmd, additionalInfo);
	
	//
	// (GET INPUT, SMS default alphabet, Terminal to echo text, me problem without additional info)
	//

	TestGetInputWithUnsuccessfulTerminalResponseL(pcmdNumber, RSat::KMeUnableToProcessCmd, KNullDesC, KErrCorrupt);
	
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-GUI-TNGIPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyGetInputPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyGetInputPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInputFU::TestNotifyGetInputPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyGetInputPCmd
 	//-------------------------------------------------------------------------
	
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);

	RSat::TGetInputV1 getInpData;
	RSat::TGetInputV1Pckg getInpDataPckg(getInpData);
	TRequestStatus requestStatus;
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	iSat.CancelAsyncRequest(ESatNotifyGetInputPCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-GUI-TNGIPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyGetInputPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyGetInputPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatGetInputFU::TestNotifyGetInputPCmd0004L()
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
	ret = sat2.Open(phone2);
	ASSERT_EQUALS(KErrNone, ret);
	CleanupClosePushL(sat2);

	//-------------------------------------------------------------------------
	// Test A: Test multiple clients requesting RSat::NotifyGetInputPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// send 1st request
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);

	RSat::TGetInputV1 getInpData;
	RSat::TGetInputV1Pckg getInpDataPckg(getInpData);
	TRequestStatus requestStatus;
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	// send 2nd request
	RSat::TGetInputV1 getInpData2;
	RSat::TGetInputV1Pckg getInpDataPckg2(getInpData2);
	TRequestStatus requestStatus2;
	sat2.NotifyGetInputPCmd(requestStatus2, getInpDataPckg2);
	
	// generate completion
	TUint8 pcmdNumber = 1;

	TTlv getInpTlv;
	TUint8 getInputQualifier = 0; // digits (0-9, *, # and +) only, SMS default alphabet, input in unpacked format, Terminal to echo text, no help information available
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KGetInputText, "Enter 12345"); 
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(5); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(5); //ETLV_MaximumLengthOfResponse

	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);
	
	// completion of requests
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	RSat::TGetInputV1 expGetInpData;
	RSat::TGetInputV1Pckg expGetInpDataPckg(expGetInpData);
	PrepareDefaultGetInpData(expGetInpData, getInpData);
	expGetInpData.iText.Copy(KGetInputText);
	expGetInpData.iRspLength.iMinRspLength = 5;
	expGetInpData.iRspLength.iMaxRspLength = 5;

	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg));

	PrepareDefaultGetInpData(expGetInpData, getInpData2);
	expGetInpData.iText.Copy(KGetInputText);
	expGetInpData.iRspLength.iMinRspLength = 5;
	expGetInpData.iRspLength.iMaxRspLength = 5;
	
	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg2));

	// generate the terminal response
	RSat::TGetInputRspV1 resp;
	RSat::TGetInputRspV1Pckg respPckg(resp);
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KTextString;
	_LIT(KUserInputText, "12345"); 
	resp.iAdditionalInfo.Copy(KUserInputText);
	resp.iRspFormat = RSat::EDigitOnlyUnpacked;
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(getInputQualifier, resp);
	GenerateTerminalResponseL(getInputQualifier, resp);
 
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifyGetInputPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------

	// When multiple clients requesting RSat::NotifyGetInputPCmd and they both pass
	// different packaged parameter versions only the last request completes, because
	// etel reposts request with new parameter size and looses previous requests.
	// May be it would be worth to make ESatNotifyGetInputPCmd ipc requests flow controlled
	// by CSATTSY instead of multiple completed by ETEL. Similar defects are 51,
	// 58, 71, 78, 86 and 90
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 99);
	// Following assert was added to prevent test execution hang
	ASSERT_TRUE( EFalse );

	// send 1st request
	PrepareMockWithExpDataForPCmdNotifyL(KGetInput);
	iSat.NotifyGetInputPCmd(requestStatus, getInpDataPckg);

	// send 2nd request
	RSat::TGetInputV5 getInpDataV5;
	RSat::TGetInputV5Pckg getInpDataV5Pckg(getInpDataV5);
	sat2.NotifyGetInputPCmd(requestStatus2, getInpDataV5Pckg);
	
	// generate completion
	GetInputProactiveCmdTlvBegin( getInpTlv, pcmdNumber, getInputQualifier );
	
	getInpTlv.AddTag(KTlvTextStringTag);
	getInpTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	getInpTlv.AddData(KGetInputText);
	
	getInpTlv.AddTag(KTlvResponseLengthTag);
	getInpTlv.AddByte(5); //ETLV_MinimumLengthOfResponse
	getInpTlv.AddByte(5); //ETLV_MaximumLengthOfResponse

	PrepareMockWithCompleteDataForPCmdNotifyL(getInpTlv.End(), KErrNone);
	
	// completion of requests
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	PrepareDefaultGetInpData(expGetInpData, getInpData);
	expGetInpData.iText.Copy(KGetInputText);
	expGetInpData.iRspLength.iMinRspLength = 5;
	expGetInpData.iRspLength.iMaxRspLength = 5;

	ASSERT_EQUALS(0, expGetInpDataPckg.Compare(getInpDataPckg));

	PrepareDefaultGetInpData(expGetInpData, getInpDataV5);
	expGetInpData.iText.Copy(KGetInputText);
	expGetInpData.iRspLength.iMinRspLength = 5;
	expGetInpData.iRspLength.iMaxRspLength = 5;
	
	ASSERT_EQUALS( 0, expGetInpDataPckg.Compare( getInpDataV5Pckg.Left(sizeof(RSat::TGetInputV1)) ) );
	ASSERT_TRUE( RSat::ETextAttributeProvided != getInpDataV5.iTextAttribute.iStatus );

	// generate the terminal response
	GenerateTerminalResponseL(getInputQualifier, resp);
	GenerateTerminalResponseL(getInputQualifier, resp);
 
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}

