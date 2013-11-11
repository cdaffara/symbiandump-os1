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
 @file TwoPsysActiveStep.h
 @internalTechnology
*/
#ifndef __TWOPSYSACTIVE_STEP_H__
#define __TWOPSYSACTIVE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"

/** 
* Two PSYs - Internal GPS PSY and Network PSY
*
* PSY1 is not in Active and Ready status
*
* 1. PSY1 is given a fix before timeshift timeout. Fix from PSY1 is used
* 2. PSY1 gives a error code, and PSY2 is able to give a fix. then fix 
*    from PSY2 is used.
* 3. PSY1 does not response within timeshift. Then PSY2 will be tried. If
*    PSY2 is able to give a fix, then fix from PSY2 is used.
* 4. Both PSY1 and PSY2 gives a error code, then error code from PSY1 is 
*    used.
* 5. PSY1 does not give a fix within timeshift. Then PSY2 is tried. If then
*    PSY1 gives a fix, fix from PSY1 is used.
* 6. PSY1 does not give a fix within timeshift. Then PSY2 is tried but gives
*    an error code, after that, PSY1 gives a fix. Then fix from PSY1 is used.
* 7. PSY1 does not give a fix within timeshift. Then PSY2 is tried. if then 
*    PSY1 gives a error code, after that PSY2 gives a fix, then fix from 
*    PSY2 is used.
*/

class CTwoPsysActiveStep : public CTe_defproxySuiteStepBase
	{
public:
	CTwoPsysActiveStep();
	~CTwoPsysActiveStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

	};

_LIT(KTwoPsysActiveStep,"TwoPsysActiveStep");

#endif
