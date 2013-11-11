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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_PROCFAILRETRY0IGNORE_H__)
#define __APPFWK_SYSSTART_TEST_STEP_PROCFAILRETRY0IGNORE_H__

#include "appfwk_sysstart_test_step_AppBase.h"

/**
Literal used as name for test case.
*/
_LIT(KCAppfwkSysStartTestStepProcFailRetry0Ignore,"ProcFailRetry0Ignore");

/**
CAppfwkSysStartTestStepProcFailRetry0Ignore encapsulates the functionality required 
for the ProcFailRetry0Ignore SysStart test case.
*/
class CAppfwkSysStartTestStepProcFailRetry0Ignore : public CAppfwkSysStartTestStepAppBase
	{
public:
	CAppfwkSysStartTestStepProcFailRetry0Ignore();
	~CAppfwkSysStartTestStepProcFailRetry0Ignore();
	virtual TVerdict doTestStepL();
	void TestProcFailRetry0IgnoreL();
	};

#endif


