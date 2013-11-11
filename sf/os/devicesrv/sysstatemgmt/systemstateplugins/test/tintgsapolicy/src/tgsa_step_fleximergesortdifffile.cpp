// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "tgsa_step_fleximergesortdifffile.h"
#include "gsatestappgood.h"
#include <f32file.h>
#include <s32file.h>

/**
Old Test CaseID 		AFSS-GSA-0039
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0039
*/
void CGsaFlexiMergeSortDiffFileTest::DoTestFlexiMergeSortDiffFileL()
	{
	INFO_PRINTF1(_L("DoTestFlexiMergeSortDiffFileL test started...."));
	TInt propertyValue;
	TInt err = RProperty::Get(KTestAppUidCategory, KTestPropertyKey, propertyValue);
	//if the list is sorted as per priority, the propertyValue will be equal to KPropertySetValue
	TEST(err == KErrNone);
	if (KErrNone == err)
		{
		INFO_PRINTF2(_L("The Property Value : %d"), propertyValue);
		INFO_PRINTF2(_L("The Expected Value : %d"), KPropertySetValue);
		TEST (propertyValue == KPropertySetValue);
		}
	INFO_PRINTF1(_L("DoTestFlexiMergeSortDiffFileL case completed\n"));
	}

//---------------- CGsaFlexiMergeSortDiffFileTest step ---------------------------------------


CGsaFlexiMergeSortDiffFileTest::~CGsaFlexiMergeSortDiffFileTest()
	{
	}

CGsaFlexiMergeSortDiffFileTest::CGsaFlexiMergeSortDiffFileTest()
	{
	SetTestStepName(KTGsaFlexiMergeSortDiffFileStep);
	}

TVerdict CGsaFlexiMergeSortDiffFileTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaFlexiMergeSortDiffFileTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaFlexiMergeSortDiffFileTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaFlexiMergeSortDiffFileTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoTestFlexiMergeSortDiffFileL());
	INFO_PRINTF2(_L("DoTestFlexiMergeSortDiffFileL test completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaFlexiMergeSortDiffFileTest completed!!"));
	return TestStepResult();
	}
