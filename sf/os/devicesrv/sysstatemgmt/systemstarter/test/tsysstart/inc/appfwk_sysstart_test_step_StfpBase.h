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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_STFPBASE_H__)
#define __APPFWK_SYSSTART_TEST_STEP_STFPBASE_H__

#include <test/testexecutestepbase.h>
#include "appfwk_sysstart_test_server.h"

_LIT(KCAppfwkSysStartTestStepStfpBase,"CAppfwkSysStartTestStepStfpBase");

/**
CAppfwkSysStartTestStepStfpBase provides additional utility methods to be used
when executing the State Transition Failure Policy based test cases.
NOTE: STFP - State Transition Failure Policy
For example:
@see CAppfwkSysStartTestStepStfpCdToNcDefault
*/
class CAppfwkSysStartTestStepStfpBase : public CTestStep
	{
public:
	CAppfwkSysStartTestStepStfpBase();
	~CAppfwkSysStartTestStepStfpBase();

public:
	void GetStfpResultsL();
	void TestCsCount(const TInt& aCsCount);
	void TestCsFailCount(const TInt& aCsFailCount);
	void TestCdCount(const TInt& aCdCount);
	void TestCdFailCount(const TInt& aCdFailCount);
	void TestNcCount(const TInt& aNcCount);
	void TestNcFailCount(const TInt& aCdFailCount);
	void TestForPanicL();
	
protected:
	TInt iCsCount;
	TInt iCdCount;
	TInt iNcCount;
	TInt iCsFailCount;
	TInt iCdFailCount;
	TInt iNcFailCount;
	RFs    iFs;
	};

#endif


