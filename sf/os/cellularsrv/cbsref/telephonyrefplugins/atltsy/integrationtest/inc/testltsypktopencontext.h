//  TestLtsyPktOpenContext.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktOpenContext


#ifndef TESTLTSYPKTOPENCONTEXT_H_
#define TESTLTSYPKTOPENCONTEXT_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktOpenContext : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktOpenContext(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktOpenContext();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktOpenContext, "TestLtsyPktOpenContext");

#endif // TESTLTSYPKTOPENCONTEXT_H_
