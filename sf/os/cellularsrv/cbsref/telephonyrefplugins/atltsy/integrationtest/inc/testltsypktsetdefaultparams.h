//  TestLtsyPktSetDefaultParams.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktSetDefaultParams


#ifndef TESTLTSYPKTSETDEFAULTPARAMS_H_
#define TESTLTSYPKTSETDEFAULTPARAMS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktSetDefaultParams : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktSetDefaultParams(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktSetDefaultParams();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktSetDefaultParams, "TestLtsyPktSetDefaultParams");

#endif // TESTLTSYPKTSETDEFAULTPARAMS_H_
