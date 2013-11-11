/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* file te_sysutilburtestbase.h
* 
*
*/



#ifndef __TE_SYSUTIL_SUITE_STEP_BASE__
#define __TE_SYSUTIL_SUITE_STEP_BASE__

#include <test/testexecutestepbase.h>
#include <sysutil.h>
#include "sysutilfunctions.h"
#include "sysutilconstants.h"

class CTeSysUtilSuiteStepBase : public CTestStep
	{
public:
	virtual ~CTeSysUtilSuiteStepBase();
	CTeSysUtilSuiteStepBase(const TDesC& aStepName);
	TVerdict doTestStepL();
protected:
	CTeSysUtilSuiteStepBase();
	};

#endif // __TE_SYSUTIL_SUITE_STEP_BASE__
