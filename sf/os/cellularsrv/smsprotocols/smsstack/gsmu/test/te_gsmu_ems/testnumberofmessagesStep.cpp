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
#include "testnumberofmessagesStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestnumberofmessagesStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	_LIT(KTestEmsMsg7Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB");
	_LIT(KTestEmsMsg7Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABC");
	_LIT(KTestEmsMsg7Bit3,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\x1b\x65");   //153(134 Octet) + 6 = 1 Msg
	_LIT(KTestEmsMsg7Bit4,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB\x1b\x65");   //154(>134 Octet + 6 = 2 Msg
	_LIT(KTestEmsMsg7Bit5,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB\x1b\x65\x61\x62\x63\x64\x65\x66\x67\x68");

	_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg8Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg8Bit3,"");						//Empty Text
	_LIT(KTestEmsMsg16Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	_LIT(KTestEmsMsg16Bit2,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");


	CSmsMessage* smsMessage=NULL;
	CEmsFormatIE* object = NULL;
	TInt num=0;
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(100);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);
	CleanupStack::PopAndDestroy(smsMessage);
	//
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit2,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(100);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==2);

	CleanupStack::PopAndDestroy(smsMessage);
	//
	smsMessage=CreateSmsMessageL(KTestEmsMsg7Bit1,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(100);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);

	CleanupStack::PopAndDestroy(smsMessage);
		//
	smsMessage=CreateSmsMessageL(KTestEmsMsg7Bit2,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(100);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==2);

	CleanupStack::PopAndDestroy(smsMessage);
	//
	smsMessage=CreateSmsMessageL(KTestEmsMsg16Bit1,TSmsDataCodingScheme::ESmsAlphabetUCS2);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);

	CleanupStack::PopAndDestroy(smsMessage);
		//
	smsMessage=CreateSmsMessageL(KTestEmsMsg16Bit2,TSmsDataCodingScheme::ESmsAlphabetUCS2);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==2);

	CleanupStack::PopAndDestroy(smsMessage);
		//EMS Msg with empty text and no IEs//
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit3,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);
	CleanupStack::PopAndDestroy(smsMessage);
		//EMS IE points to empty text//
	smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit3,TSmsDataCodingScheme::ESmsAlphabet8Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(0);
	object->SetFormatLength(0);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);
	CleanupStack::PopAndDestroy(smsMessage);
		//Extended Char at the very end of the text//
	smsMessage=CreateSmsMessageL(KTestEmsMsg7Bit3,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(100);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==1);

	CleanupStack::PopAndDestroy(smsMessage);


		//EMS IE points to Extended Char that is on the boundary//
	smsMessage=CreateSmsMessageL(KTestEmsMsg7Bit4,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(153);
	object->SetFormatLength(2);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==2);
	CleanupStack::PopAndDestroy(smsMessage);

		//EMS IE points to Extended Char that is on the boundary//
	smsMessage=CreateSmsMessageL(KTestEmsMsg7Bit5,TSmsDataCodingScheme::ESmsAlphabet7Bit);
	CleanupStack::PushL(smsMessage);

	object = CEmsFormatIE::NewL();
	object->SetStartPosition(147);
	object->SetFormatLength(2);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	num=smsMessage->NumMessagePDUsL();

	INFO_PRINTF2(_L("Number of segments of smsMessage %d"),num);
	TESTL(num==2);
	CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}

