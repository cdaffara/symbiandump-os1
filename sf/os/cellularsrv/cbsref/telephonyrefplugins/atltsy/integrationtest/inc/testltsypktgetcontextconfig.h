//  TestLtsyPktGetContextConfig.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetContextConfig


#ifndef TESTLTSYPKTGETCONTEXTCONFIG_H_
#define TESTLTSYPKTGETCONTEXTCONFIG_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktGetContextConfig : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetContextConfig(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetContextConfig();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetContextConfig, "TestLtsyPktGetContextConfig");

#endif // TESTLTSYPKTGETCONTEXTCONFIG_H_
