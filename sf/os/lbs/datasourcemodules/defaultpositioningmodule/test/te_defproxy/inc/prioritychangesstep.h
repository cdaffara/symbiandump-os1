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
 @file PriorityChangesStep.h
 @internalTechnology
*/
#ifndef __PRIORITYCHANGES_STEP_H__
#define __PRIORITYCHANGES_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_defproxysuitestepbase.h"

class CPriorityChangesStep : public CTe_defproxySuiteStepBase
	{
public:
	CPriorityChangesStep();
	~CPriorityChangesStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KPriorityChangesStep,"PriorityChangesStep");

#endif
