//  TestLtsyPktGetContextQoSProfile.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktGetContextQoSProfile


#ifndef TESTLTSYPKTGETCONTEXTQOSPROFILE_H_
#define TESTLTSYPKTGETCONTEXTQOSPROFILE_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to activate a primary packet context
 */
class CTestLtsyPktGetContextQoSProfile : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktGetContextQoSProfile(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktGetContextQoSProfile();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktGetContextQoSProfile, "TestLtsyPktGetContextQoSProfile");

#endif // TESTLTSYPKTGETCONTEXTQOSPROFILE_H_
