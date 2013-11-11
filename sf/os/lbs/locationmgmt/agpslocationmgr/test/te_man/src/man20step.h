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
 @file man20Step.h
*/
#if (!defined __Man20_STEP_H__)
#define __Man20_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_manSuiteStepBase.h"
#include <e32property.h>

class Cman20Step : public CTe_manSuiteStepBase
	{
public:
	Cman20Step();
	~Cman20Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	RProperty iUpdateCodeProperty;
	};

_LIT(Kman20Step,"man20Step");

#endif
