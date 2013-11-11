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
#include "testemspredefsoundieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemspredefsoundieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		CEmsPreDefSoundIE* object = CEmsPreDefSoundIE::NewL( CEmsPreDefSoundIE::EChordLow);
		CleanupStack::PushL(object);
		object->SetStartPosition(22222);
		CEmsPreDefSoundIE* object2 = static_cast<CEmsPreDefSoundIE*>(object->DuplicateL());
		CleanupStack::PushL(object2);

		CEmsPreDefSoundIE* object3 = CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::ETaDa);
		CleanupStack::PushL(object3);
		object3->CopyL(*object2);
		TEST_CHECKL(object3->PredefinedSound(),object->PredefinedSound(),_L("Predefined sound type is not the same"));
		TEST_CHECKL(object3->StartPosition(),object->StartPosition(),_L("Start position is not the same"));

		CleanupStack::PopAndDestroy(object3);
		CleanupStack::PopAndDestroy(object2);
		CleanupStack::PopAndDestroy(object);
		TRAPD(ret,CEmsPreDefSoundIE::NewL(CEmsPreDefSoundIE::TPredefinedSound (0x0f)));
		TEST(ret == KErrArgument);

	return TestStepResult();
	}

