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

#if (!defined __APPFWK_SYSSTART_TEST_STEP_DLLINVALIDORDINAL0_H__)
#define __APPFWK_SYSSTART_TEST_STEP_DLLINVALIDORDINAL0_H__

#include "appfwk_sysstart_test_step_DllBase.h"

/**
Literal used as name for test case.
*/
_LIT(KCAppfwkSysStartTestStepDllInvalidOrdinal0,"DllInvalidOrdinal0");

/**
CAppfwkSysStartTestStepDllInvalidOrdinal0 encapsulates the functionality required 
for the DllInvalidOrdinal0 SysStart test case.
*/
class CAppfwkSysStartTestStepDllInvalidOrdinal0 : public CAppfwkSysStartTestStepDllBase
	{
public:
	CAppfwkSysStartTestStepDllInvalidOrdinal0();
	~CAppfwkSysStartTestStepDllInvalidOrdinal0();
	virtual TVerdict doTestStepL();
	void TestDllInvalidOrdinal0L();
	};

#endif


