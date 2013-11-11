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
#include "testemsodiieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemsodiieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Test EMS user prompt"));

	FbsStartup();
	User::LeaveIfError(RFbsSession::Connect());
	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;
	CSmsMessage* emsMessage=CreateSmsMessageL(KPromptMsg,alphabet);
	CleanupStack::PushL(emsMessage);

	CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(200);
	CleanupStack::PushL(prompt);
	prompt->SetStartPosition(52);

	CEmsUserPrompt* prompt2 = static_cast<CEmsUserPrompt*>(prompt->DuplicateL());
	CleanupStack::PushL(prompt2);

	CEmsUserPrompt* prompt3 = CEmsUserPrompt::NewL(0);
	CleanupStack::PushL(prompt3);
	prompt3->CopyL(*prompt2);

	TEST_CHECKL(prompt3->ObjectCount(),prompt->ObjectCount(),_L("User prompt object count is not the same"));
	TEST_CHECKL(prompt3->StartPosition(),prompt->StartPosition(),_L("Start position is not the same"));

	// add a user prompt and a small picture to EMS message
	CFbsBitmap* bitmap1=NULL;
	CEmsPictureIE* pic1=NULL;
	bitmap1 = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap1);

	TESTL(!LoadBitmap(*bitmap1,KPicsMBM,0));

	pic1 = CEmsPictureIE::NewL(*bitmap1);
	CleanupStack::PushL(pic1);
	pic1->SetStartPosition(20);

	emsMessage->AddEMSInformationElementL(*pic1);
	CleanupStack::PopAndDestroy(pic1);

	//

	TESTL(!LoadBitmap(*bitmap1,KPicsMBM,0));

	pic1 = CEmsPictureIE::NewL(*bitmap1);
	CleanupStack::PushL(pic1);
	pic1->SetStartPosition(20);
	emsMessage->AddEMSInformationElementL(*pic1);
	CleanupStack::PopAndDestroy(pic1);
	//

	CEmsUserPrompt* prompt4 = CEmsUserPrompt::NewL(10);
	CleanupStack::PushL(prompt4);
	prompt4->SetStartPosition(20);
	prompt4->SetObjectCount(2);
	emsMessage->AddEMSInformationElementL(*prompt4);
	CleanupStack::PopAndDestroy(prompt4);

	// now force the message encoding
	TInt numMsg = emsMessage->NumMessagePDUsL();
	INFO_PRINTF2(_L("Number of segments = %d"), numMsg);

	//check IEs, their types and positions
	const RPointerArray<const CEmsInformationElement>&  emsElements= emsMessage->GetEMSInformationElementsL();
	TInt numElements = emsElements.Count();
	INFO_PRINTF2(_L("Message contains %d Information Elements!"), numElements);



	CleanupStack::PopAndDestroy(bitmap1);
	CleanupStack::PopAndDestroy(prompt3);
	CleanupStack::PopAndDestroy(prompt2);
	CleanupStack::PopAndDestroy(prompt);
	CleanupStack::PopAndDestroy(emsMessage);
	RFbsSession::Disconnect();

	return TestStepResult();
	}


