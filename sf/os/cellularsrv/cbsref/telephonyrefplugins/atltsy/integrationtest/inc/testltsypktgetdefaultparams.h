//  TestLtsyPktGetDefaultParams.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetDefaultParams


#ifndef TESTLTSYPKTGETDEFAULTPARAMS_H_
#define TESTLTSYPKTGETDEFAULTPARAMS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktGetDefaultParams : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetDefaultParams(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetDefaultParams();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetDefaultParams, "TestLtsyPktGetDefaultParams");

#endif // TESTLTSYPKTGETDEFAULTPARAMS_H_
