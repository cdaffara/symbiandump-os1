//  TestLtsyPktSetContextQoSProfile.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktSetContextQoSProfile


#ifndef TESTLTSYPKTSETCONTEXTQOSPROFILE_H_
#define TESTLTSYPKTSETCONTEXTQOSPROFILE_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktSetContextQoSProfile : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktSetContextQoSProfile(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktSetContextQoSProfile();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktSetContextQoSProfile, "TestLtsyPktSetContextQoSProfile");

#endif // TESTLTSYPKTSETCONTEXTQOSPROFILE_H_
