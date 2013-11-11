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

#if (!defined __SYSSTARTDLLTESTSTEP_H__)
#define __SYSSTARTDLLTESTSTEP_H__

#include <test/testexecutestepbase.h>
#include "appfwk_sysstart_test_server.h"
#include "appfwk_sysstart_test_dll.h"

_LIT(KCAppfwkSysStartTestStepDllBase,"CAppfwkSysStartTestStepDllBase");

/**
CAppfwkSysStartTestStepDllBase provides additional utility methods to be used
when executing the DLL Function based test cases.
For example:
@see CAppfwkSysStartTestStepDllCsFailPanic
*/
class CAppfwkSysStartTestStepDllBase : public CTestStep
	{
public:
	CAppfwkSysStartTestStepDllBase();
	~CAppfwkSysStartTestStepDllBase();
	
protected:
	void TestDllResultsL(const TDesC& aFileName, const TTestDllResults& aExpectedResults);
	void TestEpocWindResultsL(const TDesC& aFileName, const TInt& aExpectedResult);
	void TestResultFileDoesNotExist(const TDesC& aFileName);
	void ReadTestDllResultsFileL(const TDesC& aFileName, TTestDllResults& aTestDllResults);
	void ReadTestDllEpocWindResultsFileL(const TDesC& aFileName, TInt& aTestDllResults);
	
protected:
	TInt iCallCount;
	TInt iArg1;
	TInt iArg2;
	TInt iArg3;
	RFs  iFs;
	};

#endif


