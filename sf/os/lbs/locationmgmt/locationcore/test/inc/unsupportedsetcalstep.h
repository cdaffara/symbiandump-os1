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
 @file unsupportedsetcalstep.h
 @internalTechnology
*/
#ifndef __UNSUPPORTEDSETCAL_STEP_H__
#define __UNSUPPORTEDSETCAL_STEP_H__
#include <test/testexecutestepbase.h>
#include "te_lbsadminsuitestepbase.h"

class CUnsupportedSetCalStep : public CTe_LbsAdminSuiteStepBase
	{
public:
	CUnsupportedSetCalStep();
	~CUnsupportedSetCalStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(KUnsupportedSetCalStep,"UnsupportedSetCalStep");

#endif
