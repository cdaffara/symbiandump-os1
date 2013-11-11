/**
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file reminderstep.h
 @internalTechnology
*/
#if (!defined __REMINDER_STEP_H__)
#define __REMINDER_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "te_lbsapisuitestepbase.h"

class CReminderStep : public CTe_LbsApiSuiteStepBase
	{
public:
	CReminderStep();
	~CReminderStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KReminderStep,"NoMemoryStep");

#endif
