//  TestLtsyPktDeactivateContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktDeactivateContext


#ifndef TESTLTSYPKTDEACTIVATECONTEXT_H_
#define TESTLTSYPKTDEACTIVATECONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to deactivate a packet context
 */
class CTestLtsyPktDeactivateContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktDeactivateContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktDeactivateContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktDeactivateContext, "TestLtsyPktDeactivateContext");

#endif // TESTLTSYPKTDEACTIVATECONTEXT_H_
