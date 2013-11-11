//  TestLtsyPktGetServiceStatus.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetServiceStatus


#ifndef TESTLTSYPKTGETSERVICESTATUS_H_
#define TESTLTSYPKTGETSERVICESTATUS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktGetServiceStatus : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetServiceStatus(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetServiceStatus();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetServiceStatust, "TestLtsyPktGetServiceStatust");

#endif // TESTLTSYPKTGETSERVICESTATUS_H_
