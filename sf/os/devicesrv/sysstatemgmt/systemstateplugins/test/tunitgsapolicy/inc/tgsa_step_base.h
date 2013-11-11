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
// This class wraps up all the functionality concerned with creating and using an Active 
// Scheduler within the std test environment.  Tests that want to use an Active Scheduler 
// should derive from this class.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#ifndef __TGSA_STEP_BASE_H__
#define __TGSA_STEP_BASE_H__

#include <test/testexecutestepbase.h>
#include "ssmdebug.h"

class CActiveScheduler;

class CGsaTestBase : public CTestStep
	{
public:
	CGsaTestBase(const TDesC& aStepName);
	~CGsaTestBase();

	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL() = 0;
	TVerdict doTestStepPostambleL();

	void StartScheduler();	// adds a low-priority callback to stop the scheduler when there are no more requests
	void StopScheduler();

private:
	CActiveScheduler* iActiveScheduler;
	CAsyncCallBack* iAsyncStopScheduler;
	CAsyncCallBack* iAsyncCancelCommands;

	TBool iActivateCancel;
	};

#endif	// __TGSA_STEP_BASE_H__
