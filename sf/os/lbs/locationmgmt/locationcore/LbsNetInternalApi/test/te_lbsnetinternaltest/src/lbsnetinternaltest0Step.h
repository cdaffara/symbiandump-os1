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
 @file lbsnetinternaltest0Step.h
*/
#if (!defined __LBSNETINTERNALTEST0_STEP_H__)
#define __LBSNETINTERNALTEST0_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbsnetinternaltestSuiteStepBase.h"

class Clbsnetinternaltest0Step : public CTe_lbsnetinternaltestSuiteStepBase
	{
public:
	Clbsnetinternaltest0Step();
	~Clbsnetinternaltest0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Klbsnetinternaltest0Step,"lbsnetinternaltest0Step");

#endif
