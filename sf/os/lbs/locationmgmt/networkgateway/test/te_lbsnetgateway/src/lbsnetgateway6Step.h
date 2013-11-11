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
 @file lbsnetgateway6Step.h
*/
#if (!defined __LBSNETGATEWAY6_STEP_H__)
#define __LBSNETGATEWAY6_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_lbsnetgatewaySuiteStepBase.h"

class Clbsnetgateway6Step : public CTe_lbsnetgatewaySuiteStepBase
	{
public:
	Clbsnetgateway6Step();
	~Clbsnetgateway6Step();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

private:
	};

_LIT(Klbsnetgateway6Step,"lbsnetgateway6Step");

#endif
