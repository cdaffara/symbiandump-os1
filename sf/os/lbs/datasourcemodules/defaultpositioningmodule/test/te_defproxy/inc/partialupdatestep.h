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
 @file PartialUpdateStep.h
 @internalTechnology
*/
#ifndef __PARTIALUPDATE_STEP_H__
#define __PARTIALUPDATE_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_defproxySuiteStepBase.h"

/**
Partial update 
*/

class CPartialUpdateStep : public CTe_defproxySuiteStepBase
	{
public:
	CPartialUpdateStep();
	~CPartialUpdateStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	};

_LIT(KPartialUpdateStep,"PartialUpdateStep");

#endif
