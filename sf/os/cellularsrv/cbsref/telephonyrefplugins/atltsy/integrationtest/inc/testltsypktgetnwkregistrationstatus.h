//  TestLtsyPktNetworkRegistrationStatus.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktNetworkRegistrationStatus


#ifndef TESTLTSYPKTNWKREGSTATUS_H_
#define TESTLTSYPKTNWKREGSTATUS_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktNetworkRegistrationStatus : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktNetworkRegistrationStatus(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktNetworkRegistrationStatus();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktNetworkRegistrationStatus, "TestLtsyPktNetworkRegistrationStatus");

#endif // TESTLTSYPKTNWKREGSTATUS_H_
