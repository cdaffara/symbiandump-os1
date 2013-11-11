/*
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
 @file 
 @internalTechnology
 @test
*/

#ifndef __CT_FULLROMPOSITIONERTEST_STEP_H__
#define __CT_FULLROMPOSITIONERTEST_STEP_H__

#include <test/TestExecuteStepBase.h>
#include <e32property.h>
#include <e32std.h>

#include "ctlbstestsuitestepbase.h"


class CPositionerTestStep : public CTe_TestSuiteStepBase
	{
public:
	CPositionerTestStep();
	~CPositionerTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
    void TestByIndexL(TInt aIndex);
    TVerdict TdVariant2_StandAlonePrivacyRom_Check_LocMonitor_TransientL();
	TVerdict TdVariant2_FullRom_Check_LocMonitor_PersistentL();
	};

_LIT(KPositionerTestStep,"PositionerTestStep");

#endif //__CT_FULLROMPOSITIONERTEST_STEP_H__
