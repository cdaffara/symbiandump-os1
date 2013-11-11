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
 @file CenrepTestStep.h
 @internalTechnology
*/
#ifndef __CENREPTEST_STEP_H__
#define __CENREPTEST_STEP_H__
#include <test/testexecutestepbase.h>
#include <ecom/ecom.h>
#include <centralrepository.h>
#include "te_defproxysuitestepbase.h"

class CCenrepTestStep : public CTe_defproxySuiteStepBase
	{
public:
	CCenrepTestStep();
	~CCenrepTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KCenrepTestStep,"CenrepTestStep");

#endif
