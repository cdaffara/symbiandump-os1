// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 

#ifndef __TSSM_STEP_SERVER_H__
#define __TSSM_STEP_SERVER_H__

#include <test/testexecutestepbase.h>

_LIT(KTSsmServerStep,"SsmServerStep");

class CSsmServerTest : public CTestStep
	{
public:
	CSsmServerTest();
	~CSsmServerTest();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void doSsmServerTestsL();
	void doTestForMemoryLeaksL();
	void doTestHeapFailureL();
	void doTestProcessCriticalL();
	void doTestConnectedSessionL();
private:
	void doSwpHandlingTestsL();
	void doTestMultipleSwpRequestsL();
	void doTestErrorenousSwpPolicyL();
	void doTestCancelSwpRequestsQueuedL();
	void doTestCancelSwpRequestsRunningL();
private:
	void doStateHandlingTestsL();
	void doTestMultipleStateRequestsL();
	void doTestFailStateRequestsL();
	void doTestValidStateRequestsCancelCurrentL();
	void doTestValidStateRequestsCancelQueuedL();

private:
	RSemaphore	iSemaphore;	//	This is used to wait for the swp request completion.
	};

#endif
