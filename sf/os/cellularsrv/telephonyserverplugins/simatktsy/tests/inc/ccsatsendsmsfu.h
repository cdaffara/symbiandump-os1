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
// functional unit of the SAT.
//



/**
 @file The TEFUnit header file which tests the SendSMS
*/

#ifndef CCSATSENDSMSFU_H
#define CCSATSENDSMSFU_H

#include <test/tefunit.h>

#include <etelmm.h>
#include <etelmmcs.h>

#include "ccsatcomponenttestbase.h"

class CCSatSendSMSFU : public CCSatComponentTestBase
	{
public:
	// Create a suite of all the tests
	static CTestSuite* CreateSuiteL(const TDesC& aName);

public:
	// Individual test steps

	void TestNotifySendSmPCmd0001L();
	void TestNotifySendSmPCmd0002L();
	void TestNotifySendSmPCmd0004L();

private:
	// comments in the below enum refers to 3GPP TS 31.124 V7.4.0 (2008-06) 
	enum TSendSmTC {
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.1
	ETestSequence11,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.2
	ETestSequence12,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.3
	ETestSequence13,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.4
	ETestSequence14,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.5
	ETestSequence15,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.6
	ETestSequence16,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.7
	ETestSequence17,        
	// clause 27.22.4.10.1 SEND SHORT MESSAGE (normal), sequence 1.8
	ETestSequence18,        
	// clause 27.22.4.10.3 SEND SHORT MESSAGE (icon support), sequence 3.1A
	ETestSequence31A,  
	// clause 27.22.4.10.3 SEND SHORT MESSAGE (icon support), sequence 3.1B
	ETestSequence31B,  
	// clause 27.22.4.10.3 SEND SHORT MESSAGE (icon support), sequence 3.2A
	ETestSequence32A,  
	// clause 27.22.4.10.3 SEND SHORT MESSAGE (icon support), sequence 3.2B
	ETestSequence32B,  
	// clause 27.22.4.10.4 SEND SHORT MESSAGE (Support of Text Attribute)
	//ETestSequence41, 
	// run test with address TLV missing all data after the tag  
	ETestMissingAddressType,
	// run test with address TLV missing a dialling number  
	ETestMissingDiallingNumber,
	// run test with "relative" TP-VPF value set in the TPDU header
	ETestRelativeVPF,
	ETestRelativeVPFPacked,
	// run test with "enchanced" TP-VPF value set in the TPDU header
	ETestEnchancedVPF,
	ETestEnchancedVPFPacked,
	// run test with "absolute" TP-VPF value set the TPDU header
	ETestAbsoluteVPF,
	ETestAbsoluteVPFPacked,
	// run test using SMS-COMMAND TPDU instead of SMS-SUBMIT one
	ETestSmsCommand,
	// run test with 8-bit coded "SMS-SUBMIT" TPDU having too big value in TP-UDL
	ETestTooBigSmsSubmit8bit,
	// run test with GSM coded "SMS-SUBMIT" TPDU having too big value in TP-UDL
	ETestTooBigSmsSubmit7bit,
	// run test with packed 8-bit coded "SMS-SUBMIT" TPDU having too big value in TP-UDL
	ETestTooBigSmsSubmit8bitPacked,
	// run test with packed GSM coded "SMS-SUBMIT" TPDU having too big value in TP-UDL
	ETestTooBigSmsSubmit7bitPacked,
	// run test with "SMS-COMMAND" TPDU having a too big value in TP-CDL
	ETestTooBigSmsCommand,
	// run test with missing SMS TPDU
	ETestMissingSmsTpdu,
	// test unsolicted event 
	ETestUnsolictedCompletion
	};

	void DoPositiveNotifySendSmPCmdTestL(TSendSmTC aTC);
	void DoNegativeNotifySendSmPCmdTestL(TSendSmTC aTC);
	void DoTestTerminalResponseL();

	void MockPrimeSendSmPCmdL(TTpdu& aTpdu, TSendSmTC aTC, TInt aResultCode = KErrNone);
	void MockPrimeSendSmTerminalResponseL(TSendSmTC aTC);	
	void MockPrimeSendSmTerminalResponseL(TSendSmTC aTC, const RSat::TSendSmRspV1& aRsp, TInt aResultCode = KErrNone);
	void MockCompleteSendSmPCmdL(TTpdu& aTpdu, TSendSmTC aTC, TInt aResultCode = KErrNone);

	static void CreateSmsSubmitTpdu(TTpdu& aTpdu,TSendSmTC aTC);
	static void CreateSmsCommandTpdu(TTpdu& aTpdu);
	static void CreateCorruptedSmsTpdu(TTpdu& aTpdu, TSendSmTC aTC);
	static void CreateSmsTpdu(TTpdu& aTpdu, TSendSmTC aTC);
	static const TDesC8& CreateSendSmTlv(TTlvBase& aTlv, const TTpdu& aTpdu, TSendSmTC aTC);
	static const TDesC8& CreateSendSmTerminalResponseTlv(TTlvBase& aTlv,
										TUint8 aCommandQualifier, const RSat::TSendSmRspV1& aRsp);

	static TBool PackingIsUsed(TSendSmTC aTC);
	static const TDesC8& ExpectedTpdu(TSendSmTC aTC);
	static TUint8 ValidityPeriodFormat(TSendSmTC aTC);
	static const TDesC8& DestinationAddress(TSendSmTC aTC);
	static TUint8 DataCodingScheme(TSendSmTC aTC);
	static TUint8 UserDataLength(TSendSmTC aTC);
	static const TDesC8& UserData(TSendSmTC aTC);
	static TUint8 CommandQualifier(TSendSmTC aTC);
	static TBool AlphaIdIsUsed(TSendSmTC aTC);
	static const TDesC8& AlphaId(TSendSmTC aTC);
	static const TDesC& ExpectedAlphaId(TSendSmTC aTC);
	static RSat::TAlphaIdStatus ExpectedAlphaIdStatus(TSendSmTC aTC);
	static TBool CompleteAddressIsUsed(TSendSmTC aTC);
	static RSat::TNumberingPlan ExpectedNumberPlan(TSendSmTC aTC);
	static RSat::TTypeOfNumber ExpectedTypeOfNumber(TSendSmTC aTC);	
	static TBool IsIconRelatedTC(TSendSmTC aTC);
	static TUint8 IconTlvTag(TSendSmTC aTC);
	static TUint8 IconQualifier(TSendSmTC aTC);
	static RSat::TIconQualifier ExpectedIconQualifier(TSendSmTC aTC);	
	static void FillResponse(RSat::TSendSmRspV1& aRsp,TSendSmTC aTC);

	}; // class CCSatSendSMSFU

#endif // CCSATSENDSMSFU_H

