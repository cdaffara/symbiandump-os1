//  TestLtsyPktDetachNetwork.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktDetachNetwork


#ifndef TESTLTSYPKTDETACHNETWORK_H_
#define TESTLTSYPKTDETACHNETWORK_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to detach packet service from network
 */
class CTestLtsyPktDetachNetwork : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktDetachNetwork(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktDetachNetwork();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktDetachNetwork, "TestLtsyPktDetachNetwork");

#endif // TESTLTSYPKTDETACHNETWORK_H_
