//  TestLtsyPktCloseContextQos.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktCloseContextQos


#ifndef TESTLTSYPKTCLOSECONTEXTQOS_H_
#define TESTLTSYPKTCLOSECONTEXTQOS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktCloseContextQos : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktCloseContextQos(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktCloseContextQos();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktCloseContextQos, "TestLtsyPktCloseContextQos");

#endif // TESTLTSYPKTCLOSECONTEXTQOS_H_
