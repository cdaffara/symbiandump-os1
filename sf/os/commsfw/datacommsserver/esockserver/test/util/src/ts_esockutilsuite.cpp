// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains main DLL entry point for dll and definition of test suite class.
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include <networking/log.h>
#include <networking/teststep.h>
#include <networking/testsuite.h>

// Test includes
#include "ts_esockutilsuite.h"
#include "ts_esockutilsteps.h"

EXPORT_C CTestSuiteESockUtil* NewTestSuiteESockUtil() 
    { 
	return new CTestSuiteESockUtil;
    }

// Make a version string available for test system 
TPtrC CTestSuiteESockUtil::GetVersion()
	{
#ifdef _DEBUG
	_LIT(KTxtVersion,"1.000 (udeb)");
#else
	_LIT(KTxtVersion,"1.000");
#endif
	return KTxtVersion();
	}

void CTestSuiteESockUtil::InitialiseL()
	{
	AddTestStepL(new(ELeave)CTestStepLoadESock);
	AddTestStepL(new(ELeave)CTestStepUnloadESock);
	AddTestStepL(new(ELeave)CTestStepStopAllInterfaces);
	AddTestStepL(new(ELeave)CTestStepUnloadESockForced);
	AddTestStepL(new(ELeave)CTestStepUnloadESockMayLeak);
	AddTestStepL(new(ELeave)CTestStepUnloadESockLeaks);
	AddTestStepL(new(ELeave)CTestStepDefaultToConfig1);
	AddTestStepL(new(ELeave)CTestStepAlwaysFail);
	}

GLDEF_C TInt E32Dll()
	{
	return 0;
	}

