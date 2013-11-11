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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_APPBASE_H__)
#define __APPFWK_SYSSTART_TEST_STEP_APPBASE_H__

#include <test/testexecutestepbase.h>
#include "appfwk_sysstart_test_server.h"

_LIT(KAppfwkSysStartTestStepAppBase,"CAppfwkSysStartTestStepAppBase");

/**
CAppfwkSysStartTestStepAppBase provides additional utility methods to be used
when executing test cases utilising the test apps
For example:
@see CAppfwkSysStartTestStepProcFailRetry0Ignore
*/
class CAppfwkSysStartTestStepAppBase : public CTestStep
	{
public:
	CAppfwkSysStartTestStepAppBase();
	~CAppfwkSysStartTestStepAppBase();

public:
	void TestAppStartCount(const TInt&, const TInt&);
	void TestAppRendCount(const TInt&, const TInt&);
	void TestAppRendFailCount(const TInt&, const TInt&);
	void TestResultFileDoesNotExistL(const TDesC&);
	void TestEpocWindResultsL(const TDesC&, const TInt&);
	};

#endif


