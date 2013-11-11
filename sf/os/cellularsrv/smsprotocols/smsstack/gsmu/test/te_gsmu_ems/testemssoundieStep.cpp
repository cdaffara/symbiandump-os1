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
#include "testemssoundieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemssoundieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		CEmsSoundIE* object = CEmsSoundIE::NewL(KBarbieMelody());
		CleanupStack::PushL(object);
		object->SetStartPosition(257);
		CEmsSoundIE* object2 = static_cast<CEmsSoundIE*>(object->DuplicateL());
		CleanupStack::PushL(object2);
		object2->CopyL(*object);
		const HBufC8* melody=object2->Melody();
		TPtrC8 ptr(KBarbieMelody());
		TEST(Mem::Compare(melody->Ptr(),melody->Size(), ptr.Ptr(),ptr.Size())==0);
		CleanupStack::PopAndDestroy(object2);
		CleanupStack::PopAndDestroy(object);

	return TestStepResult();
	}


