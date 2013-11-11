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
 @file
 @internalComponent
*/
#if (!defined LOADLOOPBACKST_STEP_H)
#define LOADLOOPBACKST_STEP_H

#ifdef SYMBIAN_OLD_EXPORT_LOCATION
#include <testexecutestepbase.h>
#else
#include <test/testexecutestepbase.h>
#endif

#include "Te_C32PerformanceSuiteStepBase.h"

class CC32LoadLoopbackSTStep : public CTe_C32PerformanceSuiteStepBase
	{
public:
	CC32LoadLoopbackSTStep();
	~CC32LoadLoopbackSTStep();
//	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
//	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KC32LoadLoopbackSTStep,"CC32LoadLoopbackSTStep");

#endif

