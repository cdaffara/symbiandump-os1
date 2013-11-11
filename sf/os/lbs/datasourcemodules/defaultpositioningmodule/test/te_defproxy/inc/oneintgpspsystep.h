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
 @file OneIntGpsPsyStep.h
 @internalTechnology
*/
#ifndef __ONEINTGPSPSY_STEP_H__
#define __ONEINTGPSPSY_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"

/** 
* One internal GPS PSY 
* 
* Only one interal GPS PSY present in the system. 
* 
* 1. Make a location request when the PSY returns a fix imediately. 
* 2. Make a location request when the PSY returns a fix after 10s.(shorter than 
*    time shift value whne PSY is not in ready/active state)
* 3. Make a location request when the PSY returns a fix after 35s.(longer than
*    time shift value when PSY is not in ready/active state)
* 4. Make a location request when the PSY is in ready state and PSY returns a
*    fix imediately. 
* 5. Make a location request when the PSY is in ready state and PSY returns a
*    fix in 1 second(shorter than time shift value when PSY is in ready state). 
* 6. Make a location request when the PSY is in ready state and PSY returns a
*    fix in 5 second(longer than time shift value when PSY is in ready state). 
* 7. Make a location request when the PSY is in active state and PSY returns a
*    fix imediately. 
* 8. Make a location request when the PSY is in active state and PSY returns a
*    fix in 1 second(shorter than time shift value when PSY is in active state). 
* 9. Make a location request when the PSY is in active state and PSY returns a
*    fix in 5 second(longer than time shift value when PSY is in activestate). 
*/
class COneIntGpsPsyStep : public CTe_defproxySuiteStepBase
	{
public:
	COneIntGpsPsyStep();
	~COneIntGpsPsyStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KOneIntGpsPsyStep,"OneIntGpsPsyStep");

#endif
