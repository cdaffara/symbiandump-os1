// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
 

#ifndef __SSMSWPPOLICYMAPTEST_H__
#define __SSMSWPPOLICYMAPTEST_H__

#include <test/testexecutestepbase.h>
#include <ssm/ssmswp.h>

_LIT(KTSsmSwpPolicyMapStep,"SsmSwpPolicyMapStep");

class CSsmSwpPolicyMap;

class CSsmSwpPolicyMapTest : public CTestStep
	{
public:
	CSsmSwpPolicyMapTest();
	~CSsmSwpPolicyMapTest();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
private:	
	void doTestForMemoryLeaksL();
	void doTestForInsertionOrderL();
	void doTestForRandomAccessL();
	void doTestHeapFailureL();
	void doTestItemMissingL();
	void doItemMissingL(CSsmSwpPolicyMap* aSwpPolicyMap);
	void doTestDoubleInsertionL();
	void doDoubleInsertionL(CSsmSwpPolicyMap* aSwpPolicyMap);
};


#endif // #ifndef __SSMSWPPOLICYMAPTEST_H__

