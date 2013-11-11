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
 @file LbsDebug0Step.h
 @internalTechnology
*/
#if (!defined __LBSDEBUG0_STEP_H__)
#define __LBSDEBUG0_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_LbsDebugSuiteStepBase.h"

class CLbsDebug0Step : public CTe_LbsDebugSuiteStepBase
	{
public:
	CLbsDebug0Step();
	~CLbsDebug0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
protected:
    void StartLbsTestStepL(TRequestStatus& aStatus);
    void FinishLbsTestStep(TRequestStatus& aStatus);
    TBool DoLbsTestStepL(TRequestStatus& aStatus);

private:
	};

_LIT(KLbsDebug0Step,"LbsDebug0Step");

#endif
