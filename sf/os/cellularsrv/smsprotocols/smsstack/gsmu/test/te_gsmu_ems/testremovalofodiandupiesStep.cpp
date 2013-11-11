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
#include "testremovalofodiandupiesStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestremovalofodiandupiesStep::doTestStepL()
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

		CEmsSoundIE* object2 = CEmsSoundIE::NewL(KOasisMelody());
		object2->SetStartPosition(10);
		CleanupStack::PushL(object2);
		smsMessage->AddEMSInformationElementL(*object2);
		CleanupStack::PopAndDestroy(object2);

		CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(1);
		prompt->SetStartPosition(10);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		// Attempt to remove UP (no controlling ODI)
		RPointerArray<CEmsInformationElement>* array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserPromptIndicator);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Replace user prompt
		prompt = CEmsUserPrompt::NewL(1);
		prompt->SetStartPosition(10);
		CleanupStack::PushL(prompt);
		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(2, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(10);
		CleanupStack::PushL(odi);
		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

		// Attempt to remove UP (controlling ODI), should fail
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserPromptIndicator);
		TESTL(array==NULL);

		// Attempt to remove controlling ODI
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedODI);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Attempt to remove UP now possible
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserPromptIndicator);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Finally remove remaining IE
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		TESTL(array!=NULL);

		//
		CleanupStack::PushL(array);
		TESTL(array->Count()==1);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}

