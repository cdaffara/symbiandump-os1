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
 @file lbsnrhprivctrlstep.h
*/

#ifndef LBSNRHPRIVCTRLSTEP_H_
#define LBSNRHPRIVCTRLSTEP_H_

#include <test/testexecutestepbase.h>
#include "te_lbsnrhsuite2stepbase.h"


class CLbsNrhPrivCtrlStep : public CTe_LbsNrhSuiteStepBase
	{
public:
	CLbsNrhPrivCtrlStep();
	~CLbsNrhPrivCtrlStep();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();

private:
	void Test001L();
	void Test002L();
	void Test003L();
	};

_LIT(KLbsNrhPrivCtrlStep, "lbsnrhprivctrlstep");

    
#endif // LBSNRHPRIVCTRLSTEP_H_