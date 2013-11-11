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
 @file lbsnetsimsettertests.h
*/
#if (!defined LBSSETTERTESTS_H)
#define LBSSETTERTESTS_H
#include <test/testexecutestepbase.h>
#include "Te_lbsnetsimtestSuiteStepBase.h"
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>
#include <lbs/test/lbsnetsimgatewayobserver.h>
#include <lbs/test/lbsnetsimgateway.h>

class CLbsSetterTests : 
	public CTe_lbsnetsimtestSuiteStepBase
	{
	public:
		CLbsSetterTests();
		~CLbsSetterTests();
		virtual TVerdict doTestStepPreambleL();
		virtual TVerdict doTestStepL();
		virtual TVerdict doTestStepPostambleL();
	private:
		RLbsNetSimTest		iTest;
	};


_LIT(KSetterTests,"SetterTests");

#endif // LBSSETTERTESTS_H
