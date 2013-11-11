//  TestLtsyPktActivateContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktActivateContext


#ifndef TESTLTSYPKTACTIVATECONTEXT_H_
#define TESTLTSYPKTACTIVATECONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktActivateContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktActivateContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktActivateContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktActivateContext, "TestLtsyPktActivateContext");

#endif // TESTLTSYPKTACTIVATECONTEXT_H_
