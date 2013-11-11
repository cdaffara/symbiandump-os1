//  TestLtsyPktDeleteContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktDeleteContext


#ifndef TESTLTSYPKTDELETECONTEXT_H_
#define TESTLTSYPKTDELETECONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktDeleteContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktDeleteContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktDeleteContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktDeleteContext, "TestLtsyPktDeleteContext");

#endif // TESTLTSYPKTDELETECONTEXT_H_
