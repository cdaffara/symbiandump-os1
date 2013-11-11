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
#include "testremovalofodicontrollediesStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestremovalofodicontrollediesStep::doTestStepL()
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

		CEmsObjectDistribution* odi = CEmsObjectDistribution::NewL(2, CEmsObjectDistribution::EForward);
		odi->SetStartPosition(10);
		CleanupStack::PushL(odi);
		TInt ret;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*odi));
		TEST_CHECKL(ret,KErrArgument,_L("Argument"));
		CEmsSoundIE* object2a = CEmsSoundIE::NewL(KBarbieMelody());
		object2a->SetStartPosition(10);
		CleanupStack::PushL(object2a);
		smsMessage->AddEMSInformationElementL(*object2a);
		CleanupStack::PopAndDestroy(object2a);

		CEmsSoundIE* object3 = CEmsSoundIE::NewL(KBarbieMelody());
		object3->SetStartPosition(10);
		CleanupStack::PushL(object3);
		smsMessage->AddEMSInformationElementL(*object3);
		CleanupStack::PopAndDestroy(object3);

		smsMessage->AddEMSInformationElementL(*odi);
		CleanupStack::PopAndDestroy(odi);

		//
		//

		// Attempt to remove IEs within control of ODI
		RPointerArray<CEmsInformationElement>* array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Remove controlling ODI
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedODI);
		CleanupStack::PushL(array);
		TESTL(array!=NULL);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);

		// Repeat attempt to remove IEs, should now succeed
		array=smsMessage->RemoveEMSInformationElementsL(10,CSmsInformationElement::ESmsEnhancedUserDefinedSound);
		TESTL(array!=NULL);

		//
		CleanupStack::PushL(array);
		TESTL(array->Count()==2);
		array->ResetAndDestroy();
		CleanupStack::PopAndDestroy(array);
		CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}

