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
 @file CancelTrackingStep.h
 @internalTechnology
*/
#ifndef __CANCELTRACKING_STEP_H__
#define __CANCELTRACKING_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

/**
Cancel Tracking session
    Description:
        Dummy1 and PSY6 are enabled. Both override tracking and PSY6 reports
        which tracking methods were called and when.

    - Setup PSY order: Dummy1, PSY6
    - Start tracking session with update interval 1 second.
    - 2 requests                        -> KErrNone from Dummy1 after 1 second
    - Change order: PSY6, Dummy1
    - 2 requests                        -> KErrNone from PSY6 after 1 second
    - Stop tracking, request Event Log from PSY6
    - Check PSY6 Event log to see if Tracking methods were called appropriately
*/

class CCancelTrackingStep : public CTe_defproxySuiteStepBase
	{
public:
	CCancelTrackingStep();
	~CCancelTrackingStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KCancelTrackingStep,"CancelTrackingStep");

#endif
