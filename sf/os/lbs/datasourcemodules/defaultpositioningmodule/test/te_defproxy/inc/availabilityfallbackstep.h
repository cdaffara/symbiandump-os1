#ifndef __AVAILABILITYFALLBACK_STEP_H__
#define __AVAILABILITYFALLBACK_STEP_H__

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
* Availability Fallback
* Module Disable / Enable
* Precondition:
* Dummy1 disabled. PSY order: Psy6 Dummy3
* Psy6 programmed for 2 seconds delay for every request.
* - 1st request                       -> KErrNone from PSY6
* - 2nd request, small delay, disable PSY6
* -> KErrNone from Dummy3
* - 3rd request, enable PSY6          -> KErrNone from Dummy3
* - 4th request                       -> KErrNone from PSY6
* - 5th request, no delay, disable PSY6 (during request)
* -> KErrNone from Dummy3
* - All disabled, 6th request         -> KErrNotFound
* - enable PSY6, 7th request          -> KErrNone from PSY6
* 
*
*/



/**
 @file AvailabilityFallbackStep.h
 @internalTechnology
*/

#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

class CAvailabilityFallbackStep : public CTe_defproxySuiteStepBase
	{
public:
	CAvailabilityFallbackStep();
	~CAvailabilityFallbackStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KAvailabilityFallbackStep,"AvailabilityFallbackStep");

#endif
