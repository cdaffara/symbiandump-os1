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
 @file lbsnetlocmgrCovTest1.h
*/
#if (!defined __LBSNETLOCMANAGERCOVTEST1_STEP_H__)
#define __LBSNETLOCMANAGERCOVTEST1_STEP_H__
#include <test/testexecutestepbase.h>
#include "Te_LbsNetLocMgrTestSuiteStepBase.h"

class ClbsnetlocmgrCovTest1 : public CTe_LbsNetLocMgrTestSuiteStepBase
	{
public:
	ClbsnetlocmgrCovTest1();
	~ClbsnetlocmgrCovTest1();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();

// Please add/modify your class members here:
private:
	};

_LIT(KLbsnetlocmgrCovTest1,"lbsnetlocmanagerCovTest1");

#endif
