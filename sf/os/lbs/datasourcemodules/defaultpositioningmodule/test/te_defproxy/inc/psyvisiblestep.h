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
 @file PsyVisibleStep.h
 @internalTechnology
*/
#ifndef __PSYVISIBLE_STEP_H__
#define __PSYVISIBLE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"

/** PSY become visible
*/
class CPsyVisibleStep : public CTe_defproxySuiteStepBase
	{
public:
	CPsyVisibleStep();
	~CPsyVisibleStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KPsyVisibleStep,"PsyVisibleStep");

#endif
