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
 @file lbsinternalapiTest4Step.h
*/
#if (!defined __lbsinternalapiTEST4_STEP_H__)
#define __lbsinternalapiTEST4_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsinternalapitestsuitestepbase.h"

class ClbsinternalapiTest4Step : public CTe_lbsinternalapiTestSuiteStepBase
	{
public:
	ClbsinternalapiTest4Step();
	~ClbsinternalapiTest4Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KlbsinternalapiTest4Step,"lbsinternalapitest4step");

#endif
