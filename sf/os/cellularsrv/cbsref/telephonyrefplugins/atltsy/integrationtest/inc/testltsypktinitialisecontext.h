//  TestLtsyPktInitialiseContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktInitialiseContext


#ifndef TESTLTSYPKTINITIALISECONTEXT_H_
#define TESTLTSYPKTINITIALISECONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktInitialiseContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktInitialiseContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktInitialiseContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktInitialiseContext, "TestLtsyPktInitialiseContext");

#endif // TESTLTSYPKTINITIALISECONTEXT_H_
