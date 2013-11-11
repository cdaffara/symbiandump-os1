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
 @file TwoInternalGpsPsyStep.h
 @internalTechnology
*/
#ifndef __TWOINTERNALGPSPSY_STEP_H__
#define __TWOINTERNALGPSPSY_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"

/** 
* Two internal GPS PSY 
* 
* Two interal GPS PSY and network GPS PSY present in the system. 
* 
* 1. All three PSYs are enabled( GPS PSY1, GPS PSY2, Network Psy). 
* 2. Make a location request, fix is coming from the first PSY(GPS PSY1). 
* 3. Enable only GPS PSY1
* 4. Make a location request, fix is coming from GPS PSY1
* 5. Enable only GPS PSY2
* 6. Make a location request, fix is coming from GPS PSY2
* 7. Enable both GPS PSY1 and GPS PSY2
* 8, Make a location request, fix is coming from GPS PSY1
* 9. Enable GPS PSY2 and Network PSY
* 10.Make a location request, fix is coming from GPS PSY2
*/

class CTwoInternalGpsPsyStep : public CTe_defproxySuiteStepBase
	{
public:
	CTwoInternalGpsPsyStep();
	~CTwoInternalGpsPsyStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KTwoInternalGpsPsyStep,"TwoInternalGpsPsyStep");

#endif
