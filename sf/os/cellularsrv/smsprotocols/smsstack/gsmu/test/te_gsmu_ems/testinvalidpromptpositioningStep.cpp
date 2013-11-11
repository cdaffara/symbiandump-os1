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
#include "testinvalidpromptpositioningStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestinvalidpromptpositioningStep::doTestStepL()
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

		CEmsUserPrompt* prompt = CEmsUserPrompt::NewL(2);
		prompt->SetStartPosition(10);
		CleanupStack::PushL(prompt);
		TInt ret;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*prompt));
		TEST_CHECKL(ret,KErrArgument,_L("Argument"));
		CEmsSoundIE* object2a = CEmsSoundIE::NewL(KBarbieMelody());
		object2a->SetStartPosition(10);
		CleanupStack::PushL(object2a);
		smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);

		smsMessage->AddEMSInformationElementL(*prompt);
		CleanupStack::PopAndDestroy(prompt);

		//
		//
		RPointerArray<CEmsInformationElement>* array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		TESTL(array==NULL);

		CEmsSoundIE* object3 = CEmsSoundIE::NewL(KBarbieMelody());
		object3->SetStartPosition(11);
		CleanupStack::PushL(object3);
		smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		TESTL(array==NULL);

		//
		array=smsMessage->RemoveEMSInformationElementsL(11,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		CleanupStack::PushL(array);
		TESTL(array->Count()==1);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}


