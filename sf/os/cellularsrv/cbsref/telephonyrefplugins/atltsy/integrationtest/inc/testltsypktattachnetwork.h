//  TestLtsyPktAttachNetwork.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktAttachNetwork

#ifndef TESTLTSYPKTATTACHNETWORK_H_
#define TESTLTSYPKTATTACHNETWORK_H_

// user include
#include "TestLtsyPktTestStepBase.h"

// const define
const TInt KMaxAllowedContextNum = 11;

/**
 * This is the test step class which calls methods of base class
 * to attach packet service to network
 */
class CTestLtsyPktAttachNetwork : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktAttachNetwork(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktAttachNetwork();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktAttachNetwork, "TestLtsyPktAttachNetwork");

#endif // TESTLTSYPKTATTACHNETWORK_H_
