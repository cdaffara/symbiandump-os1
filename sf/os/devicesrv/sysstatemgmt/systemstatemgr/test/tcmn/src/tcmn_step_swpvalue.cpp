// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ssm/ssmswp.h>

#include "tcmn_step_swpvalue.h"

CCmnSwpValueTest::~CCmnSwpValueTest()
	{
	}

CCmnSwpValueTest::CCmnSwpValueTest()
	{
	SetTestStepName(KTCmnSwpValueStep);
	}

TVerdict CCmnSwpValueTest::doTestStepPreambleL()
	{
	return CTestStep::doTestStepPreambleL();
	}

/**
Old Test CaseID 		APPFWK-CMN-0003
New Test CaseID 		DEVSRVS-SSMA-CMN-0003
 */

TVerdict CCmnSwpValueTest::doTestStepL()
	{
	INFO_PRINTF1(_L("CCmnSwpValueTest started..."));

	__UHEAP_MARK;
	doTestConstructors();
	doTestOperators();
	doTestOtherMethods();
	__UHEAP_MARKEND;

	INFO_PRINTF1(_L("...CCmnSwpValueTest completed!"));
	return TestStepResult();
	}

TVerdict CCmnSwpValueTest::doTestStepPostambleL()
	{
	return CTestStep::doTestStepPostambleL();
	}

void CCmnSwpValueTest::doTestConstructors()
	{
	INFO_PRINTF1(_L("Lower bound test for constructor with two arguments"));
	const TUint key = {0x1};
	TSsmSwp r1(key,1);
	TEST(r1.Key() == key);
	TEST(r1.Value() == 1);
	
	INFO_PRINTF1(_L("Upper bound test for constructor with two arguments"));
	const TUint ff = {0xFFFFFFFF}; //lint !e569 Loss of information
	TSsmSwp sF(ff, KMaxTInt);
	TEST(sF.Key() == ff);
	TEST(sF.Value() == KMaxTInt);
	
	INFO_PRINTF1(_L("Testing copy constructor"));
	TSsmSwp copy(r1);
	TEST(copy.Key() == r1.Key());
	TEST(copy.Value() == r1.Value());
	}
	
void CCmnSwpValueTest::doTestOperators()
	{
	INFO_PRINTF1(_L("Testing TSsmSwp operators"));
	const TUint dead = {0xDEAD};
	TSsmSwp a(dead, 0xBEEF);
	const TUint baad = {0xBAAD};
	TSsmSwp b(baad, 0xF00D);
	
	TSsmSwp c = a;
	TEST(c.Key() == a.Key());
	TEST(c.Value() == a.Value());
	TEST(c == a);
	
	a = b;
	a = a; //to increase coverage
	TEST(b.Key() == a.Key());
	TEST(b.Value() == a.Value());
	TEST(b == a);

	TEST(c.Key() != a.Key());
	TEST(c.Value() != a.Value());
	TEST(c != a);
	}

void CCmnSwpValueTest::doTestOtherMethods()
	{
	INFO_PRINTF1(_L("Testing remaining methids in TSsmSwp"));
	const TUint ff = {0xFFFFFFFF}; //lint !e569 Loss of information
	TSsmSwp swp(ff, KMaxTInt);
	TEST(swp.Key() == ff);
	TEST(swp.Value() == KMaxTInt);
	
	swp.Set(0, 0);
	TEST(swp.Key() == 0);
	TEST(swp.Value() == 0);
	
	const TUint cafe = {0xCAFE};
	swp.Set(cafe,0xBABE);
	TEST(swp.Key() == cafe);
	TEST(swp.Value() == 0xBABE);
	}
