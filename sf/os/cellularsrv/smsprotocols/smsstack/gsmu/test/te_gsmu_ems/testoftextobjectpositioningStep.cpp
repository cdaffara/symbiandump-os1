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
#include "testoftextobjectpositioningStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestoftextobjectpositioningStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		_LIT(KTestEmsMsg8Bit1,"");

		CSmsMessage* smsMessage=CreateSmsMessageL(KTestEmsMsg8Bit1,TSmsDataCodingScheme::ESmsAlphabet8Bit);
		CleanupStack::PushL(smsMessage);

		CEmsSoundIE* object2 = CEmsSoundIE::NewL(KItchyMelody());
		CleanupStack::PushL(object2);
		object2->SetStartPosition(1);
		TInt ret=KErrNone;
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object2));
		TEST_CHECKL(ret,KErrOverflow,_L("Overflow "));
		object2->SetStartPosition(2000);
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object2));
		TEST_CHECKL(ret,KErrOverflow,_L("Overflow 2nd"));
		object2->SetStartPosition(0);
		TRAP(ret,smsMessage->AddEMSInformationElementL(*object2));
		TEST_CHECKL(ret,KErrNone,_L("Overflow shall not appear"));
		CleanupStack::PopAndDestroy(object2);
		CleanupStack::PopAndDestroy(smsMessage);

	return TestStepResult();
	}


