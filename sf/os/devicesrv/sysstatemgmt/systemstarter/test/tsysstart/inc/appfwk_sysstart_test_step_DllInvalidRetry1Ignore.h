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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_DLLINVALIDRETRY1IGNORE_H__)
#define __APPFWK_SYSSTART_TEST_STEP_DLLINVALIDRETRY1IGNORE_H__

#include "appfwk_sysstart_test_step_DllBase.h"

/**
Literal used as name for test case.
*/
_LIT(KCAppfwkSysStartTestStepDllInvalidRetry1Ignore,"DllInvalidRetry1Ignore");

/**
CAppfwkSysStartTestStepDllInvalidRetry1Ignore encapsulates the functionality required 
for the DllInvalidRetry1Ignore SysStart test case.
*/
class CAppfwkSysStartTestStepDllInvalidRetry1Ignore : public CAppfwkSysStartTestStepDllBase
	{
public:
	CAppfwkSysStartTestStepDllInvalidRetry1Ignore();
	~CAppfwkSysStartTestStepDllInvalidRetry1Ignore();
	virtual TVerdict doTestStepL();
	void TestDllInvalidRetry1IgnoreL();
	};

#endif


