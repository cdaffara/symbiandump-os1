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

#include "testaddingpredefsoundaftermelodyStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestaddingpredefsoundaftermelodyStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
	_LIT(KTestEmsMsg7Bit1,"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
 	TSmsDataCodingScheme::TSmsAlphabet alphabet=TSmsDataCodingScheme::ESmsAlphabet7Bit;

	CSmsMessage* emsMessage=CreateSmsMessageL(KTestEmsMsg7Bit1, alphabet);
	CleanupStack::PushL(emsMessage);

	CEmsSoundIE* object1 = CEmsSoundIE::NewL(KBarbieMelody());
	object1->SetStartPosition(1);
	CleanupStack::PushL(object1);

	CEmsPreDefSoundIE* object2 = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::EChordLow);
	object1->SetStartPosition(2);
	CleanupStack::PushL(object2);

	emsMessage->AddEMSInformationElementL(*object1);
	emsMessage->AddEMSInformationElementL(*object2);
	CleanupStack::PopAndDestroy(object2);
	CleanupStack::PopAndDestroy(object1);

	emsMessage->NumMessagePDUsL();

	CleanupStack::PopAndDestroy(emsMessage);

	return TestStepResult();
	}
