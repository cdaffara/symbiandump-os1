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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_STFPCSTOCDRETRY1IGNORE_H__)
#define __APPFWK_SYSSTART_TEST_STEP_STFPCSTOCDRETRY1IGNORE_H__

#include "appfwk_sysstart_test_step_StfpBase.h"

/**
Literal used as name for test case.
*/
_LIT(KCAppfwkSysStartTestStepStfpCsToCdRetry1Ignore,"StfpCsToCdRetry1Ignore");

/**
CAppfwkSysStartTestStepStfpCsToCdRetry1Ignore encapsulates the functionality required 
for the StfpCsToCdRetry1Ignore SysStart test case.
*/
class CAppfwkSysStartTestStepStfpCsToCdRetry1Ignore : public CAppfwkSysStartTestStepStfpBase
	{
public:
	CAppfwkSysStartTestStepStfpCsToCdRetry1Ignore();
	~CAppfwkSysStartTestStepStfpCsToCdRetry1Ignore();
	virtual TVerdict doTestStepL();
	void TestStfpCsToCdRetry1IgnoreL();
	};

#endif


