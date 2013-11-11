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
#include "testremovenonexistingobjectStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestremovenonexistingobjectStep::doTestStepL()
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

	CEmsFormatIE* object = CEmsFormatIE::NewL();
	object->SetStartPosition(10);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);
	RPointerArray<CEmsInformationElement>* array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedTextFormatting);
	CleanupStack::PushL(array);
	TESTL(array->Count()==1);
	object=(CEmsFormatIE*)(*array)[0];
	array->Remove(0);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	smsMessage->ResetEMSL();
	array->ResetAndDestroy();
	CleanupStack::PopAndDestroy(array);

	array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedTextFormatting);
	TESTL(array==NULL);


	object = CEmsFormatIE::NewL();
	object->SetStartPosition(15);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	CleanupStack::PushL(object);
	smsMessage->AddEMSInformationElementL(*object);

	CEmsFormatIE* object2= (CEmsFormatIE*)object->DuplicateL();
	CleanupStack::PopAndDestroy(object);

	object2->SetStartPosition(15);
	object->SetFormatLength(20);
	CleanupStack::PushL(object2);
	smsMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);

	array=smsMessage->RemoveEMSInformationElementsL(15,CSmsInformationElement::ESmsEnhancedTextFormatting);
	CleanupStack::PushL(array);
	TESTL(array->Count()==2);
	array->ResetAndDestroy();
	CleanupStack::PopAndDestroy(array);

	CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}

