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
#include "testremovalofmultipleodiandupiesStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestremovalofmultipleodiandupiesStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		_LIT(KTestEmsMsg8Bit1,"AAAAAAAAAAAAAAAAAA");
		TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet8Bit;
		CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,alphabet);
		CleanupStack::PushL(smsMessage);

		CEmsSoundIE* object = CEmsSoundIE::NewL(KOasisMelody());
		object->SetStartPosition(10);
		CleanupStack::PushL(object);
		smsMessage->AddEMSInformationElementL(*object);
		CleanupStack::PopAndDestroy(object);

		CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(1);
		prompt->SetStartPosition(10);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		object = CEmsSoundIE::NewL(KOasisMelody());
		object->SetStartPosition(11);
		CleanupStack::PushL(object);
		smsMessage->AddEMSInformationElementL(*object);
		CleanupStack::PopAndDestroy(object);

		prompt = CEmsUserPrompt::NewL(1);
		prompt->SetStartPosition(11);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		// Add ODI at position 10 covering first UP not second
		CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(1, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(10);
		CleanupStack::PushL(odi);
		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

		// Attempt to remove second UP (no controlling ODI), should pass
		RPointerArray<CEmsInformationElement>* array=smsMessage->RemoveEMSInformationElementsL(11,CSmsInformationElement::ESmsEnhancedUserPromptIndicator);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Replace user prompt at position 11
		prompt = CEmsUserPrompt::NewL(1);
		prompt->SetStartPosition(11);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		// Add second ODI at position 11 covering second UP
		odi = CEmsObjectDistribution::NewL(1, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(11);
		CleanupStack::PushL(odi);
		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

		// Attempt to remove second UP (controlling ODI), should fail
		array=smsMessage->RemoveEMSInformationElementsL(11,CSmsInformationElement::ESmsEnhancedUserPromptIndicator);
		TESTL(array==NULL);

		// Attempt to remove second ODI (controlling ODI), should pass
		array=smsMessage->RemoveEMSInformationElementsL(11,CSmsInformationElement::ESmsEnhancedODI);
		TESTL(array!=NULL);

		//
		CleanupStack::PushL(array);
		TESTL(array->Count()==1);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}

