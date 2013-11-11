//  TestLtsyPktGetDynamicCapabilities.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetDynamicCapabilities


#ifndef TESTLTSYPKTGETDYNAICCAP_H_
#define TESTLTSYPKTGETDYNAICCAP_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktGetDynamicCapabilities : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetDynamicCapabilities(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetDynamicCapabilities();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetDynamicCapabilities, "TestLtsyPktGetDynamicCapabilities");

#endif // TESTLTSYPKTGETDYNAICCAP_H_
