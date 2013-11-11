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

#include "tgsa_step_fleximergesortsamefile.h"
#include "gsatestappgood.h"
#include <f32file.h>
#include <s32file.h>

/**
Old Test CaseID 		AFSS-GSA-0038
New Test CaseID 		DEVSRVS-SSPLUGINS-GSA-0038
*/
void CGsaFlexiMergeSortSameFileTest::DoTestFlexiMergeSortSameFileL()
	{
	INFO_PRINTF1(_L("DoTestFlexiMergeSortSameFileL test started...."));
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
	INFO_PRINTF1(_L("DoTestFlexiMergeSortSameFileL case completed\n"));
	}

//---------------- CGsaFlexiMergeSortSameFileTest step ---------------------------------------


CGsaFlexiMergeSortSameFileTest::~CGsaFlexiMergeSortSameFileTest()
	{
	}

CGsaFlexiMergeSortSameFileTest::CGsaFlexiMergeSortSameFileTest()
	{
	SetTestStepName(KTGsaFlexiMergeSortSameFileStep);
	}

TVerdict CGsaFlexiMergeSortSameFileTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

TVerdict CGsaFlexiMergeSortSameFileTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

TVerdict CGsaFlexiMergeSortSameFileTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CGsaFlexiMergeSortSameFileTest started...."));
	
	__UHEAP_MARK;
	// Run the tests
	TRAPD(err, DoTestFlexiMergeSortSameFileL());
	INFO_PRINTF2(_L("DoTestFlexiMergeSortSameFileL test completed with %d...."),err);
	TEST(err == KErrNone);
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("....CGsaFlexiMergeSortSameFileTest completed!!"));
	return TestStepResult();
	}
