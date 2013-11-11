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
 @file testX3PLocateLogStep.h
*/
#ifndef __TESTX3PLOCATELOG_STEP_H__
#define __TESTX3PLOCATELOG_STEP_H__

#include <test/testexecutestepbase.h>
#include "Te_LbsLoggingSuiteStepBase.h"

class CTestX3PLocateLogStep : public CTe_LbsLoggingSuiteStepBase
	{
public:
	CTestX3PLocateLogStep();
	~CTestX3PLocateLogStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KTestX3PLocateLogStep,"testX3PLocateLogStep");

#endif
