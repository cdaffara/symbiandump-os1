//  TestLtsyPktGetStaticCapabilities.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetStaticCapabilities


#ifndef TESTLTSYPKTGETSTATICCAP_H_
#define TESTLTSYPKTGETSTATICCAP_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktGetStaticCapabilities : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetStaticCapabilities(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetStaticCapabilities();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetStaticCapabilities, "TestLtsyPktGetStaticCapabilities");

#endif // TESTLTSYPKTGETSTATICCAP_H_
