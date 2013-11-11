//  TestLtsyPktGetContextInfo.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetContextInfo


#ifndef TESTLTSYPKTGETCONTEXTINFO_H_
#define TESTLTSYPKTGETCONTEXTINFO_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktGetContextInfo : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetContextInfo(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetContextInfo();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetContextInfo, "TestLtsyPktGetContextInfo");

#endif // TESTLTSYPKTGETCONTEXTINFO_H_
