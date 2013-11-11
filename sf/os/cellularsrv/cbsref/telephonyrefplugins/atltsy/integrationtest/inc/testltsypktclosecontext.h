//  TestLtsyPktCloseContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktCloseContext


#ifndef TESTLTSYPKTCLOSECONTEXT_H_
#define TESTLTSYPKTCLOSECONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to close packet context
 */
class CTestLtsyPktCloseContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktCloseContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktCloseContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktCloseContext, "TestLtsyPktCloseContext");

#endif // TESTLTSYPKTCLOSECONTEXT_H_
