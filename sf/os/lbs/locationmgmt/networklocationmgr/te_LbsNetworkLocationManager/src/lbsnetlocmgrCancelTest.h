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
 @file lbsnetlocmgrCancelTest.h
*/
#if (!defined __LBSNETLOCMANAGERCANCELTEST_STEP_H__)
#define __LBSNETLOCMANAGERCANCELTEST_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsNetLocMgrTestSuiteStepBase.h"

class ClbsnetlocmgrCancelTest : public CTe_LbsNetLocMgrTestSuiteStepBase
	{
public:
	ClbsnetlocmgrCancelTest();
	~ClbsnetlocmgrCancelTest();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KLbsnetlocmgrCancelTest,"lbsnetlocmanagerCancelTest");

#endif
