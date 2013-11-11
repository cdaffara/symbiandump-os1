//  TestLtsyPktClosePacket.h
//  Copyright (c) Symbian Software Ltd 2008. All rights reserved.
//
//  Implementation of the Class CTestLtsyPktClosePacket


#ifndef TESTLTSYPKTCLOSEPACKET_H_
#define TESTLTSYPKTCLOSEPACKET_H_

// user include
#include "TestLtsyPktTestStepBase.h"

/**
 * This is the test step class which calls methods of base class
 * to open packet service
 */
class CTestLtsyPktClosePacket : public CTestLtsyPktTestStepBase
/**
 * @internalAll
 * @test
 */
	{
public:
	CTestLtsyPktClosePacket(CTestLtsyModel& aModel);
	virtual ~CTestLtsyPktClosePacket();
	
protected:
	TVerdict doTestStepL();
	};

_LIT(KTestLtsyPktClosePacket, "TestLtsyPktClosePacket");

#endif // TESTLTSYPKTCLOSEPACKET_H_
