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
#include "testemsierelativestartpositionStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemsierelativestartpositionStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	// Set to use 7 bit coding scheme
	TSmsDataCodingScheme::TSmsAlphabet alphabet = TSmsDataCodingScheme::ESmsAlphabet7Bit;

	CSmsMessage* smsMessage = CreateVariableLengthSmsMessageLC(alphabet, KMaxSmsBufferLength);

	// We arbitrarily create the format IE as the base IE class
	// cannot be instantiated.
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	CleanupStack::PushL(object);

	// We set the start position to '39008' to reflect
	// the test as stipulated in the defect.
	object->SetStartPosition(39008);

	// Set rest of the parameters for completeness.
	object->SetFormatLength(120);
	object->SetBold(ETrue);
	object->SetItalic(ETrue);
	object->SetUnderline(ETrue);
	object->SetStrikethrough(ETrue);
	object->SetAlignment(CEmsFormatIE::ECenter);
	object->SetFontSize(CEmsFormatIE::ELarge);

	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	CBufSeg* writebuffer = CBufSeg::NewL(KSmsMaxEMSLength);
	CleanupStack::PushL(writebuffer);

	RBufWriteStream writestream(*writebuffer);
	writestream.Open(*writebuffer,0);
	writestream << *smsMessage;
	writestream.CommitL();

	// Now deserialize
	RBufReadStream readstream(*writebuffer);
	readstream.Open(*writebuffer,0);

	_LIT(KTempMessage,"Preparing to internalize SMS message");

	CSmsMessage* smsMessage2=CreateSmsMessageL(KTempMessage,alphabet);
	CleanupStack::PushL(smsMessage2);
	readstream >> *smsMessage2;

	// Check that the startposition of the IE is not corrupted.
	const RPointerArray<const CEmsInformationElement>&  emsElements= smsMessage2->GetEMSInformationElementsL();
	TInt numElements = emsElements.Count();
	INFO_PRINTF2(_L("Message contains %d Information Elements!"), numElements);

	if (numElements!=1) User::Leave(KErrCorrupt);

	// Get our IE same as above
	const CEmsInformationElement& ie=*emsElements[0];
	if (ie.Identifier() != CSmsInformationElement::ESmsEnhancedTextFormatting) User::Leave(KErrCorrupt);

	TInt startpos = ie.StartPosition();
	TEST_CHECKL(startpos,39008,_L("Start position corrupted"));

	CleanupStack::PopAndDestroy(smsMessage2);
	CleanupStack::PopAndDestroy(writebuffer);
	CleanupStack::PopAndDestroy(smsMessage);
	INFO_PRINTF1(_L("Test completed"));

	return TestStepResult();
	}


