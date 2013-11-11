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
 @file The TEFUnit test suite for DisplayText in the SAT.
*/

#include "ccsatdisplaytextfu.h"
#include <etel.h>
#include <etelmm.h>
#include <satcs.h>
#include <test/tmockltsydata.h>

CTestSuite* CCSatDisplayTextFU::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_TEST_STEP_ISO_CPP(CCSatDisplayTextFU, TestNotifyDisplayTextPCmd0001L);
	ADD_TEST_STEP_ISO_CPP(CCSatDisplayTextFU, TestNotifyDisplayTextPCmd0001bL);
	ADD_TEST_STEP_ISO_CPP(CCSatDisplayTextFU, TestNotifyDisplayTextPCmd0002L);
	ADD_TEST_STEP_ISO_CPP(CCSatDisplayTextFU, TestNotifyDisplayTextPCmd0004L);

	END_SUITE;
	}


/**
Fills in a Tlv with command details and device identities for DisplayText proactive command
@param aTlv Tlv to fill in
@param aPCmdNumber proactive command number
@param aCommandQualifier DisplayText proactive command qualifier
*/
void CCSatDisplayTextFU::DisplayTextPCmdTlvBegin( 
		TTlv& aTlv, 
		TUint8 aPCmdNumber,
		TUint8 aCommandQualifier)
	{
	ProactiveCmdTlvBegin( aTlv, aPCmdNumber, KDisplayText, aCommandQualifier, KDisplay );
	}


/**
Resets RSat::DisplayTextVx data fields
@param aDspTxtData data to be reset
*/
void ResetDisplayTextData(RSat::TDisplayTextV1& aDspTxtData)
	{
	// reset public members
	switch( aDspTxtData.ExtensionId() )
		{
		default:
		case RSat::KSatV2:
			(static_cast<RSat::TDisplayTextV2*>(&aDspTxtData))->iDuration.iNumOfUnits = 0; 
			(static_cast<RSat::TDisplayTextV2*>(&aDspTxtData))->iDuration.iTimeUnit = RSat::ETimeUnitNotSet; 
		case RSat::KSatV1:
			{
			RSat::TDisplayTextV1& dspTxtDataV1( *static_cast<RSat::TDisplayTextV1*>(&aDspTxtData) );
			
			dspTxtDataV1.iClearScreenTrigger = RSat::EClearScreenTriggerNotSet;
			dspTxtDataV1.iIconId.iIdentifier = 0;
			dspTxtDataV1.iIconId.iQualifier = RSat::EIconQualifierNotSet;
			dspTxtDataV1.iImmediateRsp = RSat::EImmediateRspNotSet;
			dspTxtDataV1.iPriority = RSat::EDisplayPriorityNotSet;
			dspTxtDataV1.iText.SetLength(0);
			}
			break;
		}

	// reset protected members;
	aDspTxtData.SetPCmdNumber(0);
	}


/**
Test support in CSATTSY for successful RSat::NotifyDisplayTextPCmd with specified terminal response
@param aCommandQualifier display text proactive command qualifier
@param aTextToDisplay text for the Terminal to display
@param aResp terminal response data
@param aExpResult expected result of TerminalRsp() execution 
*/
void CCSatDisplayTextFU::TestNotifyDisplayTextSuccessfulL(
			TUint8 aCommandQualifier,
			const TDesC8& aTextToDisplay,
			const RSat::TDisplayTextRspV1& aResp,
			const TInt aExpResult)
	{
	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);

	RSat::TDisplayTextV2 dspTxtData;
	ResetDisplayTextData(dspTxtData);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg(dspTxtData);
	TRequestStatus stat;
	iSat.NotifyDisplayTextPCmd(stat, dspTxtDataPckg);

	TTlv dspTxtTlv;
	TUint8 dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, aResp.PCmdNumber(), dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	dspTxtTlv.AddData(aTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(stat);
	ASSERT_EQUALS(KErrNone, stat.Int());

	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(aTextToDisplay, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(aResp.PCmdNumber(), dspTxtData.PCmdNumber());
	
	GenerateTerminalResponseL(aCommandQualifier, aResp, aExpResult);
	
	AssertMockLtsyStatusL();
	}


/**
Handles sending a terminal response
@param aCommandQualifier display text proactive command qualifier
@param aResp terminal response data
@param aExpResult expected result of TerminalRsp() execution 
*/
void CCSatDisplayTextFU::GenerateTerminalResponseL(
		TUint8 aCommandQualifier,
		const RSat::TDisplayTextRspV1& aResp,
		const TInt aExpResult)
	{

	TPtrC genResultAddInfo(KNullDesC);
	if ( RSat::KMeProblem == aResp.iInfoType )
		{
		genResultAddInfo.Set( aResp.iAdditionalInfo );
		}

	RSat::TDisplayTextRspV1Pckg respPckg(aResp);
	CCSatComponentTestBase::GenerateTerminalResponseL(
			aResp.PCmdNumber(),
			KDisplayText,
			aCommandQualifier,
			RSat::EDisplayText,
			respPckg,
			genResultAddInfo,
			aResp.iGeneralResult,
			KNullDesC8,
			aExpResult);
	}


//
// Actual test cases
//


/**
@SYMTestCaseID BA-CSAT-DT-SNDTPC-0001
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyDisplayTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyDisplayTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatDisplayTextFU::TestNotifyDisplayTextPCmd0001L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// TEST C: Successful completion request of
	// RSat::NotifyDisplayTextPCmd when result is not cached.
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.1 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.1.
	// (DISPLAY TEXT normal priority, Unpacked 8 bit data for Text String, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);

	RSat::TDisplayTextV2 dspTxtData;
	ResetDisplayTextData(dspTxtData);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg(dspTxtData);
	TRequestStatus requestStatus;
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	TTlv dspTxtTlv;
	TUint8 pcmdNumber = 1;
	TUint8 dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay, "Toolkit Test 1"); 
	dspTxtTlv.AddData(KTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());

	//generate the terminal response
	RSat::TDisplayTextRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// TEST F: Coverage test(s) for RSat::NotifyDisplayTextPCmd
 	//-------------------------------------------------------------------------
	// Expected Sequence 1.3 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.3.
	// (DISPLAY TEXT, high priority, Unpacked 8 bit data for Text String, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask | KPriorityMask; //user must acknowledge the message. high priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay2, "Toolkit Test 2"); 
	dspTxtTlv.AddData(KTextToDisplay2);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay2, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::EHighPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.5 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.5.
	// (DISPLAY TEXT, Clear message after delay, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = 0; //Clear message after a delay. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay4, "Toolkit Test 4"); 
	dspTxtTlv.AddData(KTextToDisplay4);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay4, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EClearAfterDelay, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 4.1 See ETSI TS 102 384 in subclause 27.22.4.1.4.4.2, Expected Sequence 4.1.
	// (DISPLAY TEXT, sustained text, unpacked data 8 bits, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	dspTxtTlv.AddData(KTextToDisplay);
	
	dspTxtTlv.AddTag(KTlvImmediateResponseTag);
			
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());


	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::EImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 5.1A See ETSI TS 102 384 in subclause 27.22.4.1.5.4.2, Expected Sequence 5.1A.
	// (DISPLAY TEXT, display of basic icon, self-explanatory, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay51, "Basic Icon"); 
	dspTxtTlv.AddData(KTextToDisplay51);
	
	dspTxtTlv.AddTag(KTlvIconIdentifierTag);
	TUint8 iconQualifier = 0; // self explanatory
	dspTxtTlv.AddByte(iconQualifier); //ETLV_IconQualifier
	TUint8 iconId = 1; // record 1 in EFIMG
	dspTxtTlv.AddByte(iconId); //ETLV_IconIdentifier

	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// check whole result by package
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay51, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ESelfExplanatory, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(1), dspTxtData.iIconId.iIdentifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());

 	//-------------------------------------------------------------------------
	// Expected Sequence 5.2A See ETSI TS 102 384 in subclause 27.22.4.1.5.4.2, Expected Sequence 5.2A.
	// (DISPLAY TEXT, display of colour icon, successful)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay52, "Colour Icon"); 
	dspTxtTlv.AddData(KTextToDisplay52);
	
	dspTxtTlv.AddTag(KTlvIconIdentifierTag);
	iconQualifier = 1; // not self explanatory
	dspTxtTlv.AddByte(iconQualifier); //ETLV_IconQualifier
	iconId = 2; // record 2 in EFIMG
	dspTxtTlv.AddByte(iconId); //ETLV_IconIdentifier

	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	// check whole result by package
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay52, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENotSelfExplanatory, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(2), dspTxtData.iIconId.iIdentifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST E: Unsolicited completion of RSat::NotifyDisplayTextPCmd
	// from LTSY.
 	//-------------------------------------------------------------------------

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	dspTxtTlv.AddData(KTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);

	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(pcmdNumber, KDisplayText, dispTextMask, resp.iAdditionalInfo, RSat::KMeUnableToProcessCmd, KNullDesC8);

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);
	User::WaitForRequest(mockLtsyStatus);

	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());
	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// TEST B: failure on completion of pending request from LTSY->SATTSY
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	dspTxtTlv.AddData(KTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrGeneral);

	User::WaitForRequest(requestStatus);

	ASSERT_EQUALS(KErrGeneral, requestStatus.Int());
	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-DT-SNDTPC-0001b
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for RSat::NotifyDisplayTextPCmd when problems in processing command
@SYMTestPriority High
@SYMTestActions Invokes RSat::NotifyDisplayTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatDisplayTextFU::TestNotifyDisplayTextPCmd0001bL()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.2 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.2.
	// (DISPLAY TEXT normal priority, Unpacked 8 bit data for Text String, screen busy)
 	//-------------------------------------------------------------------------

	TUint8 dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	_LIT8(KTextToDisplay, "Toolkit Test 1"); 

	RSat::TDisplayTextRspV1 resp;
	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.iAdditionalInfo.Append(RSat::KScreenBusy);
	TUint8 pcmdNumber = 1;
	resp.SetPCmdNumber(pcmdNumber);

	TestNotifyDisplayTextSuccessfulL(dispTextMask, KTextToDisplay, resp);

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.7 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.7.
	// (DISPLAY TEXT, Backward move in UICC session, successful)
 	//-------------------------------------------------------------------------

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	_LIT8(KTextToDisplayBackwards, "<GO-BACKWARDS>"); 
	
	resp.iGeneralResult = RSat::KBackwardModeRequestedByUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	TestNotifyDisplayTextSuccessfulL(dispTextMask, KTextToDisplayBackwards, resp);

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.8 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.8.
	// (DISPLAY TEXT, session terminated by user)
 	//-------------------------------------------------------------------------

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	_LIT8(KTextToDisplayAbort, "<ABORT>"); 

	resp.iGeneralResult = RSat::KPSessionTerminatedByUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	TestNotifyDisplayTextSuccessfulL(dispTextMask, KTextToDisplayAbort, resp);

 	//-------------------------------------------------------------------------
	// Expected Sequence 1.9 See ETSI TS 102 384 in subclause 27.22.4.1.1.4.2, Expected Sequence 1.9.
	// (DISPLAY TEXT, icon and text to be displayed, no text string given, not understood by ME)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);

	RSat::TDisplayTextV2 dspTxtData;
	ResetDisplayTextData(dspTxtData);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg(dspTxtData);
	TRequestStatus requestStatus;
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	TTlv dspTxtTlv;
	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	
	dspTxtTlv.AddTag(KTlvIconIdentifierTag);
	dspTxtTlv.AddByte(RSat::ESelfExplanatory); //ETLV_IconQualifier
	dspTxtTlv.AddByte(1); //ETLV_IconIdentifier

	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);

	// terminal response data
	PrepareTerminalResponseMockDataL(pcmdNumber, KDisplayText, dispTextMask, KNullDesC, RSat::KCmdDataNotUnderstood, KNullDesC8);
	
	User::WaitForRequest(requestStatus); //for the NotifyDisplayTextPCmd complete
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 2.1 See ETSI TS 102 384 in subclause 27.22.4.1.2.4.2, Expected Sequence 2.1.
	// (DISPLAY TEXT, no response from user)
 	//-------------------------------------------------------------------------
	
	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	_LIT8(KTextToDisplayTimeout, "<TIME-OUT>"); 

	resp.iGeneralResult = RSat::KNoResponseFromUser;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	TestNotifyDisplayTextSuccessfulL(dispTextMask, KTextToDisplayTimeout, resp);

 	//-------------------------------------------------------------------------
	// (DISPLAY TEXT, icon and text to be displayed, no text string tag given, required values missing )
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	// prepare data without text string tag
	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvIconIdentifierTag);
	dspTxtTlv.AddByte(RSat::ESelfExplanatory); //ETLV_IconQualifier
	dspTxtTlv.AddByte(1); //ETLV_IconIdentifier

	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	// terminal response data
	PrepareTerminalResponseMockDataL(pcmdNumber, KDisplayText, dispTextMask, KNullDesC, RSat::KErrorRequiredValuesMissing, KNullDesC8);

	User::WaitForRequest(requestStatus); //for the NotifyDisplayTextPCmd complete
	ASSERT_EQUALS(KErrCorrupt, requestStatus.Int());

	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// (DISPLAY TEXT, command beyond me capabilities)
 	//-------------------------------------------------------------------------
	
	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplayBeyondCaps, "<BEYOND-CAPS>"); 
	dspTxtTlv.AddData(KTextToDisplayBeyondCaps);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplayBeyondCaps, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KCmdBeyondMeCapabilities;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// Expected Sequence 5.1B See ETSI TS 102 384 in subclause 27.22.4.1.5.4.2, Expected Sequence 5.1B.
	// (DISPLAY TEXT, display of basic icon, self-explanatory, requested icon could not be displayed)
 	//-------------------------------------------------------------------------

	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay51, "Basic Icon"); 
	dspTxtTlv.AddData(KTextToDisplay51);
	
	dspTxtTlv.AddTag(KTlvIconIdentifierTag);
	TUint8 iconQualifier = 0; // self explanatory
	dspTxtTlv.AddByte(iconQualifier); //ETLV_IconQualifier
	TUint8 iconId = 1; // record 1 in EFIMG
	dspTxtTlv.AddByte(iconId); //ETLV_IconIdentifier

	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	
	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrNone, requestStatus.Int());

	
	// check result
	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay51, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ESelfExplanatory, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(static_cast<TUint8>(1), dspTxtData.iIconId.iIdentifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());


	//generate the terminal response
	resp.iGeneralResult = RSat::KSuccessRequestedIconNotDisplayed;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	GenerateTerminalResponseL(dispTextMask, resp);
	AssertMockLtsyStatusL();

 	//-------------------------------------------------------------------------
	// (DISPLAY TEXT normal priority, Unpacked 8 bit data for Text String, me problem without additional info)
 	//-------------------------------------------------------------------------

	dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.

	resp.iGeneralResult = RSat::KMeUnableToProcessCmd;
	resp.iInfoType = RSat::KMeProblem;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);

	TestNotifyDisplayTextSuccessfulL(dispTextMask, KTextToDisplay, resp, KErrCorrupt);

	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-DT-SNDTPC-0002
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for cancelling of RSat::NotifyDisplayTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes cancelling of RSat::NotifyDisplayTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatDisplayTextFU::TestNotifyDisplayTextPCmd0002L()
	{

	OpenEtelServerL(EUseExtendedError);
	CleanupStack::PushL(TCleanupItem(Cleanup,this));
	OpenPhoneL();
	OpenSatL();

	TRequestStatus mockLtsyStatus;
	iMockLTSY.NotifyTerminated(mockLtsyStatus);

 	//-------------------------------------------------------------------------
	// Test cancelling of RSat::NotifyDisplayTextPCmd
 	//-------------------------------------------------------------------------
	
	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);

	TTlv dspTxtTlv;
	TUint8 pcmdNumber = 1;
	TUint8 dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay, "Toolkit Test 1"); 
	dspTxtTlv.AddData(KTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone, 10);

	RSat::TAdditionalInfo addInfo;
	addInfo.Zero();
	addInfo.Append(RSat::KNoSpecificMeProblem);
	PrepareTerminalResponseMockDataL(
			pcmdNumber, KDisplayText, dispTextMask,
			addInfo, RSat::KMeUnableToProcessCmd);

	RSat::TDisplayTextV2 dspTxtData;
	ResetDisplayTextData(dspTxtData);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg(dspTxtData);
	TRequestStatus requestStatus;
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	iSat.CancelAsyncRequest(ESatNotifyDisplayTextPCmd);

	User::WaitForRequest(requestStatus);
	ASSERT_EQUALS(KErrCancel, requestStatus.Int());

	// Wait for completion of iMockLTSY.NotifyTerminated
	User::WaitForRequest(mockLtsyStatus);

	ASSERT_EQUALS(KErrNone, mockLtsyStatus.Int());

	AssertMockLtsyStatusL();
	
	CleanupStack::PopAndDestroy(this);
	
	}


/**
@SYMTestCaseID BA-CSAT-DT-SNDTPC-0004
@SYMPREQ 1780
@SYMComponent  telephony_csat
@SYMTestCaseDesc Test support in CSAT for multiple client requests to RSat::NotifyDisplayTextPCmd
@SYMTestPriority High
@SYMTestActions Invokes multiple client requests to RSat::NotifyDisplayTextPCmd
@SYMTestExpectedResults Pass
@SYMTestType CT
*/
void CCSatDisplayTextFU::TestNotifyDisplayTextPCmd0004L()
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
	// Test A: Test multiple clients requesting RSat::NotifyDisplayTextPCmd
	// when they both pass the same T-class version
 	//-------------------------------------------------------------------------

	// send 1st request
	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);

	RSat::TDisplayTextV2 dspTxtData;
	ResetDisplayTextData(dspTxtData);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg(dspTxtData);
	TRequestStatus requestStatus;
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	// send 2nd request
	RSat::TDisplayTextV2 dspTxtData2;
	ResetDisplayTextData(dspTxtData2);
	RSat::TDisplayTextV2Pckg dspTxtDataPckg2(dspTxtData2);
	TRequestStatus requestStatus2;
	sat2.NotifyDisplayTextPCmd(requestStatus2, dspTxtDataPckg2);
	
	// generate completion
	TUint8 pcmdNumber = 1;

	TTlv dspTxtTlv;
	TUint8 dispTextMask = KMessageClearMask; //user must acknowledge the message. normal priority.
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	_LIT8(KTextToDisplay, "Toolkit Test 1"); 
	dspTxtTlv.AddData(KTextToDisplay);
	
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	// completion of requests
	User::WaitForRequest(requestStatus);
	User::WaitForRequest(requestStatus2);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());

	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtData2.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData2.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData2.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData2.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData2.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData2.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData2.PCmdNumber());

	// generate the terminal response
	RSat::TDisplayTextRspV1 resp;
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);
	GenerateTerminalResponseL(dispTextMask, resp);
	GenerateTerminalResponseL(dispTextMask, resp);

	AssertMockLtsyStatusL();

	//-------------------------------------------------------------------------
	// Test B: Test multiple clients requesting RSat::NotifyDisplayTextPCmd
	// when they both pass different T-class versions
 	//-------------------------------------------------------------------------	

	// When multiple clients requesting RSat::NotifyDisplayTextPCmd and they both pass
	// different packaged parameter versions only the last request completes, because
	// etel reposts request with new parameter size and looses previous requests.
	// May be it would be worth to make ESatNotifyDisplayTextPCmd ipc requests flow controlled
	// by CSATTSY instead of multiple completed by ETEL.
	ERR_PRINTF2(_L("<font color=Orange>$CSATKnownFailure: defect id = %d</font>"), 58);
	// Following assert was added to prevent test execution hang
	ASSERT_TRUE( EFalse );

	// send 1st request
	PrepareMockWithExpDataForPCmdNotifyL(KDisplayText);
	ResetDisplayTextData(dspTxtData);
	iSat.NotifyDisplayTextPCmd(requestStatus, dspTxtDataPckg);

	// send 2nd request
	RSat::TDisplayTextV5 dspTxtDataV5;
	ResetDisplayTextData(dspTxtDataV5);
	RSat::TDisplayTextV5Pckg dspTxtDataV5Pckg(dspTxtDataV5);
	sat2.NotifyDisplayTextPCmd(requestStatus2, dspTxtDataV5Pckg);
	
	// generate completion
	DisplayTextPCmdTlvBegin( dspTxtTlv, pcmdNumber, dispTextMask );
	dspTxtTlv.AddTag(KTlvTextStringTag);
	dspTxtTlv.AddByte(K8BitDCS); //ETLV_DataCodingScheme
	dspTxtTlv.AddData(KTextToDisplay);
	PrepareMockWithCompleteDataForPCmdNotifyL(dspTxtTlv.End(), KErrNone);
	
	// completion of requests
	User::WaitForRequest(requestStatus2);
	User::WaitForRequest(requestStatus);

	// check results
	ASSERT_EQUALS(KErrNone, requestStatus.Int());
	ASSERT_EQUALS(KErrNone, requestStatus2.Int());

	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtData.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtData.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtData.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtData.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtData.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtData.iImmediateRsp);
	ASSERT_EQUALS(pcmdNumber, dspTxtData.PCmdNumber());

	ASSERT_EQUALS(0, Compare16bitDesWith8bitDesL(KTextToDisplay, dspTxtDataV5.iText));
	ASSERT_EQUALS(RSat::ENoDurationAvailable, dspTxtDataV5.iDuration.iTimeUnit);
	ASSERT_EQUALS(RSat::ENormalPriority, dspTxtDataV5.iPriority);
	ASSERT_EQUALS(RSat::EUserClear, dspTxtDataV5.iClearScreenTrigger);
	ASSERT_EQUALS(RSat::ENoIconId, dspTxtDataV5.iIconId.iQualifier);
	ASSERT_EQUALS(RSat::ENoImmediateRsp, dspTxtDataV5.iImmediateRsp);
	ASSERT_TRUE(RSat::ETextAttributeProvided != dspTxtDataV5.iTextAttribute.iStatus);
	ASSERT_EQUALS(pcmdNumber, dspTxtDataV5.PCmdNumber());


	// generate the terminal response
	resp.iGeneralResult = RSat::KSuccess;
	resp.iInfoType = RSat::KNoAdditionalInfo;
	resp.iAdditionalInfo.Zero();
	resp.SetPCmdNumber(pcmdNumber);
	GenerateTerminalResponseL(dispTextMask, resp);
	GenerateTerminalResponseL(dispTextMask, resp);

	AssertMockLtsyStatusL();

	CleanupStack::PopAndDestroy(4, this); // sat2, phone2, telServer2, this

	}

