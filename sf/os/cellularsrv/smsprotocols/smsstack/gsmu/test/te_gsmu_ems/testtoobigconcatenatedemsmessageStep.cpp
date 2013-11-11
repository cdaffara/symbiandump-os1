// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file
*/
#include "testtoobigconcatenatedemsmessageStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


/**
 *  Tests encoding of an EMS message that is too big to be sent. At the same
 *  time 4 timing measurements are taken to determine how long encoding takes.
 *  This covers both determining the number of PDUs and the actual encoding.
 *
 *  @return  A pass or fail TVerdict code.
 */
TVerdict CtesttoobigconcatenatedemsmessageStep::doTestStepL()
	{
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	HBufC* textBuf=HBufC::NewL(39270);
	CleanupStack::PushL(textBuf);
	TPtr textPtr=textBuf->Des();
	TInt i;

	CArrayFixFlat<TGsmSms>*  smsArray = new (ELeave) CArrayFixFlat<TGsmSms>(8);
	CleanupStack::PushL(smsArray);

	//
	// Test encoding a 1 PDU EMS message...
	//
	textPtr.Append(KText); // start with 1 PDU worth of text
	
	CSmsMessage* emsMessage=CreateSmsMessageL(textPtr,alphabet);
	CleanupStack::PushL(emsMessage);

	CEmsFormatIE* object1 = CEmsFormatIE::NewL();
	CleanupStack::PushL(object1);

	object1->SetStartPosition(10);
	object1->SetFormatLength(100);
	object1->SetBold(ETrue);
	object1->SetItalic(ETrue);
	object1->SetUnderline(ETrue);
	object1->SetStrikethrough(ETrue);

	emsMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	TInt  numPDUs = 0;
	TTime  start, end;
	TInt64  lengthOfCall;

	INFO_PRINTF2(_L("Count PDUs in %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAPD(ret,numPDUs = emsMessage->NumMessagePDUsL());
	end.UniversalTime();
	TEST(ret==KErrNone);
	TEST(numPDUs==1);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Count PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret, numPDUs, I64REAL(lengthOfCall) / 1000000.0);
	
	smsArray->Reset();
	
	INFO_PRINTF2(_L("Encoding of %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAP(ret, emsMessage->EncodeIntoSinglePDUL(*smsArray));
	end.UniversalTime();
	numPDUs = smsArray->Count();
	TEST(ret==KErrNone);
	TEST(numPDUs==1);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Encoding PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret, numPDUs, I64REAL(lengthOfCall) / 1000000.0);

	CleanupStack::PopAndDestroy(emsMessage);

	
	//
	// Test encoding a 255 PDU EMS message...
	//
	for(i=0;i<253;i++) // add 254 PDUs of text to the one we have...
		{
		textPtr.Append(KText);
		}

	emsMessage=CreateSmsMessageL(textPtr,alphabet);
	CleanupStack::PushL(emsMessage);

	object1 = CEmsFormatIE::NewL();
	CleanupStack::PushL(object1);

	object1->SetStartPosition(10);
	object1->SetFormatLength(100);
	object1->SetBold(ETrue);
	object1->SetItalic(ETrue);
	object1->SetUnderline(ETrue);
	object1->SetStrikethrough(ETrue);

	emsMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	numPDUs = 0;
	
	INFO_PRINTF2(_L("Count PDUs in %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAPD(ret2,numPDUs = emsMessage->NumMessagePDUsL());
	end.UniversalTime();
	TEST(ret2==KErrNone);
	TEST(numPDUs==255);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Count PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret2, numPDUs, I64REAL(lengthOfCall) / 1000000.0);

	smsArray->Reset();
	
	INFO_PRINTF2(_L("Encoding of %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAP(ret2, emsMessage->EncodeMessagePDUsL(*smsArray, 0));
	end.UniversalTime();
	numPDUs = smsArray->Count();
	TEST(ret2==KErrNone);
	TEST(numPDUs==255);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Encoding PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret2, numPDUs, I64REAL(lengthOfCall) / 1000000.0);

	CleanupStack::PopAndDestroy(emsMessage);


	//
	// Test encoding a 256 PDU EMS message...
	//
	textPtr.Append(KText); // 255th time

	emsMessage=CreateSmsMessageL(textPtr,alphabet);
	CleanupStack::PushL(emsMessage);

	object1 = CEmsFormatIE::NewL();
	CleanupStack::PushL(object1);

	object1->SetStartPosition(10);
	object1->SetFormatLength(100);
	object1->SetBold(ETrue);
	object1->SetItalic(ETrue);
	object1->SetUnderline(ETrue);
	object1->SetStrikethrough(ETrue);

	emsMessage->AddEMSInformationElementL(*object1);
	CleanupStack::PopAndDestroy(object1);

	numPDUs = 0;
	
	INFO_PRINTF2(_L("Count PDUs in %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAPD(ret3,numPDUs = emsMessage->NumMessagePDUsL());
	end.UniversalTime();
	TEST(ret3==KErrOverflow);
	TEST(numPDUs==0);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Count PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret3, numPDUs, I64REAL(lengthOfCall) / 1000000.0);

	smsArray->Reset();
	
	INFO_PRINTF2(_L("Encoding of %d character EMS message..."), textPtr.Length());
	start.UniversalTime();
	TRAP(ret3, emsMessage->EncodeMessagePDUsL(*smsArray, 0));
	end.UniversalTime();
	numPDUs = smsArray->Count();
	TEST(ret3==KErrOverflow);
	TEST(numPDUs==0);
	lengthOfCall = end.MicroSecondsFrom(start).Int64();
	INFO_PRINTF4(_L("Encoding PDUs returned %d and has %d PDUs. Time taken was %.3fs"),
	             ret3, numPDUs, I64REAL(lengthOfCall) / 1000000.0);

	CleanupStack::PopAndDestroy(emsMessage);

	CleanupStack::PopAndDestroy(smsArray);
	CleanupStack::PopAndDestroy(textBuf);

	return TestStepResult();
	}


