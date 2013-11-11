//  TestLtsyPktOpenContextQos.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktOpenContextQos


#ifndef TESTLTSYPKTOPENCONTEXTQOS_H_
#define TESTLTSYPKTOPENCONTEXTQOS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet context
 */
class CTestLtsyPktOpenContextQos : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktOpenContextQos(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktOpenContextQos();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktOpenContextQos, "TestLtsyPktOpenContextQos");

#endif // TESTLTSYPKTOPENCONTEXTQOS_H_
