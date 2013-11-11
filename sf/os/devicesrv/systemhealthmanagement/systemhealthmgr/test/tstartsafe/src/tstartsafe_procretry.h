// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TSTARTSAFE_PROCRETRY_H__
#define __TSTARTSAFE_PROCRETRY_H__


#include <test/testexecutestepbase.h>


//APPFWK-STARTSAFE-0007

_LIT(KCTestCaseProcRetry, "ProcRetry");


class CAppFwkStartSafeTestStepProcRetry : public CTestStep
	{
	
public:
	CAppFwkStartSafeTestStepProcRetry();
	
public:
	// From CTestStep
	TVerdict doTestStepL();
	};

#endif
