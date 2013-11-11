/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __LBSNETREGSTATUSINTTEST0_STEP_H__)
#define __LBSNETREGSTATUSINTTEST0_STEP_H__
#include <test/TestExecuteStepBase.h>
#include "Te_lbsnetregstatusinttestSuiteStepBase.h"

class Clbsnetregstatusinttest0Step : public CTe_lbsnetregstatusinttestSuiteStepBase
	{
public:
	Clbsnetregstatusinttest0Step();
	~Clbsnetregstatusinttest0Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(Klbsnetregstatusinttest0Step,"lbsnetregstatusinttest0Step");

#endif

