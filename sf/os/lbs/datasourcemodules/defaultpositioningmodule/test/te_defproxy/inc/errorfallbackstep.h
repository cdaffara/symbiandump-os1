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
 @file ErrorFallbackStep.h
 @internalTechnology
*/
#ifndef __ERRORFALLBACK_STEP_H__
#define __ERRORFALLBACK_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

/** Error Fallback
    Precondition: HPositionGenericInfo is used.

    - Psy6 programmed to return KPositionPartialUpdate.
        - list: PSY6, Dummy3            -> KPositionPartialUpdate from PSY6
        - list: Dummy3, PSY6            -> KErrNone from Dummy3

    - Psy6 programmed to return KPositionQualityLoss.
        - list: PSY6                    -> KPositionQualityLoss from Undefined
        - list: PSY6, Dummy1            -> KErrNone from Dummy1
        - list: Dummy1, PSY6            -> KErrNone from Dummy1

        - list: Dummy1, Dummy3, PSY6    -> KErrNone from Dummy1
        - list: Dummy1, PSY6, Dummy3    -> KErrNone from Dummy1
        - list: PSY6, Dummy3, Dummy1    -> KErrNone from Dummy3
        - list: PSY6, Dummy1, Dummy3    -> KErrNone from Dummy1
*/

class CErrorFallbackStep : public CTe_defproxySuiteStepBase
	{
public:
	CErrorFallbackStep();
	~CErrorFallbackStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KErrorFallbackStep,"ErrorFallbackStep");

#endif
