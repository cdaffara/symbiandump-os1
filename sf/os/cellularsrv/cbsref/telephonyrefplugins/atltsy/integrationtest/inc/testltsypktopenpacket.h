//  TestLtsyPktOpenPacket.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktOpenPacket


#ifndef TESTLTSYPKTOPENPACKET_H_
#define TESTLTSYPKTOPENPACKET_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet service
 */
class CTestLtsyPktOpenPacket : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktOpenPacket(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktOpenPacket();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktOpenPacket, "TestLtsyPktOpenPacket");

#endif // TESTTELEPHONYPKTACTIVATECONTEXT_H_
