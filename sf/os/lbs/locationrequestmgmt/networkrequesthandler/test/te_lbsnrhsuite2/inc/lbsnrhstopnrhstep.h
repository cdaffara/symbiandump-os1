/**
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file lbsnrhprivacystopnrhStep.h
*/
#if (!defined LBSNRHSTOPNRH_STEP_H__)
#define LBSNRHSTOPNRH_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsnrhsuite2stepbase.h"

class CLbsNrhStopNrhStep : public CTe_LbsNrhSuiteStepBase
	{
public:
	CLbsNrhStopNrhStep();
	~CLbsNrhStopNrhStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	TInt StopNrh();
	};

_LIT(KLbsNrhStopNrhStep,"lbsnrhstopnrhstep");

#endif
