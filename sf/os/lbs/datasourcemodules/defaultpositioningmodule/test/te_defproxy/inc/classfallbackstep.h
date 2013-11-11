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
 @file ClassFallbackStep.h
 @internalTechnology
*/
#ifndef __CLASSFALLBACK_STEP_H__
#define __CLASSFALLBACK_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

/**
2. Supported classes fallback
    Description:
        Satellite data requests issued

    - All modules enabled               -> KErrNone from Dummy3
    - Dummy3 disabled                   -> KErrArgument
    - All disabled                      -> KErrNotFound
*/
class CClassFallbackStep : public CTe_defproxySuiteStepBase
	{
public:
	CClassFallbackStep();
	~CClassFallbackStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KClassFallbackStep,"ClassFallbackStep");

#endif
