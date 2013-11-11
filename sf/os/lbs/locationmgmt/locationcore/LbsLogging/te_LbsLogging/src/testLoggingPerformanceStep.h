/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



/**
 @file testLoggingPerformanceStep.h
*/
#ifndef __TESTLOGGINGPERFORMANCE_STEP_H__
#define __TESTLOGGINGPERFORMANCE_STEP_H__

#include <test/testexecutestepbase.h>
#include "Te_LbsLoggingSuiteStepBase.h"

class CTestLoggingPerformanceStep : public CTe_LbsLoggingSuiteStepBase
	{
public:
	CTestLoggingPerformanceStep();
	~CTestLoggingPerformanceStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KTestLoggingPerformanceStep,"testLoggingPerformanceStep");

#endif
