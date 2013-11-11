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
#include "testemsformatieStep.h"
#include "Te_gsmu_emsSuiteDefs.h"


TVerdict CtestemsformatieStep::doTestStepL()
/**
 *  @return - TVerdict code
 *  Override of base class pure virtual
 *  Our implementation only gets called if the base class doTestStepPreambleL() did
 *  not leave. That being the case, the current test result value will be EPass.
 */
	{
		CEmsFormatIE* object = CEmsFormatIE::NewL();
		CleanupStack::PushL(object);

		object->SetStartPosition(100);
		object->SetFormatLength(120);
		object->SetBold(ETrue);
		object->SetItalic(ETrue);
		object->SetUnderline(ETrue);
		object->SetStrikethrough(ETrue);
		object->SetAlignment(CEmsFormatIE::ECenter);
		object->SetFontSize(CEmsFormatIE::ELarge);

		CEmsFormatIE* object2 = (CEmsFormatIE*)object->DuplicateL();
		CleanupStack::PushL(object2);

		object2->SetFontSize(CEmsFormatIE::ESmall);

		TEST_CHECKL(object2->Bold(),ETrue,_L("Bold not set"));
		TEST_CHECKL(object2->StartPosition(),100,_L("Start position not set"));
		TEST_CHECKL(object2->FormatLength(),120,_L("Format length not set"));
		TEST_CHECKL(object2->Italic(),ETrue,_L("Italic not set"));
		TEST_CHECKL(object2->Underline(),ETrue,_L("Underline not set"));
		TEST_CHECKL(object2->Strikethrough(),ETrue,_L("Strikethrough not set"));
		TEST_CHECKL(object2->Alignment(),CEmsFormatIE::ECenter,_L("Alignement not set"));
		TEST_CHECKL(object2->FontSize(),CEmsFormatIE::ESmall,_L("Font size not set"));


		CEmsFormatIE* object3 = CEmsFormatIE::NewL();
		CleanupStack::PushL(object3);
		object3->CopyL(*object2);

		TEST_CHECKL(object3->Bold(),ETrue,_L("Bold not set"));
		TEST_CHECKL(object3->StartPosition(),100,_L("Start position not set"));
		TEST_CHECKL(object3->FormatLength(),120,_L("Format length not set"));
		TEST_CHECKL(object3->Italic(),ETrue,_L("Italic not set"));
		TEST_CHECKL(object3->Underline(),ETrue,_L("Underline not set"));
		TEST_CHECKL(object3->Strikethrough(),ETrue,_L("Strikethrough not set"));
		TEST_CHECKL(object3->Alignment(),CEmsFormatIE::ECenter,_L("Alignement not set"));
		TEST_CHECKL(object3->FontSize(),CEmsFormatIE::ESmall,_L("Font size not set"));

		// Test for defect LEE-5AJDQR, default alignment should be 11

		CEmsFormatIE* object4 = CEmsFormatIE::NewL();
		CleanupStack::PushL(object4);

		object4->SetStartPosition(10);
		object4->SetFormatLength(20);
		object4->SetBold(ETrue);
		TEST_CHECKL(object4->Alignment(),CEmsFormatIE::ELangDepend,_L("Wrong default alignement"));


		CleanupStack::PopAndDestroy(object4);
		CleanupStack::PopAndDestroy(object3);
		CleanupStack::PopAndDestroy(object2);
		CleanupStack::PopAndDestroy(object);

	return TestStepResult();
	}


