// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_PROCFAILRETRY1PANIC_H__)
#define __APPFWK_SYSSTART_TEST_STEP_PROCFAILRETRY1PANIC_H__

#include "appfwk_sysstart_test_step_AppBase.h"

/**
Literal used as name for test case.
*/
_LIT(KCAppfwkSysStartTestStepProcFailRetry1Panic,"ProcFailRetry1Panic");

/**
CAppfwkSysStartTestStepProcFailRetry1Panic encapsulates the functionality required 
for the ProcFailRetry0Panic SysStart test case.
*/
class CAppfwkSysStartTestStepProcFailRetry1Panic : public CAppfwkSysStartTestStepAppBase
	{
public:
	CAppfwkSysStartTestStepProcFailRetry1Panic();
	~CAppfwkSysStartTestStepProcFailRetry1Panic();
	virtual TVerdict doTestStepL();
	void TestProcFailRetry1PanicL();
	};

#endif


