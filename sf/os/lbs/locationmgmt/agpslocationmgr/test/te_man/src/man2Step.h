/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file man2Step.h
*/
#if (!defined __MAN2_STEP_H__)
#define __MAN2_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_manSuiteStepBase.h"

class Cman2Step : public CTe_manSuiteStepBase
	{
public:
	Cman2Step();
	~Cman2Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Kman2Step,"man2Step");

#endif
