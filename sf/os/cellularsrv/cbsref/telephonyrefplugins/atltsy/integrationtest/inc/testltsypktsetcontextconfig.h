//  TestLtsyPktSetContextConfig.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktSetContextConfig


#ifndef TESTLTSYPKTSETCONTEXTCONFIG_H_
#define TESTLTSYPKTSETCONTEXTCONFIG_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktSetContextConfig : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktSetContextConfig(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktSetContextConfig();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktSetContextConfig, "TestLtsyPktSetContextConfig");

#endif // TESTLTSYPKTSETCONTEXTCONFIG_H_
