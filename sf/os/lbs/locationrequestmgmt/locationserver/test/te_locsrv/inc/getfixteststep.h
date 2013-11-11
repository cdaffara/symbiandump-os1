/**
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
*
*/



/**
 @file GetFixTestStep.h
 @internalTechnology
*/
#ifndef __GETFIXTEST_STEP_H__
#define __GETFIXTEST_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_locsrvsuitestepbase.h"

class CGetFixTestStep : public CTe_locsrvSuiteStepBase
	{
public:
	CGetFixTestStep();
	~CGetFixTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KGetFixTestStep,"GetFixTestStep");

#endif
